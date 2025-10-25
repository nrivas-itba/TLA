#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule();

/**
 * Bison semantic actions.
 */

Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * VariableFactorSemanticAction(Variable * variable);
Factor * ExpressionFactorSemanticAction(Expression * expression);

DoubleConstant * DoubleConstantSemanticAction(const double value);
Range * RangeSemanticAction(DoubleConstant * start, DoubleConstant * end);
View * ViewSemanticAction(Range * xRange, Range * yRange);
View * ViewSemanticAction(Range * xRange, Range * yRange);
Sentence * SentenceViewSemanticAction(View * view);
Program * ProgramSemanticAction(SentenceList * sentenceList);
Sentence * SentenceExpressionSemanticAction(Expression * expression);
SentenceList * SentenceListSemanticAction(SentenceList * sentenceList,  Sentence * sentence);
Size* SizeSemanticAction(Constant* x, Constant* y);
Sentence * SentenceSizeSemanticAction(Size * size);
Color* ColorSemanticAction(char* startColor, char* endColor);
Sentence * SentenceColorSemanticAction(Color* color);
Variable * VariableSemanticAction(char * name);
Rule * RuleSemanticAction(Variable * variable, RuleSentenceList * ruleSentenceList);
Sentence * SentenceRuleSemanticAction(Rule * rule);
RuleSentenceList * RuleSentenceListSemanticAction(RuleSentenceList * list, RuleSentence * line);
RuleSentence * RuleSentenceSemanticAction(Polygon * polygon);
Point * PointSemanticAction(DoubleConstant* x, DoubleConstant* y);
PointList * PointListSemanticAction(PointList* list, Point* point);
Polygon* PolygonSemanticAction(PointList* pointList);
Start* StartSemanticAction(Variable* variable);
Sentence* SentenceStartSemanticAction(Start* start);

#endif