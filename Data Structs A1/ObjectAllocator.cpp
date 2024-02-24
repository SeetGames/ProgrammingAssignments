/*!*************************************************************************
\file ObjectAllocator.cpp
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 1
\date 01-22-2024
\brief
This file contains the implementation for the ObjectAllocator
***************************************************************************/
#include <cstdint>   // size_t 
#include <cstring>   // strlen, memset
#include <cstdlib>   // abs
#include "ObjectAllocator.h"

using word_t = intptr_t ;

#define PTR_SIZE sizeof(word_t)      //! Size of a pointer.
#define INCREMENT_PTR(ptr) (ptr + 1) //! Move the pointer by one

constexpr size_t operator "" _z(unsigned long long n)
{
    return static_cast<size_t>(n);
}

inline size_t align(size_t n, size_t align)
{
    if (!align)
        return n;
    size_t remainder = n % align == 0 ? 0_z : 1_z;
    return align * ((n / align) + remainder);
}

MemBlockInfo::MemBlockInfo(unsigned _allocNum, const char* _label) 
    : in_use(true), label(nullptr), alloc_num(_allocNum)
{
    if (_label != nullptr)
    {
        size_t _labelLength = strlen(_label) + 1; // Including null terminator
        label = new char[_labelLength];
        strcpy(label, _label);
    }
}

MemBlockInfo::~MemBlockInfo()
{
    delete[] label;
}

ObjectAllocator::ObjectAllocator(size_t _objectSize, const OAConfig &_config) 
    : stats(), configuration(_config), headerSize(0), dataSize(0), totalDataSize(0)
{
    // Initialize configuration settings for the Object Allocator
    InitializeOAConfig();
    // Calculate the header size with padding and alignment taken into account
    this->headerSize = align(PTR_SIZE + _config.HBlockInfo_.size_ + _config.PadBytes_, _config.Alignment_);
    // Calculate the data size per object, including padding and alignment
    this->dataSize = align(_objectSize + _config.PadBytes_ * 2_z + _config.HBlockInfo_.size_, _config.Alignment_);
    // Set the object size and page size in the statistics
    this->stats.ObjectSize_ = _objectSize;
    this->stats.PageSize_ = this->headerSize + this->dataSize * (_config.ObjectsPerPage_ - 1) + _objectSize + _config.PadBytes_;
    // Total data size is calculated considering all objects on a page
    this->totalDataSize = this->dataSize * (_config.ObjectsPerPage_ - 1) + _objectSize + _config.PadBytes_;
    // Inter-block alignment size calculation to ensure proper spacing between objects
    size_t _interSize = _objectSize + this->configuration.PadBytes_ * 2_z + static_cast<size_t>(this->configuration.HBlockInfo_.size_);
    this->configuration.InterAlignSize_ = static_cast<unsigned>(align(_interSize, this->configuration.Alignment_) - _interSize);
    // Safely allocate the first page and add it to the page list
    SafeAllocateNewPage(this->PageList_);
}


ObjectAllocator::~ObjectAllocator()
{
    // Iterate through each page in the PageList
    GenericObject* currentPage = this->PageList_;
    while (currentPage != nullptr)
    {
        // Store the pointer to the next page before the current one is deallocated
        GenericObject* nextPage = currentPage->Next;

        // If using external headers, they need to be explicitly released
        if (this->configuration.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
            // Calculate the starting address for headers on the current page
            unsigned char* headerAddress = reinterpret_cast<unsigned char*>(currentPage) + this->headerSize;

            // Iterate through all objects on the page to release their headers
            for (unsigned _index = 0; _index < configuration.ObjectsPerPage_; ++_index)
            {
                // The header for each object is released, ignoring any potential exceptions
                // This is necessary because we're in the destructor and can't throw exceptions
                ReleaseObjectHeader(reinterpret_cast<GenericObject*>(headerAddress), OAConfig::hbExternal, true);

                // Move to the next object's header address
                headerAddress += this->dataSize;
            }
        }

        // Deallocate the memory for the current page
        delete[] reinterpret_cast<unsigned char*>(currentPage);

        // Move to the next page in the list
        currentPage = nextPage;
    }
}

