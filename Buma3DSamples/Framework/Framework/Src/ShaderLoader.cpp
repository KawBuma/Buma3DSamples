#include "pch.h"
#include "ShaderConductor/ShaderConductor.hpp"
#include "ShaderLoader.h"
#include <fstream>
#include <memory>
#include <map>

#pragma comment(lib, "ShaderConductor.lib")

namespace buma
{
namespace shader
{

ShaderLoader::ShaderLoader(INTERNAL_API_TYPE _type)
    : type{ _type }
{

}

void ShaderLoader::LoadShaderFromBinary(const char* _filename, std::vector<uint8_t>* _dst)
{
    std::ifstream file(_filename, std::ios::in | std::ios::binary);
    std::string source;
    source = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    *_dst = std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(source.data())
                                 , reinterpret_cast<const uint8_t*>(source.data()) + source.size());
}

void ShaderLoader::LoadShaderFromHLSL(const LOAD_SHADER_DESC& _desc, std::vector<uint8_t>* _dst)
{
    namespace SC = ShaderConductor;
    using SCC = ShaderConductor::Compiler;

    std::string source;
    {
        std::ifstream file(_desc.filename, std::ios::in);
        source = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
    
    std::vector<SC::MacroDefine> defines(_desc.defines.size());
    {
        size_t count = 0;
        for (auto& i : _desc.defines)
            defines[count++] = { i.def_name, i.def_value };
    }

    SCC::TargetDesc tgt{};
    {
        tgt.asModule = false;
        tgt.version = nullptr;
    }

    SCC::Options opt{};
    {
        opt.packMatricesInRowMajor      = _desc.options.packMatricesInRowMajor;
        opt.enable16bitTypes            = _desc.options.enable16bitTypes;
        opt.enableDebugInfo             = _desc.options.enableDebugInfo;
        opt.disableOptimizations        = _desc.options.disableOptimizations;
        //opt.invertYCoordinate           = _desc.options.invertYCoordinate;
        opt.optimizationLevel           = (int)_desc.options.optimizationLevel;
        opt.shaderModel.major_ver       = _desc.options.shaderModel.major_ver;
        opt.shaderModel.minor_ver       = _desc.options.shaderModel.minor_ver;
        opt.shiftAllTexturesBindings    = (int)_desc.options.shiftAllTexturesBindings;
        opt.shiftAllSamplersBindings    = (int)_desc.options.shiftAllSamplersBindings;
        opt.shiftAllCBuffersBindings    = (int)_desc.options.shiftAllCBuffersBindings;
        opt.shiftAllUABuffersBindings   = (int)_desc.options.shiftAllUABuffersBindings;
    }

    switch (type)
    {
    case buma::INTERNAL_API_TYPE_D3D12:
        tgt.language = SC::ShadingLanguage::Dxil;
        break;

    case buma::INTERNAL_API_TYPE_VULKAN:
        opt.invertYCoordinate = true;
        tgt.language          = SC::ShadingLanguage::SpirV;
        break;

    default:
        break;
    }

    SCC::SourceDesc src{};
    {
        src.source              = source.data();
        src.fileName            = _desc.filename;
        src.entryPoint          = _desc.entry_point;
        src.stage               = static_cast<SC::ShaderStage>(_desc.stage);
        src.defines             = defines.data();
        src.numDefines          = (uint32_t)defines.size();
        src.loadIncludeCallback = nullptr;
    }

    auto result = ShaderConductor::Compiler::Compile(src, opt, tgt);
    if (result.hasError)
    {
        std::cout << (const char*)result.errorWarningMsg.Data() << std::endl;
        return;
    }

    *_dst = std::vector<uint8_t>(  reinterpret_cast<const uint8_t*>(result.target.Data())
                                 , reinterpret_cast<const uint8_t*>(result.target.Data()) + result.target.Size());

}

}// namespace shader
}// namespace buma3d
