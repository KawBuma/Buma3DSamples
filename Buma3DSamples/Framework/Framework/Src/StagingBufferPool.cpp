#include "pch.h"
#include "StagingBufferPool.h"

#define BMTEXT (x)

namespace buma
{

#pragma region BufferPage

BufferPage::BufferPage(buma3d::IDevice* _device, size_t _page_size) 
    : resource(nullptr)
    , map_data_base_ptr(0)
    , gpu_virtual_address_base(0)
    , page_size(_page_size)
    , offset(0)
    , is_full(false)
{
    HRESULT hr;
    hr = _device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
                                         , D3D12_HEAP_FLAG_NONE
                                         , &CD3DX12_RESOURCE_DESC::Buffer(_page_size)
                                         , D3D12_RESOURCE_STATE_GENERIC_READ
                                         , nullptr
                                         , IID_PPV_ARGS(&resource)
    );
    ASSERT_HR(hr);
    resource->SetName((L"UploadBuffer size: " + std::to_wstring(_page_size)).c_str());

    hr = resource->Map(0, nullptr, &map_data_base_ptr);
    ASSERT_HR(hr);

    gpu_virtual_address_base = resource->GetGPUVirtualAddress();
}

BufferPage::~BufferPage()
{
    resource->Unmap(0, nullptr);
    map_data_base_ptr = nullptr;
    gpu_virtual_address_base = 0;
    page_size = 0;
    offset = 0;
}

void BufferPage::Reset() 
{
    offset = 0; 
}

bool BufferPage::CheckFreeSpace()
{
    if (page_size - offset < D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)
        is_full = true;

    return is_full;
}

bool BufferPage::CheckIsAllocatable(size_t _size_in_bytes, size_t _alignment)
{
    auto aligned_size    = Math::AlignUp(_size_in_bytes, _alignment);
    auto aligned_offset = Math::AlignUp(offset         , _alignment);

    // 作成したリソースのサイズを超えない場合true
    return (aligned_offset + aligned_size) <= page_size;
}

bool BufferPage::CheckIsAllocatableAligned(size_t _aligned_size_in_bytes, size_t _alignment)
{
    return (_aligned_size_in_bytes + Math::AlignUp(offset, _alignment)) <= page_size;
}

BUFFER_ALLOCATION_PART BufferPage::Allocate(size_t _size_in_bytes, size_t _aligned_size_in_bytes, size_t _alignment)
{
    //auto aligned_size    = Math::AlignUp(_size_in_bytes, _alignment);
    auto aligned_offset = Math::AlignUp(offset         , _alignment);

    //if (CheckIsAllocatableAligned(_size_in_bytes, _alignment) == false)
    //{
    //    // Can't allocate space from page.
    //    throw std::bad_alloc();
    //}

    BUFFER_ALLOCATION_PART alloc_part = {};

    std::lock_guard<std::mutex> allocate_guard(allocate_mutex);

    // サイズを超える場合nulptrを返す
    if (CheckIsAllocatableAligned(_aligned_size_in_bytes, aligned_offset))
    {
        offset = aligned_offset + _aligned_size_in_bytes;

        alloc_part.parent_resouce= resource.Get();
        alloc_part.map_data_part = static_cast<unsigned char*>(map_data_base_ptr) + aligned_offset;
        alloc_part.gpu_address      = gpu_virtual_address_base                          + aligned_offset;
        alloc_part.data_offset     = aligned_offset;
        alloc_part.size_in_bytes = _size_in_bytes;
        return alloc_part;
    }
    else
    {
        // 空き容量を確認する
        CheckFreeSpace();
        alloc_part.parent_resouce = resource.Get();
        alloc_part.map_data_part = nullptr;
        alloc_part.gpu_address      = 0;
        alloc_part.data_offset     = 0;
        alloc_part.size_in_bytes = 0;
        return alloc_part;
    }
}

