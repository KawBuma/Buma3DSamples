#pragma once

namespace buma
{
namespace scne
{

class SamplerComponent : public ISamplerComponent, public ScenesObjectImpl
{
protected:
    SamplerComponent(Scenes* _scenes);
    ~SamplerComponent();

public:
    static SamplerComponent* Create(Scenes* _scenes);

    uint32_t                Release()                           override { return ScenesObjectImpl::Release(); }
    const char*             GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                    SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*                GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                    OnDestroy()                         override;
    SCENES_OBJECT_TYPE      GetType()                     const override;
    void*                   As(SCENES_OBJECT_TYPE _type)        override;

    void                    SetSamplerFilterMode(SAMPLER_FILTER_MODE _filter_mode) override;
    void                    SetSamplerWrapMode(SAMPLER_WRAP_MODE _wrap_mode) override;

    SAMPLER_FILTER_MODE     GetSamplerFilterMode() override;
    SAMPLER_WRAP_MODE       GetSamplerWrapMode() override;

private:
    SAMPLER_FILTER_MODE     filter_mode;
    SAMPLER_WRAP_MODE       wrap_mode;

};


}// namespace scne
}// namespace buma
