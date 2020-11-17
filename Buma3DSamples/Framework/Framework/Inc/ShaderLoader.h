#pragma once

namespace buma
{
namespace shader
{

struct SHADER_MODEL
{
    uint8_t major_ver : 6;
    uint8_t minor_ver : 2;

    uint32_t FullVersion() const noexcept { return (major_ver << 2) | minor_ver; }

    bool operator< (const SHADER_MODEL& other) const noexcept { return this->FullVersion() < other.FullVersion(); }
    bool operator==(const SHADER_MODEL& other) const noexcept { return this->FullVersion() == other.FullVersion(); }
    bool operator> (const SHADER_MODEL& other) const noexcept { return other < *this; }
    bool operator<=(const SHADER_MODEL& other) const noexcept { return (*this < other) || (*this == other); }
    bool operator>=(const SHADER_MODEL& other) const noexcept { return (*this > other) || (*this == other); }
};

struct OPTIONS
{
    bool         packMatricesInRowMajor     = true;     // 実験的: 行列をどのようにパックするかを決定する                                   Experimental: Decide how a matrix get packed
    bool         enable16bitTypes           = false;    // half,uint16_tなどの16ビットタイプを有効にします。 シェーダーモデル6.2以降が必要  Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    bool         enableDebugInfo            = false;    // デバッグ情報をバイナリに埋め込む                                                 Embed debug info into the binary
    bool         disableOptimizations       = false;    // 最適化を強制的にオフにします。 以下のoptimizationLevelは無視してください。       Force to turn off optimizations. Ignore optimizationLevel below.

    uint32_t     optimizationLevel          = 3;        // 0 to 3, no optimization to most optimization
    SHADER_MODEL shaderModel                = { 6, 0 };

    uint32_t     shiftAllTexturesBindings   = 0;
    uint32_t     shiftAllSamplersBindings   = 0;
    uint32_t     shiftAllCBuffersBindings   = 0;
    uint32_t     shiftAllUABuffersBindings  = 0;
};

enum SHADER_STAGE : uint32_t
{
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_PIXEL,
    SHADER_STAGE_GEOMETRY,
    SHADER_STAGE_HULL,
    SHADER_STAGE_DOMAIN,
    SHADER_STAGE_COMPUTE,

    SHADER_STAGE_NUM_STAGES,
};

struct SHADER_DEFINES
{
    const char* def_name;
    const char* def_value;
};

struct LOAD_SHADER_DESC
{
    const char*                     filename;
    const char*                     entry_point;
    SHADER_STAGE                    stage;
    std::vector<SHADER_DEFINES>     defines;
    OPTIONS                         options;
};

class ShaderLoader
{
public:
    ShaderLoader(INTERNAL_API_TYPE _type);
    ~ShaderLoader() {}

    void LoadShaderFromBinary(const char* _filename, std::vector<uint8_t>* _dst);
    void LoadShaderFromHLSL(const LOAD_SHADER_DESC& _desc, std::vector<uint8_t>* _dst);
    static void LoadShaderFromHLSL(INTERNAL_API_TYPE _type, const LOAD_SHADER_DESC& _desc, std::vector<uint8_t>* _dst);

private:
    INTERNAL_API_TYPE type;

};



}// namespace shader
}// namespace buma3d
