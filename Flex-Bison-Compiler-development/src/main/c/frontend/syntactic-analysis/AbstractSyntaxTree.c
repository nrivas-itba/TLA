#include "AbstractSyntaxTree.h"
#include "AbstractSyntaxTreePrinter.h"

/* MODULE INTERNAL STATE */
typedef void (*SentenceDestroyer)(Sentence*);
typedef void (*RuleSentenceDestroyer)(RuleSentence*);

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
			case LOWER_THAN_OP:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case GREATER_THAN_OP:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
		}
		free(expression);
	}
}

void destroyVariable(Variable* variable);
void destroyDoubleConstant(DoubleConstant * doubleConstant);

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
			case VARIABLE:
				destroyVariable(factor->variable);
				break;
			case DOUBLE_CONSTANT:
				destroyDoubleConstant(factor->doubleConstant);
				break;
		}
		free(factor);
	}
}

void destroyDoubleConstant(DoubleConstant * doubleConstant){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(doubleConstant != NULL){
		free(doubleConstant);
	}
}

void destroyRange(Range * range) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (range != NULL) {
		destroyExpression(range->start);
		destroyExpression(range->end);
		free(range);
	}
}

void destroyView(View * view) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if ( view != NULL) {
		destroyRange(view->x);
		destroyRange(view->y);
		free(view);
	}
}

void destroySize(Size* size){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(size != NULL){
		destroyConstant(size->x);
		destroyConstant(size->y);
		free(size);
	}
}

void destroyVariable(Variable* variable){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(variable != NULL){
		free(variable->name);
		free(variable);
	}
}


void destroyPoint(Point* point){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(point != NULL){
		destroyExpression(point->x);
		destroyExpression(point->y);
		free(point);
	}
}


void destroyPointList(PointList* pointList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(pointList != NULL){
		destroyPoint(pointList->point);
		destroyPointList(pointList->next);
		free(pointList);
	}
}



void destroyPolygon(Polygon* polygon){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(polygon != NULL){
		destroyPointList(polygon->pointList);
		free(polygon);
	}
}

void destroyRuleSentencePolygon(RuleSentence* ruleSentence){
	destroyPolygon(ruleSentence->polygon);
}

void destroyExpressionList(ExpressionList* expressionList);

void destroyCall(Call* call){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(call != NULL){
		destroyVariable(call->variable);
		destroyExpressionList(call->expressionList);
		free(call);
	}
}

void destroyRuleSentenceCall(RuleSentence* ruleSentence){
	destroyCall(ruleSentence->call);
}

void destroyIfStatement(IfStatement* ifStatement){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(ifStatement != NULL){
		destroyExpression(ifStatement->condition);
		free(ifStatement);
	}
}

void destroyRuleSentenceIfStatement(RuleSentence* ruleSentence){
	destroyIfStatement(ruleSentence->ifStatement);
}

void destroyTransformationSentence(TransformationSentence* transformationSentence){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(transformationSentence != NULL){
		switch(transformationSentence->transformationSentenceType){
			case TRANSLATE_SENTENCE:
			case SCALE_SENTENCE:
			case SHEAR_SENTENCE:
				destroyExpression(transformationSentence->x);
				destroyExpression(transformationSentence->y);
				break;
			case ROTATE_SENTENCE:
				destroyExpression(transformationSentence->angle);
				break;
			default:
				break;
		}
		free(transformationSentence);
	}
}

void destroyTransformationList(TransformList* transformList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(transformList != NULL){
		destroyTransformationSentence(transformList->transformationSentence);
		destroyTransformationList(transformList->next);
		free(transformList);
	}
}

void destroyTransformation(Transformation* transformation){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(transformation != NULL){
		destroyConstant(transformation->probability);
		destroyTransformationList(transformation->transformList);
		free(transformation);
	}
}

void destroyRuleSentenceTransformation(RuleSentence* ruleSentence){
	destroyTransformation(ruleSentence->transformation);
}

void destroyPointsStatement(PointsStatement* pointsStatement){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(pointsStatement != NULL){
		destroyConstant(pointsStatement->numPoints);
		free(pointsStatement);
	}
}

void destroyRuleSentencePointsStatement(RuleSentence* ruleSentence){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	destroyPointsStatement(ruleSentence->pointsStatement);
}

void destroyRuleSentence(RuleSentence* ruleSentence){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(ruleSentence != NULL){
		RuleSentenceDestroyer ruleSentenceDestroyers[] = {
			(RuleSentenceDestroyer)destroyRuleSentencePolygon,
			(RuleSentenceDestroyer)destroyRuleSentenceCall,
			(RuleSentenceDestroyer)destroyRuleSentenceIfStatement,
			(RuleSentenceDestroyer)destroyRuleSentenceTransformation,
			(RuleSentenceDestroyer)destroyRuleSentencePointsStatement
		};
		ruleSentenceDestroyers[ruleSentence->ruleSentenceType](ruleSentence);
		free(ruleSentence);
	}
}


void destroyRuleSentenceList(RuleSentenceList* ruleSentenceList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(ruleSentenceList != NULL){
		destroyRuleSentence(ruleSentenceList->ruleSentence);
		destroyRuleSentenceList(ruleSentenceList->next);
		free(ruleSentenceList);
	}
}

void destroyIdentifierList(IdentifierList* identifierList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(identifierList != NULL){
		destroyVariable(identifierList->variable);
		destroyIdentifierList(identifierList->identifierList);
		free(identifierList);
	}
}

void destroyRule(Rule* rule){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(rule != NULL){
		destroyVariable(rule->variable);
		destroyRuleSentenceList(rule->ruleSentenceList);
		destroyIdentifierList(rule->identifierList);
		free(rule);
	}
}

void destroyStart(Start* start){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(start != NULL){
		destroyVariable(start->variable);
		free(start);
	}
}

void destroySentenceRule(Sentence* sentence){
	destroyRule(sentence->rule);
}

void destroySentenceStart(Sentence* sentence){
	destroyStart(sentence->start);
}

void destroySentenceView(Sentence* sentence){
	destroyView(sentence->view);
}

void destroySentenceExpression(Sentence* sentence){
	destroyExpression(sentence->expression);
}

void destroySentenceSize(Sentence* sentence){
	destroySize(sentence->size);
}

void destroyColor(Color* color){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(color != NULL){
		free(color->startColor);
		free(color->endColor);
		free(color);
	}
}

void destroySentenceColor(Sentence* sentence){
	destroyColor(sentence->color);
}

void destroySentence(Sentence * sentence) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (sentence != NULL) {
		static SentenceDestroyer sentenceDestroyers[] = {
			(SentenceDestroyer)destroySentenceExpression,
			(SentenceDestroyer)destroySentenceView,
			(SentenceDestroyer)destroySentenceSize,
			(SentenceDestroyer)destroySentenceColor,
			(SentenceDestroyer)destroySentenceRule,
			(SentenceDestroyer)destroySentenceStart
		};
		sentenceDestroyers[sentence->sentenceType](sentence);
		free(sentence);
	}
}

void destroySentenceList(SentenceList * sentenceList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (sentenceList != NULL) {
		destroySentence(sentenceList->sentence);
		destroySentenceList(sentenceList->next);
		free(sentenceList);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		if(program->sentenceList != NULL){
			printProgram(program);
			destroySentenceList(program->sentenceList);
		}
		free(program);
	}
}

void destroyExpressionList(ExpressionList* expressionList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if(expressionList != NULL){
		destroyExpression(expressionList->expression);
		destroyExpressionList(expressionList->next);
		free(expressionList);
	}
}
