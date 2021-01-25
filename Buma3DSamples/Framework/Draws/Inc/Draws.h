#pragma once
#include <cstdint>

#include "Buma3D.h"

// to use a clip space between 0 to 1.
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// For DirectX, Metal, Vulkan
#define GLM_FORCE_LEFT_HANDED 
#include <glm/glm.hpp>
#include <glm/matrix.hpp>


namespace buma
{
namespace draws
{

#define TO_FLAGS(T) inline constexpr T T##S(uint32_t _flags) { return (T)_flags; }

#pragma region declaration

using VertPositionT   = glm::vec4;
using VertNormatT     = glm::vec4;
using VertTangentT    = glm::vec4;
using VertTexcoordT   = glm::vec2;


struct IDrawsInstance;

struct IDrawsObject;

struct IDrawsResource;
struct IDrawsSampler;
struct IDrawsTexture;
struct IDrawsBuffer;
struct IDrawsVertexBuffer;
struct IDrawsIndexBuffer;

 // マテリアルディスクリプタのビルドはマテリアルインスタンスの作成時のみ
 // マテリアル毎にディスクリプタセットを保有
 // マテリアル毎にマテリアル定数のバッファのプールを保有
 // 各マテリアルインスタンスはマテリアル定数のバッファへのインデックスを保有
 // 描画時にプリミティブからマテリアル定数のバッファへのインデックスを摂取
struct IDrawsMaterialParameters;    // declare parameters value,sampler,texture
struct IDrawsMaterial;              // represents material 
struct IDrawsMaterialConstant;      // set parameters value
struct IDrawsMaterialInstance;      // set parameters sampler,texture
struct IDrawsMeshData;
struct IDrawsMesh;
struct IDrawsStaticMesh;

/*
                        *** BASE PASS SHADER SIGNATURE ***
// **RESERVED**
ConstantBuffer<SCENE_DATA>  scene_data             : register(b0, space0);
ConstantBuffer<VIEW_DATA>   view_data              : register(b1, space0);
Texture2D<float4>           primitive_buffer       : register(t0, space0);
ByteAddressBuffer           primitive_index_buffer : register(t1, space0);

// **STATIC/IMMUTABLE_SAMPLER**
// MATERIAL_PARAMETER_SAMPLER
SamplerState                sampler                : register(s0, space1);

// MATERIAL_PARAMETER_TEXTURE
Texture2D<float4>           base_color             : register(t1, space2);
Texture2D<float3>           normal                 : register(t2, space2);
Texture2D<float4>           metal_roughness        : register(t3, space2);
Texture2D<float4>           ambient                : register(t4, space2);
...

// MATERIAL_PARAMETER_VALUE
StructuredBuffer<>          material_buffer        : register(t0, space2);


*/

struct IDrawsScene;
struct IDrawsSceneComponent;
struct IDrawsNodeComponent;
struct IDrawsPrimitiveComponent;
struct IDrawsMeshComponent;
struct IDrawsStaticMeshComponent;
struct IDrawsLightComponent;

struct IDrawsCamera;
struct IDrawsView;
struct IDrawsRenderer;

enum VERTEX_BUFFER_TYPE
{
      VERTEX_BUFFER_TYPE_FLOAT1
    , VERTEX_BUFFER_TYPE_FLOAT2
    , VERTEX_BUFFER_TYPE_FLOAT3
    , VERTEX_BUFFER_TYPE_FLOAT4

    , VERTEX_BUFFER_TYPE_SINT1
    , VERTEX_BUFFER_TYPE_SINT2
    , VERTEX_BUFFER_TYPE_SINT3
    , VERTEX_BUFFER_TYPE_SINT4

    , VERTEX_BUFFER_TYPE_UINT1
    , VERTEX_BUFFER_TYPE_UINT2
    , VERTEX_BUFFER_TYPE_UINT3
    , VERTEX_BUFFER_TYPE_UINT4

    , VERTEX_BUFFER_TYPE_SSHORT1
    , VERTEX_BUFFER_TYPE_SSHORT2
    , VERTEX_BUFFER_TYPE_SSHORT3
    , VERTEX_BUFFER_TYPE_SSHORT4

    , VERTEX_BUFFER_TYPE_USHORT1
    , VERTEX_BUFFER_TYPE_USHORT2
    , VERTEX_BUFFER_TYPE_USHORT3
    , VERTEX_BUFFER_TYPE_USHORT4

