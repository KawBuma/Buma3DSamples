#include "pch.h"
#include "Node.h"

namespace buma
{
namespace scne
{

Node::Node(Scenes* _scenes)
    : ScenesObjectImpl(_scenes)
    , children      {}
    , components    {}
{

}

Node::~Node()
{

}

Node* Node::Create(Scenes* _scenes)
{
    return new Node(_scenes);
}

void Node::OnDestroy()
{
    for (auto& [type, by_type] : components)
    {
        if (!by_type)
            continue;

        for (auto& i : *by_type)
            i->Release();
    }
    
    delete this;
}

SCENES_OBJECT_TYPE Node::GetType() const
{
    return SCENES_OBJECT_TYPE::INode;
}

void* Node::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)             return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::INode)                     return static_cast<INode*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::NodeImpl)         return static_cast<Node*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl) return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

INode* Node::GetParent()
{
    return parent.Get();
}

INode* Node::AddChild()
{
    return children.emplace_back(GetAs<Node>(scenes->CreateNode())).Get();
}

void Node::ReleaseChild(INode* _node)
{
    EraseElem(children, _node);
}

INode* Node::GetChildren(uint32_t _index)
{
    return children[_index].Get();
}

INode* Node::GetChildren(const char* _name)
{
    auto it_find = FindByName(children, _name);
    if (it_find != children.end())
        return it_find->Get();

    return nullptr;
}

void Node::AddComponent(IComponent* _component)
{
    auto&& c = components[_component->GetType()];
    if (!c)
        c = std::make_unique<std::vector<IComponent*>>();

    GetAs<ScenesObjectImpl>(_component)->AddRef();
    c->emplace_back(_component);
}

void Node::ReleaseComponent(IComponent* _component)
{
    auto c = components[_component->GetType()].get();
    if (!c)
        return;

    auto f = std::find(c->begin(), c->end(), _component);
    if (f == c->end())
        return;

    (*f)->Release();
    c->erase(f);
}

const void* Node::GetComponents(SCENES_OBJECT_TYPE _type)
{
    return components[_type].get();
}

void Node::SetParent(Node* _parent)
{
    parent = _parent;
}


}// namespace scne
}// namespace buma
