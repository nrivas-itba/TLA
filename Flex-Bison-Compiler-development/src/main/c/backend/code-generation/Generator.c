#include "Generator.h"
#include "../domain-specific/Interpreter.h"

/* MODULE INTERNAL STATE */
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownGeneratorModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: Generator...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

/** Initialize module's internal state. */
ModuleDestructor initializeGeneratorModule() {
    _logger = createLogger("Generator");
    return _shutdownGeneratorModule;
}

/** PUBLIC FUNCTIONS */

void executeGenerator(CompilerState * compilerState) {
    logDebugging(_logger, "Iniciando proceso de generación (Backend)...");
    
    // Verificación de seguridad
    if (compilerState->abstractSyntaxtTree == NULL) {
        logError(_logger, "El AST es NULL. No hay nada que generar.");
        return;
    }

    // 1. Inicializar el módulo intérprete
    ModuleDestructor interpreterDestructor = initializeInterpreterModule();

    // 2. Ejecutar la lógica principal: Generar el fractal (BMP)
    // Esta función está definida en Interpreter.h y hace todo el trabajo pesado.
    generateFractal(compilerState->abstractSyntaxtTree, compilerState->outputImageName);

    // 3. Limpiar recursos del intérprete
    interpreterDestructor();
    
    logDebugging(_logger, "Generación finalizada. Revisa el archivo de salida (ej. output.bmp).");
}