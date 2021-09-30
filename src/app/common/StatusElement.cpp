#include <app/common/StatusElement.h>
#include <app/data-model/Decode.h>
#include <app/data-model/Encode.h>
#include <core/CHIPTLV.h>
#include <lib/support/CodeUtils.h>

namespace chip {
namespace app {
namespace Structs {
namespace StatusElement {

CHIP_ERROR Type::Encode(TLV::TLVWriter & writer, uint64_t tag) const
{
    TLV::TLVType outer;

    ReturnErrorOnFailure(writer.StartContainer(tag, TLV::kTLVType_List, outer));

    {
        ReturnErrorOnFailure(DataModel::Encode(writer, TLV::ContextTag(kGeneralCodeFieldTag), generalCode));
        ReturnErrorOnFailure(DataModel::Encode(writer, TLV::ContextTag(kProtocolIdFieldTag), protocolId));
        ReturnErrorOnFailure(DataModel::Encode(writer, TLV::ContextTag(kProtocolCodeFieldTag), protocolCode));
    }

    ReturnErrorOnFailure(writer.EndContainer(outer));

    return CHIP_NO_ERROR;
}

CHIP_ERROR Type::Decode(TLV::TLVReader & reader)
{
    CHIP_ERROR err;
    TLV::TLVType outer;

    ReturnErrorOnFailure(reader.EnterContainer(outer));

    // NOTE: StatusElement is a list
    {
        ReturnErrorOnFailure(reader.Next());
        ReturnErrorOnFailure(DataModel::Decode(reader, generalCode));
        ReturnErrorOnFailure(reader.Next());
        ReturnErrorOnFailure(DataModel::Decode(reader, protocolCode));
        ReturnErrorOnFailure(reader.Next());
        ReturnErrorOnFailure(DataModel::Decode(reader, protocolCode));
    }

    ReturnErrorOnFailure(reader.ExitContainer(outer));
    return CHIP_NO_ERROR;
}

} // namespace StatusElement
} // namespace Structs
} // namespace app
} // namespace chip