    , VERTEX_BUFFER_TYPE_NUM_TYPES
};

enum VERTEX_ELEMENT_TYPE
{
      VERTEX_ELEMENT_TYPE_POSITION  // VERTEX_BUFFER_TYPE_FLOAT4
    , VERTEX_ELEMENT_TYPE_NORMAT    // VERTEX_BUFFER_TYPE_FLOAT4
    , VERTEX_ELEMENT_TYPE_TANGENT   // VERTEX_BUFFER_TYPE_FLOAT4
    , VERTEX_ELEMENT_TYPE_TEXCOORD0 // VERTEX_BUFFER_TYPE_FLOAT2

    , VERTEX_ELEMENT_TYPE_NUM_TYPES
};

enum BUFFER_FLAG : uint32_t
{
      BUFFER_FLAG_NONE          = 0x0
    , BUFFER_FLAG_CPU_VISIBLE   = 0x1 // バッファのリビルドを省略可能にします。(パフォーマンスに影響を与える可能性があります) 
};
TO_FLAGS(BUFFER_FLAG);

enum INDEX_BUFFER_TYPE
{
      INDEX_BUFFER_TYPE_UINT16
    , INDEX_BUFFER_TYPE_UINT32
};

enum PRIMITIVE_TOPOLOGY
{
      PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    , PRIMITIVE_TOPOLOGY_LINE_LIST
};

enum SHADER_TYPE
{
      SHADER_TYPE_VS
    , SHADER_TYPE_PS
};

enum MATERIAL_SHADING_MODEL
{
    MATERIAL_SHADING_MODEL_DEFAULT_LIT
};

enum CULLING_MODE
{
      CULLING_MODE_NONE
    , CULLING_MODE_FRONT
    , CULLING_MODE_BACK
};

enum MATERIAL_BLEND_MODE
{
      MATERIAL_BLEND_MODE_OPAQUE
    , MATERIAL_BLEND_MODE_MASKED
    , MATERIAL_BLEND_MODE_TRANSLUCENT
};

enum SAMPLER_FILTER_MODE
{
      SAMPLER_FILTER_MODE_POINT
    , SAMPLER_FILTER_MODE_LINEAR
};

enum SAMPLER_WRAP_MODE
{
      SAMPLER_WRAP_MODE_WRAP
    , SAMPLER_WRAP_MODE_CLAMP
    , SAMPLER_WRAP_MODE_BOARDER_OPAQUE
    , SAMPLER_WRAP_MODE_BOARDER_TRANSPARENT
};

enum TEXTURE_FORMAT
{
    // R
      TEXTURE_FORMAT_R8_UNORM
    , TEXTURE_FORMAT_R8_SNORM
    , TEXTURE_FORMAT_R8_UINT
    , TEXTURE_FORMAT_R8_SINT
    , TEXTURE_FORMAT_R16_UNORM
    , TEXTURE_FORMAT_R16_SNORM
    , TEXTURE_FORMAT_R16_UINT
    , TEXTURE_FORMAT_R16_SINT
    , TEXTURE_FORMAT_R16_FLOAT
    , TEXTURE_FORMAT_R32_UINT
    , TEXTURE_FORMAT_R32_SINT
    , TEXTURE_FORMAT_R32_FLOAT

    // RG
    , TEXTURE_FORMAT_R8G8_UNORM
    , TEXTURE_FORMAT_R8G8_SNORM
    , TEXTURE_FORMAT_R8G8_UINT
    , TEXTURE_FORMAT_R8G8_SINT
    , TEXTURE_FORMAT_R16G16_UNORM
    , TEXTURE_FORMAT_R16G16_SNORM
    , TEXTURE_FORMAT_R16G16_UINT
    , TEXTURE_FORMAT_R16G16_SINT
    , TEXTURE_FORMAT_R16G16_FLOAT
    , TEXTURE_FORMAT_R32G32_UINT
    , TEXTURE_FORMAT_R32G32_SINT
    , TEXTURE_FORMAT_R32G32_FLOAT
    // GR

    // RGB
    , TEXTURE_FORMAT_R11G11B10_UFLOAT
    , TEXTURE_FORMAT_R32G32B32_UINT
    , TEXTURE_FORMAT_R32G32B32_SINT
    , TEXTURE_FORMAT_R32G32B32_FLOAT

    // BGR
    , TEXTURE_FORMAT_B5G6R5_UNORM

