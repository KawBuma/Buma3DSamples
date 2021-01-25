#pragma once

namespace buma
{
namespace draws
{

class DrawsInstance : public IDrawsInstance
{
protected:
    DrawsInstance();
    ~DrawsInstance();

public:
    static void Create(IDrawsInstance** _dst);
    static void Destroy(IDrawsInstance* _target);

public:
    bool CreateSampler              (const SAMPLER_CREATE_DESC&                 _desc, IDrawsSampler**              _dst) override;
    bool CreateTexture              (const TEXTURE_CREATE_DESC&                 _desc, IDrawsTexture**              _dst) override;
    bool CreateVertexBuffer         (const VERTEX_BUFFER_CREATE_DESC&           _desc, IDrawsVertexBuffer**         _dst) override;
    bool CreateIndexBuffer          (const INDEX_BUFFER_CREATE_DESC&            _desc, IDrawsIndexBuffer**          _dst) override;
    bool CreateMaterialParameters   (const MATERIAL_PARAMETERS_CREATE_DESC&     _desc, IDrawsMaterialParameters**   _dst) override;
    bool CreateMaterial             (const MATERIAL_CREATE_DESC&                _desc, IDrawsMaterial**             _dst) override;
    bool CreateMeshData             (const MESH_DATA_CREATE_DESC&               _desc, IDrawsMeshData**             _dst) override;
    bool CreateStaticMesh           (const STATIC_MESH_CREATE_DESC&             _desc, IDrawsStaticMesh**           _dst) override;
    bool CreateScene                (const SCENE_CREATE_DESC&                   _desc, IDrawsScene**                _dst) override;
    bool CreateNodeComponent        (const NODE_COMPONENT_CREATE_DESC&          _desc, IDrawsNodeComponent**        _dst) override;
    bool CreateStaticMeshComponent  (const STATIC_MESH_COMPONENT_CREATE_DESC&   _desc, IDrawsStaticMeshComponent**  _dst) override;
    bool CreateLightComponent       (const LIGHT_COMPONENT_CREATE_DESC&         _desc, IDrawsLightComponent**       _dst) override;
    bool CreateCamera               (const CAMERA_CREATE_DESC&                  _desc, IDrawsCamera**               _dst) override;
    bool CreateView                 (const VIEW_CREATE_DESC&                    _desc, IDrawsView**                 _dst) override;

public:
    DeviceResources* GetDR() { return dr; }

private:
    DeviceResources* dr;

};


}// namespace buma
}// namespace draws
