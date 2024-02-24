/*!*************************************************************************
\file ObjectAllocator.h
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 1
\date 01-22-2024
\brief
This file contains the declaration for the ObjectAllocator
***************************************************************************/

//---------------------------------------------------------------------------
#ifndef OBJECTALLOCATORH
#define OBJECTALLOCATORH
//---------------------------------------------------------------------------

#include <string>

// If the client doesn't specify these:
static const int DEFAULT_OBJECTS_PER_PAGE = 4;
static const int DEFAULT_MAX_PAGES = 3;

/*!
  Exception class
*/
class OAException
{
public:
  /*!
    Possible exception codes
  */
  enum OA_EXCEPTION
  {
    E_NO_MEMORY,      //!< out of physical memory (operator new fails)
    E_NO_PAGES,       //!< out of logical memory (max pages has been reached)
    E_BAD_BOUNDARY,   //!< block address is on a page, but not on any block-boundary
    E_MULTIPLE_FREE,  //!< block has already been freed
    E_CORRUPTED_BLOCK //!< block has been corrupted (pad bytes have been overwritten)
  };

  /*!
    Constructor

    \param ErrCode
      One of the 5 error codes listed above

    \param Message
      A message returned by the what method.
  */
  OAException(OA_EXCEPTION ErrCode, const std::string &Message) : error_code_(ErrCode), message_(Message){};

  /*!
    Destructor
  */
  virtual ~OAException()
  {
  }

  /*!
    Retrieves the error code

    \return
      One of the 5 error codes.
  */
  OA_EXCEPTION code() const
  {
    return error_code_;
  }

  /*!
    Retrieves a human-readable string regarding the error.

    \return
      The NUL-terminated string representing the error.
  */
  virtual const char *what() const
  {
    return message_.c_str();
  }

private:
  OA_EXCEPTION error_code_; //!< The error code (one of the 5)
  std::string message_;     //!< The formatted string for the user.
};

/*!
  ObjectAllocator configuration parameters
*/
struct OAConfig
{
  static const size_t BASIC_HEADER_SIZE = sizeof(unsigned) + 1; //!< allocation number + flags
  static const size_t EXTERNAL_HEADER_SIZE = sizeof(void *);    //!< just a pointer

  /*!
    The different types of header blocks
  */
  enum HBLOCK_TYPE
  {
    hbNone,
    hbBasic,
    hbExtended,
    hbExternal
  };

  /*!
    POD that stores the information related to the header blocks.
  */
  struct HeaderBlockInfo
  {
    HBLOCK_TYPE type_;  //!< Which of the 4 header types to use?
    size_t size_;       //!< The size of this header
    size_t additional_; //!< How many user-defined additional bytes

    /*!
      Constructor

      \param type
        The kind of header blocks in use.

      \param additional
        The number of user-defined additional bytes required.

    */
    HeaderBlockInfo(HBLOCK_TYPE type = hbNone, unsigned additional = 0) : type_(type), size_(0), additional_(additional)
    {
      if (type_ == hbBasic)
        size_ = BASIC_HEADER_SIZE;
      else if (type_ == hbExtended) // alloc # + use counter + flag byte + user-defined
        size_ = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(char) + additional_;
      else if (type_ == hbExternal)
        size_ = EXTERNAL_HEADER_SIZE;
    };
  };

  /*!
    Constructor

    \param UseCPPMemManager
      Determines whether or not to by-pass the OA.

    \param ObjectsPerPage
      Number of objects for each page of memory.

    \param MaxPages
      Maximum number of pages before throwing an exception. A value
      of 0 means unlimited.

    \param DebugOn
      Is debugging code on or off?

    \param PadBytes
      The number of bytes to the left and right of a block to pad with.

    \param HBInfo
      Information about the header blocks used

    \param Alignment
      The number of bytes to align on.
  */
  OAConfig(bool UseCPPMemManager = false,
           unsigned ObjectsPerPage = DEFAULT_OBJECTS_PER_PAGE,
           unsigned MaxPages = DEFAULT_MAX_PAGES,
           bool DebugOn = false,
           unsigned PadBytes = 0,
           const HeaderBlockInfo &HBInfo = HeaderBlockInfo(),
           unsigned Alignment = 0) : UseCPPMemManager_(UseCPPMemManager),
                                     ObjectsPerPage_(ObjectsPerPage),
                                     MaxPages_(MaxPages),
                                     DebugOn_(DebugOn),
                                     PadBytes_(PadBytes),
                                     HBlockInfo_(HBInfo),
                                     Alignment_(Alignment)
  {
    HBlockInfo_ = HBInfo;
    LeftAlignSize_ = 0;
    InterAlignSize_ = 0;
  }

  bool UseCPPMemManager_;      //!< by-pass the functionality of the OA and use new/delete
  unsigned ObjectsPerPage_;    //!< number of objects on each page
  unsigned MaxPages_;          //!< maximum number of pages the OA can allocate (0=unlimited)
  bool DebugOn_;               //!< enable/disable debugging code (signatures, checks, etc.)
  unsigned PadBytes_;          //!< size of the left/right padding for each block
  HeaderBlockInfo HBlockInfo_; //!< size of the header for each block (0=no headers)
  unsigned Alignment_;         //!< address alignment of each block
  unsigned LeftAlignSize_;     //!< number of alignment bytes required to align first block
  unsigned InterAlignSize_;    //!< number of alignment bytes required between remaining blocks
};

