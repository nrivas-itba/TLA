#include "Validator.h"
#include <string.h>
#include <stdio.h>

static Logger * _logger = NULL;

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

static bool _ruleExists(Program * program, char * ruleName) {
    if (!program || !program->sentenceList) return false;

    SentenceList * current = program->sentenceList;
    while (current != NULL) {
        Sentence * s = current->sentence;
        if (s && s->sentenceType == SENTENCE_RULE) {
            if (strcmp(s->rule->variable->name, ruleName) == 0) {
                return true;
            }
        }
        current = current->next;
    }
    return false;
}

static bool _validateRuleSentences(Program * program, RuleSentenceList * list) {
    bool valid = true;
    while (list != NULL) {
        RuleSentence * rs = list->ruleSentence;

        if (rs && rs->ruleSentenceType == RULE_SENTENCE_CALL) {
            char * targetName = rs->call->variable->name;
            if (!_ruleExists(program, targetName)) {
                logWarning(
                    _logger,
                    "Advertencia semántica: La regla llamada '%s' no está definida (se continuará igualmente).",
                    targetName
                );
            }
        }

        list = list->next;
    }
    return valid;
}

ComputationResult executeValidator(CompilerState * compilerState) {
    ComputationResult result = { .succeeded = true };

    if (compilerState->abstractSyntaxtTree == NULL) {
        logError(_logger, "El AST está vacío (NULL).");
        result.succeeded = false;
        return result;
    }

    Program * program = (Program *)compilerState->abstractSyntaxtTree;
    SentenceList * s = program->sentenceList;

    bool hasView = false;
    char * startRuleName = NULL;

    logDebugging(_logger, "Iniciando validación semántica...");

    while (s != NULL) {
        Sentence * sent = s->sentence;
        if (sent) {
            switch (sent->sentenceType) {
                case SENTENCE_VIEW:
                    hasView = true;
                    break;

                case SENTENCE_SIZE:
                    if (sent->size->x->value <= 0 || sent->size->y->value <= 0) {
                        logError(_logger, "Error Semántico: Las dimensiones de SIZE deben ser mayores a 0.");
                        result.succeeded = false;
                    }
                    break;

                case SENTENCE_START:
                    if (startRuleName != NULL) {
                        logWarning(_logger, "Advertencia: Múltiples sentencias START encontradas. Se usará la última.");
                    }
                    startRuleName = sent->start->variable->name;
                    break;

                case SENTENCE_RULE:
                    if (!_validateRuleSentences(program, sent->rule->ruleSentenceList)) {
                        result.succeeded = false;
                    }
                    break;

                default:
                    break;
            }
        }
        s = s->next;
    }

    if (!hasView) {
        logError(_logger, "Error Semántico: Falta la definición obligatoria de VIEW.");
        result.succeeded = false;
    }

    if (startRuleName != NULL) {
        if (!_ruleExists(program, startRuleName)) {
            logError(_logger, "Error Semántico: La regla inicial (START) '%s' no está definida.", startRuleName);
            result.succeeded = false;
        }
    } else {
        logWarning(_logger, "Advertencia: No se ha definido una sentencia START. No se ejecutará ninguna regla.");
    }

    if (result.succeeded) {
        logDebugging(_logger, "Validación semántica exitosa.");
    } else {
        logError(_logger, "Validación semántica fallida.");
    }

    return result;
}
