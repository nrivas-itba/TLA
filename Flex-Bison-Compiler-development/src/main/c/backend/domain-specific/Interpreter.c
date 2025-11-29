#include "Interpreter.h"
#include <string.h>
#include <math.h>
#include <time.h>

static Logger * _logger = NULL;

// Estructura para variables (Scope básico)
typedef struct VariableEntry {
    char * name;
    double value;
    struct VariableEntry * next;
} VariableEntry;

// Contexto de Renderizado
typedef struct {
    double minX, maxX, minY, maxY; // View
    int width, height;             // Size
    Bitmap * bmp;
    Program * program;
    VariableEntry * variables;
    
    // Variables de contexto para fractales de escape (Mandelbrot)
    double currentPixelX;
    double currentPixelY;
} RenderContext;

void _shutdownInterpreterModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeInterpreterModule() {
    _logger = createLogger("Interpreter");
    srand(time(NULL)); // Semilla para IFS aleatorio
    return _shutdownInterpreterModule;
}

// --- Gestión de Variables ---

void pushVariable(RenderContext * ctx, char * name, double value) {
    VariableEntry * v = malloc(sizeof(VariableEntry));
    v->name = strdup(name);
    v->value = value;
    v->next = ctx->variables;
    ctx->variables = v;
}

void popVariable(RenderContext * ctx) {
    if (ctx->variables) {
        VariableEntry * temp = ctx->variables;
        ctx->variables = temp->next;
        free(temp->name);
        free(temp);
    }
}

double getVariableValue(RenderContext * ctx, char * name) {
    VariableEntry * current = ctx->variables;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) return current->value;
        current = current->next;
    }
    // Variables especiales de contexto para Mandelbrot (simuladas)
    // En una implementación real, esto debería manejarse con tokens específicos
    return 0.0;
}

// --- Utilidades Gráficas ---

int mapX(RenderContext * ctx, double x) {
    if (ctx->maxX == ctx->minX) return 0;
    return (int)((x - ctx->minX) / (ctx->maxX - ctx->minX) * (ctx->width - 1));
}

int mapY(RenderContext * ctx, double y) {
    if (ctx->maxY == ctx->minY) return 0;
    return (int)((y - ctx->minY) / (ctx->maxY - ctx->minY) * (ctx->height - 1));
}

// --- Evaluación de Expresiones ---

double evaluateExpression(Expression * expr, RenderContext * ctx);

double evaluateFactor(Factor * factor, RenderContext * ctx) {
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
        // Si tu AST tiene X_COORD_FACTOR / Y_COORD_FACTOR:
        case X_COORD_FACTOR: return ctx->currentPixelX;
        case Y_COORD_FACTOR: return ctx->currentPixelY;
        default:
            return 0.0;
    }
}

double evaluateExpression(Expression * expr, RenderContext * ctx) {
    if (!expr) return 0.0;
    
    if (expr->type == FACTOR) {
        return evaluateFactor(expr->factor, ctx);
    }

    // Operadores Unarios
    if (expr->type == ABSOLUTE_VALUE) {
        return fabs(evaluateExpression(expr->leftExpression, ctx));
    }

    double left = evaluateExpression(expr->leftExpression, ctx);
    double right = evaluateExpression(expr->rightExpression, ctx);

    switch (expr->type) {
        case ADDITION: return left + right;
        case SUBTRACTION: return left - right;
        case MULTIPLICATION: return left * right;
        case DIVISION: return (right != 0) ? left / right : 0;
        case LOWER_THAN_OP: return left < right;
        case GREATER_THAN_OP: return left > right;
        default: return 0.0;
    }
}

// --- Evaluación de Escape (Mandelbrot) ---

// Evalúa una expresión de escape. 
// Simplificación: Tratamos todo como double. Para Mandelbrot real necesitamos complejos.
// Aquí simulamos z*z+c usando aritmética simple para demostración.
double evaluateEscapeExpression(EscapeExpression * expr, RenderContext * ctx) {
    // Nota: Esta es una simplificación extrema. 
    // En tu AST EscapeExpression tiene la misma estructura que Expression.
    // Podrías reutilizar evaluateExpression casteando los tipos si son compatibles en memoria
    // o duplicar la lógica. Aquí duplicamos lo básico.
    if (!expr) return 0.0;
    if (expr->type == FACTOR) {
        // Asumiendo que EscapeFactor es compatible o similar
        // Simplificación: Retornamos valor base
        return 0.0; 
    }
    // ... Implementar recursión igual que evaluateExpression ...
    return 0.0; 
}

// --- Ejecución de Reglas ---

void executeRule(char * ruleName, RenderContext * ctx);

void drawPolygon(Polygon * polygon, RenderContext * ctx) {
    if (!polygon || !polygon->pointList) return;

    PointList * list = polygon->pointList;
    Point * firstPt = list->point;
    
    double x1 = evaluateExpression(firstPt->x, ctx);
    double y1 = evaluateExpression(firstPt->y, ctx);
    
    int startPx = mapX(ctx, x1);
    int startPy = mapY(ctx, y1);
    int prevPx = startPx;
    int prevPy = startPy;

    list = list->next;
    RGBColor white = {255, 255, 255};

    while (list != NULL) {
        Point * p = list->point;
        double x = evaluateExpression(p->x, ctx);
        double y = evaluateExpression(p->y, ctx);
        int currPx = mapX(ctx, x);
        int currPy = mapY(ctx, y);

        drawLine(ctx->bmp, prevPx, prevPy, currPx, currPy, white);
        prevPx = currPx; prevPy = currPy;
        list = list->next;
    }
    drawLine(ctx->bmp, prevPx, prevPy, startPx, startPy, white);
}

