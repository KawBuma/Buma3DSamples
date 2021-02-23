#include "pch.h"
#include "SubmeshComponent.h"

namespace buma
{
namespace scne
{

SubmeshComponent::SubmeshComponent(Scenes* _scenes)
    : ScenesObjectImpl          (_scenes)
    , material                  {}
    , index_buffer_start_offset {}
    , vertex_buffers            {}
    , index_buffers             {}
{

}

SubmeshComponent::~SubmeshComponent()
{

}

SubmeshComponent* SubmeshComponent::Create(Scenes* _scenes)
{
    return new SubmeshComponent(_scenes);
}

void SubmeshComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE SubmeshComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::ISubmeshComponent;
}

void* SubmeshComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                    return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                       return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::ISubmeshComponent)                return static_cast<ISubmeshComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::SubmeshComponentImpl)    return static_cast<SubmeshComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)        return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void SubmeshComponent::SetMaterial(IMaterialComponent* _material)
{
    material = GetAs<MaterialComponent>(_material);
}

void SubmeshComponent::SetIndexBufferStartOffset(uint32_t _offset)
{
    index_buffer_start_offset = _offset;
}

void SubmeshComponent::AddVertexBuffer(IVertexBufferComponent* _vertex_buffer)
{
    vertex_buffers.emplace_back(GetAs<VertexBufferComponent>(_vertex_buffer));
}

void SubmeshComponent::AddIndexBuffer(IIndexBufferComponent* _index_buffer)
{
    index_buffers.emplace_back(GetAs<IndexBufferComponent>(_index_buffer));
}

void SubmeshComponent::ReleaseVertexBuffer(IVertexBufferComponent* _vertex_buffer)
{
    EraseElem(vertex_buffers, _vertex_buffer);
}

void SubmeshComponent::ReleaseIndexBuffer(IIndexBufferComponent* _index_buffer)
{
    EraseElem(index_buffers, _index_buffer);
}

IMaterialComponent* SubmeshComponent::GetMaterial()
{
    return material.Get();
}

uint32_t SubmeshComponent::GetIndexBufferStartOffset()
{
    return index_buffer_start_offset;
}

uint32_t SubmeshComponent::GetVertexBuffersSize()
{
    return (uint32_t)vertex_buffers.size();
}

uint32_t SubmeshComponent::GetIndexBuffersSize()
{
    return (uint32_t)index_buffers.size();
}

IVertexBufferComponent* SubmeshComponent::GetVertexBuffers(uint32_t _index)
{
    return vertex_buffers[_index].Get();
}

IIndexBufferComponent* SubmeshComponent::GetIndexBuffers(uint32_t _index)
{
    return index_buffers[_index].Get();
}


}// namespace scne
}// namespace buma
