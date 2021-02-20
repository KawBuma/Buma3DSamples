#include "pch.h"
#include "MaterialPropertyComponent.h"

namespace buma
{
namespace scne
{

MaterialPropertyComponent::MaterialPropertyComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , value_components  {}
    , texture_component {}
    , sampler_component {}
{

}

MaterialPropertyComponent::~MaterialPropertyComponent()
{

}

MaterialPropertyComponent* MaterialPropertyComponent::Create(Scenes* _scenes)
{
    return new MaterialPropertyComponent(_scenes);
}

void MaterialPropertyComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE MaterialPropertyComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IMaterialPropertyComponent;
}

void* MaterialPropertyComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                            return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                               return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IMaterialPropertyComponent)               return static_cast<IMaterialPropertyComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::MaterialPropertyComponentImpl)   return static_cast<MaterialPropertyComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)                return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void MaterialPropertyComponent::AddValueComponent(IValueComponent* _component)
{
    value_components.emplace_back(GetAs<ValueComponent>(_component));
}

void MaterialPropertyComponent::ReleaseValueComponent(IValueComponent* _component)
{
    EraseElem(value_components, _component);
}

uint32_t MaterialPropertyComponent::GetValueComponentSize()
{
    return (uint32_t)value_components.size();
}

IValueComponent* MaterialPropertyComponent::GetValueComponent(uint32_t _index)
{
    return value_components[_index].Get();
}

IValueComponent* MaterialPropertyComponent::GetValueComponent(const char* _str)
{
    auto it_find = FindByName(value_components, _str);
    if (it_find != value_components.end())
        return it_find->Get();

    return nullptr;
}

void MaterialPropertyComponent::SetTextureComponent(ITextureComponent* _component)
{
    texture_component = GetAs<TextureComponent>(_component);
}

void MaterialPropertyComponent::SetSamplerComponent(ISamplerComponent* _component)
{
    sampler_component = GetAs<SamplerComponent>(_component);
}

ITextureComponent* MaterialPropertyComponent::GetTextureComponent()
{
    return texture_component.Get();
}

ISamplerComponent* MaterialPropertyComponent::GetSamplerComponent()
{
    return sampler_component.Get();
}


}// namespace scne
}// namespace buma
