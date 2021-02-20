#include "pch.h"
#include "VertexLayoutComponent.h"

namespace buma
{
namespace scne
{

VertexLayoutComponent::VertexLayoutComponent(Scenes* _scenes)
    : ScenesObjectImpl(_scenes)
    , layouts_stride    {}
    , vertex_elements   {}
{

}

VertexLayoutComponent::~VertexLayoutComponent()
{

}

VertexLayoutComponent* VertexLayoutComponent::Create(Scenes* _scenes)
{
    return new VertexLayoutComponent(_scenes);
}

void VertexLayoutComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE VertexLayoutComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IVertexLayoutComponent;
}

void* VertexLayoutComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                        return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                           return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IVertexLayoutComponent)               return static_cast<IVertexLayoutComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::VertexLayoutComponentImpl)   return static_cast<VertexLayoutComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)            return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void VertexLayoutComponent::AddVertexElement(IVertexElementComponent* _vertex_element)
{
    vertex_elements.emplace_back(GetAs<VertexElementComponent>(_vertex_element));
    UpdateLayoutsStride();
}

void VertexLayoutComponent::ReleaseVertexElement(IVertexElementComponent* _vertex_element)
{
    bool erased = EraseElem(vertex_elements, _vertex_element);
    if (erased)
        UpdateLayoutsStride();
}

uint32_t VertexLayoutComponent::GetLayoutsStride()
{
    return layouts_stride;
}

uint32_t VertexLayoutComponent::GetVertexElementsSize()
{
    return (uint32_t)vertex_elements.size();
}

IVertexElementComponent* VertexLayoutComponent::GetVertexElements(uint32_t _index)
{
    return vertex_elements[_index].Get();
}

IVertexElementComponent* VertexLayoutComponent::GetVertexElements(const char* _name)
{
    auto it_find = FindByName(vertex_elements, _name);
    if (it_find != vertex_elements.end())
        return it_find->Get();

    return nullptr;
}

void VertexLayoutComponent::UpdateLayoutsStride()
{
    layouts_stride = 0;
    for (auto& i : vertex_elements)
    {
        layouts_stride += i->GetSizeOfElements();
    }
}


}// namespace scne
}// namespace buma
