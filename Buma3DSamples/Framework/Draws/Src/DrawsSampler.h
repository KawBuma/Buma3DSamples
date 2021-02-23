#pragma once

namespace buma
{
namespace draws
{

class DrawsSampler : public IDrawsSampler
{
protected:
    DrawsSampler();
    ~DrawsSampler();

    bool Init(DrawsInstance* _ins, const SAMPLER_CREATE_DESC& _desc);
    bool CreateSampler();

public:
    static bool Create(DrawsInstance* _ins, const SAMPLER_CREATE_DESC& _desc, IDrawsSampler** _dst);

    void     AddRef()  override;
    uint32_t Release() override;

public:
    void        SetName(const char* _name) override;
    const char* GetName() override;

private:
    std::atomic_uint32_t                    ref_count;
    DrawsInstance*                          ins;
    SAMPLER_FILTER_MODE                     min_filter;
    SAMPLER_FILTER_MODE                     mag_filter;
    SAMPLER_WRAP_MODE                       wrap_mode;
    SAMPLER_BOARDER_TYPE                    boarder_type;
    uint32_t                                max_anisotropy;
    buma3d::util::Ptr<buma3d::ISamplerView> sampler;

};


}// namespace buma
}// namespace draws