BUFFER_ALLOCATION_PART BufferPage::AllocateUnsafe(size_t _size_in_bytes, size_t _aligned_size_in_bytes, size_t _alignment)
{
    //auto aligned_size    = Math::AlignUp(_size_in_bytes, _alignment);
    auto aligned_offset = Math::AlignUp(offset, _alignment);

    std::lock_guard<std::mutex> allocate_guard(allocate_mutex);

    offset = aligned_offset + _aligned_size_in_bytes;
    BUFFER_ALLOCATION_PART alloc_part =
    {
        resource.Get()
        ,static_cast<unsigned char*>(map_data_base_ptr) + aligned_offset
        , gpu_virtual_address_base                        + aligned_offset
        , aligned_offset
        , _size_in_bytes
    };
    return alloc_part;
}

#pragma endregion


#pragma region BufferPageAllocator

BufferPageAllocator::BufferPageAllocator(buma3d::IDevice* _device, size_t _size) 
    :device(_device), buffer_page_allocation_size(_size)
{
}

void BufferPageAllocator::Reset()
{
    if (buffer_pages.empty())
        return;

    for (auto&& i : buffer_pages)
        i->Reset();

    // available_buffer_pagesのbackを設定
    available_buffer_pages    = buffer_pages;
    main_buffer_page        = available_buffer_pages.back();
    available_buffer_pages.pop_back();
}

std::shared_ptr<BufferPage> BufferPageAllocator::MakeAndGetNewBufferPage()
{
    auto new_buffer_page = std::make_shared<BufferPage>(device.Get(), buffer_page_allocation_size);
    buffer_pages.emplace_back(std::move(new_buffer_page));
#ifdef _DEBUG
    BMTEXT("BufferPageAllocator - Allocated size: " + std::to_string(buffer_page_allocation_size) + ", this size total: " + std::to_string(buffer_pages.size()));
#endif // _DEBUG
    return buffer_pages.back();
}
void BufferPageAllocator::MakeNewBufferPage()
{
    auto new_buffer_page = std::make_shared<BufferPage>(device.Get(), buffer_page_allocation_size);
    buffer_pages.emplace_back(std::move(new_buffer_page));
}

BUFFER_ALLOCATION_PART BufferPageAllocator::Allocate(size_t _size_in_bytes, size_t _alignment)
{
    if (main_buffer_page == false)
        main_buffer_page = MakeAndGetNewBufferPage();

    auto aligned_size = Math::AlignUp(_size_in_bytes, _alignment);
    BUFFER_ALLOCATION_PART buffer_part = main_buffer_page->Allocate(_size_in_bytes, aligned_size, _alignment);

    if (buffer_part.map_data_part == nullptr)
    {
        // main_buffer_pageの空き容量がなくなった場合に最優先のページを変更する
        if (main_buffer_page->IsFull() == false)
        {
            ChangeMainPage(aligned_size, _alignment);
            buffer_part = main_buffer_page->AllocateUnsafe(_size_in_bytes, aligned_size, _alignment);
        }
        else
        {
            auto page = FindAllocatablePage(aligned_size, _alignment);
            buffer_part = page->AllocateUnsafe(_size_in_bytes, aligned_size, _alignment);
        }
    }

    return buffer_part;
}

std::shared_ptr<BufferPage> BufferPageAllocator::FindAllocatablePage(size_t _aligned_size_in_bytes, size_t _alignment)
{
    //size_t aligned_size = Math::AlignUp(_size_in_bytes, _alignment);

    for (auto&& i : available_buffer_pages)
        if (i->CheckIsAllocatableAligned(_aligned_size_in_bytes, _alignment) == true)
            return i;

    // 見つからなかった場合
    return MakeAndGetNewBufferPage();
}

void BufferPageAllocator::ChangeMainPage(size_t _aligned_size_in_bytes, size_t _alignment)
{
    //size_t aligned_size = Math::AlignUp(_size_in_bytes, _alignment);

    auto* pages = available_buffer_pages.data();
    auto  size    = available_buffer_pages.size();
    for (size_t i = 0; i < size; i++)
    {
        auto page = pages[i];
        if (page->CheckIsAllocatableAligned(_aligned_size_in_bytes, _alignment) == true)
        {
            main_buffer_page = page;
            EraseContainerElem(available_buffer_pages, i); // i番目をmain_buffer_pageに渡した後に除外(buffer_pagesが所有)
            return;
        }
    }

    // 見つからなかった場合
    main_buffer_page = MakeAndGetNewBufferPage();
}

