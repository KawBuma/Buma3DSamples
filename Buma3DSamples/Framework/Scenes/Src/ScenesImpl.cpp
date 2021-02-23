#include "pch.h"
#include "ScenesImpl.h"

#define DECLARE_TEMPLATE_SPECIALIZATION(T) template<> inline SCENES_OBJECT_TYPE GetScenesObjectType<T>() { return SCENES_OBJECT_TYPE::T; }
#define DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(T) template<> inline SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<T>() { return SCENES_OBJECT_TYPE_INTERNAL::T##Impl; }

namespace buma
{
namespace scne
{

DECLARE_TEMPLATE_SPECIALIZATION(IScenesObject);
DECLARE_TEMPLATE_SPECIALIZATION(IComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IScene);
DECLARE_TEMPLATE_SPECIALIZATION(INode);
DECLARE_TEMPLATE_SPECIALIZATION(IValueComponent);
DECLARE_TEMPLATE_SPECIALIZATION(ITransformComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IAABBComponent);
DECLARE_TEMPLATE_SPECIALIZATION(ICameraComponent);
DECLARE_TEMPLATE_SPECIALIZATION(ILightComponent);
DECLARE_TEMPLATE_SPECIALIZATION(ILightPropertyComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IMaterialComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IMaterialPropertyComponent);
DECLARE_TEMPLATE_SPECIALIZATION(ITextureComponent);
DECLARE_TEMPLATE_SPECIALIZATION(ISamplerComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IBufferComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IBufferViewComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IIndexLayoutComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IVertexLayoutComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IVertexElementComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IIndexBufferComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IVertexBufferComponent);
DECLARE_TEMPLATE_SPECIALIZATION(ISubmeshComponent);
DECLARE_TEMPLATE_SPECIALIZATION(IMeshComponent);

//DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(ScenesObject);
template<> inline SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<ScenesObjectImpl>() { return SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl; }

DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(Scene);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(Node);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(ValueComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(TransformComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(AABBComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(CameraComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(LightComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(LightPropertyComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(MaterialComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(MaterialPropertyComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(TextureComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(SamplerComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(BufferComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(BufferViewComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(IndexLayoutComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(VertexLayoutComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(VertexElementComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(IndexBufferComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(VertexBufferComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(SubmeshComponent);
DECLARE_TEMPLATE_SPECIALIZATION_INTERNAL(MeshComponent);


Scenes::Scenes()
{
}

Scenes::~Scenes()
{
}

Scenes* Scenes::CreateScenes()
{
    return new Scenes();
}

void Scenes::DestroyScenes(IScenes* _scenes)
{
    delete static_cast<Scenes*>(_scenes);
}

uint32_t Scenes::GetScenesSize()
{
    return (uint32_t)scenes.size();
}

IScene* Scenes::GetScenes(uint32_t _index)
{
    return scenes[_index].Get();
}

IScene* Scenes::CreateScene()
{
    return scenes.emplace_back(GetAs<Scene>(Scene::Create(this))).Get();
}

INode* Scenes::CreateNode()
{
    return Node::Create(this);
}

IValueComponent* Scenes::CreateValueComponent()
{
    return ValueComponent::Create(this);
}

ITransformComponent* Scenes::CreateTransformComponent()
{
    return TransformComponent::Create(this);
}

IAABBComponent* Scenes::CreateAABBComponent()
{
    return AABBComponent::Create(this);
}

ICameraComponent* Scenes::CreateCameraComponent()
{
    return CameraComponent::Create(this);
}

ILightComponent* Scenes::CreateLightComponent()
{
    return LightComponent::Create(this);
}

ILightPropertyComponent* Scenes::CreateLightPropertyComponent()
{
    return LightPropertyComponent::Create(this);
}

IMaterialComponent* Scenes::CreateMaterialComponent()
{
    return MaterialComponent::Create(this);
}

IMaterialPropertyComponent* Scenes::CreateMaterialPropertyComponent()
{
    return MaterialPropertyComponent::Create(this);
}

ITextureComponent* Scenes::CreateTextureComponent()
{
    return TextureComponent::Create(this);
}

ISamplerComponent* Scenes::CreateSamplerComponent()
{
    return SamplerComponent::Create(this);
}

IBufferComponent* Scenes::CreateBufferComponent()
{
    return BufferComponent::Create(this);
}

IBufferViewComponent* Scenes::CreateBufferViewComponent()
{
    return BufferViewComponent::Create(this);
}

IIndexLayoutComponent* Scenes::CreateIndexLayoutComponent()
{
    return IndexLayoutComponent::Create(this);
}

IVertexLayoutComponent* Scenes::CreateVertexLayoutComponent()
{
    return VertexLayoutComponent::Create(this);
}

IVertexElementComponent* Scenes::CreateVertexElementComponent()
{
    return VertexElementComponent::Create(this);
}

IIndexBufferComponent* Scenes::CreateIndexBufferComponent()
{
    return IndexBufferComponent::Create(this);
}

IVertexBufferComponent* Scenes::CreateVertexBufferComponent()
{
    return VertexBufferComponent::Create(this);
}

ISubmeshComponent* Scenes::CreateSubmeshComponent()
{
    return SubmeshComponent::Create(this);
}

IMeshComponent* Scenes::CreateMeshComponent()
{
    return MeshComponent::Create(this);
}

IScenes* CreateScenes()
{
    return Scenes::CreateScenes();
}

void DestroyScenes(IScenes* _scenes)
{
    return Scenes::DestroyScenes(_scenes);
}



}// namespace scne
}// namespace buma
