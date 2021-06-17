#include <vector>

#include <app/InteractionModelEngine.h>

#include "Delegate.h"

using namespace chip;
using namespace chip::app;
using namespace chip::Controller;

extern "C" {

CHIP_ERROR pychip_im_SendReadRequest(NodeId aNodeId, Transport::AdminId aAdminId, ClusterAttributeInfo * apAttributes, size_t apAttributesLen, uint64_t aHandle)
{
    std::vector<AttributePathParams> params;
    for (size_t i = 0; i < apAttributesLen; i++)
    {
        AttributePathParams param;
        param.mNodeId = aNodeId;
        param.mEndpointId = apAttributes[i].mEndpointId;
        param.mClusterId = apAttributes[i].mClusterId;
        param.mFieldId = apAttributes[i].mAttributeId;
        param.mFlags.Set(AttributePathParams::Flags::kFieldIdValid);
        params.push_back(param);
    }
    return chip::app::InteractionModelEngine::GetInstance()->SendReadRequest(aNodeId, aAdminId, nullptr, 0, params.data(), params.size(), 0, aHandle);
}

}
