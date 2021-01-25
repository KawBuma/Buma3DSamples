#include "pch.h"
#include "SamplerComponent.h"

namespace buma
{
namespace scne
{

SamplerComponent::SamplerComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , filter_mode       { SAMPLER_FILTER_MODE_LINEAR }
    , wrap_mode         { SAMPLER_WRAP_MODE_WRAP }
{

}

SamplerComponent::~SamplerComponent()
{

}

SamplerComponent* SamplerComponent::Create(Scenes* _scenes)
{
    return new SamplerComponent(_scenes);
}

void SamplerComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE SamplerComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::ISamplerComponent;
}

void* SamplerComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                    return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                       return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::ISamplerComponent)                return static_cast<ISamplerComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::SamplerComponentImpl)    return static_cast<SamplerComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)        return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void SamplerComponent::SetSamplerFilterMode(SAMPLER_FILTER_MODE _filter_mode)
{
    filter_mode = _filter_mode;
}

void SamplerComponent::SetSamplerWrapMode(SAMPLER_WRAP_MODE _wrap_mode)
{
    wrap_mode = _wrap_mode;
}

SAMPLER_FILTER_MODE SamplerComponent::GetSamplerFilterMode()
{
    return filter_mode;
}

SAMPLER_WRAP_MODE SamplerComponent::GetSamplerWrapMode()
{
    return wrap_mode;
}


}// namespace scne
}// namespace buma
