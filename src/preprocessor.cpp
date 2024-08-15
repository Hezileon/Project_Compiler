#include "../inc/preprocessor.h"
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


inline void eolCheck(Lexer* lexer,Token* tk_2) 
{
	if (tk_2->getType() == Operator && strcmp(tk_2->getOp(), ";") == 0)
	{
	}
	else
	{
		std::cerr << "(Generate Output Statement ERROR) Expected ; but receive "  ;
		if (tk_2->getType() == Operator) { std::cout << tk_2->getOp(); }
		if (tk_2->getType() == Integer) { std::cout << tk_2->getInt(); }
		if (tk_2->getType() == Identifier) { std::cout << tk_2->getIdf(); }
		std::cout << " instead" << std::endl;
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

/*
 * Norm: check the ; and the end of block;
 *			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
				this->pushStatement(rtn);
				break;
			}
 *
 */

block::block(Lexer* lexer,environment* myEnvironment_):myEnv(myEnvironment_)
{
	for (Token* tk_1 = lexer->getToken();
		!(tk_1->getType() == Operator && strcmp(tk_1->getOp(), "EOF") == 0);
		tk_1 = lexer->getToken())
	{
		// use tk_1 to get the first token and the last token;
		// we didn't give Output a special type, just use them as identifier;
		if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "Output") == 0 || strcmp(tk_1->getIdf(), "output") == 0))
		{
			expression* exp = parseExpression(lexer);
			statement* ptr = new output{ tk_1->getIdf(),exp };
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer))
			{
				statement* rtn = nullptr;
				if(myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}else
				{
					rtn = new returnStatement();
				}
				this->pushStatement(rtn);
				
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "VAR") == 0 || strcmp(tk_1->getIdf(), "Var") == 0))
		{
			Token* tk_2 = lexer->getToken();
			statement* ptr = new varDecl{ tk_2->getIdf(),myEnv };
			this->pushStatement(ptr);
			myEnv->createVar(tk_2->getIdf());
			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "def") == 0 || strcmp(tk_1->getIdf(), "DEF") == 0 || strcmp(tk_1->getIdf(), "Def") == 0))
		{
			
			Token* tk_2 = lexer->getToken(); // tk_2 is function_name;
			functionDef* ptr = new functionDef(tk_2->getIdf());

			Token* tk_3 = lexer->getToken();
			if (tk_3->getType() == Operator && strcmp(tk_3->getOp(), "(") == 0) {}
			else { std::cout << "Expected '(' right after function name but receive something else: ..." << std::endl; }
			tk_3 = lexer->getToken();Token* tk_4 = lexer->getToken();
			environment* funcEnv = new environment(myEnv);
			// func( int x1, int x2, int x3){...}     or func() {}
			while (tk_3->getType() != Operator)
			{
				ptr->createPara(tk_4->getIdf());
				funcEnv->createVar(tk_4->getIdf()); // NOTE That the parameters of a function is the first batch of variables that are stored in a environment;
				lexer->getToken(); // take in ",";
				tk_3 = lexer->getToken(); tk_4 = lexer->getToken();
			}
			lexer->rollBack();
			myEnv->createFunc(tk_2->getIdf(), ptr->getPSize());
			// def func(){} therefore tk_4 should be {
			block* funcBlock = new block(lexer, funcEnv);
			myEnv->updateDest(tk_2->getIdf(), ptr->getPSize(), funcBlock);

			statement* ptr2 = ptr;
			this->pushStatement(ptr2);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "input") == 0 || strcmp(tk_1->getIdf(), "Input") == 0))
		{
			Token* tk_2 = lexer->getToken(); // tk_2 is variable name;

			statement* ptr = new input(tk_2->getIdf());
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
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
				environment* ifEnv = new environment(myEnv);
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
					environment* elseEnv = new environment(myEnv);
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

			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "while") == 0 || strcmp(tk_1->getIdf(), "While") == 0))
		{
			expression* cond = parseExpression(lexer);

			block* whileB = nullptr; 
			tk_1 = lexer->getToken();
			if (tk_1->getType() == Operator && strcmp(tk_1->getOp(), "{") == 0)
			{
				environment* ifEnv = new environment(myEnv,cond);
				whileB = new block(lexer, ifEnv);
			}
			else
			{
				std::cout << "Expected { but receive something else;" << std::endl;
			}
			statement* ptr = new whileStatement(cond,this ,whileB);
			this->pushStatement(ptr);

			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "return") == 0 || strcmp(tk_1->getIdf(), "Return") == 0))
		{
			expression* exp = parseExpression(lexer);
			statement* ptr = new returnValueStatement(exp);
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
				this->pushStatement(rtn);
				break;
			}
		}
		// assignment
		else if (tk_1->getType() == Identifier)
		{
			Token* tk_2 = lexer->getToken();
			if (tk_2->getType() == Operator && strcmp(tk_2->getOp(), "=") == 0)
			{
				Token* tk_3 = lexer->getToken();
				if(tk_3->getType()==Identifier && myEnv->isFuncCall(tk_3->getIdf()))
				{
					//func(1,2,3); func:tk_3 func();
					lexer->getToken();
					Token* leftBrancketTest = lexer->getToken();
					if(leftBrancketTest->getType()==Operator && (strcmp(leftBrancketTest->getOp(), ")") == 0))
					{
						// we leave ; untouched
						functionCall* ptr = new functionCall(tk_3->getIdf(), 0, this);
						this->pushStatement(ptr);
					}
					else
					{
						lexer->rollBack();
						std::vector<expression*> para;
						expression* exp = parseExpression(lexer); // Am i making things much complicated than i can handle?
						Token* comma = lexer->getToken(); // if it comes to the end, comma contains ")"
						while (comma->getType() == Operator && (strcmp(comma->getOp(), ",") == 0))
						{
							para.push_back(exp);
							exp = parseExpression(lexer); comma = lexer->getToken();
						}
						para.push_back(exp);
						
						functionCall* ptr_call = new functionCall(tk_3->getIdf(),para.size(),this);
						for(expression* exp : para)
						{
							ptr_call->addParms(exp);
						}
						this->pushStatement(ptr_call);
					}
					statement* ptr = new assignment{ tk_1->getIdf(),tk_2->getOp(),nullptr};
					this->pushStatement(ptr);
				}
				else
				{
					lexer->rollBack();
					statement* ptr = new assignment{ tk_1->getIdf(),tk_2->getOp(),parseExpression(lexer) };
					this->pushStatement(ptr);
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

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond());
				}
				else
				{
					rtn = new returnStatement();
				}
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
	MainBlock = nullptr;
	// initialization:
	curEnv = new environment( nullptr); // a.k.a global environment
	curBlock = new block(lexer, curEnv);
}

extern int MEM[500];

void ifStatement::execute()
{
	block* dest = nullptr;
	cond->evaluate_compiler(0);
	if (MEM[0])//??
	{
		dest = ifBlock;
	}
	else
	{
		dest = elseBlock;
	}

	if (dest != nullptr)
	{
		processor.push_JmpDest(dest);
		processor.push_RtnDest(myBlock);
		processor.push_RtnPos(myBlock->getCurPos());

		processor.jump();
	}

}

void whileStatement::execute()
{
	cond->evaluate_compiler(0);
	if (MEM[0])
	{
		processor.push_JmpDest(whileBlock);
		processor.push_RtnDest(myBlock);
		processor.push_RtnPos(myBlock->getCurPos());

		processor.jump();
	}
}


void environment::createFunc(char* c, int psize)
{
	std::string shortName = c;
	std::string func = funcName_generate(c, psize);
	if (!Dest.count(func)) { funcTbl.push_back(shortName); Dest[func] = nullptr; }
}

void environment::updateDest(char* c, int psize, block* dest)
{
	std::string shortName = c;
	std::string func = funcName_generate(c, psize); Dest[func] = dest;

	
	std::string name = "main";
	if (shortName == name)
	{
		processor.setMainBlock(dest);
	}
}
