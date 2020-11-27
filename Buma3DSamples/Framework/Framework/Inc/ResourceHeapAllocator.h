#pragma once

namespace buma
{

class ResourceHeapAllocationPage;
struct RESOURCE_HEAP_ALLOCATION
{
    operator bool() const { return parent_page; }
    ResourceHeapAllocationPage* parent_page;
    size_t                      alignment;
    size_t                      non_aligned_offset;
    size_t                      aligned_offset() const { return util::AlignUp(non_aligned_offset, alignment); };
    size_t                      aligned_size;
};

class ResourceHeapAllocator;
class ResourceHeapsAllocator;

class ResourceHeapAllocationPage
{
public:
    struct RESOURCE_HEAP_PAGE_DESC
    {
        size_t page_size;
        size_t heap_index;
    };

public:
    ResourceHeapAllocationPage(ResourceHeapAllocator& _owner, const RESOURCE_HEAP_PAGE_DESC& _desc);
    ResourceHeapAllocationPage(const ResourceHeapAllocationPage&) = delete;
    ~ResourceHeapAllocationPage();

    RESOURCE_HEAP_ALLOCATION Allocate(size_t _size, size_t _alignment);
    void Free(RESOURCE_HEAP_ALLOCATION& _allocation);

private:
    ResourceHeapAllocator&                          owner;
    RESOURCE_HEAP_PAGE_DESC                         desc;
    std::unique_ptr<VariableSizeAllocationsManager> allocation;
    buma3d::util::Ptr<buma3d::IResourceHeap>        heap;

};

class ResourceHeapAllocator
{
    friend class ResourceHeapAllocationPage;
public:
    ResourceHeapAllocator(ResourceHeapsAllocator& _owner);
    ResourceHeapAllocator(const ResourceHeapAllocator&) = delete;
    ~ResourceHeapAllocator();

private:
    ResourceHeapsAllocator&                                  owner;
    std::vector<std::unique_ptr<ResourceHeapAllocationPage>> pages;

};

class ResourceHeapsAllocator
{
    friend class ResourceHeapAllocationPage;
    friend class ResourceHeapAllocator;

    template<typename T>
    static constexpr T Log2(T _value)
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

public:
#pragma region constexpr
    static constexpr size_t MIN_PAGE_SIZE           = util::Mib(128);
    static constexpr size_t ALLOCATOR_INDEX_SHIFT   = Log2<size_t>(MIN_PAGE_SIZE);
    static constexpr size_t ALLOCATOR_POOL_COUNT    = sizeof(size_t) * 8 - ALLOCATOR_INDEX_SHIFT;

    static_assert((MIN_PAGE_SIZE & (MIN_PAGE_SIZE - 1)) == 0, "min_page_size size must be a power of 2");
#pragma endregion

public:
    ResourceHeapsAllocator(buma3d::IDeviceAdapter* _adapter, buma3d::IDevice* _device);
    ResourceHeapsAllocator(const ResourceHeapsAllocator&) = delete;
    ~ResourceHeapsAllocator();

private:
    using HeapAllocationsByType = std::array<std::unique_ptr<ResourceHeapAllocator>, /*heap type bitsで表現可能な種類の最大数*/32>;
    using HeapAllocationsBySize = std::array<std::unique_ptr<HeapAllocationsByType>, ALLOCATOR_POOL_COUNT>;
    buma3d::util::Ptr<buma3d::IDevice>  device;
    HeapAllocationsBySize               allocations;
    buma3d::DEVICE_ADAPTER_LIMITS       limits;

};


}// namespace buma
