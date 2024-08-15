#include "../inc/Expression.h"
#include "../inc/compiler.h"
#include "../inc/preprocessor.h"
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


/*
 *
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << asm_num << " " << x << " " << y << " " << z;
		if (anotationModeOn) std::cout << "// ...";
		std::cout << std::endl;
 *
 *
 */

extern bool anotationModeOn = false;
extern bool lineCounterModeOn = false;
extern bool compileModeOn = true;

extern bool output_banned = true;
extern int LC;
// TODO: refine the order of "!";

std::map<std::string, int> IdfToValue;
int findIdfValue(char* _str)
{
	std::string str = _str;
	if(IdfToValue.count(str))
	{
		return IdfToValue[str];
	}
	else
	{
		return IdfToValue[str] = 0;
	}
}

int assignValueToIdf(char* _str,int value)
{
	std::string str = _str;
	return IdfToValue[str] = value;
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
		std::cerr << "Expected Operator = but receive" << tk->getOp()<<std::endl;
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
void expressionBasic::evaluate_compiler(int pos)
{
	if (basicType == basicType_idf)
	{
		// MARK -> illegal behavior (findIdfValue)

		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 3 << " " << NameToAddress(var.Idf) << " " << " " << " " <<pos ;
			if (anotationModeOn) std::cout << "// 拷贝指令,语义：MEM[z] = MEM[x],此处用法为：读取变量值至MEM[z]  ";
			std::cout << std::endl;
		}
		
		MEM[pos] = MEM[NameToAddress(var.Idf)];
	}
	else if (basicType == basicType_int)
	{
		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << "0 " << var.Int << " " << " " << " " << pos;
			if (anotationModeOn)std::cout << "// 常量存储，语义：MEM[z] = x,处理Basic Expression";
			std::cout << std::endl;
		}
		
		MEM[pos] = var.Int;
	}
	else if (basicType == basicType_exp)
	{
		int posExp = pos + 1;
		var.exp->evaluate_compiler(posExp);
		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << "0 " << MEM[posExp] << " " << " " << " " << pos;
			if (anotationModeOn)std::cout << "// 常量存储，语义：MEM[z] = x,处理Basic Expression";
			std::cout << std::endl;
		}
		
		MEM[pos] = MEM[posExp];
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
	else if (strcmp(op, "||") == 0) { return exp_1->evaluate() || exp_2->evaluate(); }
	else if (strcmp(op, "&&") == 0) { return exp_1->evaluate() && exp_2->evaluate(); }
	else { std::cerr << "Unexpected token: " << op << " received in expressionBinary" << std::endl; }
}
		// FOR evaluate_compiler(int pos);
