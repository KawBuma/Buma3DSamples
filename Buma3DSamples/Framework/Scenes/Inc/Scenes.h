#pragma once

#include <stddef.h>
#include <typeinfo>
#include <typeindex>
#include <vector>

// to use a clip space between 0 to 1.
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// For DirectX, Metal, Vulkan
#define GLM_FORCE_LEFT_HANDED 
#include <glm/glm.hpp>
#include <glm/matrix.hpp>

namespace buma
{
namespace scne
{

#pragma region 

enum class SCENES_OBJECT_TYPE : uint32_t
{
      IScenesObject
    , IComponent
    , IScene
    , INode
    , IValueComponent
    , ITransformComponent
    , IAABBComponent
    , ICameraComponent
    , ILightComponent
    , ILightPropertyComponent
    , IMaterialComponent
    , IMaterialPropertyComponent
    , ITextureComponent
    , ISamplerComponent
    , IBufferComponent
    , IBufferViewComponent
    , IIndexLayoutComponent
    , IVertexLayoutComponent
    , IVertexElementComponent
    , IVertexBufferComponent
    , IIndexBufferComponent
    , ISubmeshComponent
    , IMeshComponent

    , SCENES_OBJECT_TYPE_NUM_TYPES
    , SCENES_OBJECT_TYPE_UNDEFINED
};
template<typename T> inline SCENES_OBJECT_TYPE GetScenesObjectType() { return SCENES_OBJECT_TYPE::SCENES_OBJECT_TYPE_UNDEFINED; }

struct IScenes;
struct IScenesObject;
struct IComponent;
struct IScene;
struct INode;
struct IValueComponent;
struct ITransformComponent;
struct IAABBComponent;
struct ICameraComponent;
struct ILightComponent;
struct ILightPropertyComponent;
struct IMaterialComponent;
struct IMaterialPropertyComponent;
struct ITextureComponent;
struct ISamplerComponent;
struct IBufferComponent;
struct IBufferViewComponent;
struct IIndexLayoutComponent;
struct IVertexLayoutComponent;
struct IVertexElementComponent;
struct IVertexBufferComponent;
struct IIndexBufferComponent;
struct ISubmeshComponent;
struct IMeshComponent;

template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IScenesObject>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IScene>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<INode>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IValueComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<ITransformComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IAABBComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<ICameraComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<ILightComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<ILightPropertyComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IMaterialComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IMaterialPropertyComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<ITextureComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<ISamplerComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IBufferComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IBufferViewComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IIndexLayoutComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IVertexLayoutComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IVertexElementComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IVertexBufferComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IIndexBufferComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<ISubmeshComponent>();
template<> extern SCENES_OBJECT_TYPE GetScenesObjectType<IMeshComponent>();

#pragma endregion

inline constexpr const char* SCENES_VERTEX_ELEMENT_NAME_POSITION   = "POSITION";
inline constexpr const char* SCENES_VERTEX_ELEMENT_NAME_NORMAL     = "NORMAL";
inline constexpr const char* SCENES_VERTEX_ELEMENT_NAME_TANGENT    = "TANGENT";
inline constexpr const char* SCENES_VERTEX_ELEMENT_NAME_TEXCOORD_0 = "TEXCOORD_0";
inline constexpr const char* SCENES_VERTEX_ELEMENT_NAME_TEXCOORD_1 = "TEXCOORD_1";
inline constexpr const char* SCENES_VERTEX_ELEMENT_NAME_COLOR_1    = "COLOR_0";

enum VALUE_TYPE
{
      VALUE_TYPE_INT32
    , VALUE_TYPE_UINT32
    , VALUE_TYPE_FLOAT32
    , VALUE_TYPE_STRING
};

enum LIGHT_TYPE
{
      LIGHT_TYPE_DIRECTIONAL
    , LIGHT_TYPE_POINT
    , LIGHT_TYPE_SPOT
};

enum MATERIAL_ALPHA_MODE
{
      MATERIAL_ALPHA_MODE_OPAQUE
    , MATERIAL_ALPHA_MODE_MASK
    , MATERIAL_ALPHA_MODE_BLEND
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

enum DATA_FORMAT
{
      DATA_FORMAT_UNDEFINED
    , DATA_FORMAT_UINT8
    , DATA_FORMAT_UINT16
    , DATA_FORMAT_UINT32
    , DATA_FORMAT_SINT8
    , DATA_FORMAT_SINT16
    , DATA_FORMAT_SINT32
    , DATA_FORMAT_FLOAT32
};

IScenes*    CreateScenes();
void        DestroyScenes(IScenes* _scenes);

struct IScenes
{
protected:
    virtual ~IScenes() {}

public:
    virtual uint32_t                    GetScenesSize() = 0;
    virtual IScene*                     GetScenes(uint32_t _index) = 0;

