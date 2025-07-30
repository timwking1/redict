/*=============================================================================
*       redict.c
*       timwking1
*       29-Jul 2025
*
*       Tool to downsize a dictionary to only words of a certain length
*       Usage: <input_file> <output_file> <word_length>
*
=============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define ARG_INPATH          1
#define ARG_OUTPATH         2
#define ARG_WORDLEN         3

#define NUM_REQ_ARGS        4

#define MAX_WORDLEN         255

typedef struct 
{ 
    FILE* fpIn;
    FILE* fpOut;
} HDFILE;

/*=============================================================================
*   Function Prototypes
=============================================================================*/
void PrintHelp();
bool ValidateArgs(int argc, char** argv);
bool ValidateWordLenString(const char* str);
bool ValidateWordLenInt(uint32_t wordLen);
HDFILE* OpenFileHandle(const char* inFilePath, const char* outFilePath);
void CloseFileHandle(HDFILE* file);
uint32_t WriteNewDictionary(HDFILE* file, uint32_t wordLen);

/*=============================================================================
*   main [int]
*       Entry point
*
*       int argc                Argument count
*       char** argv             Argument inputs
*
=============================================================================*/
int main(int argc, char** argv)
{
    //Validate argument inputs
    if(!ValidateArgs(argc, argv))
    {
        PrintHelp();
        return 1;
    }
    char* endPtr;
    uint32_t wordLen = (uint32_t)strtoul(argv[ARG_WORDLEN], &endPtr, 10);
    if(*endPtr != '\0' || !ValidateWordLenInt(wordLen))
    {
        fprintf(stderr, "Invalid word length '%s'\n", argv[ARG_WORDLEN]);
        return 1;
    }

    HDFILE* fileHandle = OpenFileHandle(argv[ARG_INPATH], argv[ARG_OUTPATH]);
    if(!fileHandle)
    {
        return 1;
    }

    uint32_t count = WriteNewDictionary(fileHandle, wordLen);
    fprintf(stdout, "Wrote %zu words of length %u\n", count, wordLen);
    CloseFileHandle(fileHandle);
    return 0;
}

/*=============================================================================
*   PrintHelp [void]
*       Print usage instructions text to stdout
*       This gets it's own function in case we want to add more lines later on
*
=============================================================================*/
void PrintHelp()
{
    fprintf(stdout, "Usage: <input_file> <output_file> <word_length>\n");
    return;
}

/*=============================================================================
*   ValidateArgs [bool]
*       Validate argument values
*
*       int argc                Argument count
*       char** argv             Argument inputs
*
=============================================================================*/
bool ValidateArgs(int argc, char** argv)
{
    //If arguments given is not between the range of required and total args
    if(argc != NUM_REQ_ARGS)
    {
        return false;
    }
    if(!argv[ARG_INPATH] || strlen(argv[ARG_INPATH]) == 0)
    {
        fprintf(stderr, "Input file path cannot be empty\n");
        return false;
    }
    if(!argv[ARG_OUTPATH] || strlen(argv[ARG_OUTPATH]) == 0)
    {
        fprintf(stderr, "Output file path cannot be empty\n");
        return false;
    }
    if(strcmp(argv[ARG_INPATH], argv[ARG_OUTPATH]) == 0)
    {
        fprintf(stderr, "Input and output files cannot be the same\n");
        return false;
    }
    if(!ValidateWordLenString(argv[ARG_WORDLEN]))
    {
        fprintf(stderr, "Invalid word length '%s'\n", argv[ARG_WORDLEN]);
        return false;
    }
    return true;
}

/*=============================================================================
*   ValidateWordLenString [bool]
*       Evaluate if word length string is empty
*
*       const char* str         word length string from argv
*
=============================================================================*/
bool ValidateWordLenString(const char* str)
{
    if (!str || *str == '\0') 
    {
        return false;
    }
    return true;
}

/*=============================================================================
*   ValidateWordLenInt [bool]
*       Evaluate if word length int is in an acceptable range
*
*       uint32_t wordLen        wordLength parsed from argv
*
=============================================================================*/
bool ValidateWordLenInt(uint32_t wordLen)
{
    if(wordLen <= 0 || wordLen > MAX_WORDLEN)
    {
        return false;
    }
    return true;
}

/*=============================================================================
*   OpenFileHandle [HDFILE*]
*       Get I/O handles for input and output files
*
*       const char* inFilePath  input file
*       const char* outFilePath output file to be written
*       uint32_t wordLen        length of word we want to check for
*
=============================================================================*/
HDFILE* OpenFileHandle(const char* inFilePath, const char* outFilePath)
{
    HDFILE* result = malloc(sizeof(HDFILE));
    if(!result)
    {
        //alloc fail
        fprintf(stderr, "Couldn't allocate file handle");
        free(result);
        return NULL;
    }
    //Open input file for reading
    FILE* fpIn = fopen(inFilePath, "r");
    if(!fpIn)
    {
        fprintf(stderr, "Couldn't open file: %s\n", inFilePath);
        free(result);
        return NULL;
    }

    //Open output file for writing
    FILE* fpOut = fopen(outFilePath, "w");
    if(!fpOut) 
    {
        fprintf(stderr, "Couldn't open file: %s\n", outFilePath);
        //Close previously opened input
        fclose(fpIn);
        free(result);
        return NULL;
    }
    result->fpIn = fpIn;
    result->fpOut = fpOut;
    return result;
}

/*=============================================================================
*   CloseFileHandle [void]
*       Close file handles and free memory used by HDFILE struct
*
*       HDFILE* file            pointer to handle struct
*
=============================================================================*/
void CloseFileHandle(HDFILE* file)
{
    if(file)
    {
        if(file->fpIn)
        {
            fclose(file->fpIn);
        }
        if(file->fpOut)
        {
            fclose(file->fpOut);
        }
        free(file);
    }
    return;
}

/*=============================================================================
*   WriteNewDictionary [void]
*       Iterates each newline of the input file and discards any lines
*       whose length doesn't match wordLen...
*       Prints result to out file using fprintf
*       
*       HDFILE* file            Pointer to File io handle struct
*       uint32_t wordLen        length of line (in characters, excluding \n)
*                               that are kept in the output file
*
=============================================================================*/
uint32_t WriteNewDictionary(HDFILE* file, uint32_t wordLen)
{
    uint32_t writtenLines = 0;
    char buffer[MAX_WORDLEN + 2];

    while(fgets(buffer, sizeof(buffer), file->fpIn)) 
    {
        uint32_t len = strlen(buffer);

        //Remove newline and carriage returns
        while(len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) 
        {
            buffer[--len] = '\0';
        }
        if(len == wordLen) 
        {
            fprintf(file->fpOut, "%s\n", buffer);
            writtenLines++;
        }
    }
    return writtenLines;
}
