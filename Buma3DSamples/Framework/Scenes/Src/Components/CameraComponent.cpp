#include "pch.h"
#include "CameraComponent.h"

namespace buma
{
namespace scne
{

CameraComponent::CameraComponent(Scenes* _scenes)
    : ScenesObjectImpl(_scenes)
    , aspect_ratio  { 1.f }
    , fovy          { 1.f }
    , nearz         { 0.1f }
    , farz          { 1.1f }
    , projection    { glm::perspective(1.f, 1.f, 0.1f, 1.f) }
    , transform     {}
{

}

CameraComponent::~CameraComponent()
{

}

CameraComponent* CameraComponent::Create(Scenes* _scenes)
{
    return new CameraComponent(_scenes);
}

void CameraComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE CameraComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::ICameraComponent;
}

void* CameraComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                   return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::ICameraComponent)             return static_cast<ICameraComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::CameraComponentImpl) return static_cast<CameraComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)    return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void CameraComponent::SetAspectRatio(float _aspect_ratio)
{
    aspect_ratio = _aspect_ratio;
}

void CameraComponent::SetFovY(float _fovy)
{
    fovy = _fovy;
}

void CameraComponent::SetNearZ(float _near)
{
    nearz = _near;
}

void CameraComponent::SetFarZ(float _far)
{
    farz = _far;
}

float CameraComponent::GetAspectRatio()
{
    return aspect_ratio;
}

float CameraComponent::GetFovY()
{
    return fovy;
}

float CameraComponent::GetNearZ()
{
    return nearz;
}

float CameraComponent::GetFarZ()
{
    return farz;
}

const glm::mat4& CameraComponent::GetProjection()
{
    if (is_dirty)
        UpdateProjection();
    return projection;
}

void CameraComponent::SetTransformComponent(ITransformComponent* _transform)
{
    transform = GetAs<TransformComponent>(_transform);
}

ITransformComponent* CameraComponent::GetTransformComponent()
{
    return transform.Get();
}

void CameraComponent::UpdateProjection()
{
    projection = glm::perspective(fovy, aspect_ratio, nearz, farz);
    is_dirty = false;
}


}// namespace scne
}// namespace buma
