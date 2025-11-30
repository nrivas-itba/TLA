#include "Interpreter.h"
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

static Logger * _logger = NULL;

/* ====================== Contexto y Variables ====================== */

typedef struct VariableEntry {
    char * name;
    double value;
    struct VariableEntry * next;
} VariableEntry;

typedef struct {
    double minX, maxX, minY, maxY;   
    int width, height;               
    Bitmap * bmp;                    
    Program * program;               

    VariableEntry * variables;       

    // Variables de contexto
    double currentPixelX;
    double currentPixelY;
    int numPoints;      

    // COLORES (NUEVO)
    RGBColor colorStart; // Fondo / Inicio gradiente
    RGBColor colorEnd;   // Frente / Fin gradiente
} RenderContext;

/* ====================== Inicialización ====================== */

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

/* ====================== Utilidades de Color (NUEVO) ====================== */

// Convierte string hex "#RRGGBB" a RGBColor
static RGBColor parseHexColor(char * hexStr) {
    RGBColor color = {0, 0, 0};
    if (hexStr == NULL) return color;

    // Saltar el '#' si existe
    char * start = (hexStr[0] == '#') ? hexStr + 1 : hexStr;
    
    unsigned int r, g, b;
    // Usamos sscanf para leer hexadecimales
    if (sscanf(start, "%02x%02x%02x", &r, &g, &b) == 3) {
        color.r = (uint8_t)r;
        color.g = (uint8_t)g;
        color.b = (uint8_t)b;
    }
    return color;
}

// Interpola linealmente entre dos colores (t va de 0.0 a 1.0)
static RGBColor interpolateColor(RGBColor c1, RGBColor c2, double t) {
    RGBColor res;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    res.r = (uint8_t)(c1.r + (c2.r - c1.r) * t);
    res.g = (uint8_t)(c1.g + (c2.g - c1.g) * t);
    res.b = (uint8_t)(c1.b + (c2.b - c1.b) * t);
    return res;
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

/* ====================== Evaluación Estándar ====================== */

static double evaluateExpression(Expression * expr, RenderContext * ctx);

static double evaluateFactor(Factor * factor, RenderContext * ctx) {
    if (!factor) return 0.0;
    switch (factor->type) {
        case CONSTANT: return (double)factor->constant->value;
        case DOUBLE_CONSTANT: return factor->doubleConstant->value;
        case VARIABLE: return getVariableValue(ctx, factor->variable->name);
        case EXPRESSION: return evaluateExpression(factor->expression, ctx);
        case X_COORD_FACTOR: return ctx->currentPixelX;
        case Y_COORD_FACTOR: return ctx->currentPixelY;
        default: return 0.0;
    }
}

static double evaluateExpression(Expression * expr, RenderContext * ctx) {
    if (!expr) return 0.0;
    if (expr->type == FACTOR) return evaluateFactor(expr->factor, ctx);
    if (expr->type == ABSOLUTE_VALUE) return fabs(evaluateExpression(expr->leftExpression, ctx));

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

/* ====================== Evaluación Escape ====================== */

static double evaluateEscapeExpression(EscapeExpression * expr, RenderContext * ctx);

static double evaluateEscapeFactor(EscapeFactor * factor, RenderContext * ctx) {
    if (!factor) return 0.0;
    switch (factor->type) {
        case CONSTANT: return (double)factor->constant->value;
        case DOUBLE_CONSTANT: return factor->doubleConstant->value;
        case VARIABLE: return getVariableValue(ctx, factor->variable->name);
        case X_COORD_FACTOR: return ctx->currentPixelX;
        case Y_COORD_FACTOR: return ctx->currentPixelY;
        case EXPRESSION: return evaluateEscapeExpression(factor->expression, ctx);
        case RANGE:
             if (factor->range) return evaluateEscapeExpression(factor->range->start, ctx);
             return 0.0;
        default: return 0.0;
    }
}

static double evaluateEscapeExpression(EscapeExpression * expr, RenderContext * ctx) {
    if (!expr) return 0.0;
    if (expr->type == FACTOR) return evaluateEscapeFactor(expr->factor, ctx);
    if (expr->type == ABSOLUTE_VALUE) return fabs(evaluateEscapeExpression(expr->leftExpression, ctx));

    double left  = evaluateEscapeExpression(expr->leftExpression, ctx);
    double right = evaluateEscapeExpression(expr->rightExpression, ctx);

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

/* ====================== Forward Declarations ====================== */

static void executeRule(char * ruleName, ExpressionList * args, RenderContext * ctx);

/* ====================== Dibujo de Polígonos ====================== */

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
    // USAR COLOR FINAL (Foreground)
    RGBColor color = ctx->colorEnd; 

    while (list != NULL) {
        Point * p = list->point;
        double x = evaluateExpression(p->x, ctx);
        double y = evaluateExpression(p->y, ctx);
        int currPx = mapX(ctx, x);
        int currPy = mapY(ctx, y);
        drawLine(ctx->bmp, prevPx, prevPy, currPx, currPy, color);
        prevPx = currPx; prevPy = currPy;
        list = list->next;
    }
    drawLine(ctx->bmp, prevPx, prevPy, startPx, startPy, color);
}

/* ====================== Fractal de Escape ====================== */

static void executeEscape(Escape * escape, RenderContext * ctx) {
    int maxIter = 1000;
    if (escape && escape->maxIterations) maxIter = escape->maxIterations->value;

    // Detección Julia
    ctx->currentPixelX = 123.456; 
    double initVal = evaluateEscapeExpression(escape->initialValue, ctx);
    bool isJulia = (fabs(initVal - 123.456) < 0.001); 

    double cRe_Julia = -0.8;
    double cIm_Julia = 0.156;

    for (int py = 0; py < ctx->height; py++) {
        for (int px = 0; px < ctx->width; px++) {
            double x0 = ctx->minX + (px * (ctx->maxX - ctx->minX) / ctx->width);
            double y0 = ctx->minY + (py * (ctx->maxY - ctx->minY) / ctx->height);

            ctx->currentPixelX = x0;
            ctx->currentPixelY = y0;

            double zx, zy, cx, cy;

            if (isJulia) {
                zx = x0; zy = y0;
                cx = cRe_Julia; cy = cIm_Julia;
            } else {
                zx = 0.0; zy = 0.0;
                cx = x0; cy = y0;
            }

            int iter = 0;
            while (zx * zx + zy * zy <= 4.0 && iter < maxIter) {
                double xtemp = zx * zx - zy * zy + cx;
                zy = 2.0 * zx * zy + cy;
                zx = xtemp;
                iter++;
            }

            // COLOR DINÁMICO
            if (iter < maxIter) {
                // Si escapó, usamos un gradiente basado en iteraciones
                // t va de 0.0 (escapó rápido) a 1.0 (casi no escapa)
                double t = (double)iter / (double)maxIter;
                // Aplicamos un factor para que el gradiente no sea tan lineal y se vea mejor
                t = sqrt(t); 
                
                RGBColor color = interpolateColor(ctx->colorStart, ctx->colorEnd, t);
                setPixel(ctx->bmp, px, py, color);
            } else {
                // No escapó: Pertenece al conjunto (Color Final sólido)
                // Opcionalmente negro, o el colorEnd
                setPixel(ctx->bmp, px, py, ctx->colorEnd); 
            }
        }
    }
}

/* ====================== IFS (Barnsley) ====================== */

static void executeTransformation(Transformation * t, RenderContext * ctx) {
    (void)t; 
    int points = (ctx->numPoints > 0) ? ctx->numPoints : 100000;
    double x = 0.0, y = 0.0;
    
    // USAR COLOR FINAL (Foreground)
    RGBColor color = ctx->colorEnd;

    for (int i = 0; i < points; i++) {
        int r = rand() % 100;
        double nextX, nextY;

        if (r < 1) {
            nextX = 0.0; nextY = 0.16 * y;
        } else if (r < 86) {
            nextX = 0.85 * x + 0.04 * y; nextY = -0.04 * x + 0.85 * y + 1.6;
        } else if (r < 93) {
            nextX = 0.20 * x - 0.26 * y; nextY = 0.23 * x + 0.22 * y + 1.6;
        } else {
            nextX = -0.15 * x + 0.28 * y; nextY = 0.26 * x + 0.24 * y + 0.44;
        }
        x = nextX; y = nextY;

        int px = mapX(ctx, x);
        int py = mapY(ctx, y);
        setPixel(ctx->bmp, px, py, color);
    }
}

/* ====================== Ejecución Reglas ====================== */

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
                        double cond = evaluateExpression(rs->ifStatement->condition, ctx);
                        if (cond != 0.0) return 1; 
                    }
                    break;
                default: break;
            }
        }
        list = list->next;
    }
    return 0;
}

