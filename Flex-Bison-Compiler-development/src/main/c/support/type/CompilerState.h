#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

typedef struct {
    Program * abstractSyntaxtTree;
    const char * outputImageName; 
} CompilerState;

#endif