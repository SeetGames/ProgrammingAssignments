/*!*************************************************************************
****
\file splitter.c
\author Phua Jian Kai
\par DP email: p.jiankai@digipen.edu
\par Course: Modern C++ Design Patterns
\par Section: A
\par Assignment 1
\date 09-08-2022
\brief
This program splits or joins file, according to the command line arguments.
****************************************************************************
***/
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

    FILE* openedFile;
    FILE* destFile;
    char* buffer;
    buffer =  (char*)malloc(FOUR_K);
    if (buffer == NULL) {
      rs = E_NO_MEMORY;
      return rs;
    }

  destFile = fopen(argv[3], "w");
  if (destFile == NULL)
    {
      rs = E_BAD_DESTINATION;
      return rs;
    }

  int readFile = 0;

  for (int i = 0; i < files; i ++ ) {
    openedFile = fopen(argv[5 + i], "r");
    if (openedFile == NULL) {
      rs = E_BAD_SOURCE; 
      free(buffer);
      fclose(destFile);
      return rs;
    }
  while ((readFile = fread(buffer, sizeof(char), (FOUR_K / sizeof(char)), openedFile ))) {
     fwrite(buffer, sizeof(char), readFile, destFile);

     if (readFile != (FOUR_K / sizeof(char))) {
      break;
     }
  }

fclose(openedFile);
  }
  free(buffer);
  fclose(destFile);
  rs = E_JOIN_SUCCESS;
  return rs;

}
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

  FILE* OGfile;
  FILE* NewFile;

  OGfile = fopen(argv[6], "r");

  if (OGfile == NULL) {
    rs= E_BAD_SOURCE;
    return rs;
  }

int chunkCount = 0;
int chunkSize = atoi(argv[2]);
int toWrite = chunkSize;
/*
if (chunkSize > FOUR_K) {
  chunkSize = FOUR_K;
}
*/
int stringSize = strlen(argv[4]) + 5;
int readAmount= 0;


char* buffer;
  buffer =  (char*) malloc(FOUR_K);
  if (buffer == NULL)
  {
    rs = E_NO_MEMORY;
    return rs;
  }
char* fileName;
  fileName = (char*) malloc(stringSize * sizeof(char));
  if (fileName == NULL)
  {
    free(buffer);
    rs = E_NO_MEMORY;
    return rs;
  }
if (chunkSize <= FOUR_K) {
    while((readAmount = fread(buffer, sizeof(char), chunkSize / sizeof(char), OGfile)))
      {
        sprintf(fileName, "%s%04d", argv[4], chunkCount + 1);

        NewFile = fopen(fileName, "w ");
        if (NewFile == NULL) {
          free(buffer);
          free(fileName);
          fclose(OGfile);
          rs = E_BAD_DESTINATION;
          return rs;
        }
    fwrite(buffer,sizeof(char), readAmount,NewFile);
    chunkCount++;
  }
}
else {
      while((readAmount = fread(buffer, sizeof(char), FOUR_K / sizeof(char), OGfile)))
    {
    
      sprintf(fileName, "%s%04d", argv[4], chunkCount + 1);
  
      NewFile = fopen(fileName, "w");
      if (NewFile == NULL)
      {
        free(buffer);
        free(fileName);
        fclose(OGfile);
        rs = E_BAD_DESTINATION;
        return rs;
      }

      fwrite(buffer, sizeof(char), readAmount, NewFile);

      toWrite = chunkSize - FOUR_K;

      chunkCount++;
      while(toWrite > FOUR_K)
      {

        readAmount = fread(buffer, sizeof(char), FOUR_K / sizeof(char), OGfile);

        fwrite(buffer, sizeof(char), readAmount, NewFile);

        toWrite -= FOUR_K;
      }

      readAmount = fread(buffer, sizeof(char), toWrite / sizeof(char), OGfile);

      fwrite(buffer, sizeof(char), readAmount, NewFile);

      fclose(NewFile);
    }
}
  fclose(OGfile);
  free(buffer);
  free(fileName);

  rs = E_SPLIT_SUCCESS;
  return rs;
}


/*!*************************************************************************
****
\brief
Joins and splits files based on the command line arguments that are passed
through.
\param argc
Number of arguments
\param argv
Array of arguments
\return
Result of the split or join process
****************************************************************************
***/
SplitResult split_join(int argc, char *argv[]) {
  SplitResult rs = E_NO_ACTION;
	
  if (argc < 6)
  {
    rs = E_NO_ACTION;
    return rs;
  }

   if (strcmp(argv[1], "-j") == 0)
  {
    if (strcmp(argv[2], "-o") != 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }

    if (strcmp(argv[4], "-1") == 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }

    rs = joiner(argc, argv);
    return rs;
  }
if (strcmp(argv[1], "-s") == 0)
  {
    //if not enough command to work with, no acition will be taken
    if (argc < 7)
    {
      rs = E_NO_ACTION;
      return rs;
    }

    //if user inputted negative or zero chunk size
    if (atoi(argv[2]) <= 0)
    {
      rs = E_SMALL_SIZE;
      return rs;
    }

    //if user does not specify an output
    if (strcmp(argv[3], "-o") != 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }

    //if there is no input
    if (strcmp(argv[5], "-i") != 0)
    {
      rs = E_NO_ACTION;
      return rs; 
    }
    //return the result
    rs = spliter(argv);
    return rs;

  }
  // parse command-line arguments to determine whether to split a file
  // into smaller files	or to join smaller files into original file ...

  // ...

  return rs;
}
