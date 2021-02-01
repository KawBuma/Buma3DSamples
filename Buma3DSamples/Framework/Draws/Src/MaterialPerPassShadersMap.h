#pragma once

namespace buma
{
namespace draws
{

class MaterialPerPassShadersMap;
class ShaderPerStages
{
    friend class MaterialPerPassShadersMap;
public:
    ShaderPerStages()
        : shaders{}
    {}

    ~ShaderPerStages() {}

    MaterialPerPassShader* Find(SHADER_STAGE _stage) const 
    {
        auto find = std::find_if(shaders.begin(), shaders.end()
                                 , [_stage](const std::shared_ptr<MaterialPerPassShader>& _shader) { return _shader->GetStage() == _stage; });
        if (find == shaders.end())
            return nullptr;

        return find->get();
    }

    const std::vector<std::shared_ptr<MaterialPerPassShader>>& GetShaders() const { return shaders; }

private:
    std::vector<std::shared_ptr<MaterialPerPassShader>> shaders;

};

class MaterialPerPassShadersMap
{
public:
    MaterialPerPassShadersMap(DrawsInstance* _ins);
    bool Init(DrawsMaterial* _material);
    ~MaterialPerPassShadersMap();

    const ShaderPerStages* GetPerPass(RENDER_PASS_TYPE _pass_type) const;

private:
    bool CreateForDeferedRenderingShaders(buma::draws::DrawsMaterial* _material);

private:
    DrawsInstance*                                      ins;
    std::map<RENDER_PASS_TYPE, ShaderPerStages>         modules;

};


}// namespace draws
}// namespace buma