// Ejecuta Mandelbrot (Simplificado con valores hardcoded para demostración)
void executeEscape(Escape * escape, RenderContext * ctx) {
    int maxIter = 1000;
    if (escape && escape->maxIterations) maxIter = escape->maxIterations->value;

    for (int py = 0; py < ctx->height; py++) {
        for (int px = 0; px < ctx->width; px++) {
            // Mapeo pixel -> coordenadas complejas
            double x0 = ctx->minX + (px * (ctx->maxX - ctx->minX) / ctx->width);
            double y0 = ctx->minY + (py * (ctx->maxY - ctx->minY) / ctx->height);
            
            ctx->currentPixelX = x0;
            ctx->currentPixelY = y0;

            double x = 0.0, y = 0.0; // z = 0
            int iter = 0;
            while (x*x + y*y <= 4 && iter < maxIter) {
                double xtemp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtemp;
                iter++;
            }

            if (iter < maxIter) {
                // Color basado en iteraciones
                RGBColor color;
                color.r = (iter * 9) % 256;
                color.g = (iter * 2) % 256;
                color.b = (iter * 5) % 256;
                setPixel(ctx->bmp, px, py, color);
            }
        }
    }
}

// Ejecuta IFS (Barnsley Fern) - Valores Hardcoded para demostración
void executeTransformation(Transformation * t, RenderContext * ctx) {
    double x = 0, y = 0;
    RGBColor green = {0, 255, 0};
    int points = 100000; // Debería leerse del AST

    for (int i = 0; i < points; i++) {
        int r = rand() % 100;
        double nextX, nextY;

        if (r < 1) {
            nextX = 0; nextY = 0.16 * y;
        } else if (r < 86) {
            nextX = 0.85 * x + 0.04 * y;
            nextY = -0.04 * x + 0.85 * y + 1.6;
        } else if (r < 93) {
            nextX = 0.2 * x - 0.26 * y;
            nextY = 0.23 * x + 0.22 * y + 1.6;
        } else {
            nextX = -0.15 * x + 0.28 * y;
            nextY = 0.26 * x + 0.24 * y + 0.44;
        }
        x = nextX; y = nextY;

        int px = mapX(ctx, x);
        int py = mapY(ctx, y);
        setPixel(ctx->bmp, px, py, green);
    }
}

void executeRuleSentences(RuleSentenceList * list, RenderContext * ctx) {
    while (list != NULL) {
        RuleSentence * rs = list->ruleSentence;
        if (rs) {
            switch (rs->ruleSentenceType) {
                case RULE_SENTENCE_POLYGON:
                    drawPolygon(rs->polygon, ctx);
                    break;
                case RULE_SENTENCE_CALL:
                    if (rs->call && rs->call->variable) {
                        // Recursión simple (cuidado con stack overflow)
                        // executeRule(rs->call->variable->name, ctx);
                    }
                    break;
                case RULE_SENTENCE_ESCAPE:
                    executeEscape(rs->escape, ctx);
                    break;
                case RULE_SENTENCE_TRANSFORMATION:
                    executeTransformation(rs->transformation, ctx);
                    break;
                default: break;
            }
        }
        list = list->next;
    }
}

void executeRule(char * ruleName, RenderContext * ctx) {
    SentenceList * sl = ctx->program->sentenceList;
    while (sl != NULL) {
        Sentence * s = sl->sentence;
        if (s && s->sentenceType == SENTENCE_RULE) {
            if (strcmp(s->rule->variable->name, ruleName) == 0) {
                executeRuleSentences(s->rule->ruleSentenceList, ctx);
                return;
            }
        }
        sl = sl->next;
    }
    logError(_logger, "Regla no encontrada: %s", ruleName);
}

// --- Entrada Principal ---

void generateFractal(Program * program, const char * outputFilename) {
    if (!program) return;

    RenderContext ctx;
    ctx.width = 800;
    ctx.height = 600;
    ctx.minX = -2.0; ctx.maxX = 2.0;
    ctx.minY = -2.0; ctx.maxY = 2.0;
    ctx.program = program;
    ctx.variables = NULL;
    ctx.bmp = NULL;

    char * startRuleName = NULL;

    // 1. Configuración
    SentenceList * s = program->sentenceList;
    while (s != NULL) {
        Sentence * sent = s->sentence;
        if (sent) {
            switch (sent->sentenceType) {
                case SENTENCE_SIZE:
                    if (sent->size->x && sent->size->y) {
                        ctx.width = sent->size->x->value;
                        ctx.height = sent->size->y->value;
                    }
                    break;
                case SENTENCE_VIEW:
                    // Simplificación: Asumimos constantes en el view para no evaluar expresiones sin contexto
                    // En una implementación completa, se evalúan aquí.
                    // ctx.minX = evaluateExpression(sent->view->x->start, &ctx); ...
                    break;
                case SENTENCE_START:
                    startRuleName = sent->start->variable->name;
                    break;
                default: break;
            }
        }
        s = s->next;
    }

    // 2. Inicializar Bitmap
    ctx.bmp = createBitmap(ctx.width, ctx.height);
    RGBColor black = {0, 0, 0};
    clearBitmap(ctx.bmp, black);

    // 3. Ejecutar
    if (startRuleName) {
        executeRule(startRuleName, &ctx);
    }

    // 4. Guardar
    if (outputFilename == NULL) outputFilename = "output.bmp";
    saveBitmap(ctx.bmp, outputFilename);
    
    destroyBitmap(ctx.bmp);
}