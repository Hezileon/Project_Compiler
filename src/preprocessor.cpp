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


 /*
  *		if (compileModeOn)
		 {
			 if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			 std::cout << asm_num << " " << x << " " << y << " " << z;
			 if (anotationModeOn) std::cout << "// ...";
			 std::cout << std::endl;
		 }
  */
class output;
class assignment;
class ifStatement;
class functionCall;


extern int MEM[500];
extern int LC;

extern std::map<std::string, int> varToAddress;
extern std::vector<block*> allBlocks;
extern block* global_block;

extern int global_var_cnt;

extern bool anotationModeOn;
extern bool lineCounterModeOn;
extern bool compileModeOn;

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

block::block(Lexer* lexer,environment* myEnvironment_,bool isFuncBlock_):myEnv(myEnvironment_)
{
	allBlocks.push_back(this);

	isFuncBlock = isFuncBlock_;
	if (!isFuncBlock) { parmSize = 0; }
	if (myEnv->isGlobalEnv())
	{
		myLabel = "MY_PROGRAMME";
		rtnLabel = myLabel + "_rtn";
		skipLabel = myLabel + "_skip";
	}
	else
	{
		myLabel = Label_Allocator();
		rtnLabel = myLabel + "_rtn";
		skipLabel = myLabel + "_skip";
	}
	for (Token* tk_1 = lexer->getToken();
		!(tk_1->getType() == Operator && strcmp(tk_1->getOp(), "EOF") == 0);
		tk_1 = lexer->getToken())
	{
		// use tk_1 to get the first token and the last token;
		// we didn't give Output a special type, just use them as identifier;
		if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "Output") == 0 || strcmp(tk_1->getIdf(), "output") == 0))
		{

			expression* exp = parseExpression(lexer);
			statement* ptr = new output{ tk_1->getIdf(),exp,this };
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer))
			{
				statement* rtn = nullptr;
				if(myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond(),this);
				}else
				{
					rtn = new returnStatement(this);
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
			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement( this);
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
			block* funcBlock = new block(lexer, funcEnv,true);
			funcBlock->setParmSize(ptr->getPSize());
			myEnv->updateDest(tk_2->getIdf(), ptr->getPSize(), funcBlock);

			statement* ptr2 = ptr;
			this->pushStatement(ptr2);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement(this);
				}
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "input") == 0 || strcmp(tk_1->getIdf(), "Input") == 0))
		{
			Token* tk_2 = lexer->getToken(); // tk_2 is variable name;

			statement* ptr = new input(tk_2->getIdf(),this);
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement(this);
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
				ifB = new block(lexer, ifEnv,false);
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
					elseB = new block(lexer,elseEnv,false);
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
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement(this);
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
				whileB = new block(lexer, ifEnv,false);
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
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement(this);
				}
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "return") == 0 || strcmp(tk_1->getIdf(), "Return") == 0))
		{
			expression* exp = parseExpression(lexer);
			statement* ptr = new returnValueStatement(exp,this);
			this->pushStatement(ptr);

			tk_1 = lexer->getToken();
			eolCheck(lexer, tk_1);
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement(this);
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
					Token* RightBrancketTest = lexer->getToken();
					if(RightBrancketTest->getType()==Operator && (strcmp(RightBrancketTest->getOp(), ")") == 0))
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
					statement* ptr = new assignment{ tk_1->getIdf(),tk_2->getOp(),nullptr,this};
					this->pushStatement(ptr);
				}
				else
				{
					lexer->rollBack();
					statement* ptr = new assignment{ tk_1->getIdf(),tk_2->getOp(),parseExpression(lexer),this };
					this->pushStatement(ptr);
				}
			}
			else if (tk_2->getType() == Operator && strcmp(tk_2->getOp(), "(") == 0)
			{
				if (tk_1->getType() == Identifier && myEnv->isFuncCall(tk_1->getIdf()))
				{
					Token* RightBrancketTest = lexer->getToken();
					if (RightBrancketTest->getType() == Operator && (strcmp(RightBrancketTest->getOp(), ")") == 0))
					{
						// we leave ; untouched
						functionCall* ptr = new functionCall(tk_1->getIdf(), 0, this);
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

						functionCall* ptr_call = new functionCall(tk_1->getIdf(), para.size(), this);
						for (expression* exp : para)
						{
							ptr_call->addParms(exp);
						}
						this->pushStatement(ptr_call);
					}
				}
				else
				{
					std::cerr << "Wrong!" << std::endl;
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
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement(this);
				}
				this->pushStatement(rtn);
				break;
			}
		}
		else if (tk_1->getType() == Operator && (strcmp(tk_1->getOp(), ";") == 0))
		{
			if (quitBlockCheck(lexer)) {
				statement* rtn = nullptr;
				if (myEnv->ifIsWhileBlock())
				{
					rtn = new returnStatement(myEnv->getCond(), this);
				}
				else
				{
					rtn = new returnStatement(this);
				}
				this->pushStatement(rtn);
				break;
			}
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
	global_block = new block(lexer, curEnv,false);
	curBlock = global_block;
}