void helper_expBi(char* op, int pos, int posE1, int posE2)
{
	if (compileModeOn)
	{
		int num = 0;
		if (strcmp(op, "+") == 0) { num = 4; }
		else if (strcmp(op, "-") == 0) { num = 5; }
		else if (strcmp(op, "*") == 0) { num = 6; }
		else if (strcmp(op, "/") == 0) { num = 7; }
		else if (strcmp(op, "==") == 0) { num = 9; }
		else if (strcmp(op, ">=") == 0) { num = -1; }
		else if (strcmp(op, "<=") == 0) { num = -2; }
		else if (strcmp(op, ">") == 0) { num = 10; }
		else if (strcmp(op, "<") == 0) { num = 11; }
		else if (strcmp(op, "||") == 0) { num = 13; }
		else if (strcmp(op, "&&") == 0) { num = 12; }
		if (num != -1 && num != -2)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << num << " " << posE1 << " " << posE2 << " " << pos;
			if (anotationModeOn)std::cout << "// 二目运算符指令，语义：MEM[z] = MEM[x] op MEM[y],处理Binary Expression";
			std::cout << std::endl;
		}
		else
		{
			if (num == -1)
			{
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 11 << " " << posE1 << " " << posE2 << " " << pos + 3;
				if (anotationModeOn)std::cout << "// 二目运算符指令，语义：MEM[z] = MEM[x] < MEM[y],处理Binary Expression (special case >=)";
				std::cout << std::endl;

				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 14 << " " << pos + 3 << " " << " " << " " << pos;
				if (anotationModeOn)std::cout << "// MEM[z] = !MEM[x] ";
				std::cout << std::endl;
			}
			if (num == -2)
			{
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 12 << " " << posE1 << " " << posE2 << " " << pos + 3;
				if (anotationModeOn)std::cout << "// 二目运算符指令，语义：MEM[z] = MEM[x] > MEM[y],处理Binary Expression (special case <=)";
				std::cout << std::endl;

				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 14 << " " << pos + 3 << " " << " " << " " << pos;
				if (anotationModeOn)std::cout << "// MEM[z] = !MEM[x] ";
				std::cout << std::endl;;
			}
		}

	}
}
void expressionBinary::evaluate_compiler(int pos)
{
	int posExp1, posExp2; posExp1 = pos + 1; posExp2 = pos + 2; // calculate posExp1 first, otherwise the already saved MEM[posExp2] will be changed;

	if (strcmp(op, "+") == 0) { exp_1->evaluate_compiler(posExp1); exp_2->evaluate_compiler(posExp2); helper_expBi(op, pos, posExp1, posExp2); MEM[pos] = MEM[posExp1]+MEM[posExp2]; }
	else if (strcmp(op, "-") == 0) { exp_1->evaluate_compiler(posExp1);exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]-MEM[posExp2];}
	else if (strcmp(op, "*") == 0) { exp_1->evaluate_compiler(posExp1);exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]*MEM[posExp2];}
	else if (strcmp(op, "/") == 0) { exp_1->evaluate_compiler(posExp1);exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]/MEM[posExp2];}
	else if (strcmp(op, "==") == 0) { exp_1->evaluate_compiler(posExp1); exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]==MEM[posExp2];}
	else if (strcmp(op, ">=") == 0) { exp_1->evaluate_compiler(posExp1); exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]>=MEM[posExp2];}
	else if (strcmp(op, "<=") == 0) { exp_1->evaluate_compiler(posExp1); exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]<=MEM[posExp2];}
	else if (strcmp(op, "<") == 0) { exp_1->evaluate_compiler(posExp1);exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]<MEM[posExp2];}
	else if (strcmp(op, ">") == 0) { exp_1->evaluate_compiler(posExp1);exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]>MEM[posExp2];}
	else if (strcmp(op, "||") == 0) { exp_1->evaluate_compiler(posExp1);exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]||MEM[posExp2];}
	else if (strcmp(op, "&&") == 0) { exp_1->evaluate_compiler(posExp1);exp_2->evaluate_compiler(posExp2); helper_expBi(op,pos,posExp1,posExp2);MEM[pos] = MEM[posExp1]&&MEM[posExp2];}
	else { std::cerr << "Unexpected token: " << op << " received in expressionBinary" << std::endl;}
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
	// with integer how is Ternery Operator defined?
	if (exp_1->evaluate() == 0)
	{
		return exp_3->evaluate();
	}
	else
	{
		return exp_2->evaluate();
	}
}
void expressionTernery::evaluate_compiler(int pos)
{
	int exp1_pos = pos+1;
	int exp_pos = pos+2;
	int nLine1 = exp_1->lineCount_comp();
	int nLine2 = exp_2->lineCount_comp();
	int nLine3 = exp_3->lineCount_comp();

	exp_1->evaluate_compiler(exp1_pos);	
	exp_2->evaluate_compiler(exp_pos);
	if (compileModeOn)
	{
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 20 << " " << exp1_pos << " " << " " << " " << LC + nLine3;
		if (anotationModeOn) std::cout << "// 如果MEM[x]的值非零，跳转到第z条指令继续执行, 即跳过exp_3的计算，避免了exp_2的值被覆盖";
		std::cout << std::endl;
		exp_3->evaluate_compiler(exp_pos);

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 3 << " " << exp_pos << " " << " " << " " << pos;
		if (anotationModeOn) std::cout << "// 拷贝指令，语义：MEM[y] = MEM[x], 即将exp_pos拷贝到pos";
		std::cout << std::endl;
	}
	
	if(!MEM[exp1_pos])
	{
		exp_3->evaluate_compiler(exp_pos);
	}
	MEM[pos] = MEM[exp_pos];

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
	else if (strcmp(op, "!") == 0)
	{
		return !exp_1->evaluate();
	}
	else
	{
		std::cerr << "Expected - in expressionUnary op but receive " << op << " instead"<<std::endl;
	}
}
void expressionUnary::evaluate_compiler(int pos)
{
	
	int exp_1Pos = pos + 1;
	if (strcmp(op, "-") == 0)
	{
		exp_1->evaluate_compiler(exp_1Pos);
		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << "0 " << 0 << " " << " " << " " << pos + 2;
			if (anotationModeOn)std::cout << "// 常量存储，语义：MEM[pos] = 0, 和后一步配合实现取负";
			std::cout << std::endl;
			MEM[exp_1Pos + 1] = 0;// note that: exp_1Pos = pos+1

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << "5 " << pos + 2 << " " << exp_1Pos << " " << pos;
			if (anotationModeOn)std::cout << "// 二目运算符指令，语义：MEM[z] = MEM[x] - MEM[y]";
			std::cout << std::endl;
		}
		
		MEM[pos] = MEM[exp_1Pos + 1]-MEM[exp_1Pos];
	}
	else if (strcmp(op, "!") == 0)
	{
		exp_1->evaluate_compiler(exp_1Pos);

		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << "14 " << exp_1Pos << " " << " " << " " << pos;
			if (anotationModeOn)std::cout << "// MEM[z] = !MEM[x]";
			std::cout << std::endl;
		}

		MEM[pos] =!MEM[exp_1Pos];
	}
	else
	{
		std::cerr << "Expected - in expressionUnary op but receive " << op << " instead" << std::endl;
	}
}


