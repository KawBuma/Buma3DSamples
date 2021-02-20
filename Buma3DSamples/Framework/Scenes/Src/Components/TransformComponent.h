#pragma once

namespace buma
{
namespace scne
{

class TransformComponent : public ITransformComponent, public ScenesObjectImpl
{
protected:
    TransformComponent(Scenes* _scenes);
    ~TransformComponent();

public:
    static TransformComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void                SetTranslation(glm::vec3& _translation) override;
    void                SetRotation   (glm::vec3& _rotation) override;
    void                SetScale      (glm::vec3& _scale) override;
    void                SetTransform  (glm::mat4& _transform) override;
    const glm::mat4&    GetTransform() override;

private:
    void Update();

private:
    bool        is_dirty;
    glm::vec3   translation;
    glm::vec3   rotation;
    glm::vec3   scale;
    glm::mat4   transform;

};


}// namespace scne
}// namespace buma
