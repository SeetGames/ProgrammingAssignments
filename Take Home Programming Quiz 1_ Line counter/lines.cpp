/*!*****************************************************************************
 \file lines.cpp
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course:  Modern C++ Design Patterns
 \par Section: B
 \par Take Home Programming Quiz 1
 \date 02-09-2023
 \brief
    This file contains the implementation of the lines function.
*******************************************************************************/

#include <fstream>
#include <string>
#include "lines.hpp"

namespace HLP3
{
    /*!*****************************************************************************
     * @brief returns the total number of lines in the files provided
     * 
     * @param files array of text file names
     * @return int 
    *******************************************************************************/
    int lines(char const* files[])
    {
        int totalLines = 0;
        int i = 0;
        while (files[i] != nullptr)
        {
            std::ifstream file(files[i]);
            std::string line;
            while (std::getline(file, line))
            {
                totalLines++;
            }
            i++;
        }
        return totalLines;
    }
}
