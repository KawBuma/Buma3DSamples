#pragma once

namespace buma
{
namespace scne
{

class LightPropertyComponent : public ILightPropertyComponent, public ScenesObjectImpl
{
protected:
    LightPropertyComponent(Scenes* _scenes);
    ~LightPropertyComponent();

public:
    static LightPropertyComponent* Create(Scenes* _scenes);

    uint32_t           Release()                           override { return ScenesObjectImpl::Release(); }
    const char*        GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void               SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*           GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void               OnDestroy()                         override;
    SCENES_OBJECT_TYPE GetType()                     const override;
    void*              As(SCENES_OBJECT_TYPE _type)        override;

    void        SetValueType(VALUE_TYPE _type) override;
    void        Resize(uint32_t _size_in_bytes) override;
    void        SetData(uint32_t _offset_in_bytes, uint32_t _src_size_in_bytes, const void* _src) override;

    VALUE_TYPE  GetValueType() override;
    uint32_t    GetSize() override;
    const void* GetData() override;

private:
    VALUE_TYPE              value_type;
    std::vector<uint8_t>    data;

};


}// namespace scne
}// namespace buma
