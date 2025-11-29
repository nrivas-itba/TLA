#include "Interpreter.h"
#include <string.h>
#include <math.h>
#include <time.h>

static Logger * _logger = NULL;

/* ====================== Contexto y Variables ====================== */

typedef struct VariableEntry {
    char * name;
    double value;
    struct VariableEntry * next;
} VariableEntry;

typedef struct {
    double minX, maxX, minY, maxY;   // View
    int width, height;               // Size
    Bitmap * bmp;                    // Lienzo
    Program * program;               // AST completo

    VariableEntry * variables;       // Scope muy simple

    // Para fractales de escape (Mandelbrot / Julia)
    double currentPixelX;
    double currentPixelY;

    // Para IFS (Barnsley, etc.)
    int numPoints;                   // Cantidad de puntos (POINTS statement)
} RenderContext;

/* ====================== Inicialización módulo ====================== */

void _shutdownInterpreterModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeInterpreterModule() {
    _logger = createLogger("Interpreter");
    srand((unsigned int)time(NULL));
    return _shutdownInterpreterModule;
}

/* ====================== Manejo de variables ====================== */

static void pushVariable(RenderContext * ctx, char * name, double value) {
    VariableEntry * v = malloc(sizeof(VariableEntry));
    v->name = strdup(name);
    v->value = value;
    v->next = ctx->variables;
    ctx->variables = v;
}

static void popVariable(RenderContext * ctx) {
    if (ctx->variables) {
        VariableEntry * tmp = ctx->variables;
        ctx->variables = tmp->next;
        free(tmp->name);
        free(tmp);
    }
}

static double getVariableValue(RenderContext * ctx, char * name) {
    VariableEntry * cur = ctx->variables;
    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            return cur->value;
        }
        cur = cur->next;
    }
    return 0.0;
}

/* ====================== Utilidades gráficas ====================== */

static int mapX(RenderContext * ctx, double x) {
    if (ctx->maxX == ctx->minX) return 0;
    return (int)((x - ctx->minX) / (ctx->maxX - ctx->minX) * (ctx->width - 1));
}

static int mapY(RenderContext * ctx, double y) {
    if (ctx->maxY == ctx->minY) return 0;
    return (int)((y - ctx->minY) / (ctx->maxY - ctx->minY) * (ctx->height - 1));
}

/* ====================== Evaluación de expresiones ====================== */

static double evaluateExpression(Expression * expr, RenderContext * ctx);

static double evaluateFactor(Factor * factor, RenderContext * ctx) {
    if (!factor) return 0.0;

    switch (factor->type) {
        case CONSTANT:
            return (double)factor->constant->value;
        case DOUBLE_CONSTANT:
            return factor->doubleConstant->value;
        case VARIABLE:
            return getVariableValue(ctx, factor->variable->name);
        case EXPRESSION:
            return evaluateExpression(factor->expression, ctx);
        case X_COORD_FACTOR:
            return ctx->currentPixelX;
        case Y_COORD_FACTOR:
            return ctx->currentPixelY;
        default:
            return 0.0;
    }
}

static double evaluateExpression(Expression * expr, RenderContext * ctx) {
    if (!expr) return 0.0;

    if (expr->type == FACTOR) {
        return evaluateFactor(expr->factor, ctx);
    }

    if (expr->type == ABSOLUTE_VALUE) {
        return fabs(evaluateExpression(expr->leftExpression, ctx));
    }

    double left  = evaluateExpression(expr->leftExpression, ctx);
    double right = evaluateExpression(expr->rightExpression, ctx);

    switch (expr->type) {
        case ADDITION:        return left + right;
        case SUBTRACTION:     return left - right;
        case MULTIPLICATION:  return left * right;
        case DIVISION:        return (right != 0.0) ? left / right : 0.0;
        case LOWER_THAN_OP:   return left < right;
        case GREATER_THAN_OP: return left > right;
        default:              return 0.0;
    }
}

/* ====================== Forward declarations ====================== */

static void executeRule(char * ruleName, ExpressionList * args, RenderContext * ctx) ;
static int executeRuleSentences(RuleSentenceList * list, RenderContext * ctx);

/* ====================== Dibujo de polígonos ====================== */

static void drawPolygon(Polygon * polygon, RenderContext * ctx) {
    if (!polygon || !polygon->pointList) return;

    PointList * list = polygon->pointList;
    Point * firstPt = list->point;

    double x1 = evaluateExpression(firstPt->x, ctx);
    double y1 = evaluateExpression(firstPt->y, ctx);

    int startPx = mapX(ctx, x1);
    int startPy = mapY(ctx, y1);
    int prevPx  = startPx;
    int prevPy  = startPy;

    list = list->next;
    RGBColor white = {255, 255, 255};

    while (list != NULL) {
        Point * p = list->point;
        double x = evaluateExpression(p->x, ctx);
        double y = evaluateExpression(p->y, ctx);

        int currPx = mapX(ctx, x);
        int currPy = mapY(ctx, y);

        drawLine(ctx->bmp, prevPx, prevPy, currPx, currPy, white);

        prevPx = currPx;
        prevPy = currPy;
        list   = list->next;
    }

    /* cerrar el polígono */
    drawLine(ctx->bmp, prevPx, prevPy, startPx, startPy, white);
}

