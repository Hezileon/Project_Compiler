#include "../inc/Expression.h"

#include <map>
#include <cstring>
/*
 * supporting types:
 * expression_cmp: 123 ? 2 : 1; 123 >= 122 ? 2 : 1
 *
 *
 *
 *
 * unsupporting types: 12+-12;
 */

std::map<char*, int> IdfToValue;
int findIdfValue(char* str)
{
	if(IdfToValue.count(str))
	{
		return IdfToValue[str];
	}
	else
	{
		return IdfToValue[str] = 0;
	}
}

// constructor
expressionBasic::expressionBasic(Lexer* lexer)
{
	Token* tk = lexer->getToken();
	Type firstTokenType = tk->getType();

	if(firstTokenType == Operator)
	{
		if (strcmp(tk->getOp(), "(") == 0)
		{
			basicType = basicType_exp;
		}
	}
	else if(firstTokenType == Identifier)
	{
		basicType = basicType_idf;
	}
	else if (firstTokenType == Integer)
	{
		basicType = basicType_int;
	}
	else
	{
		std::cerr << "Expected Operator = but receive" << tk->getOp();
	}

	if (basicType == basicType_idf)
	{
		var.Idf = tk->getIdf();
	}
	else if (basicType == basicType_int)
	{
		var.Int = tk->getInt();
	}
	else if (basicType == basicType_exp)
	{
		var.exp = parseExpressionInBracket(lexer);
	}
}
int expressionBasic::evaluate()
{
	if (basicType == basicType_idf)
	{
		return findIdfValue(var.Idf);
	}
	else if (basicType == basicType_int)
	{
		return var.Int;
	}
	else if (basicType == basicType_exp)
	{
		return var.exp->evaluate();
	}
}
// constructor
expressionBinary::expressionBinary(expression* _exp_1, expression* _exp_2, char* _op)
{
	exp_1 = _exp_1;
	exp_2 = _exp_2;
	op = _op;
}
int expressionBinary::evaluate()
{
	if (strcmp(op, "+") == 0) { return exp_1->evaluate() + exp_2->evaluate(); }
	else if (strcmp(op, "-") == 0) { return exp_1->evaluate() - exp_2->evaluate(); }
	else if (strcmp(op, "*") == 0) { return exp_1->evaluate() * exp_2->evaluate(); }
	else if (strcmp(op, "/") == 0) { return exp_1->evaluate() / exp_2->evaluate(); }
	else if (strcmp(op, "==") == 0) { return exp_1->evaluate() == exp_2->evaluate(); }
	else if (strcmp(op, ">=") == 0) { return exp_1->evaluate() >= exp_2->evaluate(); }
	else if (strcmp(op, "<=") == 0) { return exp_1->evaluate() <= exp_2->evaluate(); }
	else if (strcmp(op, "<") == 0) { return exp_1->evaluate() < exp_2->evaluate(); }
	else if (strcmp(op, ">") == 0) { return exp_1->evaluate() > exp_2->evaluate(); }
	else { std::cerr << "Unexpected token: " << op << " received in expressionBinary"; }
}
// constructor
expressionTernery::expressionTernery(expression* _exp_1, expression* _exp_2, expression* _exp_3)
{
	exp_1 = _exp_1;
	exp_2 = _exp_2;
	exp_3 = _exp_3;
}
int expressionTernery::evaluate()
{
	if (exp_1->evaluate() == 1)
	{
		return exp_2->evaluate();
	}
	else
	{
		return exp_3->evaluate();
	}
}
// constructor
expressionUnary::expressionUnary(expression* _exp_1, char* _op)
{
	op = _op;
	exp_1 = _exp_1;
}
int expressionUnary::evaluate()
{
	if (strcmp(op, "-") == 0)
	{
		return (- 1)* exp_1->evaluate();
	}
	else
	{
		std::cerr << "Expected - in expressionUnary op but receive " << op << " instead";
	}
}

expression* parseExpressionInBracket(Lexer* lexer, bool leftBracketIncluded)
{
	if (!leftBracketIncluded)
	{
		Token* tk = lexer->getToken();
		if (tk->getType() == Operator && strcmp(tk->getOp(), "(") == 0) {}
		else { std::cerr << "expected ( but receive sth else"; }
	}

	int leftBracketCount = 1;
	//expression* exp1 = parseExpression1();
	while (leftBracketCount >= 1)
	{
		Token* tk = lexer->getToken();
		Type tk_type = tk->getType();
		if (tk_type == Operator && strcmp(tk->getOp(), ")") == 0) { leftBracketCount--; }

		if (tk_type == Identifier)
		{

		}
	}
	return nullptr;
}

