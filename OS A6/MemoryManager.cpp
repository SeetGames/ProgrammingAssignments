#include "MemoryManager.h"
#include <iomanip>
#include <cstdlib>
#include <cstdio>

// Constructor for the MemCtrlBlk struct
MemoryManager::MemCtrlBlk::MemCtrlBlk(void* ptr, int s) : data(ptr), size(s), allocated(false) {}

// Merge function for the MemCtrlBlk struct
void MemoryManager::MemCtrlBlk::Merge(MemCtrlBlk other) {
    // Assuming that 'other' is immediately after 'this' and that they are both free (not allocated)
    size += other.size;
    // You might also want to do additional work here, like updating pointers, etc.
}


MemoryManager::MemoryManager(int total_bytes)
{
    mempool = ::operator new(total_bytes);
    memoryBlock.emplace_back(mempool, total_bytes);
}

MemoryManager::~MemoryManager(void)
{
    ::operator delete(mempool);
}

void *MemoryManager::allocate(int bytes)
{
    for (auto blockIt = memoryBlock.begin(); blockIt != memoryBlock.end(); ++blockIt) {
    // Check if the block is not allocated and is large enough
    if (!blockIt->allocated && blockIt->size >= bytes) {
      void* allocatedMemory = blockIt->data;

      // If the block is larger than needed, split it
      if (blockIt->size > bytes) {
        void *newBlockData = static_cast<char*>(blockIt->data) + bytes;
        int newSize = blockIt->size - bytes;

        // Update the size of the current block
        blockIt->size = bytes;

        // Create a new control block for the remaining memory
        MemCtrlBlk newBlock(newBlockData, newSize);

        // Insert the new block after the current block
        auto insertPos = std::next(blockIt);
        memoryBlock.insert(insertPos, newBlock);
      }

      // Mark the block as allocated
      blockIt->allocated = true;

      // Return the pointer to the allocated memory
      return allocatedMemory;
    }
  }
  // No suitable block found, return nullptr
  return nullptr;
}

void MemoryManager::deallocate(void *pointer)
{
    for (auto it = memoryBlock.begin(); it != memoryBlock.end(); ++it)
    {
        if (it->data == pointer && it->allocated)
        {
            it->allocated = false;

            // Merge with next block if it's free
            auto next = std::next(it);
            if (next != memoryBlock.end() && !next->allocated)
            {
                it->Merge(*next);
                memoryBlock.erase(next);
            }

            // Merge with previous block if it's free
            if (it != memoryBlock.begin())
            {
                auto prev = std::prev(it);
                if (!prev->allocated)
                {
                    prev->Merge(*it);
                    memoryBlock.erase(it);
                }
            }
            break;
        }
    }
}

void MemoryManager::dump(std::ostream &out)
{
    for (const auto &block : memoryBlock)
    {
        out << "relative to start address: "
            << std::hex << (static_cast<char *>(block.data) - static_cast<char *>(mempool))
            << "\nbyte count: " << std::hex << block.size
            << "\nallocated? " << (block.allocated ? "true" : "false")
            << std::endl;
    }
}