void* ObjectAllocator::Allocate(const char* _label)
{
    // Check if the Object Allocator is bypassed in favor of the C++ memory manager
    if (this->configuration.UseCPPMemManager_)
    {
        unsigned char* newObject = nullptr;
        // Attempt to allocate memory using the C++ new operator
        newObject = new unsigned char[this->stats.ObjectSize_];
        
        // Update allocation statistics
        UpdateStatistics();

        return newObject;
    }

    // Check if there are available objects in the free list
    if (this->FreeList_ == nullptr)
    {
        // Allocate a new page since the free list is empty
        SafeAllocateNewPage(this->PageList_);
    }

    // Retrieve an object from the free list
    GenericObject* allocatedObject = this->FreeList_;

    // Move to the next object in the free list
    this->FreeList_ = allocatedObject->Next;

    // If debugging is enabled, mark the allocated object with a specific pattern
    if (this->configuration.DebugOn_)
    {
        memset(allocatedObject, ALLOCATED_PATTERN, this->stats.ObjectSize_);
    }

    // Update statistics post-allocation
    UpdateStatistics();

    // Set the header for the allocated object based on the configuration
    UpdateObjectHeader(allocatedObject, this->configuration.HBlockInfo_.type_, _label);

    return allocatedObject;
}

void ObjectAllocator::Free(void* _object)
{
    // Increment the deallocation count
    ++this->stats.Deallocations_;

    // Use the C++ memory manager if configured to bypass the Object Allocator
    if (this->configuration.UseCPPMemManager_) 
    {
        delete[] reinterpret_cast<unsigned char*>(_object);
        return;
    }

    GenericObject* genericObject = reinterpret_cast<GenericObject*>(_object);

    // Perform boundary checks if debugging is enabled
    if (this->configuration.DebugOn_)
    {
        FullBoundaryCheck(reinterpret_cast<unsigned char*>(_object));

        // Validate the left and right padding for the object
        if (!ValidatePadding(LeftPaddingAddress(genericObject), this->configuration.PadBytes_))
        {
            throw OAException(OAException::E_CORRUPTED_BLOCK, "Bad left boundary.");
        }

        if (!ValidatePadding(RightPaddingAddress(genericObject), this->configuration.PadBytes_))
        {
            throw OAException(OAException::E_CORRUPTED_BLOCK, "Bad right boundary.");
        }
    }

    // Release the header associated with the object
    ReleaseObjectHeader(genericObject, this->configuration.HBlockInfo_.type_);

    // Mark the object memory with the freed pattern if debugging is enabled
    if (this->configuration.DebugOn_)
    {
        memset(genericObject, FREED_PATTERN, this->stats.ObjectSize_);
    }

    // Reset the object's next pointer to null before adding it back to the free list
    genericObject->Next = nullptr;

    // Add the object back to the free list
    AddObjectToFreeList(genericObject);

    // Update the count of objects in use
    --this->stats.ObjectsInUse_;
}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK _callbackFn) const
{
    // Return immediately if the page list is empty
    if (!PageList_)
    {
        return 0;
    }

    unsigned memoryInUse = 0;

    // Iterate through each page in the list
    for (GenericObject* currentPage = PageList_; currentPage != nullptr; currentPage = currentPage->Next)
    {
        // Calculate the starting address for data blocks on the current page
        unsigned char* dataBlock = reinterpret_cast<unsigned char*>(currentPage) + headerSize;

        // Check each object on the page to see if it's in use
        for (unsigned index = 0; index < configuration.ObjectsPerPage_; ++index)
        {
            GenericObject* currentObject = reinterpret_cast<GenericObject*>(dataBlock + index * dataSize);

            // If the object is in use, invoke the callback function and increment the count
            if (IsObjectInUse(currentObject))
            {
                _callbackFn(currentObject, stats.ObjectSize_);
                ++memoryInUse;
            }
        }
    }

    return memoryInUse;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK _validateCallback) const
{
    // Early return if debugging is off or padding bytes are not configured
    if (!configuration.DebugOn_ || configuration.PadBytes_ == 0)
    {
        return 0;
    }

    unsigned totalCorruptedBlocks = 0;

    // Iterate through each page to check padding bytes of each block
    for (GenericObject* currentPage = PageList_; currentPage != nullptr; currentPage = currentPage->Next)
    {
        // Pointer to the start of data blocks on the current page
        unsigned char* dataStart = reinterpret_cast<unsigned char*>(currentPage) + headerSize;

        // Examine each object on the page
        for (unsigned index = 0; index < configuration.ObjectsPerPage_; ++index)
        {
            // Pointer to the current object's data
            GenericObject* currentObject = reinterpret_cast<GenericObject*>(dataStart + index * dataSize);

            // Check the integrity of the left and right padding for the current object
            bool isLeftPaddingValid = ValidatePadding(LeftPaddingAddress(currentObject), configuration.PadBytes_);
            bool isRightPaddingValid = ValidatePadding(RightPaddingAddress(currentObject), configuration.PadBytes_);

            // If either padding check fails, the block is considered corrupted
            if (!isLeftPaddingValid || !isRightPaddingValid)
            {
                // Invoke the callback function for the corrupted block
                _validateCallback(currentObject, stats.ObjectSize_);
                // Increment the count of corrupted blocks
                ++totalCorruptedBlocks;
            }
        }
    }
    return totalCorruptedBlocks;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
    // Return immediately if the page list is empty
    if (PageList_ == nullptr)
        return 0;

    unsigned emptyPageCount = 0;
    GenericObject* currentPage = PageList_;
    GenericObject* previousPage = nullptr;

    // Iterate through the page list to find and release unallocated (empty) pages
    while (currentPage != nullptr)
    {
        // Check if the current page is unallocated
        if (IsPageUnallocated(currentPage))
        {
            // If the current page is the head of the list, update the head pointer
            if (currentPage == PageList_)
            {
                PageList_ = currentPage->Next;
            }
            else if (previousPage != nullptr)
            {
                // Update the next pointer of the previous page to skip the current page
                previousPage->Next = currentPage->Next;
            }

            // Release the current page and increment the count of empty pages
            GenericObject* nextPage = currentPage->Next;
            ReleasePage(currentPage);
            currentPage = nextPage;
            emptyPageCount++;
        }
        else
        {
            // Move to the next page if the current page is not empty
            previousPage = currentPage;
            currentPage = currentPage->Next;
        }
    }
    return emptyPageCount;
}

