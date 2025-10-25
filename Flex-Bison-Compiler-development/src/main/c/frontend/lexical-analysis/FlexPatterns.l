%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus EnterImportExpressionLexemeAction(FlexContext context);
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus LeaveImportExpressionLexemeAction();
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus SubexpressionLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus LexemeAction(TokenLabel label);
extern CompilationStatus DoubleLexemeAction();
extern CompilationStatus HexColorLexemeAction();
extern CompilationStatus IdentifierLexemeAction();

extern CompilationStatus IndentLexemeAction();
extern CompilationStatus DedentLexemeAction();

#define MAX_INDENT_LEVELS 128

static int _indent_stack[MAX_INDENT_LEVELS] = {0};
static int _indent_top = 1;

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x IMPORT_EXPRESSION
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace		[ \t\r\v\f]+

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%} 

\n[\t ]*[^ \t\r\n] {
	int new_indent = yyleng - 2;
	

	
	int current_indent = _indent_stack[_indent_top - 1];

	if (new_indent > current_indent) {
		if (_indent_top < MAX_INDENT_LEVELS) {
			_indent_stack[_indent_top++] = new_indent;
			yyless(yyleng - 1);
			LexemeAction(LINE_JUMP);
			return IndentLexemeAction();
		}
	} else if (new_indent < current_indent) {
		while (_indent_top > 0 && new_indent < _indent_stack[_indent_top - 1]) {
			_indent_top--;
			DedentLexemeAction();
		}
		yyless(yyleng - 1);
		return LexemeAction(LINE_JUMP);
	}
	yyless(yyleng - 1);
	return LexemeAction(LINE_JUMP);
}


"{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
<IMPORT_EXPRESSION>"}"				{ return LeaveImportExpressionLexemeAction(); }
<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); }

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }
"<"                                 { return ArithmeticOperatorLexemeAction(LOWER_THAN); }
">"                                 { return ArithmeticOperatorLexemeAction(GREATER_THAN); }

"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }

"view"                              { return LexemeAction(VIEW); }    
"["                                 { return LexemeAction(OPEN_BRACKET); }    
"]"                                 { return LexemeAction(CLOSE_BRACKET); }    
","                                 { return LexemeAction(COMMA); }   

\n                                  { return LexemeAction(LINE_JUMP); }

"size"                              { return LexemeAction(SIZE); }
    
-?[[:digit:]]+\.[[:digit:]]*		{ return DoubleLexemeAction(); }

[[:digit:]]+						{ return IntegerLexemeAction(); }

"color"                             { return LexemeAction(COLOR); }
"#"[0-9a-fA-F]{6}                   { return HexColorLexemeAction(); }

":"                                 { return LexemeAction(COLON); }
"rule"                              { return LexemeAction(RULE); }

"draw"                              { return LexemeAction(DRAW); }
"polygon"                           { return LexemeAction(POLYGON); }
"point"                             { return LexemeAction(POINT); }

"start"                             { return LexemeAction(START); }

"call" 								{ return LexemeAction(CALL); }	

"if" 							  	{ return LexemeAction(IF); }
"stop" 							  	{ return LexemeAction(STOP); }

[A-Za-z_][A-Za-z0-9_]*              { return IdentifierLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>			{ 
	
	while(--_indent_top > 0) {
		DedentLexemeAction();
	}
	return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
