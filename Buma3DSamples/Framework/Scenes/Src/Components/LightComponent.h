#pragma once

namespace buma
{
namespace scne
{

class LightComponent : public ILightComponent, public ScenesObjectImpl
{
protected:
    LightComponent(Scenes* _scenes);
    ~LightComponent();

public:
    static LightComponent* Create(Scenes* _scenes);

    uint32_t           Release()                           override { return ScenesObjectImpl::Release(); }
    const char*        GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void               SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*           GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void               OnDestroy()                         override;
    SCENES_OBJECT_TYPE GetType()                     const override;
    void*              As(SCENES_OBJECT_TYPE _type)        override;

    LIGHT_TYPE                  SetLightType() override;
    const glm::vec3&            GetDir() override;
    const glm::vec4&            GetColor() override;

    void                        SetLihtType (LIGHT_TYPE _type) override;
    void                        SetDir  (const glm::vec3& _direction) override;
    void                        SetColor(const glm::vec4& _color) override;

    void                        AddCustomProperties    (ILightPropertyComponent* _property) override;
    void                        ReleaseCustomProperties(ILightPropertyComponent* _property) override;
    uint32_t                    GetCustomPropertiesSize() override;
    ILightPropertyComponent*    GetCustomProperties    (uint32_t _index) override;
    ILightPropertyComponent*    GetCustomProperties(const char* _name) override;

private:
    LIGHT_TYPE                                      light_type;
    glm::vec3                                       direction;
    glm::vec4                                       color;
    std::vector<ScopedRef<LightPropertyComponent>>  custom_properties;

};


}// namespace scne
}// namespace buma
