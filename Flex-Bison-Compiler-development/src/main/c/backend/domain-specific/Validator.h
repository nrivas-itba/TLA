#ifndef VALIDATOR_HEADER
#define VALIDATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include <limits.h>
#include <stdbool.h>

typedef enum ExpressionTypeBack ExpressionTypeBack;
typedef enum FactorTypeBack FactorTypeBack;
typedef enum RuleSentenceTypeBack RuleSentenceTypeBack;
typedef enum TransformationSentenceTypeBack TransformationSentenceTypeBack;
typedef enum EscapeFactorTypeBack EscapeFactorTypeBack;

typedef struct ConstantBack ConstantBack;
typedef struct ExpressionBack ExpressionBack;
typedef struct FactorBack FactorBack;
typedef struct ProgramBack ProgramBack;
typedef struct RangeBack RangeBack;
typedef struct ViewBack ViewBack;
typedef struct DoubleConstantBack DoubleConstantBack;
typedef struct RuleListBack RuleListBack;
typedef struct SizeBack SizeBack;
typedef struct ColorBack ColorBack;
typedef struct VariableBack VariableBack;
typedef struct RuleBack RuleBack;
typedef struct IdentifierListBack IdentifierListBack;
typedef struct RuleSentenceListBack RuleSentenceListBack;
typedef struct RuleSentenceBack RuleSentenceBack;
typedef struct PolygonBack PolygonBack;
typedef struct PointListBack PointListBack;
typedef struct PointBack PointBack;
typedef struct StartBack StartBack;
typedef struct ExpressionListBack ExpressionListBack;
typedef struct CallBack CallBack;
typedef struct IfStatementBack IfStatementBack;
typedef struct TransformationBack TransformationBack;
typedef struct TransformListBack TransformListBack;
typedef struct TransformationSentenceBack TransformationSentenceBack;
typedef struct PointsStatementBack PointsStatementBack;
typedef struct EscapeExpressionBack EscapeExpressionBack;
typedef struct EscapeFactorBack EscapeFactorBack;
typedef struct EscapeRangeBack EscapeRangeBack;
typedef struct EscapeBack EscapeBack;
typedef struct TransformationList TransformationList;

enum ExpressionTypeBack {
	ADDITION_BACK,
	DIVISION_BACK,
	FACTOR_BACK,
	MULTIPLICATION_BACK,
	SUBTRACTION_BACK,
	LOWER_THAN_OP_BACK,
	GREATER_THAN_OP_BACK,
	ABSOLUTE_VALUE_BACK
};

enum FactorTypeBack {
	CONSTANT_BACK,
	EXPRESSION_BACK,
	DOUBLE_CONSTANT_BACK,
	VARIABLE_BACK,
	RANGE_BACK,
	X_COORD_FACTOR_BACK,
	Y_COORD_FACTOR_BACK
};

enum RuleSentenceTypeBack {
	RULE_SENTENCE_POLYGON_BACK,
	RULE_SENTENCE_CALL_BACK,
	RULE_SENTENCE_IF_BACK,
	RULE_SENTENCE_ESCAPE_BACK
};

enum TransformationSentenceTypeBack {
	TRANSLATE_SENTENCE_BACK,
	SCALE_SENTENCE_BACK,
	ROTATE_SENTENCE_BACK,
	SHEAR_SENTENCE_BACK
};

struct ConstantBack {
	int value;
};

struct EscapeFactorBack {
	union {
		EscapeExpressionBack * expression;
		ConstantBack * constant;
		VariableBack * variable;
		DoubleConstantBack* doubleConstant;
		EscapeRangeBack* range;
	};
	FactorTypeBack type;
};

struct EscapeRangeBack {
	EscapeExpressionBack * start;
	EscapeExpressionBack * end;
};

struct EscapeExpressionBack {
	union {
		EscapeFactorBack * factor;
		struct {
			EscapeExpressionBack * leftExpression;
			EscapeExpressionBack * rightExpression;
		};
	};
	ExpressionTypeBack type;
};

struct EscapeBack {
	EscapeExpressionBack* initialValue;
	VariableBack* variable;
	EscapeExpressionBack* recursiveAssigment;
	EscapeExpressionBack* untilCondition;
	ConstantBack* maxIterations;
};

struct FactorBack {
	union {
		ConstantBack * constant;
		ExpressionBack * expression;
		VariableBack * variable;
		DoubleConstantBack* doubleConstant;
	};
	FactorTypeBack type;
};

struct ExpressionBack {
	union {
		FactorBack * factor;
		struct {
			ExpressionBack * leftExpression;
			ExpressionBack * rightExpression;
		};
	};
	ExpressionTypeBack type;
};

struct ExpressionListBack {
	ExpressionBack* expression;
	ExpressionListBack* next;
};

struct ProgramBack {
    ViewBack* view;
    SizeBack* size;
    ColorBack* ColorBack;
    RuleListBack * ruleList;
    StartBack * start;
};

struct RuleListBack {
	RuleBack * rule;
	RuleListBack * next;
};

struct RangeBack {
	ExpressionBack * start;
	ExpressionBack * end;
};

struct ViewBack {
	RangeBack * x;
	RangeBack * y;
};

struct DoubleConstantBack {
	double value;
};

struct SizeBack {
	int x;
	int y;
};

struct ColorBack {
	char * startColor;
	char * endColor;
};

struct VariableBack {
	char * name;
};

struct RuleBack {
	VariableBack * variable;
	IdentifierListBack * identifierList;
    TransformationList* transformationList;
	RuleSentenceListBack * ruleSentenceList;
};

struct IdentifierListBack {
	VariableBack * variable;
	IdentifierListBack * identifierList;
};

struct RuleSentenceListBack {
	RuleSentenceBack * ruleSentence;
	RuleSentenceListBack * next;
};

struct TransformationList {
    TransformationBack * transformation;
    TransformationList* next;
    PointsStatementBack* pointsStatement;
};

struct RuleSentenceBack {
	union {
		PolygonBack * polygon;
		CallBack * call;
		IfStatementBack* ifStatement;
		EscapeBack * escape;
	};
	RuleSentenceTypeBack ruleSentenceType;
};

struct TransformationBack {
	ConstantBack* probability;
	TransformListBack* transformList;
};

struct TransformListBack {
	TransformationSentenceBack* transformationSentence;
	TransformListBack* next;
};

struct TransformationSentenceBack {
	TransformationSentenceTypeBack transformationSentenceType;
	union {
		ExpressionBack* angle;
		struct {
			ExpressionBack* x;
			ExpressionBack* y;
		};
	};
};

struct IfStatementBack {
	ExpressionBack* condition;
};

struct CallBack {
	VariableBack * variable;
	ExpressionListBack * expressionList;
};

struct PolygonBack {
	PointListBack * pointList;
};

struct PointListBack {
	PointBack* point;
	PointListBack * next;
};

struct PointBack {
	ExpressionBack * x;
	ExpressionBack * y;
};

struct StartBack {
	VariableBack * variable;
};

struct PointsStatementBack {
	ConstantBack* numPoints;
};


/** Initialize module's internal state. */
ModuleDestructor initializeValidatorModule();

/**
 * The result of a computation. It's considered valid only if "succeed" is
 * true.
 */
typedef struct {
	bool succeeded;
} ComputationResult;

/**
 * Validates the program using the current compiler state.
 * Returns a ComputationResult with succeeded=true when validation passes.
 */
ComputationResult executeValidator(CompilerState * compilerState);


#endif