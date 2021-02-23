#pragma once

namespace buma
{
namespace scne
{

class BufferComponent : public IBufferComponent, public ScenesObjectImpl
{
protected:
    BufferComponent(Scenes* _scenes);
    ~BufferComponent();

public:
    static BufferComponent* Create(Scenes* _scenes);

    uint32_t            Release()                           override { return ScenesObjectImpl::Release(); }
    const char*         GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*            GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                OnDestroy()                         override;
    SCENES_OBJECT_TYPE  GetType()                     const override;
    void*               As(SCENES_OBJECT_TYPE _type)        override;

    void                Resize(uint64_t _size_in_bytes) override;
    uint64_t            GetSize() override;
    void*               GetData() override;
    void                SetData(uint64_t _offset, uint64_t _size, void* _src_data) override;

private:
    std::vector<uint8_t> data;

};


}// namespace scne
}// namespace buma
