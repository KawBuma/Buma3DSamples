#include "pch.h"
#include "VertexBufferComponent.h"

namespace buma
{
namespace scne
{

VertexBufferComponent::VertexBufferComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , vertex_layout     {}
    , buffer_view       {}
{

}

VertexBufferComponent::~VertexBufferComponent()
{

}

VertexBufferComponent* VertexBufferComponent::Create(Scenes* _scenes)
{
    return new VertexBufferComponent(_scenes);
}

void VertexBufferComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE VertexBufferComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IVertexBufferComponent;
}

void* VertexBufferComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                        return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                           return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IVertexBufferComponent)               return static_cast<IVertexBufferComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::VertexBufferComponentImpl)   return static_cast<VertexBufferComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)            return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void VertexBufferComponent::SetVertexLayout(IVertexLayoutComponent* _vertex_layout)
{
    vertex_layout = GetAs<VertexLayoutComponent>(_vertex_layout);
}

void VertexBufferComponent::SetBufferView(IBufferViewComponent* _buffer_view)
{
    buffer_view = GetAs<BufferViewComponent>(_buffer_view);
}

IVertexLayoutComponent* VertexBufferComponent::GetVertexLayout()
{
    return vertex_layout.Get();
}

IBufferViewComponent* VertexBufferComponent::GetBufferView()
{
    return buffer_view.Get();
}


}// namespace scne
}// namespace buma
