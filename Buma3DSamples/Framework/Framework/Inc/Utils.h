#pragma once

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


class Mapper
{
public:
    template<typename T>
    class MappedData
    {
    public:
        MappedData(T* _data) :data{ _data } {}
        ~MappedData() { data = nullptr; }

        void Write(uint32_t _index, const T& _src)
        {
            if (data)
                data[_index] = _src;
        }
        bool WriteRaw(const void* _src, size_t _size)
        {
            if (data)
                memcpy((void*)data, _src, _size);
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
    MappedData<T> Get()
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


}// namespace util
}// namespace buma
