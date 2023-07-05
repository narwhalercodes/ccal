#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "math.h"
#include "sfuncs.h"

// Reads an unescaped string (inputS) into another (outputS).
// Offset the input with inputSPtr and offset the output with outputSPtr.
// 
// If outputS contains NULL-references the chunks will be allocated dynamically.
// If requestChunk is not NULL then upon need, the function will be called to supply the next chunk for inputS (when
// NULL-chunks are encountered).
// Reads at most maxWrittenSize from inputS into outputS (unless maxWrittenSize = -1).
// If maxWrittenSize is set to -1, will try to read the whole string inputS (beware, if you have
// maxWrittenSize = -1 and you set the inputS length to a dummy value you may have to triple-check your requestChunk
// function terminates the string eventually).
// As an alternative (can also be used together with), if you dont know the actual length you can use haltOnNullChar
// to stop reading when a NULL-char is encountered.
// 
// returns 0 if was able to consume the whole string.
// returns -1 if was able to consume the whole string, ending at a NULL-char.
// returns -2 if maxWrittenSize was written from inputS into outputS but did not reach end of inputS.
// returns -3 if end of outputS was reached without consuming the whole string and without writing maxWrittenSize.
// returns -4 if inputS contains NULL-references before expected and requestChunk is either NULL or did not supply chunk.
// returns -5 if outputSPtr starts in the middle of a non-existant chunk.
// 
// Result value (ignore this by pointing to NULL)
// writtenLength <- the number of chars written (excluding terminating null-char).
int SReadChars(SContainer *inputS, SPointer *inputSPtr, SContainer *outputS, SPointer *outputSPtr,
               ChunkProviderFunc requestChunk, int maxWrittenSize, bool haltOnNullChar, int *writtenLength)
{
    *writtenLength = 0;

    if (outputS->chunks[outputSPtr->chunkInd] == NULL && outputSPtr->ind != 0)
    {
        return -5;
    }

    char **chunk2DVec = inputS->chunks;
    char **chunk2DVec2 = outputS->chunks;

    int chSize = inputS->chunkSize;
    int chSize2 = outputS->chunkSize;

    int chunk = inputSPtr->chunkInd;
    int chr = inputSPtr->ind;
    char *chunkVec = chunk2DVec[chunk];

    int chunk2 = outputSPtr->chunkInd;
    int chr2 = outputSPtr->ind;
    char *chunkVec2 = chunk2DVec2[chunk2];

    // "last" means excluding
    int firstInput = chunk * chSize + chr;
    int lastInput = MIN(inputS->length, maxWrittenSize == -1 ? INT_MAX : firstInput + maxWrittenSize);
    int firstOutput = chunk2 * chSize2 + chr2;
    int lastOutput = MIN(outputS->size * chSize2, maxWrittenSize == -1 ? INT_MAX : firstOutput + maxWrittenSize);

    bool hasSpace = lastOutput - firstOutput >= lastInput - firstInput;
    int maxWrittenActual = MIN(lastOutput - firstOutput, lastInput - firstInput);

    // nothing to write
    if (maxWrittenActual <= 0)
    {
        if (!hasSpace)
        {
            return -3;
        }
        else
        {
            return 0;
        }
    }

    // request chunk
    if (chunkVec == NULL)
    {
        if (requestChunk == NULL)
        {
            return -4;
        }
        requestChunk(chunk, inputS);
        chunkVec = chunk2DVec[chunk];
        if (chunkVec == NULL)
        {
            return -4;
        }
        lastInput = MIN(inputS->length, maxWrittenSize == -1 ? INT_MAX : firstInput + maxWrittenSize);
        hasSpace = lastOutput - firstOutput >= lastInput - firstInput;
        maxWrittenActual = MIN(lastOutput - firstOutput, lastInput - firstInput);
    }

    char c;
    int i;

    for (i = 0; i < maxWrittenActual; i++)
    {
        if (chr2 == 0)
        {
            chunkVec2 = chunk2DVec2[chunk2];

            // Create new chunk for output in case it is not allocated.
            if (chunkVec2 == NULL)
            {
                chunk2DVec2[chunk2] = calloc(chSize2 + 1, sizeof(char));
                chunkVec2 = chunk2DVec2[chunk2];
            }
        }

        if (chr == 0)
        {
            chunkVec = chunk2DVec[chunk];

            // request chunk
            if (chunkVec == NULL)
            {
                if (requestChunk == NULL)
                {
                    chunkVec2[chr2] = '\0';
                    *writtenLength = i;
                    outputS->length = firstOutput + i;
                    return -4;
                }
                requestChunk(chunk, inputS);
                chunkVec = chunk2DVec[chunk];
                if (chunkVec == NULL)
                {
                    chunkVec2[chr2] = '\0';
                    *writtenLength = i;
                    outputS->length = firstOutput + i;
                    return -4;
                }
                lastInput = MIN(inputS->length, maxWrittenSize == -1 ? INT_MAX : firstInput + maxWrittenSize);
                hasSpace = lastOutput - firstOutput >= lastInput - firstInput;
                maxWrittenActual = MIN(lastOutput - firstOutput, lastInput - firstInput);
            }
        }

        // write
        c = chunkVec[chr];
        chunkVec2[chr2] = c;

        // special termination option
        if (haltOnNullChar && c == '\0')
        {
            *writtenLength = i;
            outputS->length = firstOutput + i;
            return -1;
        }

        // Go to the next chunk if reached end, both for input and output.
        // Essentially a double for-loop but for parrallell counters at different inner loop sizes.
        if (++chr == chSize)
        {
            chr = 0;
            chunk++;
        }
        if (++chr2 == chSize2)
        {
            chr2 = 0;
            chunk2++;
        }
    }

    // terminate string if needed
    if (chunk2 < outputS->size && chr2 != 0)
    {
        outputS->chunks[chunk2][chr2] = '\0';
    }
    // set length and result
    outputS->length = firstOutput + i;
    *writtenLength = i;

    // determine success
    if (hasSpace)
    {
        if (*writtenLength >= inputS->length - firstInput)
        {
            return 0;
        }
        else
        {
            return -2;
        }
    }
    else
    {
        return -3;
    }
}

