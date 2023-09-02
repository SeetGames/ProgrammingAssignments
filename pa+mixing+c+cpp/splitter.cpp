/*!*****************************************************************************
 \file splitter.cpp
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course:  Modern C++ Design Patterns
 \par Section: B
 \par Assignment 1
 \date 30-08-2023
 \brief
    Implements functions to split and join binary files. Provides efficient 
    buffer management for reading/writing and includes error handling for 
    common file and memory operations. Implemented in C++
*******************************************************************************/
#include "splitter.h"
#include <string>
#include <fstream>
// this is the maximum size of buffer for reading data from input file
// and to write data to output file ...
// if a file contains 10000 bytes, this means that you have to repeatedly
// [thrice] read the file for 4096 bytes, followed by the next 4096 bytes,
// followed by the final 1808 bytes.
// Not following this recipe by increasing the buffer to a higher value
// will mean that your submission is not following the specified requirements
// and will receive a zero grade!!!
constexpr int FOUR_K {4096}; // Don't modify this!!!

/*!*****************************************************************************
 * @brief This function will combine the chunks specified by the user.
 * 
 * @param inputFileName The name of the file to be joined
 * @param outputPathPrefix The prefix of the output file eg:./split-data/piece_
 * @param chunkSize The size of each chunk.
 * @return SplitResult 
*******************************************************************************/
SplitResult split(const std::string& inputFileName, const std::string& outputPathPrefix, int chunkSize) 
{
    SplitResult rs = E_NO_ACTION;
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile.is_open())
    {
        rs = E_BAD_SOURCE;
        return rs;
    }
    char* buffer = new char[FOUR_K];
    int bytesRead = 0;
    int fileCount = 1;
    std::string outputFileName;

    while (true)
    {
        int bytesLeftForCurrentChunk = chunkSize;
        outputFileName = outputPathPrefix + std::to_string(fileCount).insert(0, 4 - std::to_string(fileCount).length(), '0');
        std::ofstream outputFile(outputFileName, std::ios::binary);

        if (!outputFile.is_open())
        {
            delete[] buffer;
            inputFile.close();
            return E_BAD_DESTINATION;
        }

        while (bytesLeftForCurrentChunk > 0)
        {
            int bytesToRead = std::min(bytesLeftForCurrentChunk, FOUR_K);
            inputFile.read(buffer, bytesToRead);
            bytesRead = inputFile.gcount();
            if (bytesRead == 0)
                break;

            outputFile.write(buffer, bytesRead);
            bytesLeftForCurrentChunk -= bytesRead;
        }

        outputFile.close();
        if (bytesRead == 0)
            break;

        fileCount++;
    }
    delete[] buffer;
    inputFile.close();
    rs = E_SPLIT_SUCCESS;
    return rs;
}


/*!*****************************************************************************
 * @brief This function will combine the chunks specified by the user.
 * 
 * @param argv The argument vector provided by the user
 * @param totalChunks The total number of chunks to join  
 * @return SplitResult 
*******************************************************************************/
SplitResult join(char* argv[], int totalChunks)
{
    SplitResult rs = E_NO_ACTION;
    std::ofstream outputFile(argv[3], std::ios::binary);
    // Ensure the output file can be opened
    if (!outputFile.is_open())
    {
        rs = E_BAD_DESTINATION;
        return rs;
    }
    char* buffer = new char[FOUR_K];
    for (int i = 0; i < totalChunks; i++)
    {
        std::ifstream inputFile(argv[i + 5], std::ios::binary);
        // Ensure the input file can be opened
        if (!inputFile.is_open())
        {
            delete[] buffer;
            outputFile.close();
            rs = E_BAD_SOURCE;
            return rs;
        }
        int bytesRead = 0;
        while (true)
        {
            inputFile.read(buffer, FOUR_K);
            bytesRead = inputFile.gcount();
            if (bytesRead == 0) // No more data to read
                break;

            outputFile.write(buffer, bytesRead);
        }
        inputFile.close();
    }
    // Deallocate the buffer now that we're done with the files
    delete[] buffer;
    outputFile.close();
    rs = E_JOIN_SUCCESS;
    return rs;
}

/*!*****************************************************************************
 * @brief Depending on command line arguments passed through, this function will
 * either:
 * - combine the chunks specified by the user
 * - split the file into chunks specified by the user
 * 
 * @param argc Argument Count
 * @param argv Argument Vector
 * @return SplitResult Result of the split or join process enumeration
*******************************************************************************/
SplitResult split_join(int argc, char *argv[])
{
  SplitResult rs = E_NO_ACTION;
  //Check the first argument first to determine whether it's a split or join command
  // ./sj.out -s 134217728 -o ./split-data/piece_ -i ./data/IN
  // argv[0] = ./sj.out
  // argv[1] = -s
  // argv[2] = 134217728
  // argv[3] = -o
  // argv[4] = ./split-data/piece_
  // argv[5] = -i
  // argv[6] = ./data/IN
  if (std::string(argv[1]) == "-s") //Split command
  {
    //Ensure there are enough arguments
    if (argc < 7)
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Ensure the chunk size entered is positive
    try
    {
      if (std::stoi(argv[2]) <= 0)
      {
        return E_SMALL_SIZE;
      }
    }
    catch (const std::exception&)
    {
      return E_SMALL_SIZE;
    }
    //Ensure the user specifies output command
    if (std::string(argv[3]) != "-o")
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Ensure the user specifies input command
    if (std::string(argv[5]) != "-i")
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Everything good so far, split the file
    return split(argv[6], argv[4], std::stoi(argv[2]));
  }
  
  else if (std::string(argv[1]) == "-j") //Join command
  {
    // ./prog.exe -j -o ./joined-data/collected -i ./split-data/chunk*
    // argv[0] = ./prog.exe
    // argv[1] = -j
    // argv[2] = -o
    // argv[3] = ./joined-data/collected
    // argv[4] = -i
    // argv[5] = ./split-data/chunk*

    //Ensure the user specifies output command
    if (std::string(argv[2]) != "-o")
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Ensure the user specifies input command
    if (std::string(argv[4]) != "-i")
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Determine the number of chunks to join based on the number of arguments
    int totalChunks = argc - 5;
    //Everything good so far, join the file
    return join(argv, totalChunks);
  }
  //If the user does not specify a split or join command, return no action
  return rs;
}
