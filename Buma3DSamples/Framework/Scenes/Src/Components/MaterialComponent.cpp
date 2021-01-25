#include "pch.h"
#include "MaterialComponent.h"

namespace buma
{
namespace scne
{

MaterialComponent::MaterialComponent(Scenes* _scenes)
    : ScenesObjectImpl      (_scenes)
    , alpha_mode            {}
    , alpha_cutoff          { 0.3f }
    , is_enabled_two_sided  { false }
    , custom_properties     {}
{

}

MaterialComponent::~MaterialComponent()
{

}

MaterialComponent* MaterialComponent::Create(Scenes* _scenes)
{
    return new MaterialComponent(_scenes);
}

void MaterialComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE MaterialComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IMaterialComponent;
}

void* MaterialComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                    return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                       return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IMaterialComponent)               return static_cast<IMaterialComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::MaterialComponentImpl)   return static_cast<MaterialComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)        return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void MaterialComponent::SetAlphaMode(MATERIAL_ALPHA_MODE _alpha_mode)
{
    alpha_mode = _alpha_mode;
}

void MaterialComponent::SetAlphaCutoff(float _alpha_cutoff)
{
    alpha_cutoff = _alpha_cutoff;
}

void MaterialComponent::SetTwoSidedEnabled(bool _is_enabled)
{
    is_enabled_two_sided = _is_enabled;
}

MATERIAL_ALPHA_MODE MaterialComponent::GetAlphaMode()
{
    return alpha_mode;
}

float MaterialComponent::GetAlphaCutoff(float _alpha_cutoff)
{
    return alpha_cutoff;
}

bool MaterialComponent::GetTwoSidedEnabled(bool _is_enabled)
{
    return is_enabled_two_sided;
}

void MaterialComponent::AddCustomProperties(IMaterialPropertyComponent* _property)
{
    custom_properties.emplace_back(GetAs<MaterialPropertyComponent>(_property));
}

void MaterialComponent::ReleaseCustomProperties(IMaterialPropertyComponent* _property)
{
    EraseElem(custom_properties, _property);
}

uint32_t MaterialComponent::GetMaterialPropertiesSize()
{
    return (uint32_t)custom_properties.size();
}

IMaterialPropertyComponent* MaterialComponent::GetMaterialProperties(uint32_t _index)
{
    return custom_properties[_index].Get();
}

IMaterialPropertyComponent* MaterialComponent::GetMaterialProperties(const char* _str)
{
    auto if_find = FindByName(custom_properties, _str);
    if (if_find != custom_properties.end())
        return if_find->Get();

    return nullptr;
}


}// namespace scne
}// namespace buma
