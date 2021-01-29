#pragma once

namespace buma
{
namespace draws
{

class MaterialShader
{
public:
    MaterialShader();
    ~MaterialShader();

    void Init(SHADER_TYPE _type, const char* _hlsl_code);

    const std::string&              GetShaderCode() const { return hlsl_code; }
    const SHADER_TYPE               GetType()       const { return type; }
    const buma3d::SHADER_STAGE_FLAG GetStageType()  const { return stage; }

private:
    std::string                 hlsl_code;
    SHADER_TYPE                 type;
    buma3d::SHADER_STAGE_FLAG   stage;

};


}// buma
}// draws
