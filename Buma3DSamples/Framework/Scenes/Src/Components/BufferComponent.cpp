#include "pch.h"
#include "BufferComponent.h"

namespace buma
{
namespace scne
{

BufferComponent::BufferComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , data              {}
{

}

BufferComponent::~BufferComponent()
{

}

BufferComponent* BufferComponent::Create(Scenes* _scenes)
{
    return new BufferComponent(_scenes);
}

void BufferComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE BufferComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IBufferComponent;
}

void* BufferComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                    return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                       return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IBufferComponent)                 return static_cast<IBufferComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::BufferComponentImpl)     return static_cast<BufferComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)        return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void BufferComponent::Resize(uint64_t _size_in_bytes)
{
    data.resize(_size_in_bytes);
}

uint64_t BufferComponent::GetSize()
{
    return (uint64_t)data.size();
}

void* BufferComponent::GetData()
{
    return data.data();
}

void BufferComponent::SetData(uint64_t _offset, uint64_t _size, void* _src_data)
{
    memcpy(data.data() + _offset, _src_data, _size);
}


}// namespace scne
}// namespace buma
