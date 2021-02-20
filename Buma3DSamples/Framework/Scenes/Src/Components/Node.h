#pragma once

namespace buma
{
namespace scne
{

class Node : public INode, public ScenesObjectImpl
{
protected:
    Node(Scenes* _scenes);
    ~Node();

public:
    static Node* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    INode*              GetParent() override;

    INode*              AddChild() override;
    void                ReleaseChild(INode* _node) override;
    INode*              GetChildren(uint32_t _index) override;
    INode*              GetChildren(const char* _name) override;

    void                AddComponent    (IComponent* _component) override;
    void                ReleaseComponent(IComponent* _component) override;
    const void*         GetComponents(SCENES_OBJECT_TYPE _type) override;

    void SetParent(Node* _parent);

private:
    ScopedRef<Node>                                                                     parent;
    std::vector<ScopedRef<Node>>                                                        children;
    std::unordered_map<SCENES_OBJECT_TYPE, std::unique_ptr<std::vector<IComponent*>>>   components;

};


}// namespace scne
}// namespace buma
