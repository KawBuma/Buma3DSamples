#include "pch.h"
#include "MaterialPerPassShader.h"

namespace b = buma3d;

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

inline const char* GetShaderNameFromPassType(RENDER_PASS_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::RENDER_PASS_TYPE_PRE_DEPTH : return "DrawsPrePass.hlsl";
    case buma::draws::RENDER_PASS_TYPE_BASE      : return "DrawsBasePass";
    case buma::draws::RENDER_PASS_TYPE_LIGHTING  : return "DrawsLighting";
    default:
        return "";
    }
}

inline const char* GetShaderTypeName(SHADER_STAGE _type)
{
    switch (_type)
    {
    case buma::draws::SHADER_STAGE_VS : "_VS.hlsl";
    case buma::draws::SHADER_STAGE_PS : "_PS.hlsl";
    default:
        return ".hlsl";
    }
}

inline std::string GetShaderFileName(RENDER_PASS_TYPE _pass_type, SHADER_STAGE _shader_type)
{
    return std::string(GetShaderNameFromPassType(_pass_type)) + GetShaderTypeName(_shader_type);
}

inline shader::SHADER_STAGE GetShaderStageForCompile(buma::draws::SHADER_STAGE _type)
{
    switch (_type)
    {
    case buma::draws::SHADER_STAGE_VS: return shader::SHADER_STAGE_PIXEL;
    case buma::draws::SHADER_STAGE_PS: return shader::SHADER_STAGE_VERTEX;
    default:
        assert(false && __FUNCTION__);
        return shader::SHADER_STAGE(-1);
    }
}


}// namespace /*anonymous*/

MaterialPerPassShader::MaterialPerPassShader(DrawsInstance* _ins)
    : ins           { _ins }
    , material      {}
    , shader        {}
    , pass_type     {}
    , shader_module {}
{

}

MaterialPerPassShader::~MaterialPerPassShader()
{
}

bool MaterialPerPassShader::Init(DrawsMaterial* _material, SHADER_STAGE _stage, const std::shared_ptr<MaterialShader>& _shader, RENDER_PASS_TYPE _pass_type)
{
    material  = _material;
    shader    = _shader;
    pass_type = _pass_type;
    stage     = _stage;

    /*
    DrawsInstance側で用意するシェーダ
              | pre_pass    | base_pass   | trans_pass  |
      opaque  | d{(vs)}     | d{(vs)}     |             |
      masked  | d{(vs)}     | d{(vs)}     |             |
      trans   |             |             | d{(vs),ps}  |

    ユーザー側で用意するシェーダ
              | pre_pass    | base_pass   | trans_pass  |
      opaque  | x{vs}       | x{vs,ps}    |             |
      masked  | x{vs,psm}   | x{vs,ps}    |             |
      trans   |             |             | x{vs,ps}    |
    */

    shader::LIBRARY_LINK_DESC desc{};
    PerpareLinkDesc(desc);
    ComposeModules(desc);

    auto dr = ins->GetDR();

    // コアライブラリとユーザーコードをリンク
    std::vector<uint8_t> bytecode;
    dr->GetShaderLoader()->LinkLibrary(desc, &bytecode);
    assert(!bytecode.empty());
    if (bytecode.empty())
        return false;

    b::SHADER_MODULE_DESC module_desc{};
    module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
    module_desc.bytecode.bytecode_length = bytecode.size();
    module_desc.bytecode.shader_bytecode = bytecode.data();
    auto bmr = dr->GetDevice()->CreateShaderModule(module_desc, &shader_module);
    BMR_RET_IF_FAILED(bmr);

    return true;
}

void MaterialPerPassShader::PerpareLinkDesc(buma::shader::LIBRARY_LINK_DESC& _desc)
{
    _desc.options.pack_matrices_in_row_major = false;       // Experimental: Decide how a matrix get packed
    _desc.options.enable16bit_types          = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    _desc.options.enable_debug_info          = false;       // Embed debug info into the binary
    _desc.options.disable_optimizations      = false;       // Force to turn off optimizations. Ignore optimizationLevel below.
    _desc.options.optimization_level         = 3; // 0 to 3, no optimization to most optimization
    _desc.options.shader_model               = { 6, 2 };
}

void MaterialPerPassShader::ComposeModules(buma::shader::LIBRARY_LINK_DESC& _desc)
{
    assert(material->GetBlendMode() == MATERIAL_BLEND_MODE_MASKED &&
           pass_type                == RENDER_PASS_TYPE_PRE_DEPTH &&
           stage                    == SHADER_STAGE_PS &&
           shader                   != nullptr);

    auto&& core_module = ins->GetCoreShaderModules().GetPerPass(pass_type)->Find(stage);
    _desc.link.modules.emplace_back(core_module);
    _desc.link.modules.emplace_back(&shader->GetModuleDesc());
    _desc.link.entry_point  = "main";
    _desc.link.stage        = GetShaderStageForCompile(stage);
}


}// buma
}// draws
