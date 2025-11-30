#include "Validator.h"
#include <string.h>
#include <stdio.h>

static Logger *_logger = NULL;

void _shutdownValidatorModule()
{
    if (_logger != NULL)
    {
        logDebugging(_logger, "Destroying module: Validator...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeValidatorModule()
{
    _logger = createLogger("Validator");
    return _shutdownValidatorModule;
}

static bool _variableExistsInParams(char *name, IdentifierList *params)
{
    IdentifierList *p = params;
    while (p != NULL)
    {
        if (p->variable && strcmp(p->variable->name, name) == 0)
        {
            return true;
        }
        p = p->identifierList;
    }
    return false;
}

static bool _validateExpressionVariables(Expression *expr, IdentifierList *params)
{
    if (!expr)
        return true;

    switch (expr->type)
    {
    case FACTOR:
        if (expr->factor && expr->factor->type == VARIABLE)
        {
            char *name = expr->factor->variable->name;
            if (!_variableExistsInParams(name, params))
            {
                logError(
                    _logger,
                    "Error Semántico: La variable '%s' no está definida en esta regla.",
                    name);
                return false;
            }
        }
        break;

    case ADDITION:
    case SUBTRACTION:
    case MULTIPLICATION:
    case DIVISION:
    case LOWER_THAN_OP:
    case GREATER_THAN_OP:
        return _validateExpressionVariables(expr->leftExpression, params) && _validateExpressionVariables(expr->rightExpression, params);

    case ABSOLUTE_VALUE:
        return _validateExpressionVariables(expr->leftExpression, params);

    default:
        break;
    }
    return true;
}

static bool _validateEscapeExpression(
    EscapeExpression *expr,
    IdentifierList *params,
    const char *escapeVarName)
{
    if (!expr)
        return true;

    if (expr->type == FACTOR)
    {
        EscapeFactor *f = expr->factor;
        if (!f)
            return true;

        switch (f->type)
        {
        case VARIABLE:
        {
            char *name = f->variable->name;

            if (escapeVarName != NULL && strcmp(name, escapeVarName) == 0)
            {
                return true;
            }

            if (_variableExistsInParams(name, params))
            {
                return true;
            }

            logError(
                _logger,
                "Error Semántico: La variable '%s' no está definida en esta regla (ESCAPE).",
                name);
            return false;
        }

        case RANGE:

            return _validateEscapeExpression(f->range->start, params, escapeVarName) && _validateEscapeExpression(f->range->end, params, escapeVarName);

        case CONSTANT:
        case DOUBLE_CONSTANT:
        case X_COORD_FACTOR:
        case Y_COORD_FACTOR:
        default:

            return true;
        }
    }

    return _validateEscapeExpression(expr->leftExpression, params, escapeVarName) && _validateEscapeExpression(expr->rightExpression, params, escapeVarName);
}

static bool _ruleExists(Program *program, char *ruleName)
{
    if (!program || !program->sentenceList)
        return false;

    SentenceList *current = program->sentenceList;
    while (current != NULL)
    {
        Sentence *s = current->sentence;
        if (s && s->sentenceType == SENTENCE_RULE)
        {
            if (strcmp(s->rule->variable->name, ruleName) == 0)
            {
                return true;
            }
        }
        current = current->next;
    }
    return false;
}

static bool _validateRuleSentences(
    Program *program,
    RuleSentenceList *list,
    IdentifierList *params)
{
    bool valid = true;

    while (list != NULL)
    {
        RuleSentence *rs = list->ruleSentence;
        if (!rs)
        {
            list = list->next;
            continue;
        }

        switch (rs->ruleSentenceType)
        {

        case RULE_SENTENCE_POLYGON:
        {
            PointList *pl = rs->polygon->pointList;
            while (pl != NULL)
            {
                Point *p = pl->point;
                if (p)
                {
                    if (!_validateExpressionVariables(p->x, params))
                        valid = false;
                    if (!_validateExpressionVariables(p->y, params))
                        valid = false;
                }
                pl = pl->next;
            }
            break;
        }

        case RULE_SENTENCE_CALL:
        {
            char *targetName = rs->call->variable->name;

            if (!_ruleExists(program, targetName))
            {

                logWarning(
                    _logger,
                    "Advertencia semántica: La regla llamada '%s' no está definida (se continuará igualmente).",
                    targetName);
            }

            ExpressionList *args = rs->call->expressionList;
            while (args != NULL)
            {
                if (!_validateExpressionVariables(args->expression, params))
                {
                    valid = false;
                }
                args = args->next;
            }
            break;
        }

        case RULE_SENTENCE_IF:
            if (!_validateExpressionVariables(rs->ifStatement->condition, params))
            {
                valid = false;
            }
            break;

        case RULE_SENTENCE_ESCAPE:
        {
            Escape *e = rs->escape;
            if (!e)
                break;

            const char *escapeVarName = NULL;
            if (e->variable && e->variable->name)
            {
                escapeVarName = e->variable->name;
            }

            if (!_validateEscapeExpression(e->initialValue, params, escapeVarName))
                valid = false;
            if (!_validateEscapeExpression(e->recursiveAssigment, params, escapeVarName))
                valid = false;
            if (!_validateEscapeExpression(e->untilCondition, params, escapeVarName))
                valid = false;
            break;
        }

        case RULE_SENTENCE_TRANSFORMATION:
        case RULE_SENTENCE_POINTS_STATEMENT:

            break;

        default:
            break;
        }

        list = list->next;
    }

    return valid;
}

ComputationResult executeValidator(CompilerState *compilerState)
{
    ComputationResult result = {.succeeded = true};

    if (compilerState->abstractSyntaxtTree == NULL)
    {
        logError(_logger, "El AST está vacío (NULL).");
        result.succeeded = false;
        return result;
    }

    Program *program = (Program *)compilerState->abstractSyntaxtTree;
    SentenceList *s = program->sentenceList;

    bool hasView = false;
    char *startRuleName = NULL;

    logDebugging(_logger, "Iniciando validación semántica...");

    while (s != NULL)
    {
        Sentence *sent = s->sentence;

        if (sent)
        {
            switch (sent->sentenceType)
            {

            case SENTENCE_VIEW:
                hasView = true;
                break;

            case SENTENCE_SIZE:
                if (sent->size->x->value <= 0 || sent->size->y->value <= 0)
                {
                    logError(
                        _logger,
                        "Error Semántico: Las dimensiones de SIZE deben ser mayores a 0.");
                    result.succeeded = false;
                }
                break;

            case SENTENCE_START:
                if (startRuleName != NULL)
                {
                    logWarning(
                        _logger,
                        "Advertencia: Múltiples sentencias START encontradas. Se usará la última.");
                }
                startRuleName = sent->start->variable->name;
                break;

            case SENTENCE_RULE:
            {
                Rule *r = sent->rule;
                if (!_validateRuleSentences(program, r->ruleSentenceList, r->identifierList))
                {
                    result.succeeded = false;
                }
                break;
            }

            default:
                break;
            }
        }

        s = s->next;
    }

    if (!hasView)
    {
        logError(_logger, "Error Semántico: Falta la definición obligatoria de VIEW.");
        result.succeeded = false;
    }

    if (startRuleName != NULL)
    {
        if (!_ruleExists(program, startRuleName))
        {
            logError(
                _logger,
                "Error Semántico: La regla inicial (START) '%s' no está definida.",
                startRuleName);
            result.succeeded = false;
        }
    }
    else
    {
        logWarning(
            _logger,
            "Advertencia: No se ha definido una sentencia START. No se ejecutará ninguna regla.");
    }

    if (result.succeeded)
    {
        logDebugging(_logger, "Validación semántica exitosa.");
    }
    else
    {
        logError(_logger, "Validación semántica fallida.");
    }

    return result;
}