/* ====================== Fractal de escape (Mandelbrot / Julia simple) ====================== */

static void executeEscape(Escape * escape, RenderContext * ctx) {
    int maxIter = 1000;
    if (escape && escape->maxIterations) {
        maxIter = escape->maxIterations->value;
    }

    for (int py = 0; py < ctx->height; py++) {
        for (int px = 0; px < ctx->width; px++) {

            double x0 = ctx->minX + (px * (ctx->maxX - ctx->minX) / ctx->width);
            double y0 = ctx->minY + (py * (ctx->maxY - ctx->minY) / ctx->height);

            ctx->currentPixelX = x0;
            ctx->currentPixelY = y0;

            /* Mandelbrot clásico: z_{n+1} = z_n^2 + c, z_0 = 0 */
            double x = 0.0, y = 0.0;
            int iter = 0;
            while (x * x + y * y <= 4.0 && iter < maxIter) {
                double xtemp = x * x - y * y + x0;
                y = 2.0 * x * y + y0;
                x = xtemp;
                iter++;
            }

            if (iter < maxIter) {
                RGBColor color;
                color.r = (iter * 9) % 256;
                color.g = (iter * 2) % 256;
                color.b = (iter * 5) % 256;
                setPixel(ctx->bmp, px, py, color);
            }
        }
    }
}

/* ====================== Fractal por transformaciones (IFS tipo Barnsley) ====================== */

static void executeTransformation(Transformation * t, RenderContext * ctx) {
    (void)t; /* por ahora no usamos la info del AST, IFS hardcodeado */

    int points = (ctx->numPoints > 0) ? ctx->numPoints : 100000;

    double x = 0.0, y = 0.0;
    RGBColor green = {0, 255, 0};

    for (int i = 0; i < points; i++) {
        int r = rand() % 100;
        double nextX, nextY;

        if (r < 1) {
            nextX = 0.0;
            nextY = 0.16 * y;
        } else if (r < 86) {
            nextX = 0.85 * x + 0.04 * y;
            nextY = -0.04 * x + 0.85 * y + 1.6;
        } else if (r < 93) {
            nextX = 0.20 * x - 0.26 * y;
            nextY = 0.23 * x + 0.22 * y + 1.6;
        } else {
            nextX = -0.15 * x + 0.28 * y;
            nextY = 0.26 * x + 0.24 * y + 0.44;
        }

        x = nextX;
        y = nextY;

        int px = mapX(ctx, x);
        int py = mapY(ctx, y);
        setPixel(ctx->bmp, px, py, green);
    }
}

/* ====================== Ejecución de listas de sentencias de regla ====================== */

static int executeRuleSentences(RuleSentenceList * list, RenderContext * ctx) {
    while (list != NULL) {
        RuleSentence * rs = list->ruleSentence;
        if (rs) {
            switch (rs->ruleSentenceType) {
                case RULE_SENTENCE_POLYGON:
                    drawPolygon(rs->polygon, ctx);
                    break;

                case RULE_SENTENCE_CALL:
                    if (rs->call && rs->call->variable) {
                        // ACTUALIZADO: Pasamos rs->call->expressionList
                        executeRule(rs->call->variable->name, rs->call->expressionList, ctx);
                    }
                    break;

                case RULE_SENTENCE_ESCAPE:
                    executeEscape(rs->escape, ctx);
                    break;

                case RULE_SENTENCE_TRANSFORMATION:
                    executeTransformation(rs->transformation, ctx);
                    break;

                case RULE_SENTENCE_POINTS_STATEMENT:
                    if (rs->pointsStatement && rs->pointsStatement->numPoints) {
                        ctx->numPoints = rs->pointsStatement->numPoints->value;
                    }
                    break;

                case RULE_SENTENCE_IF:
                    if (rs->ifStatement && rs->ifStatement->condition) {
                        // Evaluamos la condición
                        double cond = evaluateExpression(rs->ifStatement->condition, ctx);
                        
                        // En C, cualquier valor != 0 es verdadero.
                        // Si es verdadero, ejecutamos STOP -> retornamos 1
                        if (cond != 0.0) {
                            return 1; // Señal de parada
                        }
                    }
                    break;

                default:
                    break;
            }
        }
        list = list->next;
    }
}

/* ====================== Búsqueda y ejecución de una regla ====================== */

