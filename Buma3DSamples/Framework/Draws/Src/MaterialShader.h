#pragma once

namespace buma
{
namespace draws
{

class MaterialShader
{
public:
    MaterialShader(DrawsInstance* _ins);
    ~MaterialShader();

    bool Init(DrawsMaterial* _material, const MATERIAL_SHADER& _shader);

    const std::string&              GetShaderCode() const { return hlsl_code; }
    const std::string&              GetName()       const { return name; }
    const SHADER_TYPE               GetType()       const { return type; }
    const buma3d::SHADER_STAGE_FLAG GetStageType()  const { return stage; }
    const std::vector<uint8_t>&     GetLibrary()    const { return library; }

private:
    bool CreateLibrary();

private:
    DrawsInstance*              ins;
    DrawsMaterial*              material;
    std::string                 name;
    std::string                 hlsl_code;
    SHADER_TYPE                 type;
    buma3d::SHADER_STAGE_FLAG   stage;
    std::vector<uint8_t>        library;

};


}// buma
}// draws
