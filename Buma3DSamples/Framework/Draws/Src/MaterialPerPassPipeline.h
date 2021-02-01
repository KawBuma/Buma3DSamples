#pragma once

namespace buma
{
namespace draws
{

class MaterialPerPassPipeline
{
public:
    MaterialPerPassPipeline(DrawsInstance* _ins);
    ~MaterialPerPassPipeline();

    bool Init(DrawsMaterial* _material, RENDER_PASS_TYPE _pass_type);
    const buma3d::util::Ptr<buma3d::IPipelineState>&    GetPipeline()       const { return pipeline; }
    RENDER_PASS_TYPE                                    GetRenderPassType() const { return pass_type; }

private:
    DrawsInstance*                                                              ins;
    DrawsMaterial*                                                              material;
    RENDER_PASS_TYPE                                                            pass_type;
    buma3d::util::Ptr<buma3d::IPipelineState>                                   pipeline;
    std::array<std::shared_ptr<MaterialPerPassShader>, SHADER_STAGE_NUM_STAGES> shader_modules;

};


}// namespace draws
}// namespace buma