// Realization: call parseExpression(), they call each other to get the job done.
// Functionality: to take care of those expression in brackets -- that is you get the expression inside ( )
expression* parseExpressionInBracket(Lexer* lexer, bool leftBracketAlreadyIncluded)
{
	if (!leftBracketAlreadyIncluded)
	{
		Token* tk = lexer->getToken();
		if (tk->getType() == Operator && strcmp(tk->getOp(), "(") == 0) {}
		else { std::cerr << "(parseExpressionInBracket error) Expected ( but receive sth else"<<std::endl; }
	}

	int leftBracketCount = 1;
	expression* exp1 = parseExpression(lexer);
	while (leftBracketCount >= 1)
	{
		Token* tk = lexer->getToken();
		if (tk->getType() == Operator && strcmp(tk->getOp(), ")") == 0) {leftBracketCount--;}
		else 
		{
			std::cerr << "(parseExpressionInBracket ERROR) Expected ) but receive: ";
			if (tk->getType() == Operator) { std::cout << tk->getOp(); }
		if (tk->getType() == Integer) { std::cout << tk->getInt(); }
		if (tk->getType() == Identifier) { std::cout << tk->getIdf(); }
		std::cout << " instead" << std::endl;
		}
	}
	
	return exp1;
}
/*
expression* parseExpression(Lexer* lexer)
{
	expression* exp1_1 = parseExpression1(lexer);
	Token* tk_1 = lexer->getToken();
	if (tk_1->getType() == Operator &&

		(strcmp(tk_1->getOp(), "==") == 0 || strcmp(tk_1->getOp(), ">=") == 0 || strcmp(tk_1->getOp(), "<=") == 0 
		|| strcmp(tk_1->getOp(), "<") == 0 || strcmp(tk_1->getOp(), ">") == 0 ) )
	{
		// exp1
		expression* exp_cmp = new expressionBinary{ exp1_1, parseExpression1(lexer), tk_1->getOp() };

		Token* tk_2 = lexer->getToken();
		if (tk_2->getType() == Operator && (strcmp(tk_2->getOp(), "?") != 0)) { std::cerr << "right after expressionCmp expected ? but receive sth else"<<std::endl; }
		// exp2
		expression* exp1_1 = parseExpression1(lexer);

		Token* tk_3 = lexer->getToken();
		if (tk_3->getType() == Operator && (strcmp(tk_3->getOp(), ":") != 0)) { std::cerr << "right after expressionCmp expected : but receive sth else"<<std::endl; }
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
		if (tk_3->getType() == Operator && (strcmp(tk_3->getOp(), ":") != 0)) { std::cerr << "right after expressionCmp expected : but receive sth else"<<std::endl; }
		// exp3
		expression* exp1_2 = parseExpression1(lexer);

		// expression = exp1 ? exp2 : exp3;
		expression* rtn = new expressionTernery{ exp_cmp, exp1_1, exp1_2 };
		return rtn;
	}
	else
	{
		lexer->rollBack();
		return exp1_1;
	}
}
*/
expression* parseExpressionCmp(expression* exp1_1, Token* tk_1,Lexer* lexer)
{
	if (tk_1->getType() == Operator &&

		(strcmp(tk_1->getOp(), "==") == 0 || strcmp(tk_1->getOp(), ">=") == 0 || strcmp(tk_1->getOp(), "<=") == 0
			|| strcmp(tk_1->getOp(), "<") == 0 || strcmp(tk_1->getOp(), ">") == 0))
	{
		// exp1
		expression* exp_cmp = new expressionBinary{ exp1_1, parseExpression1(lexer), tk_1->getOp() };
		
		return exp_cmp;
	}
	else
	{
		lexer->rollBack();
		return exp1_1;
	}
}