void ObjectAllocator::ReleasePage(GenericObject* _page)
{
    // Remove all objects from the specified page from the free list
    RemoveObjectsFromFreeList(_page);

    // Deallocate the memory for the page
    delete[] reinterpret_cast<unsigned char*>(_page);

    // Decrement the count of pages currently in use
    --this->stats.PagesInUse_;
}

void ObjectAllocator::SetDebugState(bool _state)
{
    this->configuration.DebugOn_ = _state;
}

const void* ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}

const void* ObjectAllocator::GetPageList() const
{
    return PageList_;
}

OAConfig ObjectAllocator::GetConfig() const
{
    return this->configuration;
}

OAStats ObjectAllocator::GetStats() const
{
    return this->stats;
}

void ObjectAllocator::SafeAllocateNewPage(GenericObject *&_pageList)
{
    // Check if the maximum number of pages has been reached
    if (stats.PagesInUse_ == configuration.MaxPages_)
    {
        throw OAException(OAException::OA_EXCEPTION::E_NO_PAGES, "Out of pages!");
    }

    // Allocate a new page
    GenericObject* newPage = NewPageAllocation(this->stats.PageSize_);

    // Fill the new page with the alignment pattern if debugging is enabled
    if (this->configuration.DebugOn_)
    {
        memset(newPage, ALIGN_PATTERN, this->stats.PageSize_);
    }

    // Add the new page to the beginning of the page list
    InsertAtListHead(_pageList, newPage);

    // Calculate the start address for data blocks on the new page
    unsigned char* pageStartAddress = reinterpret_cast<unsigned char*>(newPage);
    unsigned char* dataStartAddress = pageStartAddress + this->headerSize;

    // Populate the free list with objects from the new page
    while (static_cast<unsigned>(dataStartAddress - pageStartAddress) < this->stats.PageSize_)
    {
        GenericObject* dataAddress = reinterpret_cast<GenericObject*>(dataStartAddress);

        // Add the object to the free list
        AddObjectToFreeList(dataAddress);

        // If debugging is enabled, set the padding bytes to the unallocated pattern
        if (this->configuration.DebugOn_)
        {
            memset(reinterpret_cast<unsigned char*>(dataAddress) + PTR_SIZE, UNALLOCATED_PATTERN, this->stats.ObjectSize_ - PTR_SIZE);
            memset(LeftPaddingAddress(dataAddress), PAD_PATTERN, this->configuration.PadBytes_);
            memset(RightPaddingAddress(dataAddress), PAD_PATTERN, this->configuration.PadBytes_);
        }

        // Clear the header area for the object
        memset(HeaderAddress(dataAddress), 0, configuration.HBlockInfo_.size_);

        // Move to the next data block
        dataStartAddress += this->dataSize;
    }
}

