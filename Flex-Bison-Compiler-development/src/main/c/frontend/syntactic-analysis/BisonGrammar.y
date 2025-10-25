%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	TokenLabel token;
	double Double;
	char * string;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
	Range * range;
	View * view;
	DoubleConstant * doubleConstant;
	Sentence * sentence;
	SentenceList * sentenceList;
	Size * size;
	Color * color;
	Start * start;
	Variable * variable;
	Rule * rule;
	IdentifierList * identifierList;
	RuleSentenceList * ruleSentenceList;
	RuleSentence * ruleSentence;
	PointList * pointList;
	Point * point;
	Polygon * polygon;
	ExpressionList * expressionList;
	Call * call;
	IfStatement * ifStatement;
	TransformList * transformList;
	Transformation * transformation;
	TransformationSentence * transformationSentence;
	PointsStatement * pointsStatement;
	EscapeExpression * escapeExpression;
	EscapeFactor * escapeFactor;
	EscapeRange * escapeRange;
	Escape * escape;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>
%destructor { free($$); } <string>
%destructor { destroyView($$); } <view>
%destructor { destroyRange($$); } <range>
%destructor { destroyDoubleConstant($$); } <doubleConstant>
%destructor { destroySize($$); } <size>
%destructor { destroyColor($$); } <color>
%destructor { destroyVariable($$); } <variable>
%destructor { destroyRule($$); } <rule>
%destructor { destroyIdentifierList($$); } <identifierList>
%destructor { destroyRuleSentenceList($$); } <ruleSentenceList>
%destructor { destroyRuleSentence($$); } <ruleSentence>
%destructor { destroyPoint($$); } <point>
%destructor { destroyPointList($$); } <pointList>
%destructor { destroyPolygon($$); } <polygon>
%destructor { destroyStart($$); } <start>
%destructor { destroyExpressionList($$); } <expressionList>
%destructor { destroyCall($$); } <call>
%destructor { destroyIfStatement($$); } <ifStatement>
%destructor { destroyTransformation($$); } <transformation>
%destructor { destroyTransformationList($$); } <transformList>
%destructor { destroyTransformationSentence($$); } <transformationSentence>
%destructor { destroyPointsStatement($$); } <pointsStatement>
%destructor { destroyEscapeExpression($$); } <escapeExpression>
%destructor { destroyEscapeFactor($$); } <escapeFactor>
%destructor { destroyEscapeRange($$); } <escapeRange>
%destructor { destroyEscape($$); } <escape>
%destructor { destroySentence($$); } <sentence>
%destructor { destroySentenceList($$); } <sentenceList>

/** Terminals. */
%token <integer> INTEGER
%token <token> ADD
%token <token> CLOSE_BRACE
%token <token> CLOSE_COMMENT
%token <token> CLOSE_PARENTHESIS
%token <token> DIV
%token <token> MUL
%token <token> OPEN_BRACE
%token <token> OPEN_COMMENT
%token <token> OPEN_PARENTHESIS
%token <token> SUB
%token <token> LOWER_THAN
%token <token> GREATER_THAN

%token <token> IGNORED
%token <token> UNKNOWN

%token <token> VIEW
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <token> COMMA
%token <Double> DOUBLE
%token <token> LINE_JUMP
%token <token> SIZE
%token <token> COLOR
%token <string> HEX_COLOR
%token <token> RULE
%token <string> IDENTIFIER
%token <token> DRAW_POLYGON
%token <token> POINT
%token <token> START
%token <token> INDENT
%token <token> DEDENT
%token <token> CALL
%token <token> IF
%token <token> STOP
%token <token> PERCENT
%token <token> TRANSFORM
%token <token> SCALE
%token <token> TRANSLATE
%token <token> ROTATE
%token <token> SHEAR
%token <token> POINTS
%token <token> ESCAPE
%token <token> ASSIGNMENT
%token <token> UNTIL
%token <token> MAX
%token <token> X_COORD
%token <token> Y_COORD
%token <token> PIPE


