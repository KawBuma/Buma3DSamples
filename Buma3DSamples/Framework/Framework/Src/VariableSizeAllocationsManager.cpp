#include "pch.h"
#include "VariableSizeAllocationsManager.h"

namespace buma
{

VariableSizeAllocationsManager::VariableSizeAllocationsManager(SizeT _page_size)
    : page_size             { _page_size }
    , free_size             { _page_size }
    , free_blocks_by_offset {}
    , free_blocks_by_size   {}
{
    AddNewBlock(0, page_size);
}

VariableSizeAllocationsManager::~VariableSizeAllocationsManager()
{
    assert(page_size == free_size && "VariableSizeAllocationsManager: memory leaked");
}

void VariableSizeAllocationsManager::Reset()
{
    free_size = page_size;
    free_blocks_by_offset.clear();
    free_blocks_by_size.clear();
}

VariableSizeAllocationsManager::ALLOCATION
VariableSizeAllocationsManager::Allocate(SizeT _size)
{
    // 割当可能かをチェック
    if (_size == 0 || _size > free_size)
        return ALLOCATION{};

    auto&& it_size_capable_blocks = free_blocks_by_size.lower_bound(_size);
    if (it_size_capable_blocks == free_blocks_by_size.end())
        return ALLOCATION{};

    // ブロックの情報を取り出す
    auto&& size_capable_blocks = *it_size_capable_blocks;
    auto&& it_size_capable_block = size_capable_blocks.second;
    auto block_offset = size_capable_blocks.first;
    auto block_size   = it_size_capable_block->first;

    ALLOCATION result{ block_offset, block_size };

    // ブロックの情報を更新
    free_size -= _size;
    free_blocks_by_size  .erase(it_size_capable_blocks);
    free_blocks_by_offset.erase(it_size_capable_block);
    block_offset += _size;
    block_size   -= _size;
    if (block_size > 0)
        AddNewBlock(block_offset, block_size);

    return result;
}

void
VariableSizeAllocationsManager::Free(ALLOCATION& _allocation)
{
    assert(_allocation.size);

    if (free_blocks_by_offset.empty())
    {
        free_size += _allocation.size;
        AddNewBlock(_allocation.offset, _allocation.size);
        return;
    }

    MapFreeBlocksByOffset::iterator it_next_block = free_blocks_by_offset.upper_bound(_allocation.offset);
    OffsetT offset = _allocation.offset;
    SizeT   size   = _allocation.size;

    if (it_next_block == free_blocks_by_offset.begin())
    {
        // begin()の場合it_prev_blockは存在しない
    }
    else
    {
        MapFreeBlocksByOffset::iterator it_prev_block = it_next_block;
        --it_prev_block;
        auto&& prev_block = *it_prev_block;
        if (_allocation.offset == prev_block.first + prev_block.second.size)
        {
            // prev_blockと隣接している
            // |prev_block_offset            |_allocation.offset            |~~
            // |<------prev_block_size------>|<------_allocation.size------>|~~
            offset = prev_block.first;
            size  += prev_block.second.size;
            free_blocks_by_size.erase(prev_block.second.it_order_by_size);
            free_blocks_by_offset.erase(it_prev_block);
        }
    }

    auto&& next_block = *it_next_block;
    if (_allocation.offset + _allocation.size == next_block.first)
    {
        // next_blockと隣接している
        // ~~|_allocation.offset            |next_block.first
        // ~~|<------_allocation.size------>|<-----next_block.second.size----->|
        size += next_block.second.size;
        free_blocks_by_size.erase(next_block.second.it_order_by_size);
        free_blocks_by_offset.erase(it_next_block);
    }

    // こんな場合は結合出来ない
    // |prev_block.first                  |使用中  |_allocation.offset              |使用中  |next_block.first                  |
    // |<-----prev_block.second.size----->|<xxxxxx>|<------_allocation.size-------->|<xxxxxx>|<-----next_block.second.size----->|

    free_size += size;
    AddNewBlock(offset, size);
}

void VariableSizeAllocationsManager::AddNewBlock(OffsetT _offset, SizeT _size)
{
    auto&& it_new_block_offset = free_blocks_by_offset.emplace(_offset, FREE_BLOCK_INFO{ _size, {} }).first;
    auto&& it_new_block_size   = free_blocks_by_size.emplace(_size, it_new_block_offset);
    it_new_block_offset->second.it_order_by_size = it_new_block_size;
}


}// namespace buma
