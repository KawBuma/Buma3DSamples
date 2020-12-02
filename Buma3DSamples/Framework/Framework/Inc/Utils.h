#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include "Util/Buma3DPtr.h"

namespace buma
{
namespace util
{

inline std::string GetUUIDString(const uint8_t _uuid[16])
{
#define B3DFW std::setfill('0') << std::setw(2) 
    std::stringstream ss;
    ss << std::hex 
        << B3DFW << (uint32_t)_uuid[0]  << B3DFW << (uint32_t)_uuid[1] << B3DFW << (uint32_t)_uuid[2] << B3DFW << (uint32_t)_uuid[3] << "-"
        << B3DFW << (uint32_t)_uuid[4]  << B3DFW << (uint32_t)_uuid[5] << '-'
        << B3DFW << (uint32_t)_uuid[6]  << B3DFW << (uint32_t)_uuid[7] << '-'
        << B3DFW << (uint32_t)_uuid[8]  << B3DFW << (uint32_t)_uuid[9] << '-'
        << B3DFW << (uint32_t)_uuid[10] << B3DFW << (uint32_t)_uuid[11] << B3DFW << (uint32_t)_uuid[12] << B3DFW << (uint32_t)_uuid[13] << B3DFW << (uint32_t)_uuid[14] << B3DFW << (uint32_t)_uuid[15]
        << std::dec;
    return ss.str();
#undef B3DFW
}

inline std::string GetLUIDString(const uint8_t _luid[8])
{
#define B3DFW std::setfill('0') << std::setw(2) 
    std::stringstream ss;
    ss << std::hex
        << "Low: "    << B3DFW << (uint32_t)_luid[0] << B3DFW << (uint32_t)_luid[1] << B3DFW << (uint32_t)_luid[2] << B3DFW << (uint32_t)_luid[3]
        << ", High: " << B3DFW << (uint32_t)_luid[4] << B3DFW << (uint32_t)_luid[5] << B3DFW << (uint32_t)_luid[6] << B3DFW << (uint32_t)_luid[7]
        << std::dec;
    return ss.str();
#undef B3DFW
}

enum CODEPAGE : uint32_t
{
      CODEPAGE_ACP        = 0        // default to ANSI code page
    , CODEPAGE_OEMCP      = 1        // default to OEM  code page
    , CODEPAGE_MACCP      = 2        // default to MAC  code page
    , CODEPAGE_THREAD_ACP = 3        // current thread's ANSI code page
    , CODEPAGE_SYMBOL     = 42       // SYMBOL translations

