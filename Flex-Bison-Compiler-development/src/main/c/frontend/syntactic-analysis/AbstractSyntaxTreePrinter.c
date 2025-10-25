#include <stdio.h>
#include "AbstractSyntaxTree.h"
#include "AbstractSyntaxTreePrinter.h"


typedef void (*SentencePrinter)(Sentence*);
typedef void (*RuleSentencePrinter)(RuleSentence*);
typedef void (*FactorPrinter)(Factor*);
typedef void (*ExpressionPrinter)(Expression*);


void printRuleSentenceList(RuleSentenceList* list);
void printPolygon(Polygon* polygon);
void printPointList(PointList* list);
void printExpression(Expression* expression);
void printExpressionList(ExpressionList* expressionList);
void printIdentifierList(IdentifierList* list);
void printIfStatement(IfStatement* ifStatement);
void printRuleSentencePolygon(RuleSentence* ruleSentence);
void printRuleSentenceCall(RuleSentence* ruleSentence);
void printRuleSentenceIf(RuleSentence* ruleSentence);
void printCall(Call* call);
void printTransformation(Transformation* transformation);
void printTransformList(TransformList* transformList);
void printTransformationSentence(TransformationSentence* transformationSentence);
void printPointsStatement(PointsStatement* pointsStatement);



void printViewSentence(Sentence* sentence) {
    View* view = sentence->view;
    printf("      View: [");
    printExpression(view->x->start);
    printf(", ");
    printExpression(view->x->end);
    printf("]\n");

    printf("[");
    printExpression(view->y->start);
    printf(", ");
    printExpression(view->y->end);
    printf("]\n");
}

void printRuleSentence(Sentence* sentence) {
    Rule* rule = sentence->rule;
    printf("      Rule: variable=%s\n", rule->variable->name);
    if (rule->identifierList != NULL) {
        printf("      IdentifierList:\n");
        printIdentifierList(rule->identifierList);
    }
    printRuleSentenceList(rule->ruleSentenceList);
}

void printExpressionSentence(Sentence* sentence) {
    Expression* expression = sentence->expression;
    printf("      Expression: (Add logic to print expression details)\n");
}

void printRuleSentenceTransformation(RuleSentence* ruleSentence) {
    if (ruleSentence == NULL) {
            printf("          Transformation RuleSentence is NULL\n");
            return;
    }
    printTransformation(ruleSentence->transformation);
}

void printRuleSentencePointsStatement(RuleSentence* ruleSentence) {
    if (ruleSentence == NULL) {
            printf("          PointsStatement RuleSentence is NULL\n");
            return;
    }
    printPointsStatement(ruleSentence->pointsStatement);
}

RuleSentencePrinter ruleSentencePrinters[] = {
    printRuleSentencePolygon, // RULE_SENTENCE_POLYGON
    printRuleSentenceCall,    // RULE_SENTENCE_CALL
    printRuleSentenceIf,       // RULE_SENTENCE_IF
    printRuleSentenceTransformation,       // RULE_SENTENCE_TRANSFORMATION
    printRuleSentencePointsStatement       // RULE_SENTENCE_POINTS_STATEMENT
};

void printRuleSentenceList(RuleSentenceList* list) {
    while (list != NULL) {
        printf("        RuleSentence:\n");
        if (list->ruleSentence->ruleSentenceType >= 0 &&
            list->ruleSentence->ruleSentenceType < sizeof(ruleSentencePrinters) / sizeof(RuleSentencePrinter)) {
            ruleSentencePrinters[list->ruleSentence->ruleSentenceType](list->ruleSentence);
        } else {
            printf("          Unknown RuleSentenceType\n");
        }
        list = list->next;
    }
}

void printPolygon(Polygon* polygon) {
    printf("          Polygon:\n");
    printPointList(polygon->pointList);
}

void printPointList(PointList* list) {
    while (list != NULL) {
        printf("            Point: x=");
        printExpression(list->point->x);
        printf(", y=");
        printExpression(list->point->y);
        printf("\n");
        list = list->next;
    }
}

void printSizeSentence(Sentence* sentence) {
    Size* size = sentence->size;
    printf("      Size: x=%d, y=%d\n", size->x->value, size->y->value);
}

void printColorSentence(Sentence* sentence) {
    Color* color = sentence->color;
    printf("      Color: start=%s, end=%s\n", color->startColor, color->endColor);
}

void printStartSentence(Sentence* sentence) {
    Start* start = sentence->start;
    printf("      Start: variable=%s\n", start->variable->name);
}

