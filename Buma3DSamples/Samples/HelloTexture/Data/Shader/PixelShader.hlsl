#include "./ShaderHeader.hlsli"

Texture2D<float4>   color_texture : register(t0);
SamplerState        color_sampler : register(s0);

[shader("pixel")]
float4 main(PS_IN _in) : SV_TARGET
{
    return color_texture.Sample(color_sampler, _in.uv);
    //return float4(_in.uv, 0., 1.);
}