    // RGBA
    , TEXTURE_FORMAT_R8G8B8A8_UNORM
    , TEXTURE_FORMAT_R8G8B8A8_UNORM_SRGB
    , TEXTURE_FORMAT_R8G8B8A8_SNORM
    , TEXTURE_FORMAT_R8G8B8A8_UINT
    , TEXTURE_FORMAT_R8G8B8A8_SINT
    , TEXTURE_FORMAT_R10G10B10A2_UNORM
    , TEXTURE_FORMAT_R10G10B10A2_UINT
    , TEXTURE_FORMAT_R16G16B16A16_UNORM
    , TEXTURE_FORMAT_R16G16B16A16_SNORM
    , TEXTURE_FORMAT_R16G16B16A16_UINT
    , TEXTURE_FORMAT_R16G16B16A16_SINT
    , TEXTURE_FORMAT_R16G16B16A16_FLOAT
    , TEXTURE_FORMAT_R32G32B32A32_UINT
    , TEXTURE_FORMAT_R32G32B32A32_SINT
    , TEXTURE_FORMAT_R32G32B32A32_FLOAT
    // BGRA
    , TEXTURE_FORMAT_B5G5R5A1_UNORM
    , TEXTURE_FORMAT_B8G8R8A8_UNORM
    , TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB

    // RGBE
    , TEXTURE_FORMAT_R9G9B9E5_UFLOAT

    // 深度ステンシルフォーマット                        
    , TEXTURE_FORMAT_D16_UNORM
    , TEXTURE_FORMAT_D32_FLOAT
    , TEXTURE_FORMAT_D24_UNORM_S8_UINT
    , TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT
};

enum TEXTURE_USAGE
{
      TEXTURE_USAGE_TEXTURE
    , TEXTURE_USAGE_CUBEMAP
};

enum MATERIAL_PARAMETER_TYPE
{
      MATERIAL_PARAMETER_TYPE_VALUE
    , MATERIAL_PARAMETER_TYPE_SAMPLER
    , MATERIAL_PARAMETER_TYPE_TEXTURE
};


#pragma endregion declaration

#pragma region structures

struct PRIMITIVE_PARAMETERS
{
    glm::mat4 local_to_world;
    glm::mat4 world_to_local;
    glm::mat4 previous_local_to_world;
    glm::mat4 previous_world_to_local;
};

struct SAMPLER_CREATE_DESC
{
    SAMPLER_FILTER_MODE min_filter;
    SAMPLER_FILTER_MODE mag_filter;
    SAMPLER_WRAP_MODE   wrap_mode;
};

struct TEXTURE_CREATE_DESC
{
    TEXTURE_USAGE   usage;
    TEXTURE_FORMAT  format;
    uint32_t        width;
    uint32_t        height;
    uint32_t        depth;
    uint32_t        array_size;
    uint32_t        num_mips;
};

struct VERTEX_BUFFER_CREATE_DESC
{
    VERTEX_BUFFER_TYPE          type;
    size_t                      vertex_count;
    BUFFER_FLAG                 flags;
};

struct INDEX_BUFFER_CREATE_DESC
{
    INDEX_BUFFER_TYPE           type;
    size_t                      index_count;
    BUFFER_FLAG                 flags;
};

struct MATERIAL_VALUE_PARAMETER
{
    const char*     name;
    uint32_t        size;
};
struct MATERIAL_SAMPLER_PARAMETER
{
    const char*     name;
};
struct MATERIAL_TEXTURE_PARAMETER
{
    const char*     name;
};
struct MATERIAL_PARAMETERS_CREATE_DESC
{
    MATERIAL_PARAMETER_TYPE         type;
    uint32_t                        num_parameters;
    union 
    {
        MATERIAL_VALUE_PARAMETER*   value;      // 配列の要素順にパラメーター(構造体メンバー)が宣言されます。 
        MATERIAL_SAMPLER_PARAMETER* sampler;    // 配列の要素順にパラメーターが宣言されます。 
        MATERIAL_TEXTURE_PARAMETER* texture;    // 配列の要素順にパラメーターが宣言されます。 
    } parameters;
};

struct MATERIAL_SHADER
{
    SHADER_TYPE type;
    const char* shader_hlsl;
};
struct MATERIAL_CREATE_DESC
{
    bool                        is_wireframe;
    PRIMITIVE_TOPOLOGY          topology;
    CULLING_MODE                culling_mode;
    MATERIAL_BLEND_MODE         blend_mode;
    MATERIAL_SHADING_MODEL      shading_model;

    IDrawsMaterialParameters*   value_parameters;
    IDrawsMaterialParameters*   sampler_parameters;
    IDrawsMaterialParameters*   texture_parameters;

    uint32_t                    num_shaders;
    MATERIAL_SHADER*            shaders;
};

struct MATERIAL_CONSTANT_CREATE_DESC
{

};

struct MATERIAL_INSTANCE_SAMPLER
{
    const char*     name;
    IDrawsSampler*  sampler;
};
struct MATERIAL_INSTANCE_TEXTURE
{
    const char*     name;
    IDrawsTexture*  texture;
};
struct MATERIAL_INSTANCE_CREATE_DESC
{
    uint32_t                    num_samplers;
    MATERIAL_INSTANCE_SAMPLER*  samplers;

