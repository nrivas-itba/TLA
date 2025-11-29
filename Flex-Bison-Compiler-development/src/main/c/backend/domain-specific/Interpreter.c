#include "Interpreter.h"
#include <string.h>
#include <math.h>

static Logger * _logger = NULL;

// Contexto de Renderizado: Guarda el estado global del dibujo
typedef struct {
    double minX, maxX, minY, maxY; // View (Mundo)
    int width, height;             // Size (Pixeles)
    Bitmap * bmp;                  // Lienzo
    Program * program;             // AST para buscar reglas
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
    // Invertimos Y para que coincida con coordenadas cartesianas estándar (abajo-arriba)
    // bitmap (0,0) suele ser arriba-izquierda o abajo-izquierda dependiendo del formato. 
    // BMP estándar es abajo-izquierda, así que mapeo directo está bien.
    return (int)((y - ctx->minY) / (ctx->maxY - ctx->minY) * (ctx->height - 1));
}

// --- Evaluación de Expresiones ---

// (Versión simplificada: asume que todo es constante por ahora)
double evaluateExpression(Expression * expr) {
    if (!expr) return 0.0;
    
    // Si es un FACTOR
    if (expr->type == FACTOR) {
        Factor * f = expr->factor;
        if (f->type == CONSTANT) return (double)f->constant->value;
        if (f->type == DOUBLE_CONSTANT) return f->doubleConstant->value;
        if (f->type == EXPRESSION) return evaluateExpression(f->expression);
        return 0.0; // Variable no soportada en esta versión simple
    }

    // Si es operación
    double left = evaluateExpression(expr->leftExpression);
    double right = evaluateExpression(expr->rightExpression);

    switch (expr->type) {
        case ADDITION: return left + right;
        case SUBTRACTION: return left - right;
        case MULTIPLICATION: return left * right;
        case DIVISION: return (right != 0) ? left / right : 0;
        default: return 0.0;
    }
}

// --- Lógica de Dibujo ---

// Prototipo para recursión
void executeRule(char * ruleName, RenderContext * ctx);

void drawPolygon(Polygon * polygon, RenderContext * ctx) {
    if (!polygon || !polygon->pointList) return;

    PointList * list = polygon->pointList;
    Point * firstPt = list->point;
    
    // 1. Calcular primer punto
    double x1 = evaluateExpression(firstPt->x);
    double y1 = evaluateExpression(firstPt->y);
    
    int startPx = mapX(ctx, x1);
    int startPy = mapY(ctx, y1);
    
    int prevPx = startPx;
    int prevPy = startPy;

    list = list->next;
    RGBColor color = {255, 255, 255}; // Blanco

    // 2. Conectar puntos siguientes
    while (list != NULL) {
        Point * p = list->point;
        double x = evaluateExpression(p->x);
        double y = evaluateExpression(p->y);
        
        int currPx = mapX(ctx, x);
        int currPy = mapY(ctx, y);

        drawLine(ctx->bmp, prevPx, prevPy, currPx, currPy, color);
        
        prevPx = currPx;
        prevPy = currPy;
        list = list->next;
    }
    // 3. Cerrar polígono
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
                        // Recursión simple sin argumentos
                        executeRule(rs->call->variable->name, ctx);
                    }
                    break;
                default: break;
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

// --- Entrada Principal ---

void generateFractal(Program * program, const char * outputFilename) {
    if (!program) return;

    RenderContext ctx;
    // Valores por defecto
    ctx.width = 800;
    ctx.height = 600;
    ctx.minX = -2.0; ctx.maxX = 2.0;
    ctx.minY = -2.0; ctx.maxY = 2.0;
    ctx.program = program;
    ctx.bmp = NULL;

    char * startRuleName = NULL;

    // 1. Leer configuración global (Size, View, Start)
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
                        ctx.minX = evaluateExpression(sent->view->x->start);
                        ctx.maxX = evaluateExpression(sent->view->x->end);
                        ctx.minY = evaluateExpression(sent->view->y->start);
                        ctx.maxY = evaluateExpression(sent->view->y->end);
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

    logDebugging(_logger, "Canvas: %dx%d. Rule: %s", ctx.width, ctx.height, startRuleName);

    // 3. Ejecutar
    if (startRuleName) {
        executeRule(startRuleName, &ctx);
    } else {
        logError(_logger, "No se encontró sentencia START.");
    }

// 4. Guardar resultado con el nombre dinámico
    if (outputFilename == NULL) {
        outputFilename = "output.bmp";
    }
    saveBitmap(ctx.bmp, outputFilename); // <--- AQUI ESTA EL CAMBIO CLAVE
    
    // Limpieza
    destroyBitmap(ctx.bmp);
}