#include "Validator.h"

typedef struct RuleList RuleList;
typedef struct ColorRgb ColorRgb;
typedef struct State State;

struct RuleList {
	Rule rule;
	RuleList* next;
};

struct ColorRgb {
	char r;
	char g;
	char b;
};

struct State {
	RuleList* ruleList;
	bool succeeded;
	
	View view;

	ColorRgb* image; //3chars per pixel
	int sizeX; //pixels
	int sizeY; //pixels

	ColorRgb start;
	ColorRgb end;
};

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownValidatorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Validator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeValidatorModule() {
	_logger = createLogger("Validator");
	return _shutdownValidatorModule;
}

/** PRIVATE FUNCTIONS */

static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type);
static ComputationResult _invalidBinaryOperator(const int x, const int y);
static ComputationResult _invalidComputation();

/**
 * Converts and expression type to the proper binary operator. If that's not
 * possible, returns computationResult binary operator that always returns an invalid
 * computation result.
 */
static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type) {
	switch (type) {
		case ADDITION: return add;
		case DIVISION: return divide;
		case MULTIPLICATION: return multiply;
		case SUBTRACTION: return subtract;
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return _invalidBinaryOperator;
	}
}

/**
 * A binary operator that always returns an invalid computation result.
 */
static ComputationResult _invalidBinaryOperator(const int x, const int y) {
	return _invalidComputation();
}

/**
 * A computation that always returns an invalid result.
 */
static ComputationResult _invalidComputation() {
	ComputationResult computationResult = {
		.succeeded = false,
		.value = 0
	};
	return computationResult;
}

/** PUBLIC FUNCTIONS */

ComputationResult add(const int leftAddend, const int rightAddend) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = leftAddend + rightAddend
	};
	return computationResult;
}

ComputationResult divide(const int dividend, const int divisor) {
	const int sign = dividend < 0 ? -1 : +1;
	const bool divisionByZero = divisor == 0 ? true : false;
	if (divisionByZero) {
		logError(_logger, "The divisor cannot be zero (the computation was %d/%d).", dividend, divisor);
	}
	ComputationResult computationResult = {
		.succeeded = divisionByZero ? false : true,
		.value = divisionByZero ? (sign * INT_MAX) : (dividend / divisor)
	};
	return computationResult;
}

ComputationResult multiply(const int multiplicand, const int multiplier) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = multiplicand * multiplier
	};
	return computationResult;
}

ComputationResult subtract(const int minuend, const int subtract) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = minuend - subtract
	};
	return computationResult;
}

ComputationResult computeConstant(Constant * constant) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = constant->value
	};
	return computationResult;
}

ComputationResult computeExpression(Expression * expression) {
	switch (expression->type) {
		case ADDITION:
		case DIVISION:
		case MULTIPLICATION:
		case SUBTRACTION:
			ComputationResult leftResult = computeExpression(expression->leftExpression);
			ComputationResult rightResult = computeExpression(expression->rightExpression);
			if (leftResult.succeeded && rightResult.succeeded) {
				BinaryOperator binaryOperator = _expressionTypeToBinaryOperator(expression->type);
				return binaryOperator(leftResult.value, rightResult.value);
			}
			else {
				return _invalidComputation();
			}
		case FACTOR:
			return computeFactor(expression->factor);
		default:
			return _invalidComputation();
	}
}

ComputationResult computeFactor(Factor * factor) {
	switch (factor->type) {
		case CONSTANT:
			return computeConstant(factor->constant);
		case EXPRESSION:
			return computeExpression(factor->expression);
		default:
			return _invalidComputation();
	}
}

ComputationResult computeSentenceList(State state, SentenceList* sentenceList){
	if (sentenceList == NULL){
		return (ComputationResult){
			.succeeded = true
		};
	}
	return computeSentenceList(state, sentenceList->next);
}

ComputationResult computeProgram(State state, Program* program){
	return computeSentenceList(state, program->sentenceList);
}

ComputationResult executeValidator(CompilerState * compilerState) {
	State state = {
		.succeeded = true,
		.image = malloc(1920*1080*sizeof(ColorRgb)),
		.sizeX = 1920,
		.sizeY = 1080,
		.start = {255, 255, 255},
		.end = {255, 255, 255}
	};
	Program* program = compilerState->abstractSyntaxtTree;
	for(int i = 0; i<1000; i++){
		state.image[i*i] = (ColorRgb){255,255,255}; //TODO
	}
	return computeProgram(state, program);
}
