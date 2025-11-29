#include "Interpreter.h"
#include <string.h>
#include <math.h>

static Logger * _logger = NULL;

// Estructura para almacenar variables simples (si las hubiera en el futuro)
typedef struct VariableEntry {
    char * name;
    double value;
    struct VariableEntry * next;
} VariableEntry;

// Contexto que pasamos por todas las funciones de dibujo
typedef struct {
    double minX, maxX, minY, maxY; // Definido por 'view'
    int width, height;             // Definido por 'size'
    Bitmap * bmp;                  // El lienzo
    Program * program;             // Para buscar reglas
    VariableEntry * variables;     // Variables activas (scope)
} RenderContext;

void _shutdownInterpreterModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeInterpreterModule() {
    _logger = createLogger("Interpreter");
    return _shutdownInterpreterModule;
}

// --- Funciones Auxiliares ---

// Convierte coordenada del mundo (ej: -2.5) a píxel de pantalla (ej: 50)
int mapX(RenderContext * ctx, double x) {
    if (ctx->maxX == ctx->minX) return 0;
    return (int)((x - ctx->minX) / (ctx->maxX - ctx->minX) * (ctx->width - 1));
}

int mapY(RenderContext * ctx, double y) {
    if (ctx->maxY == ctx->minY) return 0;
    // Invertimos Y porque en BMP el 0 está abajo, pero matemática/visualmente suele ser arriba en computación gráfica básica
    // O lo dejamos matemático estándar (abajo-arriba). Usaremos estándar matemático.
    return (int)((y - ctx->minY) / (ctx->maxY - ctx->minY) * (ctx->height - 1));
}

// Obtiene el valor de una variable (Placeholder simple)
double getVariableValue(RenderContext * ctx, char * name) {
    // Aquí podrías buscar en ctx->variables si implementaras argumentos en las funciones
    return 0.0;
}

// --- Evaluación ---

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
        default:
            return 0.0;
    }
}

double evaluateExpression(Expression * expr, RenderContext * ctx) {
    if (!expr) return 0.0;
    
    if (expr->type == FACTOR) {
        return evaluateFactor(expr->factor, ctx);
    }

    double left = evaluateExpression(expr->leftExpression, ctx);
    double right = evaluateExpression(expr->rightExpression, ctx);

    switch (expr->type) {
        case ADDITION: return left + right;
        case SUBTRACTION: return left - right;
        case MULTIPLICATION: return left * right;
        case DIVISION: return (right != 0) ? left / right : 0;
        default: return 0.0;
    }
}

// --- Ejecución de Reglas ---

void executeRule(char * ruleName, RenderContext * ctx);

void drawPolygon(Polygon * polygon, RenderContext * ctx) {
    if (!polygon || !polygon->pointList) return;

    PointList * list = polygon->pointList;
    Point * firstPt = list->point;
    
    // Evaluar primer punto
    double x1 = evaluateExpression(firstPt->x, ctx);
    double y1 = evaluateExpression(firstPt->y, ctx);
    
    int startPx = mapX(ctx, x1);
    int startPy = mapY(ctx, y1);
    
    int prevPx = startPx;
    int prevPy = startPy;

    list = list->next;
    RGBColor color = {255, 255, 255}; // Blanco

    // Dibujar líneas entre puntos
    while (list != NULL) {
        Point * p = list->point;
        double x = evaluateExpression(p->x, ctx);
        double y = evaluateExpression(p->y, ctx);
        
        int currPx = mapX(ctx, x);
        int currPy = mapY(ctx, y);

        drawLine(ctx->bmp, prevPx, prevPy, currPx, currPy, color);
        
        prevPx = currPx;
        prevPy = currPy;
        list = list->next;
    }
    // Cerrar el polígono (conectar último con primero)
    drawLine(ctx->bmp, prevPx, prevPy, startPx, startPy, color);
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
                        // Recursión simple (sin args por ahora)
                        executeRule(rs->call->variable->name, ctx);
                    }
                    break;
                default:
                    // Otros tipos (IF, TRANSFORM, ETC) se implementarían aquí
                    break;
            }
        }
        list = list->next;
    }
}

void executeRule(char * ruleName, RenderContext * ctx) {
    // Buscar la regla en la lista global de sentencias
    SentenceList * sl = ctx->program->sentenceList;
    while (sl != NULL) {
        Sentence * s = sl->sentence;
        if (s && s->sentenceType == SENTENCE_RULE) {
            if (s->rule && s->rule->variable && strcmp(s->rule->variable->name, ruleName) == 0) {
                executeRuleSentences(s->rule->ruleSentenceList, ctx);
                return;
            }
        }
        sl = sl->next;
    }
    logError(_logger, "No se encontró la regla: %s", ruleName);
}

// --- Función Principal ---

void generateFractal(Program * program) {
    if (!program) return;

    RenderContext ctx;
    // Valores por defecto
    ctx.width = 800;
    ctx.height = 600;
    ctx.minX = -2.0; ctx.maxX = 2.0;
    ctx.minY = -2.0; ctx.maxY = 2.0;
    ctx.program = program;
    ctx.variables = NULL;
    ctx.bmp = NULL;

    char * startRuleName = NULL;

    // 1. Primera pasada: Leer configuración (Size, View, Start)
    SentenceList * s = program->sentenceList;
    while (s != NULL) {
        Sentence * sent = s->sentence;
        if (sent) {
            switch (sent->sentenceType) {
                case SENTENCE_SIZE:
                    if (sent->size && sent->size->x && sent->size->y) {
                        ctx.width = sent->size->x->value;
                        ctx.height = sent->size->y->value;
                    }
                    break;
                case SENTENCE_VIEW:
                    if (sent->view && sent->view->x && sent->view->y) {
                        // Asumimos que los rangos del view son constantes o expresiones evaluables globalmente
                        ctx.minX = evaluateExpression(sent->view->x->start, &ctx);
                        ctx.maxX = evaluateExpression(sent->view->x->end, &ctx);
                        ctx.minY = evaluateExpression(sent->view->y->start, &ctx);
                        ctx.maxY = evaluateExpression(sent->view->y->end, &ctx);
                    }
                    break;
                case SENTENCE_START:
                    if (sent->start && sent->start->variable) {
                        startRuleName = sent->start->variable->name;
                    }
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

    logDebugging(_logger, "Canvas inicializado: %dx%d. Regla inicio: %s", ctx.width, ctx.height, startRuleName);

    // 3. Ejecutar la lógica de dibujo
    if (startRuleName) {
        executeRule(startRuleName, &ctx);
    } else {
        logError(_logger, "No se encontró sentencia START.");
    }

    // 4. Guardar resultado
    saveBitmap(ctx.bmp, "output.bmp");
    
    // Limpieza
    destroyBitmap(ctx.bmp);
}