    , CODEPAGE_UTF7       = 65000    // UTF-7 translation
    , CODEPAGE_UTF8       = 65001    // UTF-8 translation
};

std::string ConvertWideToCp(CODEPAGE _code_page /*= CP_UTF8*/, int _len_with_null_term, const wchar_t* _wstr);
std::wstring ConvertCpToWide(CODEPAGE _code_page /*= CP_UTF8*/, int _len_with_null_term, const char* _str);

inline std::string  ConvertWideToCp  (CODEPAGE _code_page, const std::wstring& _wstr)    { return ConvertWideToCp(_code_page, int(_wstr.size() + 1ull), _wstr.c_str()); }
inline std::wstring ConvertCpToWide  (CODEPAGE _code_page, const std::string& _str)      { return ConvertCpToWide(_code_page, int(_str.size() + 1ull), _str.c_str()); }

inline std::wstring ConvertUtf8ToWide(const char*          _str)                          { return ConvertCpToWide(CODEPAGE_UTF8, _str); }
inline std::wstring ConvertUtf8ToWide(const std::string&  _str)                           { return ConvertCpToWide(CODEPAGE_UTF8, int(_str.size() + 1ull), _str.c_str()); }
inline std::string  ConvertWideToUtf8(const wchar_t*       _wstr)                         { return ConvertWideToCp(CODEPAGE_UTF8, _wstr); }
inline std::string  ConvertWideToUtf8(const std::wstring& _wstr)                          { return ConvertWideToCp(CODEPAGE_UTF8, int(_wstr.size() + 1ull), _wstr.c_str()); }

inline std::wstring ConvertAnsiToWide(const char*          _str)                          { return ConvertCpToWide(CODEPAGE_ACP, _str); }
inline std::wstring ConvertAnsiToWide(const std::string&  _str)                           { return ConvertCpToWide(CODEPAGE_ACP, int(_str.size() + 1ull), _str.c_str()); }
inline std::string  ConvertWideToAnsi(const wchar_t*       _wstr)                         { return ConvertWideToCp(CODEPAGE_ACP, _wstr); }
inline std::string  ConvertWideToAnsi(const std::wstring& _wstr)                          { return ConvertWideToCp(CODEPAGE_ACP, int(_wstr.size() + 1ull), _wstr.c_str()); }

inline buma3d::UINT3 CalcMipExtents(uint32_t _mip_slice, const buma3d::EXTENT3D& _extent_mip0)
{
    return buma3d::UINT3{  (std::max)(_extent_mip0.width  >> _mip_slice, 1ui32)
                         , (std::max)(_extent_mip0.height >> _mip_slice, 1ui32)
                         , (std::max)(_extent_mip0.depth  >> _mip_slice, 1ui32) };
}

class Mapper
{
public:
    template<typename T>
    class MappedData
    {
    public:
        MappedData(T* _data) :data{ _data } {}
        ~MappedData() { data = nullptr; }

        T* GetData()
        {
            return data;
        }

        void Write(uint32_t _index, const T& _src)
        {
            if (data)
                data[_index] = _src;
        }
        bool WriteRaw(size_t _offset, const void* _src, size_t _size)
        {
            if (data)
                memcpy((uint8_t*)data + _offset, _src, _size);
        }

    private:
        T* data;

    };
    
public:
    Mapper(buma3d::util::Ptr<buma3d::IResourceHeap> _heap)
        : heap  { _heap }
        , range {}
        , data  {}
    {
        heap->Map();
        heap->GetMappedData(&range, &data);
    }

    Mapper(buma3d::util::Ptr<buma3d::IResourceHeap> _heap, const buma3d::MAPPED_RANGE& _range)
        : heap  { _heap }
        , range { _range }
        , data  {}
    {
        heap->Map(&range);
        heap->GetMappedData(&range, &data);
    }

    ~Mapper()
    {
        heap->Unmap();
        heap.Reset();
        range = {};
        data  = nullptr;
    }

    template<typename T>
    MappedData<T> As()
    {
        return MappedData<T>(reinterpret_cast<T*>(data));
    }

private:
    buma3d::util::Ptr<buma3d::IResourceHeap>    heap;
    buma3d::MAPPED_RANGE                        range;
    void*                                       data;

};

struct FENCE_VALUES
{
    uint64_t value;
    FENCE_VALUES& operator++()    { ++value; return *this; } 
    FENCE_VALUES  operator++(int) { auto tmp = *this; value++; return tmp; }
    uint64_t wait  () const { return value; }
    uint64_t signal() const { return value + 1; }
};

class FenceSubmitDesc
{
public:
    FenceSubmitDesc()
        : signal_desc  {}
        , wait_desc    {}
        , num_fences   {}
        , fences       {}
        , fence_values {}
    {

    }

    ~FenceSubmitDesc()
    {

    }

    void Reset()
    {
        num_fences = 0;

        signal_desc.signal_fence.num_fences     = 0;
        signal_desc.signal_fence.fences         = 0;
        signal_desc.signal_fence.fence_values   = 0;
        signal_desc.signal_fence_to_cpu = nullptr;

        wait_desc.wait_fence.num_fences         = 0;
        wait_desc.wait_fence.fences             = 0;
        wait_desc.wait_fence.fence_values       = 0;
    }