    uint32_t                    num_textures;
    MATERIAL_INSTANCE_TEXTURE*  textures;
};

struct SUBMESH_DATA
{
    uint32_t                    material_index; // STATIC_MESH_CREATE_DESC::materialsへアクセスする配列のインデックスです。
    uint32_t                    vertex_offset;  // base_vertex_location
    uint32_t                    index_count;    // index_count_per_instance
    uint32_t                    index_offset;   // start_index_location
};
struct MESH_DATA_LOD_INFO
{
    float                       start_distance;
    uint32_t                    num_submeshes;
    SUBMESH_DATA*               submeshes;
};
struct MESH_DATA_VERTEX_BUFFER
{
    VERTEX_ELEMENT_TYPE         type;
    IDrawsVertexBuffer*         vertex_buffer;
    uint32_t                    vertex_buffer_offset; // Viewの作成に使用します。
};
struct MESH_DATA_CREATE_DESC
{
    uint32_t                    num_lods;
    MESH_DATA_LOD_INFO*         lods;

    uint32_t                    num_elements;
    MESH_DATA_VERTEX_BUFFER*    elements;

    IDrawsIndexBuffer*          index_buffer;
    uint32_t                    index_buffer_offset; // Viewの作成に使用します。
};

struct STATIC_MESH_CREATE_DESC
{
    IDrawsMeshData*             mesh_data;

    uint32_t                    num_materials;
    IDrawsMaterialInstance**    materials;
};

struct SCENE_CREATE_DESC
{

};

struct NODE_COMPONENT_CREATE_DESC
{

};

struct STATIC_MESH_COMPONENT_CREATE_DESC
{
    IDrawsStaticMesh*           mesh;

    uint32_t                    num_materials;
    IDrawsMaterialInstance**    override_materials;

    uint32_t                    num_constants;
    IDrawsMaterialConstant**    constants;
};

struct LIGHT_COMPONENT_CREATE_DESC
{

};

struct CAMERA_CREATE_DESC
{

};

struct VIEW_CREATE_DESC
{

};

#pragma endregion structures

#pragma region interfaces

struct IDrawsObject
{
protected:
    virtual ~IDrawsObject() {}

public:
    virtual void     AddRef() = 0;
    virtual uint32_t Release() = 0;

};

struct IDrawsInstance
{
protected:
    virtual ~IDrawsInstance() {}

public:
    virtual bool CreateSampler              (const SAMPLER_CREATE_DESC&                 _desc, IDrawsSampler**              _dst) = 0;
    virtual bool CreateTexture              (const TEXTURE_CREATE_DESC&                 _desc, IDrawsTexture**              _dst) = 0;
    virtual bool CreateVertexBuffer         (const VERTEX_BUFFER_CREATE_DESC&           _desc, IDrawsVertexBuffer**         _dst) = 0;
    virtual bool CreateIndexBuffer          (const INDEX_BUFFER_CREATE_DESC&            _desc, IDrawsIndexBuffer**          _dst) = 0;
    virtual bool CreateMaterialParameters   (const MATERIAL_PARAMETERS_CREATE_DESC&     _desc, IDrawsMaterialParameters**   _dst) = 0;
    virtual bool CreateMaterial             (const MATERIAL_CREATE_DESC&                _desc, IDrawsMaterial**             _dst) = 0;
    virtual bool CreateMeshData             (const MESH_DATA_CREATE_DESC&               _desc, IDrawsMeshData**             _dst) = 0;
    virtual bool CreateStaticMesh           (const STATIC_MESH_CREATE_DESC&             _desc, IDrawsStaticMesh**           _dst) = 0;
    virtual bool CreateScene                (const SCENE_CREATE_DESC&                   _desc, IDrawsScene**                _dst) = 0;
    virtual bool CreateNodeComponent        (const NODE_COMPONENT_CREATE_DESC&          _desc, IDrawsNodeComponent**        _dst) = 0;
    virtual bool CreateStaticMeshComponent  (const STATIC_MESH_COMPONENT_CREATE_DESC&   _desc, IDrawsStaticMeshComponent**  _dst) = 0;
    virtual bool CreateLightComponent       (const LIGHT_COMPONENT_CREATE_DESC&         _desc, IDrawsLightComponent**       _dst) = 0;
    virtual bool CreateCamera               (const CAMERA_CREATE_DESC&                  _desc, IDrawsCamera**               _dst) = 0;
    virtual bool CreateView                 (const VIEW_CREATE_DESC&                    _desc, IDrawsView**                 _dst) = 0;

};

IDrawsInstance* CreateDrawsInstance();
void            DestroyDrawsInstance(IDrawsInstance* _instance);

struct IDrawsResource : IDrawsObject
{
protected:
    virtual ~IDrawsResource() {}

public:
    virtual void        SetName(const char* _name) = 0;
    virtual const char* GetName() = 0;

};

struct IDrawsSampler : IDrawsResource
{
protected:
    virtual ~IDrawsSampler() {}

public:

};

struct IDrawsTexture : IDrawsResource
{
protected:
    virtual ~IDrawsTexture() {}

public:
    virtual bool SetData(uint32_t _mip_slice, uint32_t _array_slice, uint64_t _src_row_pitch, uint64_t _src_texture_height, size_t _src_size, const void* _src_data) = 0;

};

struct IDrawsBuffer : IDrawsResource
{
protected:
    virtual ~IDrawsBuffer() {}

public:
    virtual bool SetData(size_t _dst_offset, size_t _src_size, const void* _src_data) = 0;

};

struct IDrawsVertexBuffer : IDrawsBuffer
{
protected:
    virtual ~IDrawsVertexBuffer() {}

public:

};

struct IDrawsIndexBuffer : IDrawsBuffer
{
protected:
    virtual ~IDrawsIndexBuffer() {}

public:

};

struct IDrawsMaterialParameters : IDrawsObject
{
protected:
    virtual ~IDrawsMaterialParameters() {}

public:

};

struct IDrawsMaterial : IDrawsObject
{
protected:
    virtual ~IDrawsMaterial() {}

public:
    virtual bool CreateMaterialConstant(const MATERIAL_CONSTANT_CREATE_DESC& _desc, IDrawsMaterialConstant** _dst) = 0;
    virtual bool CreateMaterialInstance(const MATERIAL_INSTANCE_CREATE_DESC& _desc, IDrawsMaterialInstance** _dst) = 0;

};

struct IDrawsMaterialConstant : IDrawsObject
{
protected:
    virtual ~IDrawsMaterialConstant() {}

public:
    virtual bool SetValue(const char* _dst_name, uint32_t _src_size, const void* _src_value) = 0;

