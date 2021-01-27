#pragma once

namespace buma
{
namespace draws
{

template<typename T>
inline void hash_combine(size_t& _seed, T const& _val)
{
    // https://suzulang.com/cpp-64bit-hash-combine/
    _seed ^= std::hash<T>{}(_val)+0x9e3779b97f4a7c15ull + (_seed << 12) + (_seed >> 4);
}

inline bool operator==(const MATERIAL_PARAMETERS_LAYOUT& _v1, const MATERIAL_PARAMETERS_LAYOUT& _v2)
{
    return _v1.value_parameters   == _v2.value_parameters   &&
           _v1.sampler_parameters == _v2.sampler_parameters &&
           _v1.texture_parameters == _v2.texture_parameters;
}


}// namespace draws
}// namespace buma



namespace std
{

template<>
struct hash<buma::draws::MATERIAL_PARAMETERS_LAYOUT>
{
    size_t operator()(const buma::draws::MATERIAL_PARAMETERS_LAYOUT& _data) const
    {
        size_t seed = 0;
        buma::draws::hash_combine(seed, _data.value_parameters);
        buma::draws::hash_combine(seed, _data.sampler_parameters);
        buma::draws::hash_combine(seed, _data.texture_parameters);

        return seed;
    }
};


}// namespace std