expression* parseExpression(Lexer* lexer)
{
	expression* exp_cmp = parseExpression1(lexer);
	expression* exp1_1 = nullptr;
	expression* exp1_2 = nullptr;
	Token* tk_1 = lexer->getToken();
	while (tk_1->getType() == Operator && 
		(strcmp(tk_1->getOp(), "&&") == 0 || strcmp(tk_1->getOp(), "||") == 0 
		|| strcmp(tk_1->getOp(), "==") == 0 || strcmp(tk_1->getOp(), ">=") == 0 || strcmp(tk_1->getOp(), "<=") == 0
		|| strcmp(tk_1->getOp(), "<") == 0 || strcmp(tk_1->getOp(), ">") == 0))
	{
		if (tk_1->getType() == Operator && 
			(strcmp(tk_1->getOp(), "&&") == 0 || strcmp(tk_1->getOp(), "||") == 0))
		{
			// exp_cmp && exp_temp, where exp_temp could be expressionCmp or BoolValue;
			expression* exp_temp = parseExpression1(lexer);
			Token* tk_2 = lexer->getToken();
			exp_temp = parseExpressionCmp(exp_temp, tk_2, lexer);
			exp_cmp = new expressionBinary(exp_cmp, exp_temp, tk_1->getOp());
			tk_1 = tk_2;
		}
		else
		{
			exp_cmp = parseExpressionCmp(exp_cmp, tk_1, lexer);
		}
		tk_1 = lexer->getToken();
	}
	// i.e. exp_cmp should stop, or it is expression1 instead; 
	if((tk_1->getType() == Operator && (strcmp(tk_1->getOp(), "?") == 0)))
	{
		// exp2
		exp1_1 = parseExpression1(lexer);

		Token* tk_3 = lexer->getToken();
		if (tk_3->getType() == Operator && (strcmp(tk_3->getOp(), ":") != 0)) { std::cerr << "right after expressionCmp expected : but receive sth else" << std::endl; }
		// exp3
		exp1_2 = parseExpression1(lexer);

		// expression = exp1 ? exp2 : exp3;
		expression* rtn = new expressionTernery{ exp_cmp, exp1_1, exp1_2 };
		return rtn;
	}
	else
	{
		lexer->rollBack();
		return exp_cmp;
	}
}