    virtual IScene*                     CreateScene() = 0;
    virtual IValueComponent*            CreateValueComponent() = 0;
    virtual ITransformComponent*        CreateTransformComponent() = 0;
    virtual IAABBComponent*             CreateAABBComponent() = 0;
    virtual ICameraComponent*           CreateCameraComponent() = 0;
    virtual ILightComponent*            CreateLightComponent() = 0;
    virtual ILightPropertyComponent*    CreateLightPropertyComponent() = 0;
    virtual IMaterialComponent*         CreateMaterialComponent() = 0;
    virtual IMaterialPropertyComponent* CreateMaterialPropertyComponent() = 0;
    virtual ITextureComponent*          CreateTextureComponent() = 0;
    virtual ISamplerComponent*          CreateSamplerComponent() = 0;
    virtual IBufferComponent*           CreateBufferComponent() = 0;
    virtual IBufferViewComponent*       CreateBufferViewComponent() = 0;
    virtual IIndexLayoutComponent*      CreateIndexLayoutComponent() = 0;
    virtual IVertexLayoutComponent*     CreateVertexLayoutComponent() = 0;
    virtual IVertexElementComponent*    CreateVertexElementComponent() = 0;
    virtual IIndexBufferComponent*      CreateIndexBufferComponent() = 0;
    virtual IVertexBufferComponent*     CreateVertexBufferComponent() = 0;
    virtual ISubmeshComponent*          CreateSubmeshComponent() = 0;
    virtual IMeshComponent*             CreateMeshComponent() = 0;

};

struct IScenesObject
{
protected:
    virtual ~IScenesObject() {}

public:
    // @brief ユーザーがIScenes::Create*でオブジェクト作成後、そのオブジェクトをどのシーン、コンポーネントにも設定しなかった場合に呼び出す必要があります。
    virtual uint32_t                Release() = 0;

    virtual const char*             GetName() const = 0;
    virtual void                    SetName(const char* _name) = 0;

    virtual IScenes*                GetScenes() = 0;

    virtual SCENES_OBJECT_TYPE      GetType() const = 0;
    virtual void*                   As(SCENES_OBJECT_TYPE _type) = 0;
    template <typename T> T*        As() { return (T*)(As(GetScenesObjectType<T>())); }

};

struct IScene : IScenesObject
{
protected:
    virtual ~IScene() {}

public:
    virtual INode* GetRoot() = 0;

};

struct IComponent : IScenesObject
{
protected:
    virtual ~IComponent() {}

public:

};

struct INode : IScenesObject
{
protected:
    virtual ~INode() {}

public:
    virtual INode* GetParent() = 0;

    virtual INode*                              AddChild    () = 0;
    virtual void                                ReleaseChild(INode* _node) = 0;
    virtual INode*                              GetChildren(uint32_t _index) = 0;
    virtual INode*                              GetChildren(const char* _name) = 0;

    virtual void                                AddComponent    (IComponent* _component) = 0;
    virtual void                                ReleaseComponent(IComponent* _component) = 0;
    virtual const void*                         GetComponents(SCENES_OBJECT_TYPE _type) = 0;
    template<typename T> const std::vector<T*>* GetComponents() { return static_cast<std::vector<IComponent*>*>(GetComponents(GetScenesObjectType<T>())); }

};

struct IValueComponent : IComponent
{
protected:
    virtual ~IValueComponent() {}

public:
    virtual void        SetValueType(VALUE_TYPE _type) = 0;
    virtual void        Resize(uint32_t _size_in_bytes) = 0;
    virtual void        SetData(uint32_t _offset_in_bytes, uint32_t _src_size_in_bytes, const void* _src) = 0;

    virtual VALUE_TYPE  GetValueType() = 0;
    virtual uint32_t    GetSize() = 0;
    virtual const void* GetData() = 0;

};

struct ITransformComponent : IComponent
{
protected:
    virtual ~ITransformComponent() {}

public:
    virtual void                SetTranslation(glm::vec3& _translation) = 0;
    virtual void                SetRotation   (glm::vec3& _rotation) = 0;
    virtual void                SetScale      (glm::vec3& _scale) = 0;

    virtual void                SetTransform  (glm::mat4& _transform) = 0;
    virtual const glm::mat4&    GetTransform() = 0;

};

struct IAABBComponent : IComponent
{
protected:
    virtual ~IAABBComponent() {}

public:
    virtual void UpdateBounds(IMeshComponent* _mesh) = 0;
    virtual const glm::vec3& GetMinBounds() = 0;
    virtual const glm::vec3& GetMaxBounds() = 0;

};

struct ICameraComponent : IComponent
{
protected:
    virtual ~ICameraComponent() {}

public:
    virtual void                    SetAspectRatio  (float _aspect_ratio) = 0;
    virtual void                    SetFovY         (float _fovy) = 0;
    virtual void                    SetNearZ        (float _near) = 0;
    virtual void                    SetFarZ         (float _far) = 0;