GenericObject* ObjectAllocator::NewPageAllocation(size_t _pageSize)
{
    // Allocate memory for a new page
    unsigned char* newPageMemory = new unsigned char[_pageSize]();

    // Cast the allocated memory to GenericObject* for consistency with the object allocator's data structures
    GenericObject* newPage = reinterpret_cast<GenericObject*>(newPageMemory);

    // Increment the count of pages in use
    ++this->stats.PagesInUse_;

    // Return the pointer to the newly allocated page
    return newPage;
}

void ObjectAllocator::AddObjectToFreeList(GenericObject* _object)
{
    // Insert the object at the beginning of the free list
    _object->Next = this->FreeList_;
    this->FreeList_ = _object;

    // Increment the count of free objects
    ++this->stats.FreeObjects_;
}

void ObjectAllocator::RemoveObjectsFromFreeList(GenericObject* _pageAddr)
{
    GenericObject* current = this->FreeList_;
    GenericObject* previous = nullptr;

    // Iterate through the free list to remove objects that belong to the specified page
    while (current != nullptr)
    {
        // Check if the current object is within the specified page
        if (IsAddressInPage(_pageAddr, reinterpret_cast<unsigned char*>(current)))
        {
            // If the current object is the head of the free list, update the head
            if (current == this->FreeList_)
            {
                this->FreeList_ = current->Next;
            }
            else if (previous != nullptr)
            {
                // Otherwise, unlink the current object from the free list
                previous->Next = current->Next;
            }

            // Move to the next object in the list
            current = current->Next;

            // Decrement the count of free objects
            --this->stats.FreeObjects_;
        }
        else
        {
            // Move to the next object if the current one is not within the specified page
            previous = current;
            current = current->Next;
        }
    }
}

void ObjectAllocator::UpdateStatistics()
{
    // Increment the count of objects currently in use
    ++this->stats.ObjectsInUse_;

    // Update the record of the most objects in use at one time, if the current count exceeds the previous record
    if (this->stats.ObjectsInUse_ > this->stats.MostObjects_)
    {
        this->stats.MostObjects_ = this->stats.ObjectsInUse_;
    }

    // Decrement the count of free objects available for allocation
    --this->stats.FreeObjects_;

    // Increment the total count of allocation operations performed
    ++this->stats.Allocations_;
}