// Realization: Idea1: take care of the calculation of order1 and any expression of higher order, we call other parseExpression to process;
//		Idea2: exp2 = new expressionBinary(exp2, parseExpression2(lexer), tk_check_multiply->getOp()); Recursively generate a expression*;
// expression1-> + -;
expression* parseExpression1(Lexer* lexer)
{
	expression* exp1 = parseExpression2(lexer);
	Token* tk_next = lexer->getToken();
	while (tk_next->getType() == Operator && ((strcmp(tk_next->getOp(), "+") == 0 || strcmp(tk_next->getOp(), "-") == 0)))
	{
		// after +(-) three possibilities: 1. ...*... expression2 2. (...) expression   ->->-> 3. expression mixed with expression2
		// take care of possibility 2
		expression* exp2 = nullptr;
		Token* tk_check_parathesis = lexer->getToken();
		if(tk_check_parathesis->getType() == Operator && strcmp(tk_check_parathesis->getOp(), "(") == 0)
		{
			exp2 = parseExpressionInBracket(lexer);
			Token* tk_check_multiply = lexer->getToken(); 
			if(tk_check_multiply->getType() == Operator && 
				((strcmp(tk_check_multiply->getOp(), "*") == 0 || strcmp(tk_check_multiply->getOp(), "/") == 0)))
			{
				// take care of possibility 3
				exp2 = new expressionBinary(exp2, parseExpression2(lexer), tk_check_multiply->getOp());
			}
			else
			{
				lexer->rollBack();
			}
		}
		else
		{
			lexer->rollBack();
			exp2 = parseExpression2(lexer);
		}
		exp1 = new expressionBinary(exp1, exp2, tk_next->getOp());

		tk_next = lexer->getToken();
	}
	if (tk_next->getType() == Operator && 
		(strcmp(tk_next->getOp(), ";") == 0 
		|| strcmp(tk_next->getOp(), ":") == 0 || strcmp(tk_next->getOp(), "?") == 0 || strcmp(tk_next->getOp(), ">=") == 0 
		|| strcmp(tk_next->getOp(), "<=") == 0 || strcmp(tk_next->getOp(), "==") == 0 || strcmp(tk_next->getOp(), "<") == 0 
		|| strcmp(tk_next->getOp(), ">") == 0 || strcmp(tk_next->getOp(), "(") == 0 || strcmp(tk_next->getOp(), ")") == 0
		|| strcmp(tk_next->getOp(), "&&") == 0 || strcmp(tk_next->getOp(), "||") == 0 || strcmp(tk_next->getOp(), "{") == 0
		|| strcmp(tk_next->getOp(), "}")==0 || strcmp(tk_next->getOp(), "," )== 0) )
	{
		
		lexer->rollBack();
		return exp1;
	}
	else
	{

		std::cerr << "(Generate Expression1 ERROR) Expected ; or + or - but receive :";
		if (tk_next->getType() == Operator) { std::cout << tk_next->getOp(); }
		if (tk_next->getType() == Integer) { std::cout << tk_next->getInt(); }
		if (tk_next->getType() == Identifier) { std::cout << tk_next->getIdf(); }
		std::cout << " instead" << std::endl;

		
	}
}
// * /
expression* parseExpression2(Lexer* lexer)
{
	//TODO: seems that the additional attention at parseExpression1 can save the work of the following codes;
	/*
	expression* exp1 = nullptr;
	Token* tk_check_parathesis = lexer->getToken();
	if (tk_check_parathesis->getType() == Operator && strcmp(tk_check_parathesis->getOp(), "(") == 0)
	{
		exp1 = parseExpressionInBracket(lexer);
	}
	else
	{
		lexer->rollBack();
		exp1 = parseExpression3(lexer);
	}
	*/
	expression* exp1 = nullptr;
	exp1 = parseExpression3(lexer);

	Token* tk_next = lexer->getToken();
	while (tk_next->getType() == Operator && (strcmp(tk_next->getOp(), "*") == 0 || strcmp(tk_next->getOp(), "/") == 0 ))
	{
		//after *(/) there are two possibilities: 1.()    2.identifier and integer so safe;
		
		expression* exp2 = nullptr;
		// case: ...*(...)
		Token* tk_check_parathesis = lexer->getToken();
		if (tk_check_parathesis->getType() == Operator && strcmp(tk_check_parathesis->getOp(), "(") == 0)
		{
			exp2 = parseExpressionInBracket(lexer);
		}
		else
		{
			lexer->rollBack();
			exp2 = parseExpression3(lexer);
		}
		exp1 = new expressionBinary(exp1, exp2, tk_next->getOp());

		tk_next = lexer->getToken();
	}
	if (tk_next->getType() == Operator )
	{
		if((strcmp(tk_next->getOp(), "+") == 0 || strcmp(tk_next->getOp(), "-") == 0 || strcmp(tk_next->getOp(), ")") == 0
			|| strcmp(tk_next->getOp(), ";") == 0 || strcmp(tk_next->getOp(), "?") == 0 || strcmp(tk_next->getOp(), ":") == 0
			|| strcmp(tk_next->getOp(), "(") == 0 || strcmp(tk_next->getOp(), ">") == 0 || strcmp(tk_next->getOp(), "<") == 0
			|| strcmp(tk_next->getOp(), "<=") == 0 || strcmp(tk_next->getOp(), ">=") == 0 || strcmp(tk_next->getOp(), "==") == 0
			|| strcmp(tk_next->getOp(), "&&") == 0 || strcmp(tk_next->getOp(), "||") == 0 || strcmp(tk_next->getOp(), "{") == 0 
			|| strcmp(tk_next->getOp(), "}") == 0 || strcmp(tk_next->getOp(), ",") == 0))
		{
			lexer->rollBack();
		}
		return exp1;
	}
	else
	{
		std::cerr << "(parseExpression2 error) Expected  + or -  or * or / but receive: "<<tk_next->getOp()<<" instead"<< std::endl;
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
	else if (tk_next->getType() == Operator && strcmp(tk_next->getOp(), "!") == 0)
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

// refer to the order of expressions!