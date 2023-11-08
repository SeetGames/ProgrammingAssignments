#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__
#include <iostream>
#include <list>

class MemoryManager 
{
private:
  void* mempool;
  
  struct MemCtrlBlk
  {
    void* data;
    int size;
    bool allocated;
    MemCtrlBlk(void* ptr, int size);
    void Merge(MemCtrlBlk other);
  };

  std::list<MemCtrlBlk> memoryBlock;

public:
  MemoryManager(int total_bytes);
  ~MemoryManager(void);
  void *allocate(int bytes);
  void deallocate(void *pointer);
  void dump(std::ostream& out);
};

#endif