    virtual float                   GetAspectRatio() = 0;
    virtual float                   GetFovY() = 0;
    virtual float                   GetNearZ() = 0;
    virtual float                   GetFarZ() = 0;
    virtual const glm::mat4&        GetProjection   () = 0;

    virtual void                    SetTransformComponent(ITransformComponent* _transform) = 0;
    virtual ITransformComponent*    GetTransformComponent() = 0;

};

struct ILightComponent : IComponent
{
protected:
    virtual ~ILightComponent() {}

public:
    virtual LIGHT_TYPE          SetLightType() = 0;
    virtual const glm::vec3&    GetDir() = 0;
    virtual const glm::vec4&    GetColor() = 0;

    virtual void                SetLihtType (LIGHT_TYPE _type) = 0;
    virtual void                SetDir  (const glm::vec3& _direction) = 0;
    virtual void                SetColor(const glm::vec4& _color) = 0;

    virtual void                     AddCustomProperties    (ILightPropertyComponent* _property) = 0;
    virtual void                     ReleaseCustomProperties(ILightPropertyComponent* _property) = 0;
    virtual uint32_t                 GetCustomPropertiesSize() = 0;
    virtual ILightPropertyComponent* GetCustomProperties    (uint32_t _index) = 0;
    virtual ILightPropertyComponent* GetCustomProperties    (const char* _name) = 0;

};

struct ILightPropertyComponent : IValueComponent
{
protected:
    virtual ~ILightPropertyComponent() {}

public:

};

struct IMaterialComponent : IComponent
{
protected:
    virtual ~IMaterialComponent() {}

public:
    virtual void                        SetAlphaMode(MATERIAL_ALPHA_MODE _alpha_mode) = 0;
    virtual void                        SetAlphaCutoff(float _alpha_cutoff) = 0;
    virtual void                        SetTwoSidedEnabled(bool _is_enabled) = 0;

    virtual MATERIAL_ALPHA_MODE         GetAlphaMode() = 0;
    virtual float                       GetAlphaCutoff(float _alpha_cutoff) = 0;
    virtual bool                        GetTwoSidedEnabled(bool _is_enabled) = 0;

    virtual void                        AddCustomProperties    (IMaterialPropertyComponent* _property) = 0;
    virtual void                        ReleaseCustomProperties(IMaterialPropertyComponent* _property) = 0;
    virtual uint32_t                    GetMaterialPropertiesSize() = 0;
    virtual IMaterialPropertyComponent* GetMaterialProperties(uint32_t _index) = 0;
    virtual IMaterialPropertyComponent* GetMaterialProperties(const char* _str) = 0;

};

struct IMaterialPropertyComponent : IComponent
{
protected:
    virtual ~IMaterialPropertyComponent() {}

public:
    virtual void                AddValueComponent(IValueComponent* _component) = 0;
    virtual void                ReleaseValueComponent(IValueComponent* _component) = 0;
    virtual uint32_t            GetValueComponentSize() = 0;
    virtual IValueComponent*    GetValueComponent(uint32_t _index) = 0;
    virtual IValueComponent*    GetValueComponent(const char* _str) = 0;

    virtual void                SetTextureComponent(ITextureComponent* _component) = 0;
    virtual void                SetSamplerComponent(ISamplerComponent* _component) = 0;
    virtual ITextureComponent*  GetTextureComponent() = 0;
    virtual ISamplerComponent*  GetSamplerComponent() = 0;

};

struct ITextureComponent : IComponent
{
protected:
    virtual ~ITextureComponent() {}

public:
    virtual void        SetFilename(const char* _filename) = 0;
    virtual const char* GetFilename() = 0;

};

struct ISamplerComponent : IComponent
{
protected:
    virtual ~ISamplerComponent() {}

public:
    virtual void                SetSamplerFilterMode(SAMPLER_FILTER_MODE _filter_mode) = 0;
    virtual void                SetSamplerWrapMode(SAMPLER_WRAP_MODE _wrap_mode) = 0;

    virtual SAMPLER_FILTER_MODE GetSamplerFilterMode() = 0;
    virtual SAMPLER_WRAP_MODE   GetSamplerWrapMode() = 0;

};

struct IBufferComponent : IComponent
{
protected:
    virtual ~IBufferComponent() {}

public:
    virtual void        Resize(uint64_t _size_in_bytes) = 0;