void ObjectAllocator::ReleaseObjectHeader(GenericObject* _object, OAConfig::HBLOCK_TYPE _headerType, bool _ignoreThrow)
{
    unsigned char* headerAddress = HeaderAddress(_object);

    switch (_headerType)
    {
    case OAConfig::HBLOCK_TYPE::hbNone:
        {
            if (this->configuration.DebugOn_ && !_ignoreThrow)
            {
                unsigned char* lastByte = headerAddress + this->stats.ObjectSize_ - 1;
                if (*lastByte == FREED_PATTERN)
                {
                    throw OAException(OAException::E_MULTIPLE_FREE, "Multiple free!");
                }
            }
            break;
        }

    case OAConfig::HBLOCK_TYPE::hbBasic:
        {
            if (this->configuration.DebugOn_ && !_ignoreThrow)
            {
                if (*(headerAddress + sizeof(unsigned)) == 0)
                {
                    throw OAException(OAException::E_MULTIPLE_FREE, "Multiple free!");
                }
            }
            memset(headerAddress, 0, OAConfig::BASIC_HEADER_SIZE);
            break;
        }

    case OAConfig::HBLOCK_TYPE::hbExtended:
        {
            if (this->configuration.DebugOn_ && !_ignoreThrow)
            {
                unsigned char* extendedFlag = headerAddress + sizeof(unsigned) + this->configuration.HBlockInfo_.additional_ + sizeof(unsigned short);
                if (*extendedFlag == 0)
                {
                    throw OAException(OAException::E_MULTIPLE_FREE, "Multiple free!");
                }
            }
            memset(headerAddress + this->configuration.HBlockInfo_.additional_ + sizeof(unsigned short), 0, OAConfig::BASIC_HEADER_SIZE);
            break;
        }

    case OAConfig::HBLOCK_TYPE::hbExternal:
        {
            MemBlockInfo** externalInfo = reinterpret_cast<MemBlockInfo**>(headerAddress);
            if (_ignoreThrow == false && this->configuration.DebugOn_ && *externalInfo == nullptr)
            {
                throw OAException(OAException::E_MULTIPLE_FREE, "Multiple free!");
            }
            delete *externalInfo;
            *externalInfo = nullptr;
            break;
        }

    default:
        break;
    }
}

void ObjectAllocator::InitializeBasicHeader(GenericObject* _address)
{
    // Retrieve the address of the header for the given object
    unsigned char* headerAddress = HeaderAddress(_address);

    // Set the allocation number in the header to the current allocation count
    unsigned* allocationCount = reinterpret_cast<unsigned*>(headerAddress);
    *allocationCount = this->stats.Allocations_;

    // Set the allocation flag in the header to indicate the block is in use
    unsigned char* allocationFlag = headerAddress + sizeof(unsigned);
    *allocationFlag = true;
}

void ObjectAllocator::InitializeExternalHeader(GenericObject* _object, const char* _label)
{
    // Retrieve the address of the external header for the given object
    unsigned char* headerAddress = HeaderAddress(_object);

    // Cast the header address to a pointer to MemBlockInfo pointer
    MemBlockInfo** memBlockInfoPtr = reinterpret_cast<MemBlockInfo**>(headerAddress);

    // Allocate and initialize the MemBlockInfo structure
    *memBlockInfoPtr = new MemBlockInfo(this->stats.Allocations_, _label);
}