extern int MEM[500];



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

void preprocessor::jump()
{
	curBlock = jmpDest_s.top();
	curEnv = curBlock->getMyEnv();
	nextPos = 0;

	// this is wrong?
	
	int offset = curBlock->getMyEnv()->getVarCnt() + 1;
	MEM[MEM[200] + 1] = MEM[200];
	MEM[200] = MEM[200] + offset;
	//


	curEnv->setStackAddr(MEM[200]);
}

void preprocessor::jump_para(std::vector<expression*> parms)
{
	std::vector<int> p_answer;
	// use the parameters to calculate the answer and save them
	for (int i = 0; i < parms.size(); i++)
	{
		parms[i]->evaluate_compiler(0,curEnv);
		p_answer.push_back(MEM[0]);
	}

	//

	curBlock = jmpDest_s.top();
	curEnv = curBlock->getMyEnv();
	nextPos = 0;
	// 


	int offset = curBlock->getMyEnv()->getVarCnt() + 1;
	
	MEM[MEM[200] + 1] = MEM[200];
	MEM[200] = MEM[200] + offset;
	//

	curEnv->setStackAddr(MEM[200]);

	for (int i = 0; i < parms.size(); i++)
	{
		char* var_c = curEnv->getVarTbl(i);// bind value to variables -> get the name of the variables;

		NameToAddress(var_c, curEnv, 1);
		MEM[MEM[1]] = p_answer[i]; // notice that the expression should be calculated before entering the block,
		// while the variable should be assigned after entering the block;
	}
}

void returnStatement::execute()
{
	if (isWhileVersion)
	{
		if(compileModeOn)
		{
			cond->evaluate_compiler(0,myBlock->getMyEnv());

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 20 << " " << 0 << " " << " " << " " << "goto " << myBlock->getLabel_start()<<"_whileSpecial";
			if (anotationModeOn) std::cout << "// 条件跳转至 whileBlock开头";
			std::cout << std::endl;

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 30 << " " << " " << " " << " " << " " << "goto " << myBlock->getLabel_rtn();
			if (anotationModeOn) std::cout << "// 强制跳转回 whileBlock的返回处";
			std::cout << std::endl;
		}
		else
		{
			cond->evaluate_compiler(0,myBlock->getMyEnv());
			if (MEM[0])
			{
				processor.Return_While();
			}
			else
			{
				processor.Return();
			}
		}
		
	}
	else
	{
		
		if(compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 30 << " " << " " << " " << " " << " " << "goto " << myBlock->getLabel_rtn();
			if (anotationModeOn) std::cout << "// 强制跳转回 if/else Block的返回处";
			std::cout << std::endl;
		}
		else
		{
			processor.Return();
		}
	}
}

void returnValueStatement::execute()
{
	if (compileModeOn)
	{
		exp->evaluate_compiler(0,myBlock->getMyEnv());

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 3 << " " << 0 << " " << " " << " " << 100;
		if (anotationModeOn) std::cout << "// 拷贝 MEM[100] = MEM[0]";
		std::cout << std::endl;

		MEM[100] = MEM[0];

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 30 << " " << " " << " " << " " << " " << "goto "<<myBlock->getLabel_rtn();
		if (anotationModeOn) std::cout << "// 强制跳转回func返回处";
		std::cout << std::endl;

	}
	else {
		exp->evaluate_compiler(0, myBlock->getMyEnv());

		MEM[100] = MEM[0];
		processor.Return();
	}
}

void preprocessor::Return_While()
{
	
		nextPos = 0;
	
}

void preprocessor::Return()
{
	//TODO-2 add rtnLabel to all blocks; // QUESTION is that needed?
	int offset = curBlock->getMyEnv()->getVarCnt() + 1;
	MEM[200] = MEM[MEM[200] - offset + 1];

	if (rtnPos_s.empty())
	{
		// TEST MODE
		std::cout << "end of programme;" << std::endl;
		statusOperating = false;
	}
	else
	{
		curBlock = rtnDest_s.top();
		curEnv = curBlock->getMyEnv();
		nextPos = rtnPos_s.top();

		jmpDest_s.pop();
		rtnDest_s.pop();
		rtnPos_s.pop();
	}

}


