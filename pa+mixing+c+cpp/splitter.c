/*!*****************************************************************************
 \file splitter.c
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course:  Modern C++ Design Patterns
 \par Section: B
 \par Assignment 1
 \date 30-08-2023
 \brief
    Implements functions to split and join binary files. Provides efficient 
    buffer management for reading/writing and includes error handling for 
    common file and memory operations. Implemented in C
*******************************************************************************/

#include "splitter.h"
#include <string.h>
#include <stdlib.h> 
#include <stdio.h>

// this is the maximum size of buffer for reading data from input file
// and to write data to output file ...
// if a file contains 10000 bytes, this means that you have to repeatedly
// [thrice] read the file for 4096 bytes, followed by the next 4096 bytes,
// followed by the final 1808 bytes.
// Not following this recipe by increasing the buffer to a higher value
// will mean that your submission is not following the specified requirements
// and will receive a zero grade!!!
#define FOUR_K 4096 // Don't modify this!!!

/*!*****************************************************************************
 * @brief This function will combine the chunks specified by the user.
 * 
 * @param inputFileName The name of the file to be joined
 * @param outputPathPrefix The prefix of the output file eg:./split-data/piece_
 * @param chunkSize The size of each chunk.
 * @return SplitResult 
*******************************************************************************/
SplitResult split(const char *inputFileName, const char *outputPathPrefix, int chunkSize) 
{
  SplitResult rs = E_NO_ACTION;
  //Open the input file in binary mode to maintain the integrity of the data
  //and platform independence.
  FILE* inputFile = fopen(inputFileName, "rb");
  //Ensure the input file can be opened
  if (!inputFile)
  {
    rs = E_BAD_SOURCE;
    return rs;
  }
  char* buffer = (char*)malloc(FOUR_K);
  int bytesRead = 0;
  int fileCount = 1;
  char outputFileName[256];
  int bytesLeftForCurrentChunk = chunkSize;

  while (1)
  {
      int bytesToRead = (bytesLeftForCurrentChunk < FOUR_K) ? bytesLeftForCurrentChunk : FOUR_K;
      bytesRead = fread(buffer, 1, bytesToRead, inputFile);
      if (bytesRead == 0)
          break;

      snprintf(outputFileName, sizeof(outputFileName), "%s%04d", outputPathPrefix, fileCount);
      FILE* outputFile = fopen(outputFileName, "ab"); // Append mode, so we can keep adding to the chunk
      if (!outputFile)
      {
          free(buffer);
          fclose(inputFile);
          return E_BAD_DESTINATION;
      }
      fwrite(buffer, 1, bytesRead, outputFile);
      fclose(outputFile);

      bytesLeftForCurrentChunk -= bytesRead;
      if (bytesLeftForCurrentChunk == 0) 
      {
          fileCount++;
          bytesLeftForCurrentChunk = chunkSize;
      }
  }
  //Free the memory now that we're done with the files
  free(buffer);
  fclose(inputFile);
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
  FILE* outputFile = fopen(argv[3], "wb");
  if (!outputFile) //Ensure the output file can be opened
  {
    rs = E_BAD_DESTINATION;
    return rs;
  }
  char* buffer = (char*)malloc(FOUR_K);
  for (int i = 0; i < totalChunks; i++)
  {
    FILE* inputFile = fopen(argv[i + 5], "rb");
    if (!inputFile)
    {
      free(buffer);
      fclose(outputFile);
      rs = E_BAD_SOURCE;
      return rs;
    }
    int bytesRead = 0;
    while (1)
    {
      bytesRead = fread(buffer, 1, FOUR_K, inputFile);
      if (bytesRead == 0)
        break;
      fwrite(buffer, 1, bytesRead, outputFile);
    }
    fclose(inputFile);
  }
  //Free the memory now that we're done with the files
  free(buffer);
  fclose(outputFile);
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
  if (strcmp(argv[1], "-s") == 0) //Split command
  {
    //Ensure there are enough arguments
    if (argc < 7)
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Ensure the chunk size entered is positive
    if (atoi(argv[2]) <= 0)
    {
      rs = E_SMALL_SIZE;
      return rs;
    }
    //Ensure the user specifies output command
    if (strcmp(argv[3], "-o") != 0)
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Ensure the user specifies input command
    if (strcmp(argv[5], "-i") != 0)
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Everything good so far, split the file
    rs = split(argv[6], argv[4], atoi(argv[2]));
    return rs;
  }
  
  else if (strcmp(argv[1], "-j") == 0) //Join command
  {
    // ./prog.exe -j -o ./joined-data/collected -i ./split-data/chunk*
    // argv[0] = ./prog.exe
    // argv[1] = -j
    // argv[2] = -o
    // argv[3] = ./joined-data/collected
    // argv[4] = -i
    // argv[5] = ./split-data/chunk*

    //Ensure the user specifies output command
    if (strcmp(argv[2], "-o") != 0)
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Ensure the user specifies input command
    if (strcmp(argv[4], "-i") != 0)
    {
      rs = E_NO_ACTION;
      return rs;
    }
    //Determine the number of chunks to join based on the number of arguments
    int totalChunks = argc - 5;
    //Everything good so far, join the file
    rs = join(argv, totalChunks);
    return rs;
  }
  else
  {
    //Invalid command
    rs = E_NO_ACTION;
  }
  return rs;
}
