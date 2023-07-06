#ifndef CCAL_SFUNCS_H
#define CCAL_SFUNCS_H

// Represents a chunked string.
// chunkSize - size of chunks - 1
// size - number of chunks
// length - the used number of chars
// 
// Note:
// Null-chars are vaild chars.
// While the chunkSize + 1 is the actual size of the chunk, only chunkSize is to be used to store the substrings,
// since chunks are always supposed to be null-terminated, however a null-char does not mean the string ends,
// even if the null-char is midway into the chunk.
typedef struct SContainer {
    int chunkSize;
    int size;
    int length;
    char **chunks;
} SContainer;

// An index for a specific sized SContainer.
typedef struct SPointer {
    int chunkInd;
    int ind;
} SPointer;

// A chunk provider func should allways set the bits to 0 with calloc().
typedef void (*ChunkProviderFunc)(int, SContainer *, void *);

int SReadChars(SContainer *inputS, SPointer inputSPtr, SContainer *outputS, SPointer outputSPtr,
               ChunkProviderFunc requestChunk, void *requestChunkArgs, int maxWrittenSize, bool haltOnNullChar,
               int *writtenLength);

/*int SReadEscapedChars(SContainer *inputS, SPointer *inputSPtr, SContainer *outputS, SPointer *outputSPtr,
                      ChunkProviderFunc requestChunk, void *requestChunkArgs, int maxReadSize, int maxWrittenSize, bool haltOnNullChar,
                      int *resultLength, int *resultUnescapedLength);*/

void SContainerFree(SContainer *sc);

#endif
