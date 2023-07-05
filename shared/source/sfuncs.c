#include <stdbool.h>
#include "sfuncs.h"

// Reads an escaped string starting at the char right after the opening quote '"'
// escapes are '\' + (any) while other characters are read as-is unless part of the predefined escapes.
// The line-feed charater is however illegal without a preceeding backslash,
// as this is to indicate deletion of said linefeed + potential carrige return after it.
// Please use the escaped characters '\' + 'n' for the line feed.
// All escapes:
// '\' + linefeed + carrigereturn = ""
// '\' + linefeed = ""
// "\"" = 34 = '"'
// "\\" = 92 = '\'
// "\a" = 7 = alert
// "\b" = 8 = backspace (does not remove chars)
// "\t" = 9 = tab
// "\n" = 10 = linefeed
// "\v" = 11 = vertical tab
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
// Attempts to read until the closing quote '"'.
// If buffer contains NULL-refs the chunks will be allocated dynamically.
// If maxSize is set to -1, the read will end on a NULL-char (not the nullchar escape), unless buffer fills.
//
// returns chunksize*chunks if more space is required to store the full string inside the chunked buffer with string termination.
// otherwise returns the string length of the string by concatenating resulting chunks.
// returns the end of the string to quotesPtr (points to the trailing '"').
int SReadEscapedChars(char *insideQuotesPtr, char *quotesPtr, int chunkSize, int chunks, char **buffer, int maxSize)
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
        
    }
}