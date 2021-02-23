#pragma once

namespace buma
{
namespace draws
{

class DrawsMaterial : public IDrawsMaterial
{
protected:
    DrawsMaterial();
    ~DrawsMaterial();

    bool Init(DrawsInstance* _ins, const MATERIAL_CREATE_DESC& _desc);
    void CopyDesc(const buma::draws::MATERIAL_CREATE_DESC& _desc);
    void PrepareParametersRegisterShifts();
    bool RequestParametersSignature();
    bool CreateShaderModules(uint32_t _num_shaders, const MATERIAL_SHADER* _shaders);
    bool CreatePerPassShaders();
    bool PrepareGraphicsPipelineDesc();
    bool CreateGraphicsPipelines();
    bool CreateForDeferredRenderingPipelines();

public:
    static bool Create(DrawsInstance* _ins, const MATERIAL_CREATE_DESC& _desc, IDrawsMaterial** _dst);

    void     AddRef()  override;
    uint32_t Release() override;

    bool CreateMaterialConstant(const MATERIAL_CONSTANT_CREATE_DESC& _desc, IDrawsMaterialConstant** _dst) override;
    bool CreateMaterialInstance(const MATERIAL_INSTANCE_CREATE_DESC& _desc, IDrawsMaterialInstance** _dst) override;
    
public:
    bool                                                    IsWireframe()                       const { return is_wireframe; }
    PRIMITIVE_TOPOLOGY                                      GetPrimitiveTopology()              const { return topology; }
    CULLING_MODE                                            GetCullingMode()                    const { return culling_mode; }
    MATERIAL_BLEND_MODE                                     GetBlendMode()                      const { return blend_mode; }
    MATERIAL_SHADING_MODEL                                  GetShadingModel()                   const { return shading_model; }

    const MATERIAL_PARAMETERS_LAYOUT&                       GetParametersLayout()               const { return layout; }
    const std::vector<shader::REGISTER_SHIFT>&              GetParametersRegisterShift()        const { return register_shifts; }

    const buma3d::GRAPHICS_PIPELINE_STATE_DESC&             GetPsoDesc()                        const { return pso_desc; }

    const std::shared_ptr<ParametersSignature>&             GetParametersSignature()            const { return signature; }
    const std::vector<std::shared_ptr<MaterialShader>>&     GetShaders()                        const { return shaders; }
    const MaterialPerPassShadersMap&                        GetPerPassShadersMap()              const { return *per_pass_shaders_map; }
    const MaterialPerPassPipeline*                          GetPipeline(RENDER_PASS_TYPE _type) const { return per_pass_pipelines[_type].get(); }

private:
    std::atomic_uint32_t                                                                ref_count;
    DrawsInstance*                                                                      ins;

    bool                                                                                is_wireframe;
    PRIMITIVE_TOPOLOGY                                                                  topology;
    CULLING_MODE                                                                        culling_mode;
    MATERIAL_BLEND_MODE                                                                 blend_mode;
    MATERIAL_SHADING_MODEL                                                              shading_model;
    MATERIAL_PARAMETERS_LAYOUT                                                          layout;
    std::vector<shader::REGISTER_SHIFT>                                                 register_shifts;

    buma3d::GRAPHICS_PIPELINE_STATE_DESC                                                pso_desc;
    buma3d::RASTERIZATION_STATE_DESC                                                    rasterization_state;
    buma3d::INPUT_ASSEMBLY_STATE_DESC                                                   input_assembly;

    std::shared_ptr<ParametersSignature>                                                signature;
    std::vector<std::shared_ptr<MaterialShader>>                                        shaders;
    std::unique_ptr<MaterialPerPassShadersMap>                                          per_pass_shaders_map;
    std::array<std::unique_ptr<MaterialPerPassPipeline>, RENDER_PASS_TYPE_NUM_TYPES>    per_pass_pipelines;

};


}// namespace buma
}// namespace draws
