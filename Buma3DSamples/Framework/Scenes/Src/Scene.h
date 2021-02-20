#pragma once

namespace buma
{
namespace scne
{

class Node;

class Scene : public IScene, public ScenesObjectImpl
{
protected:
    Scene(Scenes* _scenes);
    ~Scene();

public:
    static Scene* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    INode*              GetRoot() override;

private:
    ScopedRef<Node> root_node;

};


}// namespace scne
}// namespace buma
