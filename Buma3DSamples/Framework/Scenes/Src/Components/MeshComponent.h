#pragma once

namespace buma
{
namespace scne
{

class MeshComponent : public IMeshComponent, public ScenesObjectImpl
{
protected:
    MeshComponent(Scenes* _scenes);
    ~MeshComponent();

public:
    static MeshComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void                AddSubmesh    (ISubmeshComponent* _submesh) override;
    void                ReleaseSubmesh(ISubmeshComponent* _submesh) override;

    uint32_t            GetSubmeshSize() override;
    ISubmeshComponent*  GetSubmeshs(uint32_t _index) override;

private:
    std::vector<ScopedRef<SubmeshComponent>> submeshes;

};


}// namespace scne
}// namespace buma