/*!
  POD that holds the ObjectAllocator statistical info
*/
struct OAStats
{
  /*!
    Constructor
  */
  OAStats() : ObjectSize_(0), PageSize_(0), FreeObjects_(0), ObjectsInUse_(0), PagesInUse_(0),
              MostObjects_(0), Allocations_(0), Deallocations_(0){};

  size_t ObjectSize_;      //!< size of each object
  size_t PageSize_;        //!< size of a page including all headers, padding, etc.
  unsigned FreeObjects_;   //!< number of objects on the free list
  unsigned ObjectsInUse_;  //!< number of objects in use by client
  unsigned PagesInUse_;    //!< number of pages allocated
  unsigned MostObjects_;   //!< most objects in use by client at one time
  unsigned Allocations_;   //!< total requests to allocate memory
  unsigned Deallocations_; //!< total requests to free memory
};

/*!
  This allows us to easily treat raw objects as nodes in a linked list
*/
struct GenericObject
{
  GenericObject *Next; //!< The next object in the list
};

/*!
  This is used with external headers
*/
struct MemBlockInfo
{
  bool in_use;        //!< Is the block free or in use?
  char *label;        //!< A dynamically allocated NUL-terminated string
  unsigned alloc_num; //!< The allocation number (count) of this block

  MemBlockInfo(unsigned _allocNum, const char* _label);
  ~MemBlockInfo();

  MemBlockInfo(const MemBlockInfo &) = delete;
  MemBlockInfo &operator=(const MemBlockInfo &) = delete;
};

/*!
  This class represents a custom memory manager
*/
class ObjectAllocator
{
public:
  // Defined by the client (pointer to a block, size of block)
  typedef void (*DUMPCALLBACK)(const void *, size_t);     //!< Callback function when dumping memory leaks
  typedef void (*VALIDATECALLBACK)(const void *, size_t); //!< Callback function when validating blocks

  // Predefined values for memory signatures
  static const unsigned char UNALLOCATED_PATTERN = 0xAA; //!< New memory never given to the client
  static const unsigned char ALLOCATED_PATTERN = 0xBB;   //!< Memory owned by the client
  static const unsigned char FREED_PATTERN = 0xCC;       //!< Memory returned by the client
  static const unsigned char PAD_PATTERN = 0xDD;         //!< Pad signature to detect buffer over/under flow
  static const unsigned char ALIGN_PATTERN = 0xEE;       //!< For the alignment bytes

  // Creates the ObjectManager per the specified values
  // Throws an exception if the construction fails. (Memory allocation problem)
  ObjectAllocator(size_t ObjectSize, const OAConfig &config);

  // Destroys the ObjectManager (never throws)
  ~ObjectAllocator();

  // Take an object from the free list and give it to the client (simulates new)
  // Throws an exception if the object can't be allocated. (Memory allocation problem)
  void *Allocate(const char *label = 0);

  // Returns an object to the free list for the client (simulates delete)
  // Throws an exception if the the object can't be freed. (Invalid object)
  void Free(void *Object);

  // Calls the callback fn for each block still in use
  unsigned DumpMemoryInUse(DUMPCALLBACK fn) const;

  // Calls the callback fn for each block that is potentially corrupted
  unsigned ValidatePages(VALIDATECALLBACK fn) const;

  // Frees all empty page
  unsigned FreeEmptyPages();

  // Testing/Debugging/Statistic methods
  void SetDebugState(bool State);  // true=enable, false=disable
  const void *GetFreeList() const; // returns a pointer to the internal free list
  const void *GetPageList() const; // returns a pointer to the internal page list
  OAConfig GetConfig() const;      // returns the configuration parameters
  OAStats GetStats() const;        // returns the statistics for the allocator

  // Prevent copy construction and assignment
  ObjectAllocator(const ObjectAllocator &oa) = delete;            //!< Do not implement!
  ObjectAllocator &operator=(const ObjectAllocator &oa) = delete; //!< Do not implement!

private:
  /*!
 \brief Allocates a new page of objects with checking and adds it to the PageList.
 \param[in,out] PageList Reference to the head of the page list.
*/
  void SafeAllocateNewPage(GenericObject *&PageList);

  /*!
   \brief Allocates a new page of memory for objects using the actual 'new' operator.
   \param[in] pageSize The size of the new page to be allocated.
   \return Pointer to the newly allocated page.
  */
  GenericObject *NewPageAllocation(size_t pageSize);

  /*!
   \brief Adds an object to the free list, making it available for allocation.
   \param[in] Object Pointer to the object to be added to the free list.
  */
  void AddObjectToFreeList(GenericObject *Object);

  /*!
   \brief Removes all objects from a specified page from the free list.
   \param[in] pageAddr Address of the page whose objects are to be removed.
  */
  void RemoveObjectsFromFreeList(GenericObject *pageAddr);

