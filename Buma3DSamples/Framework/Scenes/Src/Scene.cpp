#include "pch.h"
#include "Scene.h"

namespace buma
{
namespace scne
{

Scene::Scene(Scenes* _scenes)
    : ScenesObjectImpl(_scenes)
{
    root_node = GetAs<Node>(scenes->CreateNode());
    root_node->SetName("__root__");
}

Scene::~Scene()
{

}

Scene* Scene::Create(Scenes* _scenes)
{
    return new Scene(_scenes);
}

void Scene::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE Scene::GetType() const
{
    return SCENES_OBJECT_TYPE::IScene;
}

void* Scene::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IScene)                       return static_cast<IScene*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IScene)                       return static_cast<IScene*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::SceneImpl)           return static_cast<IScene*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)    return static_cast<IScene*>(this);

    return nullptr;
}

INode* Scene::GetRoot()
{
    return root_node.Get();
}


}// namespace scne
}// namespace buma
