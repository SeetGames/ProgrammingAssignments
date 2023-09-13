// File documentation header is required
// This file contains the implementation of the bitset class template.

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
        return (data[byteIndex] & (1 << bitIndex)) != 0;
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
        if (value)
        {
            data[byteIndex] |= (1 << bitIndex); // Set the bit
        }
        else
        {
            data[byteIndex] &= ~(1 << bitIndex); // Clear the bit
        }
    }

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
