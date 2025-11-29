#include "Validator.h"
#include <string.h>
#include <stdio.h>

/* MODULE INTERNAL STATE */

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

/* --- Funciones Auxiliares de Validación --- */

/**
 * Busca si existe una regla con el nombre dado en el programa.
 */
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

/**
 * Valida el contenido de una lista de sentencias dentro de una regla.
 * Verifica:
 * 1. Que las llamadas recursivas (CALL) apunten a reglas existentes.
 */
static bool _validateRuleSentences(Program * program, RuleSentenceList * list) {
    bool valid = true;
    while (list != NULL) {
        RuleSentence * rs = list->ruleSentence;
        
        // Validación: CALL debe apuntar a una regla definida
        if (rs && rs->ruleSentenceType == RULE_SENTENCE_CALL) {
            char * targetName = rs->call->variable->name;
            if (!_ruleExists(program, targetName)) {
                logError(_logger, "Error Semántico: La regla llamada '%s' no está definida.", targetName);
                valid = false;
            }
        }
        // Puedes agregar más validaciones aquí (ej. parámetros de IF, etc.)
        
        list = list->next;
    }
    return valid;
}

/* --- Función Principal --- */

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

    // 1. Recorrer sentencias globales para validar existencia y coherencia
    while (s != NULL) {
        Sentence * sent = s->sentence;
        if (sent) {
            switch (sent->sentenceType) {
                case SENTENCE_VIEW:
                    hasView = true;
                    break;

                case SENTENCE_SIZE:
                    // Validación: Dimensiones positivas
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
                    // Validación recursiva del contenido de la regla
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

    // 2. Validaciones Globales Finales

    if (!hasView) {
        logError(_logger, "Error Semántico: Falta la definición obligatoria de VIEW.");
        result.succeeded = false;
    }

    if (startRuleName != NULL) {
        // Validar que la regla de inicio exista
        if (!_ruleExists(program, startRuleName)) {
            logError(_logger, "Error Semántico: La regla inicial (START) '%s' no está definida.", startRuleName);
            result.succeeded = false;
        }
    } else {
        logWarning(_logger, "Advertencia: No se ha definido una sentencia START. No se ejecutará ninguna regla.");
        // Dependiendo de tu diseño, esto podría ser un error o solo un warning.
    }

    if (result.succeeded) {
        logDebugging(_logger, "Validación semántica exitosa.");
    } else {
        logError(_logger, "Validación semántica fallida.");
    }

    return result;
}