    void AddFence(buma3d::IFence* _fence, uint64_t _fence_value)
    {
        Resize(num_fences + 1);
        fences      .data()[num_fences] = _fence;
        fence_values.data()[num_fences] = _fence_value;
        num_fences++;
    }

    void SetSignalFenceToGpu(buma3d::IFence* _fence)
    {
        signal_desc.signal_fence_to_cpu = _fence;
    }

    const buma3d::SUBMIT_SIGNAL_DESC& GetAsSignal()
    {
        signal_desc.signal_fence.num_fences   = num_fences;
        signal_desc.signal_fence.fences       = fences.data();
        signal_desc.signal_fence.fence_values = fence_values.data();
        return signal_desc;
    }

    const buma3d::SUBMIT_WAIT_DESC& GetAsWait()
    {
        wait_desc.wait_fence.num_fences   = num_fences;
        wait_desc.wait_fence.fences       = fences.data();
        wait_desc.wait_fence.fence_values = fence_values.data();
        return wait_desc;
    }

private:
    void Resize(uint32_t _num_fences)
    {
        if (_num_fences > fences.size())
        {
            fences.resize(_num_fences);
            fence_values.resize(_num_fences);

            wait_desc.wait_fence.fences             = fences.data();
            wait_desc.wait_fence.fence_values       = fence_values.data();

            signal_desc.signal_fence.fences         = fences.data();
            signal_desc.signal_fence.fence_values   = fence_values.data();
        }
    }

private:
    buma3d::SUBMIT_SIGNAL_DESC      signal_desc;
    buma3d::SUBMIT_WAIT_DESC        wait_desc;
    uint32_t                        num_fences;
    std::vector<buma3d::IFence*>    fences;
    std::vector<uint64_t>           fence_values;

};

class TextureBarrierRange
{
public:
    TextureBarrierRange()
        : barrier_range{}
        , subres_ranges{}
    {
    }

    ~TextureBarrierRange()
    {
    }

    void AddSubresRange()
    {

    }

    void Reset()
    {
        barrier_range.texture                   = nullptr;
        barrier_range.num_subresource_ranges    = 0;
    }

    void AddSubresRange(buma3d::TEXTURE_ASPECT_FLAGS _aspect, uint32_t _mip_slice, uint32_t _array_slice, uint32_t _array_size = 1, uint32_t _mip_levels = 1)
    {
        Resize(barrier_range.num_subresource_ranges + 1);
        auto&& range = subres_ranges.data()[barrier_range.num_subresource_ranges++];
        range.array_size    = _array_size;
        range.mip_levels    = _mip_levels;
        range.offset.aspect         = _aspect;
        range.offset.mip_slice      = _mip_slice;
        range.offset.array_slice    = _array_slice;
    }

    const buma3d::TEXTURE_BARRIER_RANGE* Get(buma3d::ITexture* _texture)
    {
        barrier_range.texture = _texture;
        return &barrier_range;
    }

private:
    void Resize(uint32_t _num_subres_ranges)
    {
        if (_num_subres_ranges > barrier_range.num_subresource_ranges)
        {
            subres_ranges.resize(_num_subres_ranges);
            barrier_range.subresource_ranges = subres_ranges.data();
        }
    }

    buma3d::TEXTURE_BARRIER_RANGE           barrier_range;
    std::vector<buma3d::SUBRESOURCE_RANGE>  subres_ranges;

};


class PipelineBarrierDesc
{
public:
    PipelineBarrierDesc()
        : barrier           {}
        , buffer_barreirs   {}
        , texture_barreirs  {}
    {}
    ~PipelineBarrierDesc() {}

    void Reset()
    {
        barrier.num_buffer_barriers  = 0;
        barrier.num_texture_barriers = 0;
        barrier.dependency_flags     = buma3d::DEPENDENCY_FLAG_NONE;
    }

