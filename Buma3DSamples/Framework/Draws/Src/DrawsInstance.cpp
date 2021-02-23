#include "pch.h"
#include "DrawsInstance.h"

namespace buma
{
namespace draws
{

DrawsInstance::DrawsInstance()
{
}

DrawsInstance::~DrawsInstance()
{
}

void DrawsInstance::Create(IDrawsInstance** _dst)
{
    *_dst = new DrawsInstance();
}

void DrawsInstance::Destroy(IDrawsInstance* _target)
{

}

bool DrawsInstance::CreateSampler              (const SAMPLER_CREATE_DESC&                 _desc, IDrawsSampler**              _dst) { return DrawsSampler             ::Create(_desc, _dst); }
bool DrawsInstance::CreateTexture              (const TEXTURE_CREATE_DESC&                 _desc, IDrawsTexture**              _dst) { return DrawsTexture             ::Create(_desc, _dst); }
bool DrawsInstance::CreateVertexBuffer         (const VERTEX_BUFFER_CREATE_DESC&           _desc, IDrawsVertexBuffer**         _dst) { return DrawsVertexBuffer        ::Create(_desc, _dst); }
bool DrawsInstance::CreateIndexBuffer          (const INDEX_BUFFER_CREATE_DESC&            _desc, IDrawsIndexBuffer**          _dst) { return DrawsIndexBuffer         ::Create(_desc, _dst); }
bool DrawsInstance::CreateMaterialParameters   (const MATERIAL_PARAMETERS_CREATE_DESC&     _desc, IDrawsMaterialParameters**   _dst) { return DrawsMaterialParameters  ::Create(_desc, _dst); }
bool DrawsInstance::CreateMaterial             (const MATERIAL_CREATE_DESC&                _desc, IDrawsMaterial**             _dst) { return DrawsMaterial            ::Create(_desc, _dst); }
bool DrawsInstance::CreateMeshData             (const MESH_DATA_CREATE_DESC&               _desc, IDrawsMeshData**             _dst) { return DrawsMeshData            ::Create(_desc, _dst); }
bool DrawsInstance::CreateStaticMesh           (const STATIC_MESH_CREATE_DESC&             _desc, IDrawsStaticMesh**           _dst) { return DrawsStaticMesh          ::Create(_desc, _dst); }
bool DrawsInstance::CreateScene                (const SCENE_CREATE_DESC&                   _desc, IDrawsScene**                _dst) { return DrawsScene               ::Create(_desc, _dst); }
bool DrawsInstance::CreateNodeComponent        (const NODE_COMPONENT_CREATE_DESC&          _desc, IDrawsNodeComponent**        _dst) { return DrawsNodeComponent       ::Create(_desc, _dst); }
bool DrawsInstance::CreateStaticMeshComponent  (const STATIC_MESH_COMPONENT_CREATE_DESC&   _desc, IDrawsStaticMeshComponent**  _dst) { return DrawsStaticMeshComponent ::Create(_desc, _dst); }
bool DrawsInstance::CreateLightComponent       (const LIGHT_COMPONENT_CREATE_DESC&         _desc, IDrawsLightComponent**       _dst) { return DrawsLightComponent      ::Create(_desc, _dst); }
bool DrawsInstance::CreateCamera               (const CAMERA_CREATE_DESC&                  _desc, IDrawsCamera**               _dst) { return DrawsCamera              ::Create(_desc, _dst); }
bool DrawsInstance::CreateView                 (const VIEW_CREATE_DESC&                    _desc, IDrawsView**                 _dst) { return DrawsView                ::Create(_desc, _dst); }


}// namespace buma
}// namespace draws
