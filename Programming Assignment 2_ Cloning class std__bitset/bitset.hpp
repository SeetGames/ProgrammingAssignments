/*!*****************************************************************************
 \file bitset.hpp
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course:  Modern C++ Design Patterns
 \par Section: B
 \par Assignment 2
 \date 14-09-2023
 \brief
    This file contains the implementation of the bitset class template.
*******************************************************************************/

namespace HLP3
{
    /**
     * Default constructor for bitset class.
     * Allocates memory for the bitset and initializes all bits to 0.
     * 
     * @tparam N The number of bits in the bitset.
     */
    template <std::size_t N>
    bitset<N>::bitset()
    {
        // Calculate the number of bytes needed to store N bits
        /*The expression (N + CHAR_BIT - 1) is a common trick to round up 
        the division. Without the addition of CHAR_BIT - 1, if N is not a 
        multiple of CHAR_BIT, you would get a truncated result. By adding 
        CHAR_BIT - 1, you ensure that any value of N that is not a perfect 
        multiple of CHAR_BIT will round up to the next whole byte.
        
        The division by CHAR_BIT then gives the number of bytes needed to 
        store N bits.*/
        std::size_t numBytes = (N + CHAR_BIT - 1) / CHAR_BIT;
        data = new unsigned char[numBytes](); // Allocate and initialize to 0
    }

    /**
     * @brief Destructor for the bitset class.
     * 
     * This function deallocates the memory allocated for the bitset data.
     * 
     * @tparam N The size of the bitset.
     */
    template <std::size_t N>
    bitset<N>::~bitset()
    {
        delete[] data;
    }

    /**
     * Returns the value of the bit at the specified position.
     * Throws std::out_of_range exception if the position is out of range.
     * @param pos The position of the bit to be tested.
     * @return The value of the bit at the specified position.
     */
    template <std::size_t N>
    bool bitset<N>::test(std::size_t pos) const
    {
        if (pos >= N)
        {
            throw std::out_of_range("Position out of range");
        }
        std::size_t byteIndex = pos / CHAR_BIT;
        std::size_t bitIndex = pos % CHAR_BIT;

        // Access the byte that contains the bit we're interested in.
        unsigned char targetByte = data[byteIndex];

        // Create a mask where only the desired bit is set to 1.
        // All other bits in the mask will be 0.
        unsigned char bitMask = (1 << bitIndex);

        // Use bitwise AND to check if the desired bit in targetByte is set.
        // If the bit is set, the result will be non-zero (i.e., the mask value).
        // If the bit is not set, the result will be 0.
        unsigned char result = targetByte & mask;
        
        // Check if the result is non-zero (i.e., the bit was set).
        bool isBitSet = result != 0;

        return isBitSet;
    }
    
    /**
     * Sets the bit at the specified position to the specified value.
     * Throws std::out_of_range exception if the position is out of range.
     * 
     * @param pos The position of the bit to set.
     * @param value The value to set the bit to.
     */
    template <std::size_t N>
    void bitset<N>::set(std::size_t pos, bool value)
    {
        if (pos >= N)
        {
            throw std::out_of_range("Position out of range");
        }
        std::size_t byteIndex = pos / CHAR_BIT;
        std::size_t bitIndex = pos % CHAR_BIT;

        // Access the byte that contains the bit we're interested in.
        unsigned char targetByte = data[byteIndex];

        // Create a mask where only the desired bit is set to 1.
        // All other bits in the mask will be 0.
        unsigned char bitMask = (1 << bitIndex);
        if (value)
        {
            targetByte |= bitMask; // Set the bit
        }
        else
        {
            targetByte &= ~bitMask; // Clear the bit
        }
    }

    /*
    uint32_t set_bit(uint32_t _value, uint32_t _position) 
    {
        // Check if position is within the valid range of [0, 31]
        if (_position > 31) 
            return _value;

        // Create a mask with the i-th bit set to 1
        uint32_t bitMask = 1 << _position;

        // Use bitwise OR to set the i-th bit of x to 1
        return _value | bitMask;
    }
    */

    /**
     * Resets the bit at the specified position to 0.
     * 
     * @param pos The position of the bit to reset.
     */
    template <std::size_t N>
    void bitset<N>::reset(std::size_t pos)
    {
        set(pos, false);
    }

    /**
     * Flips the bit at the specified position.
     * Throws std::out_of_range exception if the position is out of range.
     * 
     * @param pos The position of the bit to flip.
     */
    template <std::size_t N>
    void bitset<N>::flip(std::size_t pos)
    {
        if (pos >= N)
        {
            throw std::out_of_range("Position out of range");
        }
        std::size_t byteIndex = pos / CHAR_BIT;
        std::size_t bitIndex = pos % CHAR_BIT;
        data[byteIndex] ^= (1 << bitIndex); // Toggle the bit
    }

    /**
     * Returns the number of bits set to 1 in the bitset.
     * 
     * @tparam N the number of bits in the bitset
     * @return the number of bits set to 1 in the bitset
     */
    template <std::size_t N>
    std::size_t bitset<N>::count() const
    {
        std::size_t count = 0;
        for (std::size_t i = 0; i < N; ++i)
        {
            if (test(i))
            {
                ++count;
            }
        }
        return count;
    }

    /**
     * Returns the number of bits in the bitset.
     *
     * @return The number of bits in the bitset.
     */
    template <std::size_t N>
    std::size_t bitset<N>::size() const
    {
        return N;
    }

    /**
     * Returns a string representation of the bitset, where 'zero' and 'one' represent the values of 0 and 1 respectively.
     * The string is constructed by iterating over each bit in the bitset and appending the corresponding character to the result string.
     * @param zero The character to represent a 0 bit.
     * @param one The character to represent a 1 bit.
     * @return A string representation of the bitset.
     */
    template <std::size_t N>
    std::string bitset<N>::to_string(char zero, char one) const
    {
        std::string result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result = (test(i) ? one : zero) + result;
        }
        return result;
    }

    /**
     * @brief Overloads the [] operator to access the bit at the specified position.
     * 
     * @tparam N The number of bits in the bitset.
     * @param pos The position of the bit to access.
     * @return true if the bit at the specified position is set, false otherwise.
     */
    template <std::size_t N>
    bool bitset<N>::operator[](std::size_t pos) const
    {
        return test(pos);
    }

} // namespace HLP3
