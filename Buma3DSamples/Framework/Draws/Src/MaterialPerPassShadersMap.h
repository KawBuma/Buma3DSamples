#pragma once

namespace buma
{
namespace draws
{

class MaterialPerPassShadersMap
{
public:
    MaterialPerPassShadersMap(DrawsInstance* _ins);
    ~MaterialPerPassShadersMap();

    bool Init(DrawsMaterial* _material);

    const MaterialPerPassShader* Get(SHADER_STAGE _stage, RENDER_PASS_TYPE _pass) const;

private:
    using ShaderPerStages = std::array<std::shared_ptr<MaterialPerPassShader>, SHADER_STAGE_NUM_STAGES>;
    using ShaderPerPasses = std::array<ShaderPerStages                       , RENDER_PASS_TYPE_NUM_TYPES>;
    DrawsInstance*  ins;
    ShaderPerPasses modules;

};


}// namespace draws
}// namespace buma