    void AddBufferBarrier(const buma3d::BUFFER_BARRIER_DESC& _buffer_barrier)
    {
        Resize(barrier.num_buffer_barriers + 1, barrier.buffer_barriers, &buffer_barreirs);
        buffer_barreirs.data()[barrier.num_buffer_barriers++] = _buffer_barrier;
    }

    void AddTextureBarrier(const buma3d::TEXTURE_BARRIER_DESC& _texture_barrier)
    {
        Resize(barrier.num_texture_barriers + 1, barrier.texture_barriers, &texture_barreirs);
        texture_barreirs.data()[barrier.num_texture_barriers++] = _texture_barrier;
    }

    const buma3d::CMD_PIPELINE_BARRIER& Get(buma3d::PIPELINE_STAGE_FLAGS _src_stages, buma3d::PIPELINE_STAGE_FLAGS _dst_stages, buma3d::DEPENDENCY_FLAGS _dependency_flags = buma3d::DEPENDENCY_FLAG_NONE)
    {
        barrier.src_stages       = _src_stages;
        barrier.dst_stages       = _dst_stages;
        barrier.dependency_flags = _dependency_flags;
        return barrier;
    }

private:
    template<typename T>
    void Resize(uint32_t _num, const T*& _ptr, std::vector<T>* _dst)
    {
        if (_num > _dst->size())
        {
            _dst->resize(_num);
            _ptr = _dst->data();
        }
    }

    buma3d::CMD_PIPELINE_BARRIER                    barrier;
    std::vector<buma3d::BUFFER_BARRIER_DESC>        buffer_barreirs;
    std::vector<buma3d::TEXTURE_BARRIER_DESC>       texture_barreirs;

};


#pragma region containerhelper

template <typename T>
inline void SwapClear(T& _container)
{
    { T().swap(_container); }
}

template <typename T>
inline typename T::iterator EraseContainerElem(T& _container, const size_t _erase_pos)
{
    return _container.erase(_container.begin() + _erase_pos);
}

// _first_pos: 0~, _last_pos: _container.size()までの間で設定してください
template <typename T>
inline typename T::iterator EraseContainerRange(T& _container, const size_t _first_pos, const size_t _last_pos)
{
    typename T::const_iterator it = _container.begin();
    return _container.erase(it + _first_pos, it + _last_pos);
}

template <typename T>
inline typename T::iterator InsertContainerElem(T& _container, const size_t _insert_pos, const typename T::value_type& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _value);
}

template <typename T>
inline typename T::iterator InsertContainerElem(T& _container, const size_t _insert_pos, typename T::value_type&& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _value);
}

template <typename T>
inline typename T::iterator InsertContainerElemCount(T& _container, const size_t _insert_pos, const size_t _insert_count, const typename T::value_type& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _insert_count, _value);
}

template <typename T>
inline typename T::iterator InsertContainerElemCount(T& _container, const size_t _insert_pos, const size_t _insert_count, typename T::value_type&& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _insert_count, _value);
}

// _insert_first: 0 ~ _insert_container.size()までの間で設定してください
// _insert_last: 0 ~ _insert_container.size()までの間で設定してください
// _insert_firstと _insert_lastが同じの場合要素は挿入されません
template <typename T>
inline typename T::iterator InsertContainerElemRange(T& _container, const size_t _insert_pos, T& _insert_container, const size_t _insert_first, const size_t _insert_last)
{
    typename T::iterator ins_it = _insert_container.begin();
    return _container.insert(_container.begin() + _insert_pos, ins_it + _insert_first, ins_it + _insert_last);
}

#pragma endregion

#pragma region valhelper

template <typename T>
inline constexpr T AlignUpWithMask(T _value, size_t _mask)
{
    return static_cast<T>((static_cast<size_t>(_value) + _mask) & ~_mask);
}

