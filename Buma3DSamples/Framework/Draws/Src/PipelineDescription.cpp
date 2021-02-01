#include "pch.h"
#include "PipelineDescription.h"

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

inline buma3d::RESOURCE_FORMAT GetVertexElementFormat(VERTEX_ELEMENT_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::VERTEX_ELEMENT_TYPE_POSITION  : return buma3d::RESOURCE_FORMAT_R32G32B32A32_FLOAT; 
    case buma::draws::VERTEX_ELEMENT_TYPE_NORMAL    : return buma3d::RESOURCE_FORMAT_R32G32B32A32_FLOAT; 
    case buma::draws::VERTEX_ELEMENT_TYPE_TANGENT   : return buma3d::RESOURCE_FORMAT_R32G32B32A32_FLOAT; 
    case buma::draws::VERTEX_ELEMENT_TYPE_TEXCOORD0 : return buma3d::RESOURCE_FORMAT_R32G32_FLOAT;
    default:
        return buma3d::RESOURCE_FORMAT_UNKNOWN;
    }
}

}// namespace /*anonymous*/


PipelineDescription::PipelineDescription(DrawsInstance* _ins)
    : ins{ _ins }
{

}

PipelineDescription::~PipelineDescription()
{

}

void PipelineDescription::Init()
{
    PrepareInputLayout();
    PrepareTessellationState();
    PrepareViewportState();
    PrepareRasterizationState();
    PrepareStreamOutput();
    PrepareMultisampleState();
    PrepareDepthStencilState();
    PrepareBlendState();
    PrepareDynamicState();
    PrepareDynamicStates();
}
void PipelineDescription::PrepareInputLayout()
{
    // RENDER_PASS_TYPE_PRE_DEPTH
    {
        auto&& l = *(input_layout[RENDER_PASS_TYPE_PRE_DEPTH] = std::make_unique<util::InputLayoutDesc>());
        l.AddNewInputSlot().SetSlotNumber(0).SetStrideInBytes(sizeof(VertPositionT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_POSITION , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_POSITION));
        l.AddNewInputSlot().SetSlotNumber(1).SetStrideInBytes(sizeof(VertTexcoordT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_TEXCOORD0, 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TEXCOORD0));
        constexpr uint32_t s = (uint32_t)((uint8_t)(222) & ~(uint8_t)(0xff));
    }
    // RENDER_PASS_TYPE_BASE
    {
        auto&& l = *(input_layout[RENDER_PASS_TYPE_BASE] = std::make_unique<util::InputLayoutDesc>());
        l.AddNewInputSlot().SetSlotNumber(0).SetStrideInBytes(sizeof(VertPositionT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_POSITION , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_POSITION));
        l.AddNewInputSlot().SetSlotNumber(1).SetStrideInBytes(sizeof(VertNormalT))  .AddNewInputElement(VERTEX_SEMANTIC_NAME_NORMAL   , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_NORMAL));
        l.AddNewInputSlot().SetSlotNumber(2).SetStrideInBytes(sizeof(VertTangentT)) .AddNewInputElement(VERTEX_SEMANTIC_NAME_TANGENT  , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TANGENT));
        l.AddNewInputSlot().SetSlotNumber(3).SetStrideInBytes(sizeof(VertTexcoordT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_TEXCOORD0, 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TEXCOORD0));
    }

}
void PipelineDescription::PrepareTessellationState()
{

}
void PipelineDescription::PrepareViewportState()
{

}
void PipelineDescription::PrepareRasterizationState()
{

}
void PipelineDescription::PrepareStreamOutput()
{

}
void PipelineDescription::PrepareMultisampleState()
{

}
void PipelineDescription::PrepareDepthStencilState()
{

}
void PipelineDescription::PrepareBlendState()
{

}
void PipelineDescription::PrepareDynamicState()
{

}
void PipelineDescription::PrepareDynamicStates()
{

}




}// namespace draws
}// namespace buma
