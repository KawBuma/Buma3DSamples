#pragma once

namespace buma
{

// 単一リソースのデータの一部
struct BUFFER_ALLOCATION_PART
{
    buma3d::IBuffer*            parent_resouce; // マップしたあるリソース
    void*                       map_data_part;  // マップしたあるリソースのデータの一部(バイト単位でオフセットされた)
    buma3d::GpuVirtualAddress   gpu_address;    // マップしたあるリソースのGPU仮想アドレス(バイト単位でオフセットされたアドレス)
    size_t                      data_offset;    // マップしたあるリソースのデータのアドレスオフセット
    size_t                      size_in_bytes;  // バイト数
};

class BufferPage
{
public:
    BufferPage(buma3d::IDevice* _device, size_t _page_size);
    ~BufferPage();

    void                    Reset                       ();

    bool                    CheckFreeSpace              ();                                                                 // 空き容量が256byte以下の場合ページの割当を止めます
    bool                    CheckIsAllocatable          (size_t _size_in_bytes, size_t _alignment);           // 引数を条件にサイズを確保できるか確認します
    bool                    CheckIsAllocatableAligned   (size_t _aligned_size_in_bytes, size_t _alignment);   // アライメントされたサイズを確保できるか確認します

    BUFFER_ALLOCATION_PART  Allocate                    (size_t _size_in_bytes, size_t _aligned_size_in_bytes, size_t _alignment);
    BUFFER_ALLOCATION_PART  AllocateUnsafe              (size_t _size_in_bytes, size_t _aligned_size_in_bytes, size_t _alignment);

    size_t                  GetPageSize                 () const { return page_size; }
    size_t                  GetOffset                   () const { return offset; }
                                                           
    bool                    IsFull                      () const { return is_full; }

private:
    buma3d::util::Ptr<buma3d::IResource>    resource;
    void*                                   map_data_base_ptr;          // mapしたデータのベースポインタ
    buma3d::GpuVirtualAddress               gpu_virtual_address_base;   // GPU仮想アドレスの先頭 

    size_t                                  page_size;                  // 作成するリソースのサイズ
    size_t                                  offset;                     // Allocateした際に進めるオフセット

    bool                                    is_full;                    // almost full
    std::mutex                              allocate_mutex;

};

class BufferPageAllocator
{
public:
    BufferPageAllocator(buma3d::IDevice* _device, const size_t& _size);
    ~BufferPageAllocator() { Reset(); }

    void Reset();

    // 新しいページ(ID3D12Resource)を作成します
    std::shared_ptr<BufferPage> MakeAndGetNewBufferPage();
    // 新しいページ(ID3D12Resource)を作成します
    void                        MakeNewBufferPage();
    // 指定サイズを確保可能なページを返します
    std::shared_ptr<BufferPage> FindAllocatablePage     (size_t _aligned_size_in_bytes, size_t _alignment);
    // main_buffer_pageを入れ替えます                                                          
    void                        ChangeMainPage          (size_t _aligned_size_in_bytes, size_t _alignment);
    // 指定サイズの領域を割り当たBUFFER_ALLOCATION_PARTを返します
    BUFFER_ALLOCATION_PART      Allocate                (size_t _size_in_bytes, size_t _alignment);

    const size_t                GetPageCount            () const { return buffer_pages.size(); }
    const size_t                GetTotalBufferSize      () const { return buffer_page_allocation_size * buffer_pages.size(); }

    bool                        IsAllocated             () const { return main_buffer_page.operator bool(); }

private:
    buma3d::util::Ptr<buma3d::IDevice>          device;
    std::vector<std::shared_ptr<BufferPage>>    buffer_pages;
    std::vector<std::shared_ptr<BufferPage>>    available_buffer_pages;
    std::shared_ptr<BufferPage>                 main_buffer_page;

    const size_t                                buffer_page_allocation_size;

};


class UploadBuffer
{
public:
    static constexpr size_t alignment_min           = 4;            // 4byte (頂点バッファなど)
    static constexpr size_t alignment_default       = 16;           // 4 * 4 byte float4
    static constexpr size_t alignment_constant      = 256;          // 256byte (定数バッファはリソースのデータ先頭アドレスから数えて256バイトでアライメントされた場所に設定しなければならない)

    static constexpr size_t min_page_size           = 64 * 1024;    
    static constexpr size_t min_alloc_size          = 4 * 1024;
    static constexpr size_t allocator_index_shift   = 12;           // start block sizes at 4KB
    static constexpr size_t allocator_pool_count    = 21;           // allocation sizes up to 2GB supported
    static constexpr size_t pool_index_scale        = 1;            // multiply the allocation size this amount to push large values into the next bucket

    static_assert((1 << allocator_index_shift) == min_alloc_size, "1 << allocator_index_shift must == min_page_size (in KiB)");
    static_assert((min_page_size & (min_page_size - 1)) == 0    , "min_page_size size must be a power of 2");
    static_assert((min_alloc_size & (min_alloc_size - 1)) == 0  , "min_alloc_size size must be a power of 2");
    static_assert(min_alloc_size >= (4 * 1024)                  , "min_alloc_size size must be greater than 4K");

private:
    // 受け取った引数を その値以上の ( x <= 返却値 ) 2の乗数刻みの値にすることができる
    // 例えば xが7だった場合は8に
    // 256の場合は256 256+1の場合は512になる
    size_t NextPow2(size_t _x);
    size_t GetPoolIndexFromSize(size_t _x);
    size_t GetPoolIndex(size_t _x);
    size_t GetPageSizeFromPoolIndex(size_t _x);
public:

    UploadBuffer(buma3d::IDevice* _device);
    ~UploadBuffer() 
    {
        ResetPages();
    }

    BUFFER_ALLOCATION_PART AllocateBufferPart(size_t _size_in_bytes, size_t _alignment = alignment_default);
    BUFFER_ALLOCATION_PART AllocateConstantBufferPart(size_t _size_in_bytes);

    void ResetPages();
    buma3d::util::Ptr<buma3d::IDevice> GetDevice() { return device; }

private:
    buma3d::util::Ptr<buma3d::IDevice>      device;
    std::unique_ptr<BufferPageAllocator>    buffer_page_allocators[allocator_pool_count];

};


}// namespace buma
