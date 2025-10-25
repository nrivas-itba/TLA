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
	RuleSentenceList * ruleSentenceList;
	RuleSentence * ruleSentence;
	PointList * pointList;
	Point * point;
	Polygon * polygon;
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
%token <token> COLON
%token <string> IDENTIFIER
%token <token> DRAW
%token <token> POLYGON
%token <token> POINT
%token <token> START
%token <token> INDENT
%token <token> DEDENT

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
%type <ruleSentenceList> ruleSentenceList
%type <ruleSentence> ruleSentence
%type <pointList> pointList
%type <point> point
%type <polygon> polygon

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
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
	| factor												{ $$ = FactorExpressionSemanticAction($1); }
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

range: OPEN_BRACKET doubleConstant[start] COMMA doubleConstant[end] CLOSE_BRACKET	{ $$ = RangeSemanticAction($start, $end); }
	;

view: VIEW range[x] range[y] 								{ $$ = ViewSemanticAction($x, $y); }
	;

size: SIZE constant[x] constant[y]							{ $$ = SizeSemanticAction($x, $y); }
	;

color: COLOR HEX_COLOR[start] HEX_COLOR[end]                { $$ = ColorSemanticAction($start, $end); }
	;

variable: IDENTIFIER 										{ $$ = VariableSemanticAction($1); }
	;

rule: RULE variable[var] COLON lineJumps INDENT ruleSentenceList[list] DEDENT	{ $$ = RuleSemanticAction($var, $list); }
    ;

ruleSentenceList: ruleSentenceList[list] lineJumps ruleSentence[line]	{ $$ = RuleSentenceListSemanticAction($list, $line); }
    | ruleSentence[line]												{ $$ = RuleSentenceListSemanticAction(NULL, $line); }
    ;
    
ruleSentence: DRAW POLYGON lineJumps INDENT polygon[poligono] DEDENT		{ $$ = RuleSentenceSemanticAction($poligono); }
    ;

polygon: optionalLineJumps pointList[list] optionalLineJumps	{ $$ = PolygonSemanticAction($list); }
	;

pointList: pointList[list] lineJumps point[punto] 			{ $$ = PointListSemanticAction($list, $punto); }
    | point[punto]								{ $$ = PointListSemanticAction(NULL, $punto); }
    ;

point: POINT doubleConstant[x] doubleConstant[y]			{ $$ = PointSemanticAction($x, $y); }
    ;

start: START variable[var] 									{ $$ = StartSemanticAction($var); }	
	;
%%
