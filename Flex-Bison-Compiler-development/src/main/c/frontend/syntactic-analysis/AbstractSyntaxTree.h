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
typedef enum RuleSentenceType RuleSentenceType;
typedef enum TransformationSentenceType TransformationSentenceType;

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
typedef struct IdentifierList IdentifierList;
typedef struct RuleSentenceList RuleSentenceList;
typedef struct RuleSentence RuleSentence;
typedef struct Polygon Polygon;
typedef struct PointList PointList;
typedef struct Point Point;
typedef struct Start Start;
typedef struct ExpressionList ExpressionList;
typedef struct Call Call;
typedef struct IfStatement IfStatement;
typedef struct Transformation Transformation;
typedef struct TransformList TransformList;
typedef struct TransformationSentence TransformationSentence;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION,
	LOWER_THAN_OP,
	GREATER_THAN_OP
};

enum FactorType {
	CONSTANT,
	EXPRESSION,
	DOUBLE_CONSTANT,
	VARIABLE
};

enum SentenceType {
	SENTENCE_EXPRESSION,
	SENTENCE_VIEW,
	SENTENCE_SIZE,
	SENTENCE_COLOR,
	SENTENCE_RULE,
	SENTENCE_START
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
		Variable * variable;
		DoubleConstant* doubleConstant;
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

struct ExpressionList {
	Expression* expression;
	ExpressionList* next;
};

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
		Start * start;
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
	Variable * variable;
	IdentifierList * identifierList;
	RuleSentenceList * ruleSentenceList;
};

struct IdentifierList {
	Variable * variable;
	IdentifierList * identifierList;
};

struct RuleSentenceList {
	RuleSentence * ruleSentence;
	RuleSentenceList * next;
};

enum RuleSentenceType {
	RULE_SENTENCE_POLYGON,
	RULE_SENTENCE_CALL,
	RULE_SENTENCE_IF,
	RULE_SENTENCE_TRANSFORMATION
};

struct RuleSentence {
	union {
		Polygon * polygon;
		Call * call;
		IfStatement* ifStatement;
		Transformation * transformation;
	};
	RuleSentenceType ruleSentenceType;
};

struct Transformation {
	Constant* probability;
	TransformList* transformList;
};

struct TransformList {
	TransformationSentence* transformationSentence;
	TransformList* next;
};

enum TransformationSentenceType {
	TRANSLATE_SENTENCE,
	SCALE_SENTENCE,
	ROTATE_SENTENCE,
	SHEAR_SENTENCE
};

struct TransformationSentence {
	TransformationSentenceType transformationSentenceType;
	union {
		Expression* angle;
		struct {
			Expression* x;
			Expression* y;
		};
	};
};

struct IfStatement {
	Expression* condition;
};

struct Call {
	Variable * variable;
	ExpressionList * expressionList;
};

struct Polygon {
	PointList * pointList;
};

struct PointList {
	Point* point;
	PointList * next;
};

struct Point {
	Expression * x;
	Expression * y;
};

struct Start {
	Variable * variable;
};


/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);
void destroyStart(Start * start);

#endif