    template<typename T>
    bool SetValue(const char* _dst_name, const T* _src_value)
    { return SetValue(_dst_name, sizeof(T), _src_value); }

};

struct IDrawsMaterialInstance : IDrawsObject
{
protected:
    virtual ~IDrawsMaterialInstance() {}

public:
    virtual bool SetParameter(const char* _name, IDrawsSampler* _sampler) = 0;
    virtual bool SetParameter(const char* _name, IDrawsTexture* _texture) = 0;

};

struct IDrawsMeshData : IDrawsObject
{
protected:
    virtual ~IDrawsMeshData() {}

public:

};

struct IDrawsMesh : IDrawsObject
{
protected:
    virtual ~IDrawsMesh() {}

public:

};

struct IDrawsStaticMesh : IDrawsMesh
{
protected:
    virtual ~IDrawsStaticMesh() {}

public:

};

struct IDrawsScene : IDrawsObject
{
protected:
    virtual ~IDrawsScene() {}

public:

};

struct IDrawsSceneComponent : IDrawsObject
{
protected:
    virtual ~IDrawsSceneComponent() {}

public:

};

struct IDrawsNodeComponent : IDrawsSceneComponent
{
protected:
    virtual ~IDrawsNodeComponent() {}

public:

};

struct IDrawsPrimitiveComponent : IDrawsSceneComponent
{
protected:
    virtual ~IDrawsPrimitiveComponent() {}

public:

};

struct IDrawsMeshComponent : IDrawsPrimitiveComponent
{
protected:
    virtual ~IDrawsMeshComponent() {}

public:

};

struct IDrawsStaticMeshComponent : IDrawsMeshComponent
{
protected:
    virtual ~IDrawsStaticMeshComponent() {}

public:

};

struct IDrawsLightComponent : IDrawsSceneComponent
{
protected:
    virtual ~IDrawsLightComponent() {}

public:

};

struct IDrawsCamera : IDrawsObject
{
protected:
    virtual ~IDrawsCamera() {}

public:

};

struct IDrawsView : IDrawsObject
{
protected:
    virtual ~IDrawsView() {}

public:

};

struct IDrawsRenderer : IDrawsObject
{
protected:
    virtual ~IDrawsRenderer() {}

public:

};


#pragma endregion interfaces


}// namespace draws
}// namespace buma
