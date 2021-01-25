#pragma once

namespace buma
{
namespace scne
{

class TextureComponent : public ITextureComponent, public ScenesObjectImpl
{
protected:
    TextureComponent(Scenes* _scenes);
    ~TextureComponent();

public:
    static TextureComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void        SetFilename(const char* _filename) override;
    const char* GetFilename() override;

private:
    std::string filename;

};


}// namespace scne
}// namespace buma
