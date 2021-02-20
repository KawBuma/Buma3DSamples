#pragma once

namespace buma
{
namespace scne
{

class MaterialComponent : public IMaterialComponent, public ScenesObjectImpl
{
protected:
    MaterialComponent(Scenes* _scenes);
    ~MaterialComponent();

public:
    static MaterialComponent* Create(Scenes* _scenes);

    uint32_t                    Release()                           override { return ScenesObjectImpl::Release(); }
    const char*                 GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                        SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*                    GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                        OnDestroy()                         override;
    SCENES_OBJECT_TYPE          GetType()                     const override;
    void*                       As(SCENES_OBJECT_TYPE _type)        override;

    void                        SetAlphaMode(MATERIAL_ALPHA_MODE _alpha_mode) override;
    void                        SetAlphaCutoff(float _alpha_cutoff) override;
    void                        SetTwoSidedEnabled(bool _is_enabled) override;

    MATERIAL_ALPHA_MODE         GetAlphaMode() override;
    float                       GetAlphaCutoff(float _alpha_cutoff) override;
    bool                        GetTwoSidedEnabled(bool _is_enabled) override;

    void                        AddCustomProperties(IMaterialPropertyComponent* _property) override;
    void                        ReleaseCustomProperties(IMaterialPropertyComponent* _property) override;
    uint32_t                    GetMaterialPropertiesSize() override;
    IMaterialPropertyComponent* GetMaterialProperties(uint32_t _index) override;
    IMaterialPropertyComponent* GetMaterialProperties(const char* _str) override;

private:
    MATERIAL_ALPHA_MODE                                 alpha_mode;
    float                                               alpha_cutoff;
    float                                               is_enabled_two_sided;
    std::vector<ScopedRef<MaterialPropertyComponent>>   custom_properties;

};


}// namespace scne
}// namespace buma
