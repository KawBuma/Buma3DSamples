#include "pch.h"
#include "LightPropertyComponent.h"

namespace buma
{
namespace scne
{

LightPropertyComponent::LightPropertyComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , value_type        {}
    , data              {}
{

}

LightPropertyComponent::~LightPropertyComponent()
{

}

LightPropertyComponent* LightPropertyComponent::Create(Scenes* _scenes)
{
    return new LightPropertyComponent(_scenes);
}

void LightPropertyComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE LightPropertyComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::ILightPropertyComponent;
}

void* LightPropertyComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                        return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                           return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::ILightPropertyComponent)              return static_cast<ILightPropertyComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::LightPropertyComponentImpl)  return static_cast<LightPropertyComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)            return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void LightPropertyComponent::SetValueType(VALUE_TYPE _type)
{
    value_type = _type;
}

void LightPropertyComponent::Resize(uint32_t _size_in_bytes)
{
    data.resize(_size_in_bytes);
}

void LightPropertyComponent::SetData(uint32_t _offset_in_bytes, uint32_t _src_size_in_bytes, const void* _src)
{
    memcpy_s(data.data() + _offset_in_bytes, data.size(), _src, _src_size_in_bytes);
}

VALUE_TYPE LightPropertyComponent::GetValueType()
{
    return value_type;
}

uint32_t LightPropertyComponent::GetSize()
{
    return (uint32_t)data.size();
}

const void* LightPropertyComponent::GetData()
{
    return data.data();
}


}// namespace scne
}// namespace buma
