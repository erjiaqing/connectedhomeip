#pragma once

#include <lib/core/CHIPTLV.h>
#include <protocols/Protocols.h>
#include <protocols/secure_channel/Constants.h>

namespace chip {
namespace app {
namespace Structs {
namespace StatusElement {

enum FieldId
{
    kGeneralCodeFieldTag  = 1,
    kProtocolIdFieldTag   = 2,
    kProtocolCodeFieldTag = 3,
};

struct Type
{
    Protocols::SecureChannel::GeneralStatusCode generalCode;
    uint32_t protocolId;
    uint16_t protocolCode;

    CHIP_ERROR Encode(TLV::TLVWriter & writer, uint64_t tag) const;
    CHIP_ERROR Decode(TLV::TLVReader & reader);
};

} // namespace StatusElement
} // namespace Structs
} // namespace app
} // namespace chip
