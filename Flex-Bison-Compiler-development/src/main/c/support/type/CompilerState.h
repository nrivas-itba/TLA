#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

/**
 * The global state of the compiler. Should transport every data structure
 * needed across the different phases of a compilation.
 */

typedef struct ColorRgb ColorRgb;

struct ColorRgb {
	char r;
	char g;
	char b;
};
typedef struct {
	/**
	 * The root node of the AST.
	 */
	void * abstractSyntaxtTree;

	ColorRgb* image; //3chars per pixel
	int sizeX; //pixels
	int sizeY; //pixels
} CompilerState;

#endif