expression* parseExpression(Lexer* lexer)
{
	expression* exp1_1 = parseExpression1(lexer);
	Token* tk_1 = lexer->getToken();

	if (tk_1->getType() == Operator && (strcmp(tk_1->getOp(), "==") == 0 || strcmp(tk_1->getOp(), ">=") == 0 || strcmp(tk_1->getOp(), "<=") == 0))
	{
		// exp1
		expression* exp_cmp = new expressionBinary{ exp1_1, parseExpression1(lexer), tk_1->getOp() };

		Token* tk_2 = lexer->getToken();
		if (tk_2->getType() == Operator && (strcmp(tk_2->getOp(), "?") != 0)) { std::cerr << "right after expressionCmp expected ? but receive sth else"; }
		// exp2
		expression* exp1_1 = parseExpression1(lexer);

		Token* tk_3 = lexer->getToken();
		if (tk_3->getType() == Operator && (strcmp(tk_3->getOp(), ":") != 0)) { std::cerr << "right after expressionCmp expected : but receive sth else"; }
		// exp3
		expression* exp1_2 = parseExpression1(lexer);

		// expression = exp1 ? exp2 : exp3;
		expression* rtn = new expressionTernery{ exp_cmp, exp1_1, exp1_2 };
		return rtn;
	}
	else if(tk_1->getType() == Operator && ( strcmp(tk_1->getOp(), "?") == 0))
	{
		expression* exp_cmp = exp1_1;
		expression* exp1_1 = parseExpression1(lexer);

		Token* tk_3 = lexer->getToken();
		if (tk_3->getType() == Operator && (strcmp(tk_3->getOp(), ":") != 0)) { std::cerr << "right after expressionCmp expected : but receive sth else"; }
		// exp3
		expression* exp1_2 = parseExpression1(lexer);

		// expression = exp1 ? exp2 : exp3;
		expression* rtn = new expressionTernery{ exp_cmp, exp1_1, exp1_2 };
		return rtn;
	}
	else
	{
		// expression = exp1;
		return exp1_1;
	}

}
// stop at ";" or stop at ?
// expression1 are sum of many expression2 
expression* parseExpression1(Lexer* lexer)
{
	expression* exp1 = parseExpression2(lexer);
	Token* tk_next = lexer->getToken();
	while (tk_next->getType() == Operator && ((strcmp(tk_next->getOp(), "+") == 0 || strcmp(tk_next->getOp(), "-") == 0)))
	{
		expression* exp2 = parseExpression2(lexer);
		exp1 = new expressionBinary(exp1, exp2, tk_next->getOp());

		tk_next = lexer->getToken();
	}
	if (tk_next->getType() == Operator && (strcmp(tk_next->getOp(), ";") == 0 
		|| strcmp(tk_next->getOp(), ":") == 0 || strcmp(tk_next->getOp(), "?") == 0 || strcmp(tk_next->getOp(), ">=") == 0 
		|| strcmp(tk_next->getOp(), "<=") == 0 || strcmp(tk_next->getOp(), "==") == 0 || strcmp(tk_next->getOp(), "<") == 0 
		|| strcmp(tk_next->getOp(), ">") == 0))
	{
		lexer->rollBack();
		return exp1;
	}
	else
	{
		std::cerr << "(parseExpression1 error)Expected ; or + or - but receive sth else :" << tk_next->getOp();
	}
}

expression* parseExpression2(Lexer* lexer)
{
	expression* exp1 = parseExpression3(lexer);
	Token* tk_next = lexer->getToken();
	while (tk_next->getType() == Operator && (strcmp(tk_next->getOp(), "*") == 0 || strcmp(tk_next->getOp(), "/") == 0))
	{
		expression* exp2 = parseExpression3(lexer);
		exp1 = new expressionBinary(exp1, exp2, tk_next->getOp());

		tk_next = lexer->getToken();
	}
	if (tk_next->getType() == Operator)
	{
		
		if (strcmp(tk_next->getOp(), "+") == 0 || strcmp(tk_next->getOp(), "-") == 0 || strcmp(tk_next->getOp(), ")") == 0 
			|| strcmp(tk_next->getOp(), ";") == 0 || strcmp(tk_next->getOp(), "?") == 0 || strcmp(tk_next->getOp(), ":") == 0 )
		{
			lexer->rollBack();
		}
		return exp1;
	}
	else
	{
		std::cerr << "(parseExpression2 error) Expected  + or -  or * or / but receive sth else";
	}
}
expression* parseExpression3(Lexer* lexer)
{
	Token* tk_next = lexer->getToken();

	
	if (tk_next->getType() == Operator && strcmp(tk_next->getOp(), "-") == 0)
	{
		expression* exp = parseExpression4(lexer);
		expression* rtn = new expressionUnary{ exp, tk_next->getOp() };
		return rtn;
	}
	else if (tk_next->getType() == Operator && strcmp(tk_next->getOp(), "(") == 0)
	{
		expression* exp = parseExpressionInBracket(lexer);
		return exp;
	}
	else
	{
		lexer->rollBack();
		return parseExpression4(lexer);
	}
}
expression* parseExpression4(Lexer* lexer)
{
	expression* exp = new expressionBasic(lexer);
	return exp;
}
