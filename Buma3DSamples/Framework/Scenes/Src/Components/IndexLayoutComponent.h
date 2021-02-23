#pragma once

namespace buma
{
namespace scne
{

class IndexLayoutComponent : public IIndexLayoutComponent, public ScenesObjectImpl
{
protected:
    IndexLayoutComponent(Scenes* _scenes);
    ~IndexLayoutComponent();

public:
    static IndexLayoutComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void                SetDataFormat(DATA_FORMAT _format) override;
    DATA_FORMAT         GetDataFormat() override;

private:
    DATA_FORMAT data_format;

};


}// namespace scne
}// namespace buma