void ObjectAllocator::InitializeExtendedHeader(GenericObject* _object)
{
    // Retrieve the address of the extended header for the given object
    unsigned char* headerAddress = HeaderAddress(_object);

    // Initialize the usage counter part of the extended header
    unsigned short* usageCounter = reinterpret_cast<unsigned short*>(headerAddress + this->configuration.HBlockInfo_.additional_);
    ++(*usageCounter);

    // Set the allocation number in the extended header
    unsigned* allocationCount = reinterpret_cast<unsigned*>(usageCounter + 1); // Move past the usage counter
    *allocationCount = this->stats.Allocations_;

    // Set the allocation flag in the extended header to indicate the block is in use
    unsigned char* allocationFlag = reinterpret_cast<unsigned char*>(allocationCount + 1); // Move past the allocation count
    *allocationFlag = true;
}

void ObjectAllocator::FullBoundaryCheck(unsigned char* _address) const
{
    // Start with the first page in the list
    GenericObject* currentPage = this->PageList_;

    // Loop until the address is found within a page or the end of the list is reached
    while (currentPage && !IsAddressInPage(currentPage, _address))
    {
        currentPage = currentPage->Next;
    }

    // If the address wasn't found in any page, it's outside the allocator's managed memory
    if (!currentPage)
    {
        throw OAException(OAException::E_BAD_BOUNDARY, "Address is outside allocated pages.");
    }

    // Calculate the offset of the address from the start of its page
    unsigned char* pageStart = reinterpret_cast<unsigned char*>(currentPage);
    size_t offsetFromPageStart = _address - pageStart;

    // Check if the address is within the header region
    if (offsetFromPageStart < this->headerSize)
    {
        throw OAException(OAException::E_BAD_BOUNDARY, "Address is within the page header.");
    }

    // Calculate the offset of the address from the start of the data region
    size_t offsetFromDataStart = offsetFromPageStart - this->headerSize;

    // Check if the address is properly aligned with the start of an object
    if (offsetFromDataStart % this->dataSize != 0)
    {
        throw OAException(OAException::E_BAD_BOUNDARY, "Address is not aligned with an object boundary.");
    }
}

bool ObjectAllocator::ValidatePadding(unsigned char* _paddingAddress, size_t _size) const
{
    // Iterate through each byte of the padding to check if it matches the PAD_PATTERN
    for (size_t index = 0; index < _size; ++index)
    {
        if (_paddingAddress[index] != PAD_PATTERN)
        {
            // If any byte does not match, the padding is invalid
            return false;
        }
    }

    // If all bytes match PAD_PATTERN, the padding is valid
    return true;
}

bool ObjectAllocator::VerifyObjectData(GenericObject* _objectData, const unsigned char _pattern) const
{
    // Access the object data as a sequence of bytes
    unsigned char* objectBytes = reinterpret_cast<unsigned char*>(_objectData);

    // Iterate through each byte of the object data
    for (size_t index = 0; index < stats.ObjectSize_; ++index)
    {
        // Check if any byte matches the specified pattern
        if (objectBytes[index] == _pattern)
        {
            // If a match is found, return true
            return true;
        }
    }

    // If no byte matches the pattern, return false
    return false;
}

bool ObjectAllocator::IsAddressInPage(GenericObject* _pageAddress, unsigned char* _address) const
{
    // Calculate the start address of the page
    unsigned char* pageStart = reinterpret_cast<unsigned char*>(_pageAddress);

    // Calculate the end address of the page
    unsigned char* pageEnd = pageStart + stats.PageSize_;

    // Check if the given address falls within the range of the page
    return (_address >= pageStart && _address < pageEnd);
}

bool ObjectAllocator::IsPageUnallocated(GenericObject* _page) const
{
    // Initialize a counter for the number of free objects found in the page
    unsigned freeObjectCount = 0;

    // Iterate through the free list to check each object
    for (GenericObject* currentObject = this->FreeList_; currentObject != nullptr; currentObject = currentObject->Next)
    {
        // Check if the current free object is within the specified page
        if (IsAddressInPage(_page, reinterpret_cast<unsigned char*>(currentObject)))
        {
            // Increment the count of free objects found within the page
            ++freeObjectCount;

            // If the count of free objects in the page equals the number of objects per page, the page is unallocated
            if (freeObjectCount >= configuration.ObjectsPerPage_)
            {
                return true;
            }
        }
    }

    // If the function hasn't returned true by this point, the page is not fully unallocated
    return false;
}

