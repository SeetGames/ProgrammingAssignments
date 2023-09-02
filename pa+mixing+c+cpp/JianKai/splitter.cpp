/*!*************************************************************************
****
\file splitter.cpp
\author Phua Jian Kai
\par DP email: p.jiankai@digipen.edu
\par Course: Modern C++ Design Patterns
\par Section: A
\par Assignment 1
\date 09-08-2022
\brief
This program splits or joins file, according to the command line arguments.
****************************************************************************/
#include "splitter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
// this is the maximum size of buffer for reading data from input file
// and to write data to output file ...
// if a file contains 10000 bytes, this means that you have to repeatedly
// [thrice] read the file for 4096 bytes, followed by the next 4096 bytes,
// followed by the final 1808 bytes.
// Not following this recipe by increasing the buffer to a higher value
// will mean that your submission is not following the specified requirements
// and will receive a zero grade!!!
constexpr int FOUR_K {4096}; // Don't modify this!!!
/******************************************************************************/
/*!
Function Level Documentation:
\brief  
    This function will split files into chunks.
\param argv
    This contains the various arguments that is stored in an array.
*/
/******************************************************************************/ 

SplitResult spliter(char* argv[]) {
  SplitResult rs = E_NO_ACTION;
  int chunkCount =0;
  int chunkSize = atoi(argv[2]);
    int toWrite = chunkSize;
    /*
  if (chunkSize > FOUR_K) {
    chunkSize = FOUR_K;
  }
  */

  std::ifstream OGfile;
  std::ofstream newFile;

  OGfile.open(argv[6]);
  if (OGfile.fail()) {
    rs = E_BAD_SOURCE;
    return rs;
  }
  char* buffer;
  buffer = new char[FOUR_K];
  if (buffer == NULL) {
    rs = E_NO_MEMORY;
    return rs;
  }


if (chunkSize <= FOUR_K) {
  while(!OGfile.eof()) {

    OGfile.read(buffer,chunkSize);
    std::stringstream fileName;
    fileName << argv[4] << std::setfill('0') << std::setw (4) << (chunkCount +1) ;
    if (OGfile.gcount() == 0) {
      break;
    }
    newFile.open(fileName.str(),std::ios::out);
    if (newFile.fail()) {
      OGfile.close();
      delete[] buffer;
      rs = E_BAD_DESTINATION;
      return rs;
    }

    newFile.write(buffer,OGfile.gcount());
    chunkCount++;
    newFile.close();
  }
}
else {
  while(!OGfile.eof()) {
    OGfile.read(buffer,chunkSize);
    std::stringstream fileName;
    fileName << argv[4] << std::setfill('0') << std::setw (4) << (chunkCount +1) ;
    if (OGfile.gcount() == 0) {
      break;
    }
    newFile.open(fileName.str(),std::ios::out);
    if(newFile.fail()) {
      OGfile.close();
      delete[] buffer;
      rs = E_BAD_DESTINATION;
      return rs;
    }
    newFile.write(buffer,OGfile.gcount());

    toWrite = chunkSize - FOUR_K;
    chunkCount++;

  while(toWrite > FOUR_K) {
    OGfile.read(buffer,FOUR_K);
    newFile.write(buffer,OGfile.gcount());
    if (OGfile.gcount() != (FOUR_K / sizeof(char))) {
      break;
    }
    toWrite -= FOUR_K;
  }
  OGfile.read(buffer,FOUR_K);
  newFile.write(buffer,OGfile.gcount());
  newFile.close();
  }
}

  OGfile.close();
  delete[] buffer;
  rs = E_SPLIT_SUCCESS;
  return rs;


}
/******************************************************************************/
/*!
Function Level Documentation:
\brief  
    This function will combine the chunks specified by the user.
\param argc
    This contains the number of elements in argv
\param argv
    This contains the various arguments that is stored in an array.
*/
/******************************************************************************/ 
SplitResult joiner(int argc, char* argv[]) {
  int files = argc - 5;
  SplitResult rs = E_NO_ACTION;

  std::ifstream openedFile;
  std::ofstream destFile;

  destFile.open(argv[3]);
  if (destFile.fail()) {
    rs = E_BAD_DESTINATION;
    return rs;
  }

  char* buffer;
  buffer = new char[FOUR_K];
  if (buffer == nullptr)
  {
    rs = E_NO_MEMORY;
    destFile.close();
    return rs;
  }

  int readFiles = 0;

  for (int i = 0; i < files; i ++) {
    openedFile.open(argv[5 + i]);
    if (openedFile.fail()) {
      rs = E_BAD_SOURCE;
      delete[] buffer;
      destFile.close();
      return rs;
    }

    while (!openedFile.eof()) {
      openedFile.read(buffer, FOUR_K);
      readFiles = openedFile.gcount();
      destFile.write(buffer,readFiles);
      if (readFiles != (FOUR_K / sizeof(char))) {
        break;
      }
    }
    openedFile.close();
  }
  delete[] buffer;
  destFile.close();
  rs = E_JOIN_SUCCESS;
  return rs;
}

/******************************************************************************/
/*!
Function Level Documentation:
\brief  
Joins and splits files based on the command line arguments that are passed
through.
\param argc
Number of arguments
\param argv
Array of arguments
\return
Result of the split or join process
/******************************************************************************/ 
SplitResult split_join(int argc, char *argv[]) {
  SplitResult rs = E_NO_ACTION;
 if (argc < 6)
  {
    rs = E_NO_ACTION;
    return rs;
  }
  std::string argv1;
  argv1 = argv[1];
  //if user wants to join chunks
  if (argv1.compare("-j") == 0)
  {
    argv1 = argv[2];
    if (argv1.compare("-o") != 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }
    argv1 = argv[4];
    if (argv1.compare("-1") == 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }

    rs = joiner(argc, argv);
    return rs;
  }


  //if user wants to split chunks
  argv1 = argv[1];
  if (argv1.compare("-s") == 0)
  {
    //if not enough command to work with, no acition will be taken
    if (argc < 7)
    {
      rs = E_NO_ACTION;
      return rs;
    }

    if (atoi(argv[2]) <= 0)
    {
      rs = E_SMALL_SIZE;
      return rs;
    }
    argv1 = argv[3];
    if (argv1.compare("-o") != 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }
    argv1 = argv[5];
    if (argv1.compare("-i") != 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }

    rs = spliter(argv);
    return rs;

  }

  return rs;
}
