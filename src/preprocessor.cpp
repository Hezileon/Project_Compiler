#include "../inc/preprocessor.h"
#include "../inc/Statement.h"
/*
 *1. generate blocks and else
 *2. get the statics
 *3. generate code
 *
 *
 *
 *  (tk_1->getType() == Operator && strcmp(tk_1->getOp(), "{") == 0)
 */

class output;
class assignment;
class ifStatement;
class functionCall;

inline void eolCheck(Lexer* lexer,Token* tk_1) 
{
	if (tk_1->getType() == Operator && strcmp(tk_1->getOp(), ";") == 0)
	{
	}
	else
	{
		std::cerr << "(Generate Output Statement ERROR) Expected ; but receive " << tk_1->getOp() << " instead" << std::endl;
	}
}
inline bool quitBlockCheck(Lexer* lexer)
{
	Token* tk = lexer->getToken();
	if(tk->getType() == Operator && strcmp(tk->getOp(), "}") == 0 )
	{
		
		return true;
	}
	else
	{
		lexer->rollBack();
		return false;
	}
}

int TagAllocator()
{
	static int tagCnt = 0;
	return tagCnt++;
}

block::block(Lexer* lexer,environment* myEnvironment)
{
	for (Token* tk_1 = lexer->getToken();
		!(tk_1->getType() == Operator && strcmp(tk_1->getOp(), "EOF") == 0);
		tk_1 = lexer->getToken())
	{
		// use tk_1 to get the first token and the last token;
		// we didn't give Output a special type, just use them as identifier;
		if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "Output") == 0 || strcmp(tk_1->getIdf(), "output") == 0))
		{
			Token* tk_2 = lexer->getToken();
			statement* ptr = new output{ tk_1->getIdf(),tk_2->getIdf() };
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer))
			{
				statement* rtn = new returnStatement();
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "VAR") == 0 || strcmp(tk_1->getIdf(), "Var") == 0))
		{
			Token* tk_2 = lexer->getToken();
			statement* ptr = new varDecl{ tk_2->getIdf() };
			this->pushStatement(ptr);
			myEnvironment->createVar(tk_2->getIdf());

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = new returnStatement();
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "Def") == 0 || strcmp(tk_1->getIdf(), "DEF") == 0))
		{
			Token* tk_2 = lexer->getToken(); // tk_2 is function_name;
			functionDef* ptr = new functionDef(tk_2->getIdf());

			Token* tk_3 = lexer->getToken();
			if (tk_3->getType() == Operator && strcmp(tk_3->getOp(), "(") == 0) {}
			else { std::cout << "Expected '(' right after function name but receive something else: ..." << std::endl; }
			tk_3 = lexer->getToken();Token* tk_4 = lexer->getToken();
			environment* funcEnv = new environment(myEnvironment);
			while (tk_3->getType() != Operator)
			{
				ptr->createPara(tk_4->getIdf());
				funcEnv->createVar(tk_4->getIdf());
				lexer->getToken(); // take in ",";
				tk_3 = lexer->getToken(); tk_4 = lexer->getToken();
			}

			
			myEnvironment->createFunc(tk_2->getIdf(), ptr->getPSize());
			// def func(){} therefore tk_4 should be {

			block* funcBlock = new block(lexer, funcEnv);
			myEnvironment->updateDest(tk_2->getIdf(), ptr->getPSize(), funcBlock);

			statement* ptr2 = ptr;
			this->pushStatement(ptr2);
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "input") == 0 || strcmp(tk_1->getIdf(), "Input") == 0))
		{
			Token* tk_2 = lexer->getToken(); // tk_2 is variable name;

			statement* ptr = new input(tk_2->getIdf());
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = new returnStatement();
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "if") == 0 || strcmp(tk_1->getIdf(), "If") == 0))
		{
			expression* cond = parseExpression(lexer);
			
			block* ifB = nullptr; block* elseB = nullptr;
			tk_1 = lexer->getToken();
			if (tk_1->getType() == Operator && strcmp(tk_1->getOp(), "{") == 0)
			{
				environment* ifEnv = new environment(myEnvironment);
				ifB = new block(lexer, ifEnv);
			}
			else
			{
				std::cout << "Expected { but receive something else;" << std::endl;
			}
			tk_1 = lexer->getToken();
			if (tk_1->getType() == Identifier && strcmp(tk_1->getIdf(), "else") == 0)
			{
				
				tk_1 = lexer->getToken();
				if (tk_1->getType() == Operator && strcmp(tk_1->getOp(), "{") == 0)
				{
					environment* elseEnv = new environment(myEnvironment);
					elseB = new block(lexer,elseEnv);
				}
				else
				{
					std::cout << "Expected { but receive something else;" << std::endl;
				}
			}
			else { lexer->rollBack(); }
			
			statement* ptr = new ifStatement(cond, this,ifB,elseB);
			this->pushStatement(ptr); 
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "while") == 0 || strcmp(tk_1->getIdf(), "While") == 0))
		{
			expression* cond = parseExpression(lexer);

			block* whileB = nullptr; 
			tk_1 = lexer->getToken();
			if (tk_1->getType() == Operator && strcmp(tk_1->getOp(), "{") == 0)
			{
				environment* ifEnv = new environment(myEnvironment);
				whileB = new block(lexer, ifEnv);
			}
			else
			{
				std::cout << "Expected { but receive something else;" << std::endl;
			}
			statement* ptr = new whileStatement(cond,this ,whileB);
			this->pushStatement(ptr);
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "return") == 0 || strcmp(tk_1->getIdf(), "Return") == 0))
		{

		}
		else if (tk_1->getType() == Identifier)
		{
			Token* tk_2 = lexer->getToken();
			if (tk_2->getType() == Operator && strcmp(tk_2->getOp(), "=") == 0)
			{
				statement* ptr = new assignment{ tk_1->getIdf(),tk_2->getOp(),parseExpression(lexer) };
				this->pushStatement(ptr);

				{
					tk_1 = lexer->getToken();
					if (tk_1->getType() == Operator && strcmp(tk_1->getOp(), ";") == 0)
					{
					}
					else
					{
						std::cerr << "(Generate Output Statement ERROR) Expected ; but receive " << tk_1->getOp() << " instead" << std::endl;
					}
				}
			}
			else
			{
				std::cerr << "(Generate Assignment Statement ERROR) Expected = but receive ";
				if (tk_2->getType() == Operator) { std::cout << tk_2->getOp(); }
				if (tk_2->getType() == Integer) { std::cout << tk_2->getInt(); }
				if (tk_2->getType() == Identifier) { std::cout << tk_2->getIdf(); }
				std::cout << " instead" << std::endl;
			}
			if (quitBlockCheck(lexer)) {
				statement* rtn = new returnStatement();
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Operator && (strcmp(tk_1->getOp(), ";") == 0))
		{
			if (quitBlockCheck(lexer)) { break; }
		}
		else if (tk_1->getType() == Operator && (strcmp(tk_1->getOp(), "{") == 0))
		{
			// i don't quit feel like supporting this feature;
		}
	}
}

preprocessor::preprocessor(Lexer* lexer)
{
	// initialization:
	curEnv = new environment( nullptr); // a.k.a global environment
	curBlock = new block(lexer, curEnv);
}

extern int MEM[500];
