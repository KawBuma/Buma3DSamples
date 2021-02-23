#include "pch.h"
#include "VertexElementComponent.h"

namespace buma
{
namespace scne
{

namespace /*anonymous*/
{

uint32_t GetSizeOfDataFormat(DATA_FORMAT _data_format)
{
    switch (_data_format)
    {
    case buma::scne::DATA_FORMAT_UNDEFINED : return 0;
    case buma::scne::DATA_FORMAT_UINT8     : return 1;
    case buma::scne::DATA_FORMAT_UINT16    : return 2;
    case buma::scne::DATA_FORMAT_UINT32    : return 4;
    case buma::scne::DATA_FORMAT_SINT8     : return 1;
    case buma::scne::DATA_FORMAT_SINT16    : return 2;
    case buma::scne::DATA_FORMAT_SINT32    : return 4;
    case buma::scne::DATA_FORMAT_FLOAT32   : return 4;

    default:
        return 0;
    }
}

}// namespace /*anonymous*/


VertexElementComponent::VertexElementComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , data_format       {}
    , component_count   {}
{

}

VertexElementComponent::~VertexElementComponent()
{

}

VertexElementComponent* VertexElementComponent::Create(Scenes* _scenes)
{
    return new VertexElementComponent(_scenes);
}

void VertexElementComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE VertexElementComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IVertexElementComponent;
}

void* VertexElementComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                        return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                           return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IVertexElementComponent)              return static_cast<IVertexElementComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::VertexElementComponentImpl)  return static_cast<VertexElementComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)            return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void VertexElementComponent::SetDataFormat(DATA_FORMAT _format)
{
    data_format = _format;
}

DATA_FORMAT VertexElementComponent::GetDataFormat()
{
    return data_format;
}

uint32_t VertexElementComponent::GetSizeOfElements()
{
    return GetSizeOfDataFormat(data_format) * component_count;
}

void VertexElementComponent::SetComponentCount(uint32_t _count)
{
    component_count = _count;
}

uint32_t VertexElementComponent::GetComponentCount()
{
    return component_count;
}


}// namespace scne
}// namespace buma
