#include "pch.h"
#include "BufferViewComponent.h"

namespace buma
{
namespace scne
{

BufferViewComponent::BufferViewComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , buffer            {}
    , offset            {}
    , size              {}
    , data              {}
{

}

BufferViewComponent::~BufferViewComponent()
{

}

BufferViewComponent* BufferViewComponent::Create(Scenes* _scenes)
{
    return new BufferViewComponent(_scenes);
}

void BufferViewComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE BufferViewComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IBufferViewComponent;
}

void* BufferViewComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                    return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                       return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IBufferViewComponent)             return static_cast<IBufferViewComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::BufferViewComponentImpl) return static_cast<BufferViewComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)        return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void BufferViewComponent::SetBuffer(IBufferComponent* _buffer)
{
    buffer = GetAs<BufferComponent>(_buffer);
    UpdateDataPtr();
}

void BufferViewComponent::SetOffset(uint64_t _offset_in_bytes)
{
    offset = _offset_in_bytes;
    UpdateDataPtr();
}

void BufferViewComponent::SetSize(uint64_t _size_in_bytes)
{
    size = _size_in_bytes;
}

IBufferComponent* BufferViewComponent::GetBuffer()
{
    return buffer.Get();
}

uint64_t BufferViewComponent::GetOffset()
{
    return offset;
}

uint64_t BufferViewComponent::GetSize()
{
    return size;
}

void* BufferViewComponent::GetData()
{
    return data;
}

void BufferViewComponent::UpdateDataPtr()
{
    if (buffer.Get())
        data = offset + (uint8_t*)buffer->GetData();
    else
        data = nullptr;
}


}// namespace scne
}// namespace buma
