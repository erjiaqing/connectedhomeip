#!/usr/bin/env python
# Copyright (c) 2022 Project CHIP Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from pydoc import resolve
import click
import logging
import enum
import os
import typing
import jinja2

from typing import *
from ctypes import *

from dataclasses import dataclass
from lark import Lark
from lark.visitors import Transformer, v_args


def RemovePrefix(string: str):
    if string.startswith("pychip_"):
        return string[7:]
    else:
        return string


@dataclass
class Struct:
    name: str
    fields: Any = None


@dataclass
class Type:
    is_const: bool
    pointer_level: int
    base_type: str

    def python_type(self):
        if self.base_type == 'void':
            if not self.is_ptr:
                return 'None'
            else:
                return 'c_void_p'
        elif self.base_type == 'PyChipError':
            if not self.is_ptr:
                return 'PyChipError'
            else:
                return 'POINTER(PyChipError)'
        elif self.base_type == 'PyObject':
            if not self.is_ptr:
                raise "PyObject must be pointer"
            else:
                return 'py_object'

    def c_type(self):
        c_type = ''
        if self.base_type == 'void':
            if self.is_ptr:
                return 'c_void_p'
            else:
                return 'None'
        elif self.base_type == 'PyObject':
            if not self.is_ptr:
                raise "PyObject must be pointer"
            else:
                return 'py_object'
        elif self.is_ptr and self.base_type == 'char':
            return 'c_char_p'
        elif self.base_type in ['uint8_t', 'uint16_t', 'uint32_t', 'uint64_t', 'int8_t', 'int16_t', 'int32_t', 'int64_t']:
            c_type = 'c_' + self.base_type[:-2]
        elif self.base_type in ['bool' 'char']:
            c_type = 'c_' + self.base_type
        else:
            # TODO: We may want to ensure that: if the type is not defined, then it can only be passed as a pointer.
            c_type = self.base_type
        if self.is_ptr:
            return f'POINTER({c_type})'
        return c_type

    @property
    def is_ptr(self):
        return self.pointer_level == 1

    @property
    def is_ptrptr(self):
        return self.pointer_level == 2

    def __str__(self):
        return f"{'const ' if self.is_const else ''}{self.base_type}{'*' if self.is_ptr else ''}"


@dataclass
class FunctionArg:
    type: Type
    name: str

    def __str__(self):
        return f"({self.type}) {self.name}"


@dataclass
class Function:
    ret: Type
    args: List[FunctionArg]
    name: str

    def __str__(self):
        return f"{self.ret} {self.name}({self.args})"


@dataclass
class FunctionPtr:
    ret: Type
    args: List[FunctionArg]
    name: str

    def __str__(self):
        return f"{self.ret}(*{self.name})({self.args})"


@dataclass
class TypeAlias:
    type: Type
    name: str


class CTransformer(Transformer):
    def __init__(self):
        pass

    def id(self, tokens):
        return tokens[0].value

    def struct(self, tokens):
        if tokens[0] == "PyObject":
            return None
        return Struct(name=tokens[0])

    def const_type(self, tokens):
        ret = tokens[0]
        if isinstance(ret, Type):
            ret.is_const = True
        else:
            ret = Type(is_const=True, pointer_level=0, base_type=ret)
        return ret

    def ptr_type(self, tokens):
        ret = tokens[0]
        if isinstance(ret, Type):
            ret.pointer_level = ret.pointer_level + 1
        else:
            ret = Type(is_const=False, pointer_level=1, base_type=ret)
        return ret

    def type(self, tokens):
        return Type(is_const=False, pointer_level=0, base_type=tokens[0])

    def var_type(self, tokens):
        return tokens[0]

    def function_arg(self, tokens):
        ret = FunctionArg(type=tokens[0], name=tokens[1])
        return ret

    def function_def(self, tokens):
        ret = Function(ret=tokens[0], name=tokens[1], args=tokens[2:])
        return ret

    def function_ptr_def(self, tokens):
        ret = FunctionPtr(ret=tokens[0], name=tokens[1], args=tokens[2:])
        return ret

    def c_macro_ifdef(self, tokens):
        return None

    def type_alias(self, tokens):
        return TypeAlias(type=tokens[0], name=tokens[1])

    def header(self, tokens):
        function_ptrs = [tok for tok in tokens if isinstance(tok, FunctionPtr)]
        functions = [tok for tok in tokens if isinstance(tok, Function)]
        structs = [tok for tok in tokens if isinstance(tok, Struct)]
        typealias = [tok for tok in tokens if isinstance(tok, TypeAlias)]
        return {
            "function_ptrs": function_ptrs,
            "functions": functions,
            "structs": structs,
            "type_alias": typealias
        }


