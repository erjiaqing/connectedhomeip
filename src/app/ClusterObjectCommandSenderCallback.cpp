#include <ClusterObjectCommandSenderCallback.h>

#include <app/CommandSender.h>
#include <app/common/StatusElement.h>
#include <app/data-model/Decode.h>

#include <functional>

namespace chip {
namespace app {
template <>
void ClusterObjectCommandSenderCallback<void>::OnResponse(const CommandSender * apCommandSender,
                                                          const CommandPath::Type & aCommandPath, TLV::TLVReader & aReader)
{
    mOnResponse(apCommandSender, aCommandPath);
}

} // namespace app
} // namespace chip
