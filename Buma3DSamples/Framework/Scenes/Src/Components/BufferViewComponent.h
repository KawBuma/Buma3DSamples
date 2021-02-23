#pragma once

namespace buma
{
namespace scne
{

class BufferViewComponent : public IBufferViewComponent, public ScenesObjectImpl
{
protected:
    BufferViewComponent(Scenes* _scenes);
    ~BufferViewComponent();

public:
    static BufferViewComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void                SetBuffer(IBufferComponent* _buffer) override;
    void                SetOffset(uint64_t _offset_in_bytes) override;
    void                SetSize(uint64_t _size_in_bytes) override;
    IBufferComponent*   GetBuffer() override;
    uint64_t            GetOffset() override;
    uint64_t            GetSize() override;
    void*               GetData() override;

private:
    void UpdateDataPtr();

private:
    ScopedRef<BufferComponent>  buffer;
    uint64_t                    offset;
    uint64_t                    size;
    void*                       data;

};


}// namespace scne
}// namespace buma
