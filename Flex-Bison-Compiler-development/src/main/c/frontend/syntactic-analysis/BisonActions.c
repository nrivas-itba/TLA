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

Factor * DoubleConstantFactorSemanticAction(DoubleConstant * doubleConstant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->doubleConstant = doubleConstant;
	factor->type = DOUBLE_CONSTANT;
	return factor;
}

Factor * VariableFactorSemanticAction(Variable * variable) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->variable = variable;
	factor->type = VARIABLE;
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
	return sentence;
}

DoubleConstant * DoubleConstantSemanticAction(const double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	DoubleConstant * doubleConstant = calloc(1, sizeof(DoubleConstant));
	doubleConstant->value = value;
	return doubleConstant;
}

Range * RangeSemanticAction(Expression * start, Expression * end) {
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
	return sentence;
}

Sentence * SentenceSizeSemanticAction(Size * size) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->size = size;
	sentence->sentenceType = SENTENCE_SIZE;
	return sentence;
}

SentenceList * SentenceListSemanticAction(SentenceList * sentenceList, Sentence * sentence) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (sentence == NULL) {
		return sentenceList;
	}
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
	return sentence;
}

Variable * VariableSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Variable * variable = calloc(1, sizeof(Variable));
	variable->name = name;
	return variable;
}

Rule * RuleSemanticAction(Variable * variable, IdentifierList * identifierList, RuleSentenceList * ruleSentenceList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Rule * rule = calloc(1, sizeof(Rule));
	rule->variable = variable;
	rule->identifierList = identifierList;
	rule->ruleSentenceList = ruleSentenceList;
	return rule;
}

IdentifierList * IdentifiersListSemanticAction(IdentifierList * identifierList, Variable * variable) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (variable == NULL) {
		return identifierList;
	}

	if(identifierList == NULL){
		identifierList = calloc(1, sizeof(IdentifierList));
		identifierList->variable = variable;
		identifierList->identifierList = NULL;
		return identifierList;
	}
		
	IdentifierList * current = identifierList;
	while(current->identifierList != NULL){
		current = current->identifierList;
	}
	current->identifierList = calloc(1, sizeof(IdentifierList));
	current->identifierList->variable = variable;
	current->identifierList->identifierList = NULL;

	return identifierList;
}

Sentence * SentenceRuleSemanticAction(Rule * rule) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Sentence * sentence = calloc(1, sizeof(Sentence));
	sentence->rule = rule;
	sentence->sentenceType = SENTENCE_RULE;
	return sentence;
}

RuleSentence * RuleSentenceIfStatementSemanticAction(IfStatement* ifStatement){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RuleSentence* ruleSentence = calloc(1, sizeof(RuleSentence));
	ruleSentence->ifStatement = ifStatement;
	ruleSentence->ruleSentenceType = RULE_SENTENCE_IF;
	return ruleSentence;
}

IfStatement* IfStatementSemanticAction(Expression* condition){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	IfStatement* ifStatement = calloc(1, sizeof(IfStatement));
	ifStatement->condition = condition;
	return ifStatement;
}

RuleSentenceList * RuleSentenceListSemanticAction(RuleSentenceList * list, RuleSentence * line) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (line == NULL) {
		return list;
	}

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

RuleSentence * RuleSentencePolygonSemanticAction(Polygon * polygon) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RuleSentence * ruleSentence = calloc(1, sizeof(RuleSentence));
	ruleSentence->polygon = polygon;
	ruleSentence->ruleSentenceType = RULE_SENTENCE_POLYGON;
	return ruleSentence;
}

RuleSentence* RuleSentenceTransformationSemanticAction(Transformation* transformation){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RuleSentence* ruleSentence = calloc(1, sizeof(RuleSentence));
	ruleSentence->transformation = transformation;
	ruleSentence->ruleSentenceType = RULE_SENTENCE_TRANSFORMATION;
	return ruleSentence;
}

RuleSentence* RuleSentencePointsStatementSemanticAction(PointsStatement* pointsStatement){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RuleSentence* ruleSentence = calloc(1, sizeof(RuleSentence));
	ruleSentence->pointsStatement = pointsStatement;
	ruleSentence->ruleSentenceType = RULE_SENTENCE_POINTS_STATEMENT;
	return ruleSentence;
}

Point * PointSemanticAction(Expression * x, Expression * y) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Point * point = calloc(1, sizeof(Point));
	point->x = x;
	point->y = y;
	return point;
}

PointList * PointListSemanticAction(PointList * list, Point * point) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (point == NULL) {
		return list;
	}

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
	return sentence;
}

RuleSentence* RuleSentenceCallSemanticAction(Call* call){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RuleSentence* ruleSentence = calloc(1, sizeof(RuleSentence));
	ruleSentence->call = call;
	ruleSentence->ruleSentenceType = RULE_SENTENCE_CALL;
	return ruleSentence;
}

Call* CallSemanticAction(Variable* variable, ExpressionList* expressionList){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Call* call = calloc(1, sizeof(Call));
	call->variable = variable;
	call->expressionList = expressionList;
	return call;
}

ExpressionList* ExpressionListSemanticAction(ExpressionList* list, Expression* expression){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (expression == NULL) {
		return list;
	}

	if(list == NULL){
		list = calloc(1, sizeof(ExpressionList));
		list->expression = expression;
		list->next = NULL;
		return list;
	}
		
	ExpressionList * current = list;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = calloc(1, sizeof(ExpressionList));
	current->next->expression = expression;
	current->next->next = NULL;

	return list;
}

TransformationSentence* TransformationSentenceSemanticAction(Expression* expr1, Expression* expr2, TransformationSentenceType type){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransformationSentence* transformationSentence = calloc(1, sizeof(TransformationSentence));
	transformationSentence->transformationSentenceType = type;
	switch(type){
		case TRANSLATE_SENTENCE:
		case SCALE_SENTENCE:
		case SHEAR_SENTENCE:
			transformationSentence->x = expr1;
			transformationSentence->y = expr2;
			break;
		case ROTATE_SENTENCE:
			transformationSentence->angle = expr1;
			break;
		default:
			break;
	}
	return transformationSentence;
}

TransformList* TransformListSemanticAction(TransformList* list, TransformationSentence* transformationSentence){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (transformationSentence == NULL) {
		return list;
	}

	if(list == NULL){
		list = calloc(1, sizeof(TransformList));
		list->transformationSentence = transformationSentence;
		list->next = NULL;
		return list;
	}
		
	TransformList* current = list;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = calloc(1, sizeof(PointList));
	current->next->transformationSentence = transformationSentence;
	current->next->next = NULL;

	return list;
}

Transformation* TransformationSemanticAction(Constant* probability, TransformList* transformationList){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Transformation* transformation = calloc(1, sizeof(Transformation));
	transformation->probability = probability;
	transformation->transformList = transformationList;
	return transformation;
}

PointsStatement* PointsStatementSemanticAction(Constant* numPoints){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	PointsStatement* pointsStatement = calloc(1, sizeof(PointsStatement));
	pointsStatement->numPoints = numPoints;
	return pointsStatement;
}

Factor * RangeFactorSemanticAction(Range * range) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->range = range;
	factor->type = COMPLEX;
	return factor;
}