/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program
%type <range> range
%type <view> view
%type <doubleConstant> doubleConstant
%type <sentence> sentence
%type <sentenceList> sentenceList
%type <size> size
%type <color> color
%type <start> start
%type <rule> rule
%type <variable> variable
%type <identifierList> identifierList
%type <ruleSentenceList> ruleSentenceList
%type <ruleSentence> ruleSentence
%type <pointList> pointList
%type <point> point
%type <polygon> polygon
%type <expressionList> expressionList
%type <call> call
%type <ifStatement> ifStatement
%type <transformList> transformList
%type <transformation> transformation
%type <transformationSentence> transformationSentence
%type <pointsStatement> pointsStatement
%type <escapeExpression> escapeExpression
%type <escapeFactor> escapeFactor
%type <escapeRange> escapeRange
%type <escape> escape

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left LOWER_THAN GREATER_THAN
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: optionalLineJumps sentenceList[sentences]	optionalLineJumps							{ $$ = ProgramSemanticAction($sentences); }
	;

optionalLineJumps: 
	| lineJumps
	;
	
lineJumps: lineJumps LINE_JUMP
	| LINE_JUMP
	;


sentenceList: sentenceList[list] lineJumps sentence[line]	{ $$ = SentenceListSemanticAction($list, $line); }
	| sentence[line]										{ $$ = SentenceListSemanticAction(NULL, $line); }
	;

sentence: expression										{ $$ = SentenceExpressionSemanticAction($1); }
	| view													{ $$ = SentenceViewSemanticAction($1); }
	| size													{ $$ = SentenceSizeSemanticAction($1); }
	| color                                                 { $$ = SentenceColorSemanticAction($1); }
	| rule                                                  { $$ = SentenceRuleSemanticAction($1); }
	| start                                                 { $$ = SentenceStartSemanticAction($1); }
	;