void block::generateCode()
{
	if (lineCounterModeOn) std::cout << LC << ": "; LC++;
	std::cout << myLabel;
	if (anotationModeOn) std::cout << "// Label:";
	std::cout << std::endl;

	// stack push operation
	int offset = this->getMyEnv()->getVarCnt() + 1;

	if (lineCounterModeOn) std::cout << LC << ": "; LC++;
	std::cout << 0 << " " << offset << " " << " " << " " << 1;
	if (anotationModeOn) std::cout << "// MEM[1] 保存 stack offset";
	std::cout << std::endl;

	if (lineCounterModeOn) std::cout << LC << ": "; LC++;
	std::cout << 1 << " " << 200 << " " << 1 << " " << 200;
	if (anotationModeOn) std::cout << "// 数组元素存储，语义：MEM[1+MEM[200]] = MEM[200] ";
	std::cout << std::endl;

	// MEM[MEM[200] + 1] = MEM[200];
	

	if (lineCounterModeOn) std::cout << LC << ": "; LC++;
	std::cout << 4 << " " << 200 << " " << 1 << " " << 200;
	if (anotationModeOn) std::cout << "// MEM[200] = MEM[200] + MEM[1],stack shift";
	std::cout << std::endl;

	// MEM[200] = MEM[200] + offset;
	// stack push end;
	if(myEnv->ifIsWhileBlock())
	{
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << myLabel << "_whileSpecial";
		if (anotationModeOn) std::cout << "// Label whileSpecial";
		std::cout << std::endl;
	}
	

	// 接受传递参数
	if(isFuncBlock)
	{
		for(int i = 0; i < parmSize ; i++)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 1 << " " << 99-i << " " << -i << " " << 200;
			if (anotationModeOn) std::cout << "// 数组元素存储，语义：MEM[MEM[200]+-i] = MEM[200] ";
			std::cout << std::endl;

			// MEM[MEM[200] - i] = MEM[99 - i];
		}
	}

	for(statement* s : stm)
	{
		s->execute();
	}
}

void preprocessor::execute()
{
	// MAIN_PROCESS_LOOP
	statusOperating = true;
	MEM[200] = 200;
	if(compileModeOn)
	{
		for (block* p : allBlocks)
		{
		if (p == MainBlock || p == global_block)
		{
			continue;
		}
		else
		{
			p->generateCode();// if we ensure that MAIN function's generateCode is not called;
		}// TODO-2
		}
	}
	
	
	if (MainBlock != nullptr)
	{
		main_block_init();
	}

	curBlock->setPos(nextPos++);
	statement* ptr = curBlock->getNextStatement();
	while (ptr != nullptr && statusOperating)
	{
		if (showStatementModeOn) { ptr->show_yourself(); }
		ptr->execute();
		curBlock->setPos(nextPos++); ptr = curBlock->getNextStatement();
	}
}

void preprocessor::main_block_init()
{
	if(compileModeOn)
	{
		curBlock = MainBlock;
		curEnv = curBlock->getMyEnv();
		nextPos = 0;

		int offset = curBlock->getMyEnv()->getVarCnt() + 1;

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << "LABEL_MAIN_BLOCK";
		if (anotationModeOn) std::cout;
		std::cout << std::endl;

		// MEM[200] = 200;
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 0 << " " << 200 << " " << " " << " " << 200;
		if (anotationModeOn) std::cout << "// MEM[200] = 200";
		std::cout << std::endl;


		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 1 << " " << 200 << " " << 1 << " " << 200;
		if (anotationModeOn) std::cout << "// 现栈顶的存档！保存到现栈顶+1处，MEM[y+MEM[z]] = MEM[x]";
		std::cout << std::endl;
		

		//MEM[1] = offset;
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 0 << " " << offset << " " << " " << " " << 1;
		if (anotationModeOn) std::cout << "// MEM[1] 保存 栈的offset";
		std::cout << std::endl;

		


		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 4 << " " << 200 << " " << 1 << " " << 200;
		if (anotationModeOn) std::cout << "// MEM[z] = MEM[200]+MEM[1],stack shift";
		std::cout << std::endl;

		statement* last_statement_1 = MainBlock->getLastStatememt();
		if (last_statement_1->isReturnTypeStatement()) { MainBlock->deleteLastStatememt(); }
		statement* last_statement_2 = MainBlock->getLastStatememt();
		if (last_statement_2->isReturnTypeStatement()) { MainBlock->deleteLastStatememt(); }
		//MEM[200] = MEM[200+MEM[1]];
	}
	else
	{
		curBlock = MainBlock;
		curEnv = curBlock->getMyEnv();
		nextPos = 0;

		int offset = curBlock->getMyEnv()->getVarCnt() + 1;
		MEM[1] = offset;
		// mark TODO
		MEM[200] = MEM[200]+offset;
	}
}