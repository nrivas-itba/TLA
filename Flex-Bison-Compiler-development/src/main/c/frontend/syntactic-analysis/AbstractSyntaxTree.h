#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum SentenceType SentenceType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;
typedef struct Range Range;
typedef struct View View;
typedef struct DoubleConstant DoubleConstant;
typedef struct Sentence Sentence;
typedef struct SentenceList SentenceList;
typedef struct Size Size;
typedef struct Color Color;
typedef struct Variable Variable;
typedef struct Rule Rule;
typedef struct RuleSentenceList RuleSentenceList;
typedef struct RuleSentence RuleSentence;
typedef struct Polygon Polygon;
typedef struct PointList PointList;
typedef struct Point Point;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

enum SentenceType {
	SENTENCE_EXPRESSION,
	SENTENCE_VIEW,
	SENTENCE_SIZE,
	SENTENCE_COLOR,
	SENTENCE_RULE
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

/*new*/


struct Program {
	SentenceList * sentenceList;
};

struct SentenceList {
	Sentence * sentence;
	SentenceList * next;
};

struct Sentence {
	union {
		Expression * expression;
		View * view;
		Size * size;
		Color * color;
		Rule * rule;
	};
	SentenceType sentenceType;
};

struct Range {
	DoubleConstant * start;
	DoubleConstant * end;
};

struct View {
	Range * x;
	Range * y;
};

struct DoubleConstant {
	double value;
};

struct Size {
	Constant* x;
	Constant* y;
};

struct Color {
	char * startColor;
	char * endColor;
};

struct Variable {
	char * name;
};

struct Rule {
	struct Variable * variable;
	struct RuleSentenceList * ruleSentenceList;
};

struct RuleSentenceList {
	struct RuleSentence * ruleSentence;
	struct RuleSentenceList * next;
};
struct RuleSentence {
	Polygon * polygon;
};

struct Polygon {
	PointList * pointList;
};

struct PointList {
	Point* point;
	PointList * next;
};

struct Point {
	DoubleConstant * x;
	DoubleConstant * y;
};


/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);

#endif
