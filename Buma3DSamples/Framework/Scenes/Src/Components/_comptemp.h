#pragma once

namespace buma
{
namespace scne
{

class Component : public Component, public ScenesObjectImpl
{
protected:
    Component(Scenes* _scenes);
    ~Component();

public:
    static Component* Create(Scenes* _scenes)
    {
        return new Component(_scenes);
    }

    uint32_t           Release()                           override { return ScenesObjectImpl::Release(); }
    const char*        GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void               SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*           GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void               OnDestroy()                         override;
    SCENES_OBJECT_TYPE GetType()                     const override;
    void*              As(SCENES_OBJECT_TYPE _type)        override;



private:

};


}// namespace scne
}// namespace buma
