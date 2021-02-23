#include "pch.h"
#include "TransformComponent.h"

namespace buma
{
namespace scne
{



TransformComponent::TransformComponent(Scenes* _scenes)
    : ScenesObjectImpl(_scenes)
    , is_dirty    { false }
    , translation { 0,0,0 }
    , rotation    { 0,0,0 }
    , scale       { 1,1,1 }
    , transform   { 1 }
{

}

TransformComponent::~TransformComponent()
{

}

TransformComponent* TransformComponent::Create(Scenes* _scenes)
{
    return new TransformComponent(_scenes);
}

void TransformComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE TransformComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::ITransformComponent;
}

void* TransformComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                   return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::ITransformComponent)          return static_cast<ITransformComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ValueComponentImpl)  return static_cast<TransformComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)    return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void TransformComponent::SetTranslation(glm::vec3& _translation)
{
    translation = _translation;
}

void TransformComponent::SetRotation(glm::vec3& _rotation)
{
    rotation = _rotation;
}

void TransformComponent::SetScale(glm::vec3& _scale)
{
    scale = _scale;
}

void TransformComponent::SetTransform(glm::mat4& _transform)
{
    transform = _transform;
}

const glm::mat4& TransformComponent::GetTransform()
{
    if (is_dirty)
        Update();
    return transform;
}

void TransformComponent::Update()
{
    transform =
        glm::translate(glm::mat4(1.f), translation) *
        glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z) *
        glm::scale(glm::mat4(1.f), scale);
    is_dirty = false;
}


}// namespace scne
}// namespace buma
