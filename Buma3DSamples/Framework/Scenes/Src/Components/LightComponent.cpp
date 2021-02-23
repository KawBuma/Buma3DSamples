#include "pch.h"
#include "LightComponent.h"

namespace buma
{
namespace scne
{

LightComponent::LightComponent(Scenes* _scenes)
    : ScenesObjectImpl(_scenes)
    , light_type        { LIGHT_TYPE_DIRECTIONAL }
    , direction         { 0.1f, 1.f, 0.3f }
    , color             { 1, 1, 1, 1 }
    , custom_properties {}
{

}

LightComponent::~LightComponent()
{

}

LightComponent* LightComponent::Create(Scenes* _scenes)
{
    return new LightComponent(_scenes);
}

void LightComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE LightComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::ILightComponent;
}

void* LightComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                   return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::ILightComponent)              return static_cast<ILightComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::LightComponentImpl)  return static_cast<LightComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)    return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

LIGHT_TYPE LightComponent::SetLightType()
{
    return light_type;
}

const glm::vec3& LightComponent::GetDir()
{
    return direction;
}

const glm::vec4& LightComponent::GetColor()
{
    return color;
}

void LightComponent::SetLihtType(LIGHT_TYPE _type)
{
    light_type = _type;
}

void LightComponent::SetDir(const glm::vec3& _direction)
{
    direction = _direction;
}

void LightComponent::SetColor(const glm::vec4& _color)
{
    color = _color;
}

void LightComponent::AddCustomProperties(ILightPropertyComponent* _property)
{
    custom_properties.emplace_back(GetAs<LightPropertyComponent>(_property));
}

void LightComponent::ReleaseCustomProperties(ILightPropertyComponent* _property)
{
    EraseElem(custom_properties, _property);
}

uint32_t LightComponent::GetCustomPropertiesSize()
{
    return (uint32_t)custom_properties.size();
}

ILightPropertyComponent* LightComponent::GetCustomProperties(uint32_t _index)
{
    return custom_properties[_index].Get();
}

ILightPropertyComponent* LightComponent::GetCustomProperties(const char* _name)
{
    auto it_find = FindByName(custom_properties, _name);
    if (it_find != custom_properties.end())
        return it_find->Get();

    return nullptr;
}


}// namespace scne
}// namespace buma