static void executeRule(char * ruleName, ExpressionList * args, RenderContext * ctx) {
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

    IdentifierList * params = rule->identifierList;
    int pushedCount = 0;
    int i = 0;
    while(params && params->variable) {
        if (i < argCount) {
            pushVariable(ctx, params->variable->name, values[i]);
            pushedCount++;
        }
        params = params->identifierList;
        i++;
    }
    if (values) free(values);

    executeRuleSentences(rule->ruleSentenceList, ctx);

    for(int k=0; k < pushedCount; k++) popVariable(ctx);
}

/* ====================== Main ====================== */

void generateFractal(Program * program, const char * outputFilename) {
    if (!program) return;

    RenderContext ctx;
    ctx.width  = 1920; ctx.height = 1080;
    ctx.minX   = -2.0; ctx.maxX = 2.0;
    ctx.minY   = -2.0; ctx.maxY = 2.0;
    ctx.program = program;
    ctx.variables = NULL;
    ctx.bmp = NULL;
    ctx.currentPixelX = 0;
    ctx.currentPixelY = 0;
    ctx.numPoints = 100000;
    
    // Colores por defecto (Negro -> Blanco)
    ctx.colorStart.r = 0;   ctx.colorStart.g = 0;   ctx.colorStart.b = 0;
    ctx.colorEnd.r = 255;   ctx.colorEnd.g = 255;   ctx.colorEnd.b = 255;

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
                    if (sent->view && sent->view->x && sent->view->y) {
                        ctx.minX = evaluateExpression(sent->view->x->start, &ctx);
                        ctx.maxX = evaluateExpression(sent->view->x->end,   &ctx);
                        ctx.minY = evaluateExpression(sent->view->y->start, &ctx);
                        ctx.maxY = evaluateExpression(sent->view->y->end,   &ctx);
                    }
                    break;
                // AQUI LEEMOS LOS COLORES DEL AST
                case SENTENCE_COLOR:
                    if (sent->color && sent->color->startColor && sent->color->endColor) {
                        ctx.colorStart = parseHexColor(sent->color->startColor);
                        ctx.colorEnd   = parseHexColor(sent->color->endColor);
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

    ctx.bmp = createBitmap(ctx.width, ctx.height);
    
    // Usar el colorStart como fondo
    clearBitmap(ctx.bmp, ctx.colorStart);

    if (startRuleName) {
        executeRule(startRuleName, NULL, &ctx);
    }

    if (outputFilename == NULL) outputFilename = "output.bmp";
    saveBitmap(ctx.bmp, outputFilename);
    destroyBitmap(ctx.bmp);
}