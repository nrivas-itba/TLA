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
Factor * DoubleConstantFactorSemanticAction(DoubleConstant * doubleConstant);
Factor * VariableFactorSemanticAction(Variable * variable);
Factor * ExpressionFactorSemanticAction(Expression * expression);

DoubleConstant * DoubleConstantSemanticAction(const double value);
Range * RangeSemanticAction(Expression * start, Expression * end);
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
Rule * RuleSemanticAction(Variable * variable, IdentifierList * identifierList, RuleSentenceList * ruleSentenceList);
RuleSentence * RuleSentenceIfStatementSemanticAction(IfStatement* ifStatement);
IfStatement* IfStatementSemanticAction(Expression* condition);
Sentence * SentenceRuleSemanticAction(Rule * rule);
IdentifierList * IdentifiersListSemanticAction(IdentifierList * identifierList, Variable * variable);
RuleSentenceList * RuleSentenceListSemanticAction(RuleSentenceList * list, RuleSentence * line);
RuleSentence * RuleSentencePolygonSemanticAction(Polygon * polygon);
RuleSentence* RuleSentenceTransformationSemanticAction(Transformation* transformation);
RuleSentence* RuleSentenceCallSemanticAction(Call* call);
RuleSentence* RuleSentencePointsStatementSemanticAction(PointsStatement* pointsStatement);
Point * PointSemanticAction(Expression* x, Expression* y);
PointList * PointListSemanticAction(PointList* list, Point* point);
Polygon* PolygonSemanticAction(PointList* pointList);
Start* StartSemanticAction(Variable* variable);
Sentence* SentenceStartSemanticAction(Start* start);
ExpressionList* ExpressionListSemanticAction(ExpressionList* list, Expression* expression);
Call* CallSemanticAction(Variable* variable, ExpressionList* expressionList);
TransformationSentence* TransformationSentenceSemanticAction(Expression* expr1, Expression* expr2, TransformationSentenceType type);
TransformList* TransformListSemanticAction(TransformList* list, TransformationSentence* transformationSentence);
Transformation* TransformationSemanticAction(Constant* probability, TransformList* transformList);
PointsStatement* PointsStatementSemanticAction(Constant* numPoints);
#endif