void printCall(Call* call) {
    if (call == NULL) {
        printf("          Call is NULL\n");
        return;
    }
    printf("          Call: variable=%s\n", call->variable->name);
    printf("          ExpressionList:\n");
    printExpressionList(call->expressionList);
}

void printAdditionExpression(Expression* expression) {
    printf("            Addition: ");
    printExpression(expression->leftExpression);
    printf(" + ");
    printExpression(expression->rightExpression);
    printf("\n");
}

void printSubtractionExpression(Expression* expression) {
    printf("            Subtraction: ");
    printExpression(expression->leftExpression);
    printf(" - ");
    printExpression(expression->rightExpression);
    printf("\n");
}

void printMultiplicationExpression(Expression* expression) {
    printf("            Multiplication: ");
    printExpression(expression->leftExpression);
    printf(" * ");
    printExpression(expression->rightExpression);
    printf("\n");
}

void printDivisionExpression(Expression* expression) {
    printf("            Division: ");
    printExpression(expression->leftExpression);
    printf(" / ");
    printExpression(expression->rightExpression);
    printf("\n");
}

// Forward declarations for factor printers
void printConstantFactor(Factor* factor);
void printExpressionFactor(Factor* factor);
void printDoubleConstantFactor(Factor* factor);
void printVariableFactor(Factor* factor);

void printFactor(Factor* factor);
void printFactorExpression(Expression* expression) {
    if (expression == NULL || expression->factor == NULL) {
        printf("            Factor Expression is NULL\n");
        return;
    }
    printf("            Factor:\n");
    printFactor(expression->factor);
}

void printConstantFactor(Factor* factor) {
    printf("            Constant: %d\n", factor->constant->value);
}

void printExpressionFactor(Factor* factor) {
    printf("            Nested Expression:\n");
    printExpression(factor->expression);
}

void printVariableFactor(Factor* factor) {
    if (factor->variable == NULL) {
        printf("            Variable is NULL\n");
        return;
    }
    printf("            Variable: %s\n", factor->variable->name);
}

void printDoubleConstantFactor(Factor* factor) {
    if (factor->doubleConstant == NULL) {
        printf("            Double Constant is NULL\n");
        return;
    }
    printf("            Double Constant: %f\n", factor->doubleConstant->value);
}

FactorPrinter factorPrinters[] = {
    printConstantFactor,       // CONSTANT
    printExpressionFactor,     // EXPRESSION
    printDoubleConstantFactor, // DOUBLE_CONSTANT
    printVariableFactor        // VARIABLE
};

void printFactor(Factor* factor) {
    if (factor == NULL) {
        printf("            Factor is NULL\n");
        return;
    }
    printf("            Printing Factor at %p\n", (void*)factor);
    if (factor->type < 0 || factor->type >= sizeof(factorPrinters) / sizeof(FactorPrinter)) {
        printf("            Unknown Factor Type\n");
        return;
    }
    factorPrinters[factor->type](factor);
}

void printLowerThanExpression(Expression* expression) {
    printf("            Lower Than: ");
    printExpression(expression->leftExpression);
    printf(" < ");
    printExpression(expression->rightExpression);
    printf("\n");
}

void printGreaterThanExpression(Expression* expression) {
    printf("            Greater Than: ");
    printExpression(expression->leftExpression);
    printf(" > ");
    printExpression(expression->rightExpression);
    printf("\n");
}

ExpressionPrinter expressionPrinters[] = {
    printAdditionExpression,       // ADDITION
    printDivisionExpression,       // DIVISION
    printFactorExpression,         // FACTOR
    printMultiplicationExpression, // MULTIPLICATION
    printSubtractionExpression,    // SUBTRACTION
    printLowerThanExpression,      // LOWER_THAN_OP
    printGreaterThanExpression     // GREATER_THAN_OP
};

void printExpression(Expression* expression) {
    if (expression == NULL) {
        printf("            Expression is NULL\n");
        return;
    }
    if (expression->type < 0 || expression->type >= sizeof(expressionPrinters) / sizeof(ExpressionPrinter)) {
        printf("            Unknown expression type\n");
        return;
    }
    expressionPrinters[expression->type](expression);
}

void printExpressionList(ExpressionList* expressionList) {
    while (expressionList != NULL) {
        if (expressionList->expression == NULL) {
            printf("            Expression is NULL\n");
        } else {
            printf("            Printing Expression at %p\n", (void*)expressionList->expression);
            printExpression(expressionList->expression);
        }
        expressionList = expressionList->next;
    }
}