    virtual uint64_t    GetSize() = 0;
    virtual void*       GetData() = 0;
    virtual void        SetData(uint64_t _offset, uint64_t _size, void* _src_data) = 0;

};

struct IBufferViewComponent : IComponent
{
protected:
    virtual ~IBufferViewComponent() {}

public:
    virtual void                SetBuffer(IBufferComponent* _buffer) = 0;
    virtual void                SetOffset(uint64_t _offset_in_bytes) = 0;
    virtual void                SetSize(uint64_t _size_in_bytes) = 0;

    virtual IBufferComponent*   GetBuffer() = 0;
    virtual uint64_t            GetOffset() = 0;
    virtual uint64_t            GetSize() = 0;

    virtual void*               GetData() = 0;

};

struct IIndexLayoutComponent : IComponent
{
protected:
    virtual ~IIndexLayoutComponent() {}

public:
    virtual void        SetDataFormat(DATA_FORMAT _format) = 0;
    virtual DATA_FORMAT GetDataFormat() = 0;

};

struct IVertexLayoutComponent : IComponent
{
protected:
    virtual ~IVertexLayoutComponent() {}

public:
    virtual void                        AddVertexElement    (IVertexElementComponent* _vertex_element) = 0;
    virtual void                        ReleaseVertexElement(IVertexElementComponent* _vertex_element) = 0;

    virtual uint32_t                    GetLayoutsStride() = 0;
    virtual uint32_t                    GetVertexElementsSize() = 0;
    virtual IVertexElementComponent*    GetVertexElements(uint32_t _index) = 0;
    virtual IVertexElementComponent*    GetVertexElements(const char* _name) = 0;

};

struct IVertexElementComponent : IComponent
{
protected:
    virtual ~IVertexElementComponent() {}

public:
    virtual void        SetDataFormat(DATA_FORMAT _format) = 0;
    virtual DATA_FORMAT GetDataFormat() = 0;

    virtual uint32_t    GetSizeOfElements() = 0;
    virtual void        SetComponentCount(uint32_t _count) = 0;
    virtual uint32_t    GetComponentCount() = 0;

};

struct IVertexBufferComponent : IComponent
{
protected:
    virtual ~IVertexBufferComponent() {}

public:
    virtual void                    SetVertexLayout(IVertexLayoutComponent* _vertex_layout) = 0;
    virtual void                    SetBufferView(IBufferViewComponent* _buffer_view) = 0;

    virtual IVertexLayoutComponent* GetVertexLayout() = 0;
    virtual IBufferViewComponent*   GetBufferView() = 0;

};

struct IIndexBufferComponent : IComponent
{
protected:
    virtual ~IIndexBufferComponent() {}

public:
    virtual void                    SetIndexLayout(IIndexLayoutComponent* _index_layout) = 0;
    virtual void                    SetBufferView(IBufferViewComponent* _buffer_view) = 0;

    virtual IIndexLayoutComponent*  GetIndexLayout() = 0;
    virtual IBufferViewComponent*   GetBufferView() = 0;

};

struct ISubmeshComponent : IComponent
{
protected:
    virtual ~ISubmeshComponent() {}

public:
    virtual void                    SetMaterial                 (IMaterialComponent* _material) = 0;
    virtual void                    SetIndexBufferStartOffset   (uint32_t _offset) = 0;
    virtual void                    AddVertexBuffer             (IVertexBufferComponent* _vertex_buffer) = 0;
    virtual void                    AddIndexBuffer              (IIndexBufferComponent* _index_buffer) = 0;
    virtual void                    ReleaseVertexBuffer         (IVertexBufferComponent* _vertex_buffer) = 0;
    virtual void                    ReleaseIndexBuffer          (IIndexBufferComponent* _index_buffer) = 0;

    virtual IMaterialComponent*     GetMaterial() = 0;
    virtual uint32_t                GetIndexBufferStartOffset() = 0;
    virtual uint32_t                GetVertexBuffersSize() = 0;
    virtual uint32_t                GetIndexBuffersSize() = 0;
    virtual IVertexBufferComponent* GetVertexBuffers(uint32_t _index) = 0;
    virtual IIndexBufferComponent*  GetIndexBuffers(uint32_t _index) = 0;

};

struct IMeshComponent : IComponent
{
protected:
    virtual ~IMeshComponent() {}

public:
    virtual void                AddSubmesh    (ISubmeshComponent* _submesh) = 0;
    virtual void                ReleaseSubmesh(ISubmeshComponent* _submesh) = 0;

    virtual uint32_t            GetSubmeshSize() = 0;
    virtual ISubmeshComponent*  GetSubmeshs(uint32_t _index) = 0;

};


}// namespace scne
}// namespace buma
