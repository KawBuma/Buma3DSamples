#include "pch.h"
#include "IndexBufferComponent.h"

namespace buma
{
namespace scne
{

IndexBufferComponent::IndexBufferComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , index_layout      {}
    , buffer_view       {}
{

}

IndexBufferComponent::~IndexBufferComponent()
{

}

IndexBufferComponent* IndexBufferComponent::Create(Scenes* _scenes)
{
    return new IndexBufferComponent(_scenes);
}

void IndexBufferComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE IndexBufferComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IIndexBufferComponent;
}

void* IndexBufferComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                        return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                           return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IIndexBufferComponent)                return static_cast<IIndexBufferComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::IndexBufferComponentImpl)    return static_cast<IndexBufferComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)            return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void IndexBufferComponent::SetIndexLayout(IIndexLayoutComponent* _index_layout)
{
    index_layout = GetAs<IndexLayoutComponent>(_index_layout);
}

void IndexBufferComponent::SetBufferView(IBufferViewComponent* _buffer_view)
{
    buffer_view = GetAs<BufferViewComponent>(_buffer_view);
}

IIndexLayoutComponent* IndexBufferComponent::GetIndexLayout()
{
    return index_layout.Get();
}

IBufferViewComponent* IndexBufferComponent::GetBufferView()
{
    return buffer_view.Get();
}


}// namespace scne
}// namespace buma
