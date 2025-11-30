#include "Generator.h"
#include "Interpreter.h"


static Logger * _logger = NULL;


void _shutdownGeneratorModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: Generator...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}


ModuleDestructor initializeGeneratorModule() {
    _logger = createLogger("Generator");
    return _shutdownGeneratorModule;
}


void executeGenerator(CompilerState * compilerState) {
    logDebugging(_logger, "Iniciando proceso de generación (Backend)...");
    
    
    if (compilerState->abstractSyntaxtTree == NULL) {
        logError(_logger, "El AST es NULL. No hay nada que generar.");
        return;
    }

    
    ModuleDestructor interpreterDestructor = initializeInterpreterModule();


    generateFractal(compilerState->abstractSyntaxtTree, compilerState->outputImageName);

    
    interpreterDestructor();
    
    logDebugging(_logger, "Generación finalizada. Revisa el archivo de salida (ej. output.bmp).");
}