bool ObjectAllocator::IsObjectInUse(GenericObject* _object) const
{
    switch (this->configuration.HBlockInfo_.type_)
    {
        case OAConfig::HBLOCK_TYPE::hbNone:
        {
            // Iterate through the free list to check if the object is not allocated
            for (GenericObject* current = this->FreeList_; current != nullptr; current = current->Next)
            {
                if (current == _object)
                {
                    // Object is in the free list, hence not in use
                    return false;
                }
            }
            // Object not found in the free list, hence it's in use
            return true;
        }

        case OAConfig::HBLOCK_TYPE::hbBasic:
        case OAConfig::HBLOCK_TYPE::hbExtended:
        {
            // Access the flag bit directly to determine if the object is in use
            unsigned char* flagByte = reinterpret_cast<unsigned char*>(_object) - configuration.PadBytes_ - 1;
            return *flagByte != 0;  // Non-zero value indicates the object is in use
        }

        case OAConfig::HBLOCK_TYPE::hbExternal:
        {
            // For external headers, check the presence of a MemBlockInfo pointer
            MemBlockInfo** memBlockInfoPtr = reinterpret_cast<MemBlockInfo**>(HeaderAddress(_object));
            return *memBlockInfoPtr != nullptr;  // Non-null pointer indicates the object is in use
        }

        default:
            return false;
    }
}

void ObjectAllocator::UpdateObjectHeader(GenericObject* _object, OAConfig::HBLOCK_TYPE _headerType, const char* _label)
{
    // Determine the header type and initialize the header accordingly
    switch (_headerType)
    {
        case OAConfig::HBLOCK_TYPE::hbBasic:
            // Initialize the basic header for the object
            InitializeBasicHeader(_object);
            break;

        case OAConfig::HBLOCK_TYPE::hbExtended:
            // Initialize the extended header for the object
            InitializeExtendedHeader(_object);
            break;

        case OAConfig::HBLOCK_TYPE::hbExternal:
            // Initialize the external header for the object with the provided label
            InitializeExternalHeader(_object, _label);
            break;

        default:
            // No action needed for undefined header types
            break;
    }
}

 unsigned char* ObjectAllocator::HeaderAddress(GenericObject* obj) const
{
    return reinterpret_cast<unsigned char*>(obj) - this->configuration.PadBytes_ - this->configuration.HBlockInfo_.size_;
}

unsigned char* ObjectAllocator::LeftPaddingAddress(GenericObject* obj) const
{
    return reinterpret_cast<unsigned char*>(obj) - this->configuration.PadBytes_;
}

unsigned char* ObjectAllocator::RightPaddingAddress(GenericObject* obj) const
{
    return reinterpret_cast<unsigned char*>(obj) + this->stats.ObjectSize_;
}

void ObjectAllocator::InsertAtListHead(GenericObject*& head, GenericObject* node)
{
    node->Next = head;
    head = node;
}

void ObjectAllocator::InitializeOAConfig()
{
    // Perform the calculations and adjustments that were originally in the redefined constructor
    unsigned leftHeaderSize = static_cast<unsigned>(PTR_SIZE + this->configuration.HBlockInfo_.size_ + static_cast<size_t>(this->configuration.PadBytes_));
    this->configuration.LeftAlignSize_ = static_cast<unsigned>(align(leftHeaderSize, this->configuration.Alignment_) - leftHeaderSize);

    size_t interSize = this->stats.ObjectSize_ + this->configuration.PadBytes_ * 2_z + static_cast<size_t>(this->configuration.HBlockInfo_.size_);
    this->configuration.InterAlignSize_ = static_cast<unsigned>(align(interSize, this->configuration.Alignment_) - interSize);

    // Any other necessary configuration adjustments...
}
