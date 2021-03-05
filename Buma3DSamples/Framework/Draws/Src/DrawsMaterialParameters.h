#pragma once

namespace buma
{
namespace draws
{

class DrawsMaterialParameters : public IDrawsMaterialParameters
{
protected:
    DrawsMaterialParameters();
    ~DrawsMaterialParameters();

    bool Init(DrawsInstance* _ins, const MATERIAL_PARAMETERS_CREATE_DESC& _desc);
    bool CreateDescriptorSetLayout();

public:
    static bool Create(DrawsInstance* _ins, const MATERIAL_PARAMETERS_CREATE_DESC& _desc, IDrawsMaterialParameters** _dst);

    void     AddRef()  override;
    uint32_t Release() override;

public:
    const buma3d::util::Ptr<buma3d::IDescriptorSetLayout>& GetDescriptorSetLayout() const { return layout; }

private:
    std::atomic_uint32_t                                        ref_count;
    DrawsInstance*                                              ins;
    MATERIAL_PARAMETER_TYPE                                     type;
    std::unique_ptr<std::vector<MATERIAL_VALUE_PARAMETER>>      value;
    std::unique_ptr<std::vector<MATERIAL_SAMPLER_PARAMETER>>    sampler;
    std::unique_ptr<std::vector<MATERIAL_TEXTURE_PARAMETER>>    texture;
    buma3d::util::Ptr<buma3d::IDescriptorSetLayout>             layout;

};


}// namespace buma
}// namespace draws