// Reads an escaped string (inputS) into another (outputS).
// Offset the input with inputSPtr and offset the output with outputSPtr.
// The inputSPtr should begin not at the opening '"' but at the character after.
// inputS must contain an unescaped '"' for this function to return 0.
// Escapes are '\' + (escape code) while other characters are read as-is.
// The line-feed charater is the only illegal character without a preceeding backslash,
// as this is to prevent multi-line input restriction circumvetions.
// To escape unintentional line-feed, use '\' + LF optionally followed by CR.
// To escape intentional line-feed, use '\' + 'n'.
// Any escape code not listed below is to be treated as the resulting character '?'.
// All escape codes:
// '\' + linefeed + carrigereturn = ""
// '\' + linefeed = ""
// "\"" = 34 = '"'
// "\'" = 39 = '''
// "\e" = 27 = escape
// "\?" = 63 = '?'
// "\\" = 92 = '\'
// "\a" = 7 = alert
// "\b" = 8 = backspace (does not remove chars)
// "\t" = 9 = tab
// "\n" = 10 = linefeed
// "\v" = 11 = verticaltab
// "\f" = 12 = formfeed
// "\r" = 13 = carrigereturn
// "\0" to "\7" = 0 to 9
// "\00" to "\77" = 0 to 63
// "\000" to "\777" = 0 to 511 (prone to UTF-8 convertion)
// "\x0" to "\xF" = 0 to 15
// "\x00" to "\xFF" = 0 to 255 (prone to UTF-8 convertion)
// "\x000" to "\xFFF" = 0 to 4095 (prone to UTF-8 convertion)
// "\x0000" to "\0xFFFF" = 0 to 65535 (prone to UTF-8 convertion)
// "\x00000" to "\0xFFFFF" = 0 to 1048575 (prone to UTF-8 convertion)
// "\x000000" to "\0xFFFFFF" = 0 to 16777215 (prone to UTF-8 convertion, replaced by '?' when over 0x10FFFF)
// "\u0000" to "\uFFFF" = 0 to 65535 (prone to UTF-8 convertion)
// "\U00000000" to "\UFFFFFFFF" = 0 to 4294967295 (prone to UTF-8 convertion, replaced by '?' when over 0x10FFFF)
// 
// Attempts to read until the closing quote '"'.
// If outputS contains NULL-references the chunks will be allocated dynamically.
// If requestChunk is not NULL then upon need, the function will be called to supply the next chunk for inputS (when
// NULL-chunks are encountered).
// Reads at most maxReadSize and writes at most maxWrittenSize (unless the respective is equal to -1).
// If maxReadSize is set to -1, will try to read the whole string inputS (beware, if you have maxReadSize = -1 and
// you set the inputS length to a dummy value you may have to triple-check your requestChunk function terminates the
// string eventually).
// As an alternative (can also be used together with), if you dont know the actual length you can use haltOnNullChar
// to stop reading when a NULL-char is encountered.
// 
// returns 0 if no unescaped newline persent and was able to consume the whole string.
// returns -1 if no unescaped newline persent and was able to consume the whole string, ending at a NULL-char.
// returns -2 if unescaped newline was present before reading maxReadChars.
// returns -3 if no unescaped newline persent but maxChars was read without an unescaped '"'.
// returns -4 if no unescaped newline persent but reached end of inputS before reading maxReadChars.
// returns -5 if no unescaped newline persent but wrote maxWrittenSize chars to outputS before reading maxReadChars.
// returns -6 if no unescaped newline persent but reached end of outputS before reading maxReadChars.
// returns -7 if inputS contains NULL-references before expected and requestChunk is either NULL or did not supply chunk.
// returns -8 if outputSPtr starts in the middle of a non-existant chunk.
// 
// Result values (ignore these by pointing to NULL)
// resultLength <- number of written chars (excluding terminating null-char).
// resultUnescapedLength <- number of read chars to produce the written chars (excluding terminating null-char)
//                          (if an escape is midway the escape is not written and the resultUnescapedLength is set
//                          to the value as if not read).
/*int SReadEscapedChars(SContainer *inputS, SPointer *inputSPtr, SContainer *outputS, SPointer *outputSPtr,
                      ChunkProviderFunc requestChunk, int maxReadSize, int maxWrittenSize, bool haltOnNullChar,
                      int *resultLength, int *resultUnescapedLength)
{
    quotesPtr = insideQuotesPtr;
    bool breakOnNullChar = maxSize == -1;
    int actualMax = maxSize == -1 ? chunkSize*chunks - 1 : MIN(maxSize, chunkSize*chunks - 1);
    int currChunk = 0;
    if (buffer[0] == NULL)
        buffer[0] = calloc(chunkSize, sizeof(char));
    char *bufPtr = buffer[0];
    bool escapeNext = false;
    char c;

    for (int i = 0; i < actualMax; i++)
    {
        // TODO
    }
}*/