SentencePrinter sentencePrinters[] = {
    printExpressionSentence, // SENTENCE_EXPRESSION
    printViewSentence,       // SENTENCE_VIEW
    printSizeSentence,       // SENTENCE_SIZE
    printColorSentence,      // SENTENCE_COLOR
    printRuleSentence,       // SENTENCE_RULE
    printStartSentence       // SENTENCE_START
};

void printSentence(Sentence* sentence) {
    if (sentence == NULL) {
        printf("    Sentence is NULL\n");
        return;
    }
    if (sentence->sentenceType < 0 || sentence->sentenceType >= sizeof(sentencePrinters) / sizeof(SentencePrinter) || sentencePrinters[sentence->sentenceType] == NULL) {
        printf("    Unknown or unimplemented sentence type\n");
        return;
    }
    sentencePrinters[sentence->sentenceType](sentence);
}

void printSentenceList(SentenceList* sentenceList) {
    while (sentenceList != NULL) {
        printSentence(sentenceList->sentence);
        sentenceList = sentenceList->next;
    }
}

void printProgram(Program* program) {
    if (program == NULL) {
        printf("Program is NULL\n");
        return;
    }
    printf("Program:\n");
    printSentenceList(program->sentenceList);
}

void printIdentifierList(IdentifierList* list) {
    while (list != NULL) {
        if (list->variable != NULL) {
            printf("            Identifier: %s\n", list->variable->name);
        } else {
            printf("            Identifier is NULL\n");
        }
        list = list->identifierList;
    }
}

void printIfStatement(IfStatement* ifStatement) {
    if (ifStatement == NULL) {
        printf("          IfStatement is NULL\n");
        return;
    }
    printf("          IfStatement:\n");
    printf("            Condition:\n");
    printExpression(ifStatement->condition);
}

void printRuleSentencePolygon(RuleSentence* ruleSentence) {
    if (ruleSentence == NULL || ruleSentence->polygon == NULL) {
        printf("          Polygon RuleSentence is NULL\n");
        return;
    }
    printPolygon(ruleSentence->polygon);
}

void printRuleSentenceCall(RuleSentence* ruleSentence) {
    if (ruleSentence == NULL || ruleSentence->call == NULL) {
        printf("          Call RuleSentence is NULL\n");
        return;
    }
    printCall(ruleSentence->call);
}

void printRuleSentenceIf(RuleSentence* ruleSentence) {
    if (ruleSentence == NULL || ruleSentence->ifStatement == NULL) {
        printf("          IfStatement RuleSentence is NULL\n");
        return;
    }
    printIfStatement(ruleSentence->ifStatement);
}

void printTransformation(Transformation* transformation) {
    if (transformation == NULL) {
        printf("Transformation: NULL\n");
        return;
    }
    printf("Transformation:\n");
    printf("  Probability: %d\n", transformation->probability->value);
    printf("  Transform List:\n");
    printTransformList(transformation->transformList);
}

// Function to print a TransformList
void printTransformList(TransformList* transformList) {
    while (transformList != NULL) {
        printf("    Transform Sentence:\n");
        printTransformationSentence(transformList->transformationSentence);
        transformList = transformList->next;
    }
}

// Function to print a TransformationSentence
void printTransformationSentence(TransformationSentence* transformationSentence) {
    if (transformationSentence == NULL) {
        printf("      Transformation Sentence: NULL\n");
        return;
    }
    switch (transformationSentence->transformationSentenceType) {
        case TRANSLATE_SENTENCE:
            printf("      Translate: x = ");
            printExpression(transformationSentence->x);
            printf(", y = ");
            printExpression(transformationSentence->y);
            printf("\n");
            break;
        case SCALE_SENTENCE:
            printf("      Scale: x = ");
            printExpression(transformationSentence->x);
            printf(", y = ");
            printExpression(transformationSentence->y);
            printf("\n");
            break;
        case ROTATE_SENTENCE:
            printf("      Rotate: angle = ");
            printExpression(transformationSentence->angle);
            printf("\n");
            break;
        case SHEAR_SENTENCE:
            printf("      Shear: x = ");
            printExpression(transformationSentence->x);
            printf(", y = ");
            printExpression(transformationSentence->y);
            printf("\n");
            break;
        default:
            printf("      Unknown Transformation Sentence Type\n");
            break;
    }
}

void printPointsStatement(PointsStatement* pointsStatement) {
    if (pointsStatement == NULL) {
        printf("      PointsStatement: NULL\n");
        return;
    }
    printf("      PointsStatement: numPoints = %d\n", pointsStatement->numPoints->value);
}
