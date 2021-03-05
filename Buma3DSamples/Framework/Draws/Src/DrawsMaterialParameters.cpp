#include "pch.h"
#include "DrawsMaterialParameters.h"

namespace buma
{
namespace draws
{

DrawsMaterialParameters::DrawsMaterialParameters()
    : ref_count {}
    , ins       {}
    , type      {}
    , value     {}
    , sampler   {}
    , texture   {}
    , layout    {}
{

}

DrawsMaterialParameters::~DrawsMaterialParameters()
{

}

bool DrawsMaterialParameters::Init(DrawsInstance* _ins, const MATERIAL_PARAMETERS_CREATE_DESC& _desc)
{
    ins = _ins;
    type = _desc.type;

    auto CopyArray = [&](auto&& _p, auto&& _parameters)
    {
        _p = std::make_unique<std::remove_reference_t<decltype(_p)>::element_type>();
        _p->resize(_desc.num_parameters);
        memcpy(_p->data(), _parameters, sizeof(std::remove_pointer_t<decltype(_parameters)>) * _desc.num_parameters);
    };
    switch (type)
    {
    case buma::draws::MATERIAL_PARAMETER_TYPE_VALUE:
        CopyArray(value, _desc.parameters.value);
        break;
    case buma::draws::MATERIAL_PARAMETER_TYPE_SAMPLER:
        CopyArray(sampler, _desc.parameters.sampler);
        break;
    case buma::draws::MATERIAL_PARAMETER_TYPE_TEXTURE:
        CopyArray(texture, _desc.parameters.texture);
        break;
    default:
        return false;
    }

    if (!CreateDescriptorSetLayout())
        return false;

    return true;
}

bool DrawsMaterialParameters::CreateDescriptorSetLayout()
{
    util::DescriptorSetLayoutDesc layout_desc;
    auto AddRanges = [&](buma3d::DESCRIPTOR_TYPE _type, auto&& _value, buma3d::DESCRIPTOR_FLAGS _flags)
    {
        uint32_t register_offset = 0;
        for (auto& i : *_value)
        {
            // i.visibility;
            // i.array_size;
            layout_desc.AddNewBinding(_type, register_offset, 1, buma3d::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE, _flags);
            register_offset += 1;
        }
    };
    switch (type)
    {
    case buma::draws::MATERIAL_PARAMETER_TYPE_VALUE:
        AddRanges(buma3d::DESCRIPTOR_TYPE_SRV_BUFFER, value, buma3d::DESCRIPTOR_FLAG_NONE);
        break;
    case buma::draws::MATERIAL_PARAMETER_TYPE_SAMPLER:
        AddRanges(buma3d::DESCRIPTOR_TYPE_SAMPLER, sampler, buma3d::DESCRIPTOR_FLAG_NONE);
        break;
    case buma::draws::MATERIAL_PARAMETER_TYPE_TEXTURE:
        AddRanges(buma3d::DESCRIPTOR_TYPE_SRV_TEXTURE, texture, buma3d::DESCRIPTOR_FLAG_NONE);
        break;
    default:
        break;
    }
    layout_desc.SetFlags(buma3d::DESCRIPTOR_SET_LAYOUT_FLAG_NONE);
    layout_desc.Finalize();

    auto bmr = ins->GetDR()->GetDevice()->CreateDescriptorSetLayout(layout_desc.Get(), &layout);
    BMR_ASSERT(bmr);
    if (util::IsFailed(bmr))
        return false;

    return true;
}

bool DrawsMaterialParameters::Create(DrawsInstance* _ins, const MATERIAL_PARAMETERS_CREATE_DESC& _desc, IDrawsMaterialParameters** _dst)
{
    auto p = new DrawsMaterialParameters();
    if (!p->Init(_ins, _desc))
    {
        p->Release();
        return false;
    }

    *_dst = p;
    return true;
}

void DrawsMaterialParameters::AddRef()
{
    ++ref_count;
}

uint32_t DrawsMaterialParameters::Release()
{
    uint32_t result = --ref_count;
    if (result == 0)
        delete this;
    return result;
}


}// namespace buma
}// namespace draws
