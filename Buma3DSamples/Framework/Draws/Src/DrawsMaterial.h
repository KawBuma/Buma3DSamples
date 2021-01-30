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
    bool RequestParametersSignature();
    bool CreateShaderModules(uint32_t _num_shaders, const MATERIAL_SHADER* _shaders);
    bool CreateGraphicsPipelines();
    void PrepareParametersRegisterShifts();

public:
    static bool Create(DrawsInstance* _ins, const MATERIAL_CREATE_DESC& _desc, IDrawsMaterial** _dst);

    void     AddRef()  override;
    uint32_t Release() override;

    bool CreateMaterialConstant(const MATERIAL_CONSTANT_CREATE_DESC& _desc, IDrawsMaterialConstant** _dst) override;
    bool CreateMaterialInstance(const MATERIAL_INSTANCE_CREATE_DESC& _desc, IDrawsMaterialInstance** _dst) override;
    
public:
    const MATERIAL_PARAMETERS_LAYOUT&                          GetParametersLayout()        const { return layout; }
    const std::vector<shader::REGISTER_SHIFT>&                 GetParametersRegisterShift() const { return register_shifts; }
    const std::vector<std::shared_ptr<MaterialShader>>&        GetShaders()                 const { return shaders; }

private:
    std::atomic_uint32_t                                ref_count;
    DrawsInstance*                                      ins;

    bool                                                is_wireframe;
    PRIMITIVE_TOPOLOGY                                  topology;
    CULLING_MODE                                        culling_mode;
    MATERIAL_BLEND_MODE                                 blend_mode;
    MATERIAL_SHADING_MODEL                              shading_model;
    MATERIAL_PARAMETERS_LAYOUT                          layout;
    std::vector<shader::REGISTER_SHIFT>                 register_shifts;

    std::vector<std::shared_ptr<MaterialShader>>        shaders;
    
    std::shared_ptr<ParametersSignature>                signature;

};


}// namespace buma
}// namespace draws
