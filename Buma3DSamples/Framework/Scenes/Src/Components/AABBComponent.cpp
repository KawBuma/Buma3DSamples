#include "pch.h"
#include "AABBComponent.h"

namespace buma
{
namespace scne
{

AABBComponent::AABBComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , min               {}
    , max               {}
{
}

AABBComponent::~AABBComponent()
{

}

AABBComponent* AABBComponent::Create(Scenes* _scenes)
{
    return new AABBComponent(_scenes);
}

void AABBComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE AABBComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IAABBComponent;
}

void* AABBComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                   return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IAABBComponent)               return static_cast<IAABBComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::AABBComponentImpl)   return static_cast<AABBComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)    return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void AABBComponent::UpdateBounds(IMeshComponent* _mesh)
{
    min = glm::vec3(FLT_MAX);
    max = glm::vec3(FLT_MIN);

    auto size = _mesh->GetSubmeshSize();
    for (uint32_t i = 0; i < size; i++)
    {
        auto submesh = _mesh->GetSubmeshs(i);
        for (uint32_t i_vb = 0, vbsize = submesh->GetVertexBuffersSize(); i_vb < vbsize; i_vb++)
        {
            auto vb = submesh->GetVertexBuffers(i_vb);
            auto layout = vb->GetVertexLayout();

            bool has_found = false;
            uint32_t element_offset = 0;
            CalcPositionElementOffset(layout, has_found, element_offset);
            if (!has_found)
                continue;

            CalcBounds(layout, vb, element_offset);
        }
    }
}

void AABBComponent::CalcPositionElementOffset(buma::scne::IVertexLayoutComponent* layout, bool& has_found, uint32_t& element_offset)
{
    std::string name;
    for (uint32_t i_elem = 0, num_elems = layout->GetVertexElementsSize(); i_elem < num_elems; i_elem++)
    {
        auto elem = GetAs<VertexElementComponent>(layout->GetVertexElements(i_elem));
        if (!elem->GetName())
            continue;

        name.clear();
        name.assign(elem->GetName());
        if (name.find(SCENES_VERTEX_ELEMENT_NAME_POSITION, 0) != std::string::npos)
        {
            has_found = true;
            break;
        }
        element_offset += elem->GetSizeOfElements();
    }
}

void AABBComponent::CalcBounds(buma::scne::IVertexLayoutComponent* layout, buma::scne::IVertexBufferComponent* vb, const uint32_t& element_offset)
{
    auto stride = layout->GetLayoutsStride();
    auto view = vb->GetBufferView();
    auto data = (uint8_t*)view->GetData();
    auto num_verts = view->GetSize() / stride;
    for (uint32_t i_vert = 0; i_vert < num_verts; i_vert++)
    {
        auto pos = (glm::vec3*)(data + element_offset);
        min = glm::min(max, *pos);
        max = glm::max(max, *pos);
        data += stride;
    }
}

const glm::vec3& AABBComponent::GetMinBounds()
{
    return min;
}

const glm::vec3& AABBComponent::GetMaxBounds()
{
    return max;
}

}// namespace scne
}// namespace buma