#pragma endregion


#pragma region UploadBuffer


UploadBuffer::UploadBuffer(buma3d::IDevice* _device) :device(_device)
{
    int counter = 0;
    for (auto&& i : buffer_page_allocators)
    {
        auto page_size = GetPageSizeFromPoolIndex(counter);
        i = std::make_unique<BufferPageAllocator>(device.Get(), page_size);
        counter++;
    }
}

BUFFER_ALLOCATION_PART UploadBuffer::AllocateBufferPart(size_t _size_in_bytes, size_t _alignment)
{
    auto pool_size    = NextPow2(_size_in_bytes + _alignment);
    auto pool_index = GetPoolIndex(pool_size);

    auto& pool = buffer_page_allocators[pool_index];
    assert(pool != nullptr);

    return pool->Allocate(_size_in_bytes, _alignment);
}

BUFFER_ALLOCATION_PART UploadBuffer::AllocateConstantBufferPart(size_t _size_in_bytes)
{
    return AllocateBufferPart(_size_in_bytes, alignment_constant);
}

void UploadBuffer::ResetPages()
{
    for (auto&& i : buffer_page_allocators)
        i->Reset();
}

size_t UploadBuffer::NextPow2(size_t _x)
{
    // 例 x = 7
    _x--;// 6 (0110)
    _x |= _x >> 1;    // 0110 |= 0011 == 0111 (7)
    _x |= _x >> 2;    // 0111 |= 0001 == 0111 (7)
    _x |= _x >> 4;    // 0111 |= 0000 == 0111 (7)
    _x |= _x >> 8;    // 0111 |= 0000 == 0111 (7)
    _x |= _x >> 16;    // 0111 |= 0000 == 0111 (7)
#ifdef _WIN64
    _x |= _x >> 32;    // 0111 |= 0000 == 0111 (7)
#endif
    return ++_x;    // 7 (0111) + 1 == 1000 (8)
                    // のような感じで値は8になる
                    // 256も然り
}
size_t UploadBuffer::GetPoolIndexFromSize(size_t _x)
{
    // xが4096よりも小さい(x < 4096)の場合は0
    // xが4096よりも大きい(x >= 4096)の場合は1
    // xが4096*2よりも大きい(x >= 4096*2)の場合は2
    // xが4096*3よりも大きい(x >= 4096*3)の場合は3
    size_t allocator_page_size = _x >> allocator_index_shift;
    // gives a value from range:
    // 0 - sub-4k allocator
    // 1 - 4k allocator
    // 2 - 8k allocator
    // 4 - 16k allocator
    // etc...
    // Need to convert to an index.
    DWORD bit_index = 0;
    // マスク データの最下位ビット (LSB) から最上位ビット (MSB) に向かって設定済みビット (1) を検索します。
    // 00001000 の場合は4
    // 10000000 の場合は8
    //                       &結果     , 検索対象のビット   ) 対象が0001(1)だった場合0が返るので+1する(これ以降))
#ifdef _WIN64
    return _BitScanForward64(&bit_index, allocator_page_size) ? bit_index + 1 : 0;
#else
    return _BitScanForward(&bit_index, static_cast<DWORD>(allocator_page_size)) ? bit_index + 1 : 0;
#endif
}
size_t UploadBuffer::GetPoolIndex(size_t _x)
{
    return GetPoolIndexFromSize(NextPow2(_x));
}
size_t UploadBuffer::GetPageSizeFromPoolIndex(size_t _x)
{
    _x = (_x == 0) ? 0 : _x - 1; // clamp to zero
    return std::max<size_t>(min_page_size, size_t(1) << (_x + allocator_index_shift));
}


#pragma endregion


}// namespace buma
