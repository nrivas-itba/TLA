#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */
typedef void (*SentenceDestroyer)(Sentence*);

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
		}
		free(expression);
	}
}

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
		}
		free(factor);
	}
}

void destroyDoubleConstant(DoubleConstant * doubleConstant){
	if(doubleConstant != NULL){
		free(doubleConstant);
	}
}

void destroyRange(Range * range) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (range != NULL) {
		destroyDoubleConstant(range->start);
		destroyDoubleConstant(range->end);
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

void destroySentenceView(Sentence* sentence){
	destroyView(sentence->view);
}

void destroySentenceExpression(Sentence* sentence){
	destroyExpression(sentence->expression);
}

void destroySentence(Sentence * sentence) {
	static SentenceDestroyer sentenceDestroyers[] = {
		(SentenceDestroyer)destroySentenceExpression,
		(SentenceDestroyer)destroySentenceView
	};
	sentenceDestroyers[sentence->sentenceType](sentence);
	free(sentence);
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
			destroySentenceList(program->sentenceList);
		}
		free(program);
	}
}
