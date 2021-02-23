#pragma once

namespace buma
{
namespace scne
{

class VertexElementComponent : public IVertexElementComponent, public ScenesObjectImpl
{
protected:
    VertexElementComponent(Scenes* _scenes);
    ~VertexElementComponent();

public:
    static VertexElementComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void                SetDataFormat(DATA_FORMAT _format) override;
    DATA_FORMAT         GetDataFormat() override;

    uint32_t            GetSizeOfElements() override;
    void                SetComponentCount(uint32_t _count) override;
    uint32_t            GetComponentCount() override;

private:
    DATA_FORMAT data_format;
    uint32_t    component_count;

};


}// namespace scne
}// namespace buma