  /*!
   \brief Releases a page of memory, deallocating it.
   \param[in] temp Pointer to the page to be released.
  */
  void ReleasePage(GenericObject *temp);

  /*!
   \brief Updates the allocation statistics.
  */
  void UpdateStatistics();

  /*!
   \brief Frees the header of an object based on its header type.
   \param[in] Object Pointer to the object whose header is to be freed.
   \param[in] headerType Type of the header to be freed.
   \param[in] ignoreThrow If true, suppresses exceptions that might be thrown.
  */
  void ReleaseObjectHeader(GenericObject *Object, OAConfig::HBLOCK_TYPE headerType, bool ignoreThrow = false);

  /*!
   \brief Creates and configures the header for an object based on the specified header type and configuration.
   \param[in] Object Pointer to the object whose header is to be configured.
   \param[in] headerType Type of the header to use.
   \param[in] label Optional label for external headers.
  */
  void UpdateObjectHeader(GenericObject *Object, OAConfig::HBLOCK_TYPE headerType, const char *label = nullptr);

  /*!
   \brief Initializes a basic header for an object, without performing any checks.
   \param[in] addr Address where the basic header is to be initialized.
  */
  void InitializeBasicHeader(GenericObject *addr);

  /*!
   \brief Initializes an external header for an allocated object, without checks.
   \param[in] Object Pointer to the object for which the external header is to be initialized.
   \param[in] label Label for the external header.
  */
  void InitializeExternalHeader(GenericObject *Object, const char *label);

  /*!
   \brief Initializes an extended header for an allocated object.
   \param[in] Object Pointer to the object for which the extended header is to be initialized.
  */
  void InitializeExtendedHeader(GenericObject *Object);

  /*!
   \brief Performs a thorough boundary check on an address, which is slower but more comprehensive.
   \param[in] addr Address to be checked.
  */
  void FullBoundaryCheck(unsigned char *addr) const;

  /*!
   \brief Validates the correctness of the padding at a specified address.
   \param[in] paddingAddr Address of the padding to be validated.
   \param[in] size Size of the padding.
   \return True if the padding is correct, false otherwise.
  */
  bool ValidatePadding(unsigned char *paddingAddr, size_t size) const;

  /*!
   \brief Verifies if the data in an object matches a specified pattern.
   \param[in] objectdata Pointer to the object data to be verified.
   \param[in] pattern Pattern to check against the object data.
   \return True if the data matches the pattern, false otherwise.
  */
  bool VerifyObjectData(GenericObject *objectdata, const unsigned char pattern) const;

  /*!
   \brief Checks if an address is within the bounds of a specified page.
   \param[in] pageAddr Address of the page.
   \param[in] addr Address to be checked.
   \return True if the address is within the page, false otherwise.
  */
  bool IsAddressInPage(GenericObject *pageAddr, unsigned char *addr) const;

  /*!
   \brief Determines if a specified page has no allocated objects.
   \param[in] page Pointer to the page to be checked.
   \return True if the page is empty, false otherwise.
  */
  bool IsPageUnallocated(GenericObject *page) const;

  /*!
   \brief Checks if a specified object is currently in use.
   \param[in] object Pointer to the object to be checked.
   \return True if the object is in use, false otherwise.
  */
  bool IsObjectInUse(GenericObject *object) const;

  /*!
   \brief Returns the address of an object's header.
   \param[in] obj Pointer to the object.
   \return Address of the object's header.
  */
  unsigned char *HeaderAddress(GenericObject *obj) const;

  /*!
   \brief Returns the address of an object's left padding.
   \param[in] obj Pointer to the object.
   \return Address of the object's left padding.
  */
  unsigned char *LeftPaddingAddress(GenericObject *obj) const;

  /*!
   \brief Returns the address of an object's right padding.
   \param[in] obj Pointer to the object.
   \return Address of the object's right padding.
  */
  unsigned char *RightPaddingAddress(GenericObject *obj) const;

  /*!
   \brief Inserts a node at the beginning of a linked list.
   \param[in,out] head Reference to the head of the list.
   \param[in] node Pointer to the node to be inserted.
  */
  void InsertAtListHead(GenericObject *&head, GenericObject *node);

  /*!
   \brief Initializes the configuration settings for the Object Allocator.

   This function sets up the initial configuration parameters for the Object Allocator, 
   including default values for object size per page, maximum number of pages, alignment, 
   padding, and header block information. It ensures that the allocator starts with a 
   consistent and predictable state.
  */
  void InitializeOAConfig();

  // Some "suggested" members (only a suggestion!)
  GenericObject *PageList_; //!< the beginning of the list of pages
  GenericObject *FreeList_; //!< the beginning of the list of objects

  // Lots of other private stuff...
  OAStats stats;          //! Stats of the object allocator
  OAConfig configuration; //! User defined configuration of the allocator
  size_t headerSize;      //! The size of the header part of the page, post alignment, post header, post padding
  size_t dataSize;        //! The size of each data part of the page (but not the last data), post alignment, post header, post padding
  size_t totalDataSize;   //! The size of the sum of data part of the page, post alignment, post header, post padding
};

#endif