Types = None


def BuildTypes(structs, function_ptrs, type_alias, functions):
    global Types

    Types = {}

    for type in type_alias:
        Types[type.name] = type.type.base_type

    for type in structs:
        if Types.get(type.name, None):
            raise ValueError(f"Duplicated type: struct {type.name}")
        Types[type.name] = type

    for type in function_ptrs:
        if Types.get(type.name, None):
            raise ValueError(f"Duplicated type: struct {type.name}")
        Types[type.name] = type


def AsCType(input_type: Type) -> str:
    global Types

    resolved_type = Types.get(input_type.base_type, input_type.base_type)
    while isinstance(resolved_type, str) and Types.get(resolved_type, None):
        resolved_type = Types.get(resolved_type, resolved_type)

    if isinstance(resolved_type, Struct):
        if resolved_type.fields:
            raise NotImplementedError("Not implemented: struct with fields")
        if input_type.is_ptr:
            return RemovePrefix(resolved_type.name + "_ptr")
        if input_type.is_ptrptr:
            return f"POINTER({RemovePrefix(resolved_type.name + '_ptr')})"
        raise ValueError(f"Unexpected type {resolved_type}")
    elif isinstance(resolved_type, FunctionPtr):
        if input_type.is_ptr or input_type.is_ptrptr:
            raise ValueError(f"Unexpected type {input_type}")
        return RemovePrefix(resolved_type.name)
    elif isinstance(resolved_type, str):
        if resolved_type == 'void':
            if resolved_type:
                return 'c_void_p'
            else:
                return 'None'
        elif resolved_type == 'PyObject':
            if not input_type.is_ptr:
                raise "PyObject must be pointer"
            return 'py_object'
        elif input_type.is_ptr and resolved_type == 'char':
            return 'c_char_p'
        elif resolved_type in ['uint8_t', 'uint16_t', 'uint32_t', 'uint64_t', 'int8_t', 'int16_t', 'int32_t', 'int64_t']:
            c_type = 'c_' + resolved_type[:-2]
        elif resolved_type in ['bool', 'char']:
            c_type = 'c_' + resolved_type
        elif resolved_type == 'PyChipError':
            if input_type.is_ptrptr:
                raise ValueError(f"Unexpected type {input_type}")
            c_type = resolved_type
        else:
            raise ValueError(f"Unexpected type {input_type}")
        if input_type.is_ptr:
            return f'POINTER({c_type})'
        elif input_type.is_ptrptr:
            raise ValueError(f"Unexpected type {input_type}")
        return c_type
    else:
        raise ValueError(f"Unexpected type {input_type} (resolved as {resolved_type} ({type(resolved_type)})")


@click.command()
@click.option(
    '--output-dir',
    type=click.Path(exists=False),
    default=".",
    help='Where to generate the code')
@click.option(
    '--c-header-lex',
    type=click.Path(exists=False),
    default=".",
    help='Where to generate the code')
@click.option(
    '--c-header',
    type=click.Path(exists=False),
    default=".",
    help='Where to generate the code')
def main(output_dir, c_header_lex, c_header):
    """
    Parses MATTER IDL files (.matter) and performs SDK code generation
    as set up by the program arguments.
    """
    with open(c_header_lex, "rt") as fp:
        parser = Lark(fp.read(), parser="lalr", start="header", debug=True)
    with open(c_header, "rt") as fp:
        header = parser.parse(fp.read())
    transformed = CTransformer().transform(header)
    logging.info("Done")

    jinja_env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(searchpath=os.path.dirname(__file__)))
    BuildTypes(**transformed)
    print(jinja_env.get_template("api_template.jinja").render(AsCType=AsCType, RemovePrefix=RemovePrefix, **transformed))


if __name__ == '__main__':
    main()
