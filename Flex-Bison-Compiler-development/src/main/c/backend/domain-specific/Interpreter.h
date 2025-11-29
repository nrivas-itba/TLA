#ifndef INTERPRETER_HEADER
#define INTERPRETER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include "../code-generation/Bitmap.h"

/** Inicializa el módulo */
ModuleDestructor initializeInterpreterModule();

/**
 * Ejecuta el programa Fractalang:
 * 1. Configura el lienzo (Size).
 * 2. Configura la vista (View).
 * 3. Ejecuta la regla inicial (Start).
 */
void generateFractal(Program * program);

#endif