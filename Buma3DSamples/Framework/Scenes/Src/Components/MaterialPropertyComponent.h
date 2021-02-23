#pragma once

namespace buma
{
namespace scne
{

class MaterialPropertyComponent : public IMaterialPropertyComponent, public ScenesObjectImpl
{
protected:
    MaterialPropertyComponent(Scenes* _scenes);
    ~MaterialPropertyComponent();

public:
    static MaterialPropertyComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void                AddValueComponent(IValueComponent* _component) override;
    void                ReleaseValueComponent(IValueComponent* _component) override;
    uint32_t            GetValueComponentSize() override;
    IValueComponent*    GetValueComponent(uint32_t _index) override;
    IValueComponent*    GetValueComponent(const char* _str) override;

    void                SetTextureComponent(ITextureComponent* _component) override;
    void                SetSamplerComponent(ISamplerComponent* _component) override;
    ITextureComponent*  GetTextureComponent() override;
    ISamplerComponent*  GetSamplerComponent() override;

private:
    std::vector<ScopedRef<ValueComponent>>  value_components;
    ScopedRef<TextureComponent>             texture_component;
    ScopedRef<SamplerComponent>             sampler_component;

};


}// namespace scne
}// namespace buma