// Cambia la firma para aceptar argumentos (args)
static void executeRule(char * ruleName, ExpressionList * args, RenderContext * ctx) {
    // 1. Buscar la regla en el AST
    SentenceList * sl = ctx->program->sentenceList;
    Rule * rule = NULL;
    
    while (sl != NULL) {
        Sentence * s = sl->sentence;
        if (s && s->sentenceType == SENTENCE_RULE && s->rule && s->rule->variable) {
            if (strcmp(s->rule->variable->name, ruleName) == 0) {
                rule = s->rule;
                break;
            }
        }
        sl = sl->next;
    }

    if (!rule) {
        logError(_logger, "Runtime Error: No se encontró la regla '%s'", ruleName);
        return;
    }

    // 2. Evaluar los Argumentos (Valores pasados en el CALL)
    // Primero los calculamos todos y los guardamos en un array temporal.
    // Esto es vital para que 'call f(x+1)' use el 'x' viejo, no el nuevo.
    int argCount = 0;
    ExpressionList * tempArgs = args;
    while(tempArgs) { argCount++; tempArgs = tempArgs->next; }

    double * values = NULL;
    if (argCount > 0) {
        values = calloc(argCount, sizeof(double));
        tempArgs = args;
        int i = 0;
        while(tempArgs) {
            values[i++] = evaluateExpression(tempArgs->expression, ctx);
            tempArgs = tempArgs->next;
        }
    }

    // 3. Asignar Valores a Parámetros (Push al Scope)
    // Mapeamos los valores calculados a los nombres de variables de la regla.
    IdentifierList * params = rule->identifierList;
    int pushedCount = 0;
    int i = 0;
    
    while(params && params->variable) {
        if (i < argCount) {
            // Guardamos la variable en el contexto (ej: "x" = 0.5)
            pushVariable(ctx, params->variable->name, values[i]);
            pushedCount++;
        }
        params = params->identifierList;
        i++;
    }
    
    if (values) free(values);

    // 4. Ejecutar el cuerpo de la regla
    // (executeRuleSentences maneja el STOP internamente devolviendo 1 o 0)
    executeRuleSentences(rule->ruleSentenceList, ctx);

    // 5. Limpiar Scope (Pop de variables locales)
    // Quitamos las variables que agregamos para no contaminar el nivel superior.
    for(int k=0; k < pushedCount; k++) {
        popVariable(ctx);
    }
}

/* ====================== Entrada principal: generateFractal ====================== */

void generateFractal(Program * program, const char * outputFilename) {
    if (!program) return;

    RenderContext ctx;

    /* Valores por defecto razonables */
    ctx.width  = 800;
    ctx.height = 600;
    ctx.minX   = -2.0;
    ctx.maxX   =  2.0;
    ctx.minY   = -2.0;
    ctx.maxY   =  2.0;

    ctx.program       = program;
    ctx.bmp           = NULL;
    ctx.variables     = NULL;
    ctx.currentPixelX = 0.0;
    ctx.currentPixelY = 0.0;
    ctx.numPoints     = 100000;

    char * startRuleName = NULL;

    /* 1. Leer configuración global: SIZE, VIEW, START */
    SentenceList * s = program->sentenceList;
    while (s != NULL) {
        Sentence * sent = s->sentence;
        if (sent) {
            switch (sent->sentenceType) {
                case SENTENCE_SIZE:
                    if (sent->size && sent->size->x && sent->size->y) {
                        ctx.width  = sent->size->x->value;
                        ctx.height = sent->size->y->value;
                    }
                    break;

                case SENTENCE_VIEW:
                    if (sent->view && sent->view->x && sent->view->y) {
                        ctx.minX = evaluateExpression(sent->view->x->start,  &ctx);
                        ctx.maxX = evaluateExpression(sent->view->x->end,    &ctx);
                        ctx.minY = evaluateExpression(sent->view->y->start,  &ctx);
                        ctx.maxY = evaluateExpression(sent->view->y->end,    &ctx);
                    }
                    break;

                case SENTENCE_START:
                    if (sent->start && sent->start->variable) {
                        startRuleName = sent->start->variable->name;
                    }
                    break;

                default:
                    break;
            }
        }
        s = s->next;
    }

    /* 2. Inicializar bitmap */
    ctx.bmp = createBitmap(ctx.width, ctx.height);
    RGBColor black = {0, 0, 0};
    clearBitmap(ctx.bmp, black);

    logDebugging(_logger, "Canvas: %dx%d, View: [%.3f, %.3f] x [%.3f, %.3f], Start rule: %s",
                 ctx.width, ctx.height, ctx.minX, ctx.maxX, ctx.minY, ctx.maxY,
                 startRuleName ? startRuleName : "(null)");

    /* 3. Ejecutar regla de inicio */
    if (startRuleName) {
        executeRule(startRuleName, NULL, &ctx);
    } else {
        logError(_logger, "No se encontró sentencia START.");
    }

    /* 4. Guardar BMP con el nombre que te pasa el caller (tests, etc.) */
    if (outputFilename == NULL) {
        outputFilename = "output.bmp";
    }
    saveBitmap(ctx.bmp, outputFilename);

    destroyBitmap(ctx.bmp);
}
