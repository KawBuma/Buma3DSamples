#pragma once

namespace buma
{
namespace scne
{

class CameraComponent : public ICameraComponent, public ScenesObjectImpl
{
protected:
    CameraComponent(Scenes* _scenes);
    ~CameraComponent();

public:
    static CameraComponent* Create(Scenes* _scenes);

    uint32_t                Release()                           override { return ScenesObjectImpl::Release(); }
    const char*             GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                    SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*                GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                    OnDestroy()                         override;
    SCENES_OBJECT_TYPE      GetType()                     const override;
    void*                   As(SCENES_OBJECT_TYPE _type)        override;

    void                    SetAspectRatio  (float _aspect_ratio) override;
    void                    SetFovY         (float _fovy) override;
    void                    SetNearZ        (float _near) override;
    void                    SetFarZ         (float _far) override;

    float                   GetAspectRatio() override;
    float                   GetFovY() override;
    float                   GetNearZ() override;
    float                   GetFarZ() override;
    const glm::mat4&        GetProjection() override;

   void                     SetTransformComponent(ITransformComponent* _transform) override;
   ITransformComponent*     GetTransformComponent() override;

private:
    void UpdateProjection();

private:
    float                           aspect_ratio;
    float                           fovy;
    float                           nearz;
    float                           farz;
    bool                            is_dirty;
    glm::mat4                       projection;
    ScopedRef<TransformComponent>   transform;

};


}// namespace scne
}// namespace buma

