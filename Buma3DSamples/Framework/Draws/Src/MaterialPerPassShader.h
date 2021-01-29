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

    bool Init(DrawsMaterial* _material, std::shared_ptr<MaterialShader>& _shader, RENDER_PASS_TYPE _pass_type);

    DrawsMaterial*                                    GetMaterial    () const { return material;      }
    const std::shared_ptr<MaterialShader>&            GetShader      () const { return shader;        }
    RENDER_PASS_TYPE                                  GetPassType    () const { return pass_type;     }
    const buma3d::util::Ptr<buma3d::IShaderModule>&   GetShaderModule() const { return shader_module; }

private:
    DrawsInstance*                              ins;
    DrawsMaterial*                              material;
    std::shared_ptr<MaterialShader>             shader;
    RENDER_PASS_TYPE                            pass_type;
    buma3d::util::Ptr<buma3d::IShaderModule>    shader_module;

};


}// buma
}// draws
