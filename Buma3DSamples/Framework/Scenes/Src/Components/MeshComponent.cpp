#include "pch.h"
#include "MeshComponent.h"

namespace buma
{
namespace scne
{

MeshComponent::MeshComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , submeshes         {}
{

}

MeshComponent::~MeshComponent()
{
}

MeshComponent* MeshComponent::Create(Scenes* _scenes)
{
    return new MeshComponent(_scenes);
}

void MeshComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE MeshComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IMeshComponent;
}

void* MeshComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                    return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                       return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IMeshComponent)                   return static_cast<IMeshComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::MeshComponentImpl)       return static_cast<MeshComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)        return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void MeshComponent::AddSubmesh(ISubmeshComponent* _submesh)
{
    submeshes.emplace_back(GetAs<SubmeshComponent>(_submesh));
}

void MeshComponent::ReleaseSubmesh(ISubmeshComponent* _submesh)
{
    EraseElem(submeshes, _submesh);
}

uint32_t MeshComponent::GetSubmeshSize()
{
    return (uint32_t)submeshes.size();
}

ISubmeshComponent* MeshComponent::GetSubmeshs(uint32_t _index)
{
    return submeshes[_index].Get();
}



}// namespace scne
}// namespace buma