template <typename T>
inline constexpr T AlignDownWithMask(T _value, size_t _mask)
{
    return static_cast<T>(static_cast<size_t>(_value) & ~_mask);
}

template <typename T>
inline constexpr T AlignUp(T _value, size_t _alignment)
{
    return AlignUpWithMask(_value, _alignment - 1);
}

template <typename T>
inline constexpr T AlignDown(T _value, size_t _alignment)
{
    return AlignDownWithMask(_value, _alignment - 1);
}

template <typename T>
inline constexpr bool IsAligned(T _value, size_t _alignment)
{
    return (static_cast<size_t>(_value) & (_alignment - 1)) == 0;
}

template <typename T>
inline constexpr T DivideByMultiple(T _value, size_t _alignment)
{
    return static_cast<T>((static_cast<size_t>(_value) + _alignment - 1) / _alignment);
}

template<typename T, typename RetT = size_t>
inline constexpr RetT Get32BitValues()
{
    return AlignUp(sizeof(T), 4) / 4;
}

uint8_t Buma3DBitScanForward(unsigned long* _result_index, uint32_t _bitmask);
uint8_t Buma3DBitScanForward(unsigned long* _result_index, uint64_t _bitmask);
uint8_t Buma3DBitScanReverse(unsigned long* _result_index, uint32_t _bitmask);
uint8_t Buma3DBitScanReverse(unsigned long* _result_index, uint64_t _bitmask);

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint32_t), int> = 0>
inline int GetFirstBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanForward(&index, static_cast<uint32_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint64_t), int> = 0>
inline int GetFirstBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanForward(&index, static_cast<uint64_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint32_t), int> = 0>
inline int GetLastBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanReverse(&index, static_cast<uint32_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint64_t), int> = 0>
inline int GetLastBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanReverse(&index, static_cast<uint64_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template<typename T>
inline T Log2(T _value)
{
    if (!_value) return 0;

    int mssb = GetLastBitIndex(_value);  // most significant set bit
    int lssb = GetFirstBitIndex(_value); // least significant set bit
    if (mssb == -1 || lssb == -1)
        return 0;

    // 2の累乗（1セットビットのみ）の場合、ビットのインデックスを返します。
    // それ以外の場合は、最上位のセットビットのインデックスに1を加算して、小数ログを切り上げます。
    return static_cast<T>(mssb) + static_cast<T>(mssb == lssb ? 0 : 1);
}

template<typename T>
inline constexpr T Log2Cexpr(T _value)
{
    if (!_value) return 0;
    int mssb = 0, lssb = 0, cnt = 0;

    cnt = (sizeof(T) * 8) - 1;
    while (cnt != -1) { if (_value & static_cast<T>(1ull << cnt)) break; cnt--; }
    mssb = cnt;

    cnt = 0;
    while (cnt < sizeof(T) * 8) { if (_value & static_cast<T>(1ull << cnt)) break; cnt++; }
    lssb = cnt;

    return static_cast<T>(mssb) + static_cast<T>(mssb == lssb ? 0 : 1);
}

template <typename T>
inline T NextPow2(T _value)
{
    return _value == 0 ? 0 : 1 << Log2(_value);
}

template <typename T>
inline bool IsPowOfTwo(T _value)
{
    return _value > 0 && (_value & (_value - 1)) == 0;
}

inline constexpr size_t Kib(size_t _x) { return 1024 * _x; }
inline constexpr size_t Mib(size_t _x) { return Kib(1024) * _x; }
inline constexpr size_t Gib(size_t _x) { return Mib(1024) * _x; }


#pragma endregion valhelper

inline bool IsSucceeded(buma3d::BMRESULT _bmr)
{
    return _bmr < buma3d::BMRESULT_FAILED;
}

inline bool IsFailed(buma3d::BMRESULT _bmr)
{
    return _bmr >= buma3d::BMRESULT_FAILED;
}

}// namespace util
}// namespace buma
