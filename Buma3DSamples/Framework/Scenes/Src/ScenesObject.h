#pragma once

namespace buma
{
namespace scne
{

class Scenes;

class ScenesObjectImpl
{
protected:
    ScenesObjectImpl(Scenes* _scenes);
    virtual ~ScenesObjectImpl();

public:
    virtual void    OnDestroy() = 0;

    uint32_t        AddRef();
    uint32_t        Release();
    const char*     GetName() const;
    void            SetName(const char* _name);
    IScenes*        GetScenes();

    // uint32_t           Release()                           override { return ScenesObjectImpl::Release(); }
    // const char*        GetName()                     const override { return ScenesObjectImpl::GetName(); }
    // void               SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    // IScenes*           GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    // void               OnDestroy()                         override;
    // SCENES_OBJECT_TYPE GetType()                     const override;
    // void*              As(SCENES_OBJECT_TYPE _type)        override;

protected: 
    Scenes*                         scenes;
    uint32_t                        ref_count;
    std::unique_ptr<std::string>    name;

};


}// namespace scne
}// namespace buma