expression: expression[left] ADD expression[right]			{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| expression[left] LOWER_THAN expression[right]       	{ $$ = ArithmeticExpressionSemanticAction($left, $right, LOWER_THAN_OP); }
	| expression[left] GREATER_THAN expression[right]     	{ $$ = ArithmeticExpressionSemanticAction($left, $right, GREATER_THAN_OP); }
	| factor												{ $$ = FactorExpressionSemanticAction($1); }
	| PIPE expression[expr] PIPE                           	{ $$ = PipeExpressionSemanticAction($expr); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS		{ $$ = ExpressionFactorSemanticAction($2); }
	| constant												{ $$ = ConstantFactorSemanticAction($1); }
	| variable												{ $$ = VariableFactorSemanticAction($1); }
	| doubleConstant										{ $$ = DoubleConstantFactorSemanticAction($1); }
	;

constant: INTEGER											{ $$ = IntegerConstantSemanticAction($1); }
	;

doubleConstant: DOUBLE										{ $$ = DoubleConstantSemanticAction($1); }
	;

range: OPEN_BRACKET expression[start] COMMA expression[end] CLOSE_BRACKET	{ $$ = RangeSemanticAction($start, $end); }
	;

view: VIEW range[x] range[y] 								{ $$ = ViewSemanticAction($x, $y); }
	;

size: SIZE constant[x] constant[y]							{ $$ = SizeSemanticAction($x, $y); }
	;

color: COLOR HEX_COLOR[start] HEX_COLOR[end]                { $$ = ColorSemanticAction($start, $end); }
	;

variable: IDENTIFIER 										{ $$ = VariableSemanticAction($1); }
	;

rule: RULE variable[var] identifierList[identifiers] lineJumps INDENT ruleSentenceList[list] optionalLineJumps DEDENT	{ $$ = RuleSemanticAction($var, $identifiers, $list); }
    ;

identifierList: identifierList[list] variable[iden]			{ $$ = IdentifiersListSemanticAction($list, $iden); }
	|														{ $$ = IdentifiersListSemanticAction(NULL, NULL); }

ruleSentenceList: ruleSentenceList[list] lineJumps ruleSentence[line]	{ $$ = RuleSentenceListSemanticAction($list, $line); }
    | ruleSentence[line]												{ $$ = RuleSentenceListSemanticAction(NULL, $line); }
    ;

ruleSentence: polygon[poligono] 								{ $$ = RuleSentencePolygonSemanticAction($poligono); }
	| call[c]													{ $$ = RuleSentenceCallSemanticAction($c); }
	| ifStatement[f]											{ $$ = RuleSentenceIfStatementSemanticAction($f); }
	| transformation[t]											{ $$ = RuleSentenceTransformationSemanticAction($t); }
	| pointsStatement[ps]                                       { $$ = RuleSentencePointsStatementSemanticAction($ps); }
	| escape[esc] 												{ $$ = RuleSentenceEscapeSemanticAction($esc); }
    ;

ifStatement: IF expression[cond] STOP							{ $$ = IfStatementSemanticAction($cond); }

call: CALL variable[var] expressionList[list]				{ $$ = CallSemanticAction($var, $list); }

expressionList: expressionList[list] expression[expr]		{ $$ = ExpressionListSemanticAction($list, $expr); }
	| 														{ $$ = ExpressionListSemanticAction(NULL, NULL); }
	;

polygon: DRAW_POLYGON lineJumps INDENT optionalLineJumps pointList[list] optionalLineJumps DEDENT	{ $$ = PolygonSemanticAction($list); }
	;

transformation: TRANSFORM constant[cnst] PERCENT lineJumps INDENT optionalLineJumps transformList[list] optionalLineJumps DEDENT { $$ = TransformationSemanticAction($cnst, $list); }
	;

transformList: transformList[list] lineJumps transformationSentence[ts]	{ $$ = TransformListSemanticAction($list, $ts); }
	| transformationSentence[ts]									{ $$ = TransformListSemanticAction(NULL, $ts); }
	;

transformationSentence: SCALE expression[x] expression[y]		{ $$ = TransformationSentenceSemanticAction($x, $y, SCALE_SENTENCE); }
	| TRANSLATE expression[x] expression[y]					{ $$ = TransformationSentenceSemanticAction($x, $y, TRANSLATE_SENTENCE); }
	| ROTATE expression[angle]								{ $$ = TransformationSentenceSemanticAction($angle, NULL, ROTATE_SENTENCE); }
	| SHEAR expression[x] expression[y]						{ $$ = TransformationSentenceSemanticAction($x, $y, SHEAR_SENTENCE); }
	;

pointList: pointList[list] lineJumps point[punto] 			{ $$ = PointListSemanticAction($list, $punto); }
    | point[punto]											{ $$ = PointListSemanticAction(NULL, $punto); }
    ;

point: POINT expression[x] expression[y]			{ $$ = PointSemanticAction($x, $y); }
    ;

pointsStatement: POINTS constant[numPoints]    { $$ = PointsStatementSemanticAction($numPoints); }
	;

start: START variable[var] 									{ $$ = StartSemanticAction($var); }	
	;

escapeExpression:	escapeExpression[left] ADD escapeExpression[right]	{ $$ = EscapeExpressionSemanticAction($left, $right, ADDITION); }
	| escapeExpression[left] DIV escapeExpression[right]				{ $$ = EscapeExpressionSemanticAction($left, $right, DIVISION); }
	| escapeExpression[left] MUL escapeExpression[right]				{ $$ = EscapeExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| escapeExpression[left] SUB escapeExpression[right]				{ $$ = EscapeExpressionSemanticAction($left, $right, SUBTRACTION); }
	| escapeExpression[left] LOWER_THAN escapeExpression[right]       	{ $$ = EscapeExpressionSemanticAction($left, $right, LOWER_THAN_OP); }
	| escapeExpression[left] GREATER_THAN escapeExpression[right]     	{ $$ = EscapeExpressionSemanticAction($left, $right, GREATER_THAN_OP); }
	| escapeFactor														{ $$ = EscapeFactorEscapeExpressionSemanticAction($1); }
	| PIPE escapeExpression[expr] PIPE                           		{ $$ = PipeEscapeExpressionSemanticAction($expr); }
	;

escapeFactor: OPEN_PARENTHESIS escapeExpression CLOSE_PARENTHESIS		{ $$ = EscapeExpressionEscapeFactorSemanticAction($2); }
	| constant															{ $$ = ConstantEscapeFactorSemanticAction($1); }
	| variable															{ $$ = VariableEscapeFactorSemanticAction($1); }
	| doubleConstant													{ $$ = DoubleConstantEscapeFactorSemanticAction($1); }
	| escapeRange														{ $$ = EscapeRangeEscapeFactorSemanticAction($1); }							
	| X_COORD															{ $$ = XCoordEscapeFactorSemanticAction(); }
	| Y_COORD															{ $$ = YCoordEscapeFactorSemanticAction(); }
	;

escapeRange: OPEN_BRACKET escapeExpression[start] COMMA escapeExpression[end] CLOSE_BRACKET	{ $$ = EscapeRangeSemanticAction($start, $end); }
	;

escape: ESCAPE escapeExpression[start] variable[var] ASSIGNMENT escapeExpression[rec] UNTIL escapeExpression[until] MAX constant[k] { $$ = EscapeSemanticAction($start, $var, $rec, $until, $k); }
	;

%%
