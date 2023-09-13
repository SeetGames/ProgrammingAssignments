// File documentation header is required
// This file contains the declaration of the bitset class template, which is a clone of the std::bitset class.

#ifndef BITSET_H
#define BITSET_H

namespace HLP3 
{

template <std::size_t N>
class bitset 
{
private:
    // Dynamically allocated array to store the bits.
    unsigned char* data;

    // Helper functions (if needed)

public:
    // Constructors, Destructor
    bitset();
    ~bitset();

    // Member functions
    bool test(std::size_t pos) const; // Check if a bit is set
    void set(std::size_t pos, bool value = true); // Set a bit
    void reset(std::size_t pos); // Reset a bit
    void flip(std::size_t pos); // Flip a bit
    std::size_t count() const; // Count set bits
    std::size_t size() const; // Return size of the bitset
    std::string to_string(char zero = '0', char one = '1') const; // Convert to string

    // Overloaded operators
    bool operator[](std::size_t pos) const; // Access bit (read-only)

    // Additional member functions (if needed)
};

} // namespace HLP3

#include "bitset.hpp"

#endif

