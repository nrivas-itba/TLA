#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Sentence * SentenceExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->expression = expression;
	sentence->sentenceType = SENTENCE_EXPRESSION;
	_compilerState->abstractSyntaxtTree = sentence;
	return sentence;
}

DoubleConstant * DoubleConstantSemanticAction(const double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	DoubleConstant * doubleConstant = calloc(1, sizeof(DoubleConstant));
	doubleConstant->value = value;
	return doubleConstant;
}

Range * RangeSemanticAction(DoubleConstant * start, DoubleConstant * end) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Range * range = calloc(1, sizeof(Range));
	range->start = start;
	range->end = end;
	return range;
}

View * ViewSemanticAction(Range * xRange, Range * yRange) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	View * view = calloc(1, sizeof(View));
	view->x = xRange;
	view->y = yRange;
	return view;
}

Sentence * SentenceViewSemanticAction(View * view) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->view = view;
	sentence->sentenceType = SENTENCE_VIEW;
	_compilerState->abstractSyntaxtTree = sentence;
	return sentence;
}

Sentence * SentenceSizeSemanticAction(Size * size) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->size = size;
	sentence->sentenceType = SENTENCE_SIZE;
	_compilerState->abstractSyntaxtTree = sentence;
	return sentence;
}

SentenceList * SentenceListSemanticAction(SentenceList * sentenceList, Sentence * sentence) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if(sentenceList == NULL){
		sentenceList = calloc(1, sizeof(SentenceList));
		sentenceList->sentence = sentence;
		sentenceList->next = NULL;
		return sentenceList;
	}
		
	SentenceList * current = sentenceList;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = calloc(1, sizeof(SentenceList));
	current->next->sentence = sentence;
	current->next->next = NULL;

	return sentenceList;
}

Program * ProgramSemanticAction(SentenceList * sentenceList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->sentenceList = sentenceList;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}


Size* SizeSemanticAction(Constant* x, Constant* y) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Size* size = calloc(1, sizeof(Size));
	size->x = x;
	size->y = y;
	return size;
}

Color* ColorSemanticAction(char* startColor, char* endColor){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Color* color = calloc(1, sizeof(Color));
	color->startColor = startColor;
	color->endColor = endColor;
	return color;
}

Sentence * SentenceColorSemanticAction(Color* color){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->color = color;
	sentence->sentenceType = SENTENCE_COLOR;
	_compilerState->abstractSyntaxtTree = sentence;
	return sentence;
}

Variable * VariableSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Variable * variable = calloc(1, sizeof(Variable));
	variable->name = name;
	return variable;
}

Rule * RuleSemanticAction(Variable * variable, RuleSentenceList * ruleSentenceList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Rule * rule = calloc(1, sizeof(Rule));
	rule->variable = variable;
	rule->ruleSentenceList = ruleSentenceList;
	return rule;
}

Sentence * SentenceRuleSemanticAction(Rule * rule) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->rule = rule;
	sentence->sentenceType = SENTENCE_RULE;
	_compilerState->abstractSyntaxtTree = sentence;
	return sentence;
}

RuleSentenceList * RuleSentenceListSemanticAction(RuleSentenceList * list, RuleSentence * line) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if(list == NULL){
		list = calloc(1, sizeof(RuleSentenceList));
		list->ruleSentence = line;
		list->next = NULL;
		return list;
	}
		
	RuleSentenceList * current = list;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = calloc(1, sizeof(RuleSentenceList));
	current->next->ruleSentence = line;
	current->next->next = NULL;

	return list;
}

RuleSentence * RuleSentenceSemanticAction(Polygon * polygon) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RuleSentence * ruleSentence = calloc(1, sizeof(RuleSentence));
	ruleSentence->polygon = polygon;
	return ruleSentence;
}

Point * PointSemanticAction(DoubleConstant * x, DoubleConstant * y) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Point * point = calloc(1, sizeof(Point));
	point->x = x;
	point->y = y;
	return point;
}

PointList * PointListSemanticAction(PointList * list, Point * point) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if(list == NULL){
		list = calloc(1, sizeof(PointList));
		list->point = point;
		list->next = NULL;
		return list;
	}
		
	PointList * current = list;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = calloc(1, sizeof(PointList));
	current->next->point = point;
	current->next->next = NULL;

	return list;
}

Polygon* PolygonSemanticAction(PointList* pointList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Polygon* polygon = calloc(1, sizeof(Polygon));
	polygon->pointList = pointList;
	return polygon;
}

Start * StartSemanticAction(Variable * variable) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Start * start = calloc(1, sizeof(Start));
	start->variable = variable;
	return start;
}

Sentence * SentenceStartSemanticAction(Start * start) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->start = start;
	sentence->sentenceType = SENTENCE_START;
	_compilerState->abstractSyntaxtTree = sentence;
	return sentence;
}