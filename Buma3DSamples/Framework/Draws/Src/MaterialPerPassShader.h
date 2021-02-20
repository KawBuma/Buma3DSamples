#pragma once

namespace buma
{
namespace draws
{

class MaterialPerPassShader
{
public:
    MaterialPerPassShader(DrawsInstance* _ins);
    ~MaterialPerPassShader();

    bool Init(DrawsMaterial* _material, SHADER_STAGE _stage, const std::shared_ptr<MaterialShader>& _shader, RENDER_PASS_TYPE _pass_type);



    SHADER_STAGE                                      GetStage()        const { return stage; }
    DrawsMaterial*                                    GetMaterial()     const { return material; }
    const std::shared_ptr<MaterialShader>&            GetShader()       const { return shader; }
    RENDER_PASS_TYPE                                  GetPassType()     const { return pass_type; }
    const buma3d::util::Ptr<buma3d::IShaderModule>&   GetShaderModule() const { return shader_module; }

private:
    void PerpareLinkDesc(buma::shader::LIBRARY_LINK_DESC& _desc);
    void ComposeModules(buma::shader::LIBRARY_LINK_DESC& _desc);

private:
    DrawsInstance*                              ins;
    DrawsMaterial*                              material;
    std::shared_ptr<MaterialShader>             shader;
    SHADER_STAGE                                stage;
    RENDER_PASS_TYPE                            pass_type;
    buma3d::util::Ptr<buma3d::IShaderModule>    shader_module;

};


}// buma
}// draws
