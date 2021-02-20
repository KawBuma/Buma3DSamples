#include "pch.h"
#include "ValueComponent.h"

namespace buma
{
namespace scne
{

ValueComponent::ValueComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , value_type        {}
    , data              {}
{

}

ValueComponent::~ValueComponent()
{

}

ValueComponent* ValueComponent::Create(Scenes* _scenes)
{
    return new ValueComponent(_scenes);
}

void ValueComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE ValueComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IValueComponent;
}

void* ValueComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                   return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IValueComponent)              return static_cast<IValueComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ValueComponentImpl)  return static_cast<ValueComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)    return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void ValueComponent::SetValueType(VALUE_TYPE _type)
{
    value_type = _type;
}

void ValueComponent::Resize(uint32_t _size_in_bytes)
{
    data.resize(_size_in_bytes);
}

void ValueComponent::SetData(uint32_t _offset_in_bytes, uint32_t _src_size_in_bytes, const void* _src)
{
    memcpy_s(data.data() + _offset_in_bytes, data.size(), _src, _src_size_in_bytes);
}

VALUE_TYPE ValueComponent::GetValueType()
{
    return value_type;
}

uint32_t ValueComponent::GetSize()
{
    return (uint32_t)data.size();
}

const void* ValueComponent::GetData()
{
    return data.data();
}


}// namespace scne
}// namespace buma
