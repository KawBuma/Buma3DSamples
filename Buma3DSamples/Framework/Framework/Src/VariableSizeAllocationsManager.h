#pragma once

namespace buma
{

class VariableSizeAllocationsManager
{
public:
    using OffsetT   = size_t;
    using SizeT     = size_t;
    struct ALLOCATION
    {
        OffsetT offset;
        SizeT   size;
    };

public:
    VariableSizeAllocationsManager(SizeT _page_size);
    VariableSizeAllocationsManager(const VariableSizeAllocationsManager&) = delete;
    ~VariableSizeAllocationsManager();

    void Reset();

    ALLOCATION Allocate (SizeT _size);
    void       Free     (ALLOCATION& _allocation);

    SizeT GetPageSize()      const { return page_size; }
    SizeT GetRemainingSize() const { return free_size; }

private:
    void AddNewBlock(OffsetT _offset, SizeT _size);

private:
    struct FREE_BLOCK_INFO;
    using MapFreeBlocksByOffset = std::map<OffsetT, FREE_BLOCK_INFO>;
    using MapFreeBlocksBySize   = std::multimap<SizeT, MapFreeBlocksByOffset::iterator>;

    struct FREE_BLOCK_INFO
    {
        OffsetT                       size;             // ブロックサイズ(割り当てサイズ用の予約スペースなし)
        MapFreeBlocksBySize::iterator it_order_by_size; // ブロックサイズでソートされたマルチマップでこのブロックを参照するイテレータ
    };

private:
    const SizeT             page_size;
    SizeT                   free_size;

    MapFreeBlocksByOffset   free_blocks_by_offset;
    MapFreeBlocksBySize     free_blocks_by_size;

};


}// namespace buma
