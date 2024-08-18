#include "../inc/Statement.h"
#include "../inc/Expression.h"
#include "../inc/compiler.h"
extern int MEM[500];
extern int LC;

extern std::map<std::string, int> varToAddress;
extern int global_var_cnt;

extern bool anotationModeOn;
extern bool lineCounterModeOn;
extern bool compileModeOn;

std::string Label_Allocator()
{
	static int LabelCnt = 0;
	std::string label = "Label_";
	char tail[4];
	itoa(LabelCnt++, tail, 10);
	std::string t = tail;
	label = label + t;
	return  label;
}

void _nameToAddress(environment* env, char* c, int pos)
{
	if (env->isGlobalEnv())
	{
		int offset = env->readAddrOffset(c);
		if(compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 0 << " " << 101+offset << " " << " " << " " << pos;
			if (anotationModeOn) std::cout << "// 常量存储，语义：MEM[z] = x, 储存解析出的addr到MEM[pos]  ";
			std::cout << std::endl;
		}
	}
	else
	{
		if (env->KeyValueFound(c))
		{
			if (compileModeOn)
			{
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 0 << " " << env->readAddrOffset(c) << " " << " " << " " << pos+1;
				if (anotationModeOn) std::cout << "// 储存OFFSET到MEM[pos+1] ";
				std::cout << std::endl;

				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 5 << " " << 199  << " " << pos+1 << " " << pos;
				if (anotationModeOn) std::cout << "// 减法实现地址计算，语义：MEM[z] = MEM[200] - offset, 储存解析出的addr到MEM[pos]  ";
				std::cout << std::endl;

			}
			
		}
		else
		{
			int varCnt = env->getVarCnt();

			int offset = varCnt + 1;

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 0 << " " << offset << " " << " " << " " << 198;
			if (anotationModeOn) std::cout << "// 储存offset到MEM[198] ";
			std::cout << std::endl;

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 5 << " " << 199 << " " << 198 << " " << 199;
			if (anotationModeOn) std::cout << "// MEM[199]寻变量指针移动";
			std::cout << std::endl;

			_nameToAddress(env->getFEnv(), c, pos);
		}
	}
}

inline void NameToAddress(char* c, environment* var_env, int pos)
{
	if (lineCounterModeOn) std::cout << LC << ": "; LC++;
	std::cout << 3 << " " << 200 << " " << " " << " " << 199;
	if (anotationModeOn) std::cout << "// 储存栈顶到MEM[199] ";
	std::cout << std::endl;


	_nameToAddress(var_env,c, pos);
}




assignment::assignment(char* _idf, char* _op, expression* _exp,block* myBlock_)
	: idf(_idf),op(_op),exp(_exp),myBlock(myBlock_)
{}

void assignment::execute()
{
	if(exp == nullptr)
	{
		if (compileModeOn)
		{
			
			NameToAddress(idf, myBlock->getMyEnv(),1);
			// wrong!
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 1 << " " << 100 << " " << 0 << " " << 1;
			if (anotationModeOn) std::cout << "// 数组存储运算符,语义：MEM[0+MEM[1]] = MEM[100],此处用法为：函数调用后对变量赋值  ";
			std::cout << std::endl;
		}
		else
		{
			NameToAddress(idf, myBlock->getMyEnv(), 1);
			MEM[MEM[1]] = MEM[100];
		}
	}
	else
	{
		exp->evaluate_compiler(0, myBlock->getMyEnv());

		if (compileModeOn)
		{
			NameToAddress(idf, myBlock->getMyEnv(),1);

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 1 << " " << 0 << " " << 0 << " " << 1;
			if (anotationModeOn) std::cout << "// 数组存储运算符,语义：MEM[0+MEM[1]] = MEM[0],此处用法为：表达式计算后对变量赋值  ";
			std::cout << std::endl;
			
		}
		else
		{
			NameToAddress(idf, myBlock->getMyEnv(), 1);
			MEM[MEM[1]] = MEM[0];
		}
	}
	
	//return assignValueToIdf(idf,exp->evaluate());
}

output::output(char* _op, expression* exp_,block* myBlock_)
	:op(_op), exp(exp_),myBlock(myBlock_)
{}

extern bool output_banned;
void output::execute()
{
	exp->evaluate_compiler(0, myBlock->getMyEnv());
	if(compileModeOn)
	{ // TODO-4 目前output只能处理 expression作为输出对象，可否尝试拓展至函数返回值？
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 50 << " " << 0 << " " << " " << " " << " ";
		if (anotationModeOn) std::cout << "// output MEM[0] ";
		std::cout << std::endl;
	}
	else
	{
		if(!output_banned)
		{
			std::cout << MEM[0] << std::endl; 
		}
	}
	
}


seqStatement::seqStatement(Lexer* lexer)
{
	
}


void seqStatement::execute()
{
	for(statement* p:seq)
	{
		p->execute();
	}
	
}
seqStatement::~seqStatement()
{
	for (statement* p : seq)
	{
		delete p;
	}
}

/*
 *		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << asm_num << " " << x << " " << y << " " << z;
			if (anotationModeOn) std::cout << "// ...";
			std::cout << std::endl;
		}
 */


varDecl::varDecl(char* c, environment* myEnv_):var(c),myEnv(myEnv_)
{
	myEnv->createVar(var);
	std::string name = var;
}

void varDecl::execute()
{

}

void input::execute()
{
	if (compileModeOn)
	{
		NameToAddress(varName, myBlock->getMyEnv(), 1);

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 60 << " " << 2 << " " << " " << " " << " ";
		if (anotationModeOn) std::cout << "// 读取一个整数值，并存放到MEM[x]   ";
		std::cout << std::endl;

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 1 << " " << 2 << " " << 0 << " " << 1;
		if (anotationModeOn) std::cout << "// 数组存储，MEM[MEM[1]] = MEM[2]  ";
		std::cout << std::endl;
	}
	else
	{
		std::cout << "input: ";
		NameToAddress(varName, myBlock->getMyEnv(), 1);
		std::cin >> MEM[MEM[1]];
	}
}

void functionCall::execute()
{
	block* dest = myBlock->getMyEnv()->findFuncDest(funcName, pSize);
	if(compileModeOn)
	{
		// use the parameters to calculate the answer and save them
		for (int i = 0; i < parms.size(); i++)
		{
			parms[i]->evaluate_compiler(0,myBlock->getMyEnv());

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 3 << " " << 0 << " " << " " << " " << 99-i;
			if (anotationModeOn) std::cout << "//MEM[z] = MEM[x] 赋值，存放传递的参数";
			std::cout << std::endl;

			// MEM[99 - i] = MEM[0];
		}
		//TODO-3 CHECK generateCode是否有接受参数的操作;

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 30 << " " << " " << " " << " " << " " << "goto " <<dest->getLabel_start();
		if (anotationModeOn) std::cout << "// goto func Block";
		std::cout << std::endl;

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << dest->getLabel_rtn();
		if (anotationModeOn) std::cout << "// return Label";
		std::cout << std::endl;

		// 弹栈操作here：// stack pop operation:

		int offset = dest->getMyEnv()->getVarCnt() + 1;

		//MEM[1] = -offset + 1;
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 0 << " " << -offset << " " << " " << " " << 1;
		if (anotationModeOn) std::cout << "// MEM[1] 保存 栈的offset";
		std::cout << std::endl;

		//MEM[2] = MEM[200]+MEM[1];
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 4 << " " << 200 << " " << 1 << " " << 2;
		if (anotationModeOn) std::cout << "// MEM[2] = MEM[200] + MEM[1]";
		std::cout << std::endl;


		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 2 << " " << 1 << " " << 2 << " " << 200;
		if (anotationModeOn) std::cout << "// MEM[z] = MEM[1+MEM[2]],stack shift";
		std::cout << std::endl;

		//MEM[200] = MEM[MEM[200] - offset + 1];
		// stack pop end; we didn't go into the block, so we don't actually did stack operation;

		// 注意到 functionCall其实没有 “不执行”的这种可能，所以不需要skip label；
	}
	else
	{
		if (dest != nullptr)
		{
			processor.push_JmpDest(dest);
			processor.push_RtnDest(myBlock);
			processor.push_RtnPos(myBlock->readPos(this));

			processor.jump_para(parms);
		}
	}
}


void ifStatement::execute()
{

	block* dest = nullptr;
	//TODO:
	if(compileModeOn)
	{
		if(elseBlock != nullptr)
		{
			cond->evaluate_compiler(0, myBlock->getMyEnv());

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 20 << " " << 0 << " " << " " << " " << "goto " << ifBlock->getLabel_start();
			if (anotationModeOn) std::cout << "// 条件进入if语句";
			std::cout << std::endl;
			// CHECk
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 30 << " " << " " << " " << " " << " " << "goto " << elseBlock->getLabel_start();
			if (anotationModeOn) std::cout << "// 强制进入else语句";
			std::cout << std::endl;

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << ifBlock->getLabel_rtn();
			if (anotationModeOn) std::cout << "// return label of IF";
			std::cout << std::endl;

			// IF's stack operation here // TODO-1
			{
				// stack pop operation:
				int offset = ifBlock->getMyEnv()->getVarCnt() + 1;

				MEM[1] = -offset + 1;
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 0 << " " << -offset << " " << " " << " " << 1;
				if (anotationModeOn) std::cout << "// MEM[1] 保存 栈的offset";
				std::cout << std::endl;


				//MEM[2] = MEM[200]+MEM[1];
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 4 << " " << 200 << " " << 1 << " " << 2;
				if (anotationModeOn) std::cout << "// MEM[2] = MEM[200] + MEM[1]";
				std::cout << std::endl;


				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 2 << " " << 1 << " " << 2 << " " << 200;
				if (anotationModeOn) std::cout << "// MEM[z] = MEM[1+MEM[2]],stack shift";
				std::cout << std::endl;

				// MEM[200] = MEM[MEM[200] - offset + 1];
				// stack pop end;
			}

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 30 << " " << " " << " " << " " << " " << "goto " << ifBlock->getLabel_skip();
			if (anotationModeOn) std::cout << "// 强制 跳转到skip label of IF";
			std::cout << std::endl;

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << elseBlock->getLabel_rtn();
			if (anotationModeOn) std::cout << "// return label of ELSE";
			std::cout << std::endl;

			// ELSE's stack operation here // TODO-1
			{
				// stack pop operation:
				int offset = elseBlock->getMyEnv()->getVarCnt() + 1;

				// MEM[1] = -offset + 1;
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 0 << " " << -offset << " " << " " << " " << 1;
				if (anotationModeOn) std::cout << "// MEM[1] 保存 栈的offset";
				std::cout << std::endl;


				//MEM[2] = MEM[200]+MEM[1];
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 4 << " " << 200 << " " << 1 << " " << 2;
				if (anotationModeOn) std::cout << "// MEM[2] = MEM[200] + MEM[1]";
				std::cout << std::endl;


				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 2 << " " << 1 << " " << 2 << " " << 200;
				if (anotationModeOn) std::cout << "// MEM[z] = MEM[1+MEM[2]],stack shift";
				std::cout << std::endl;

				// MEM[200] = MEM[MEM[200] - offset + 1];
				// stack pop end;
			}

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 30 << " " << " " << " " << " " << " " << "goto " << elseBlock->getLabel_skip();
			if (anotationModeOn) std::cout << "// 强制 跳转到skip label of ELSE";
			std::cout << std::endl;

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << ifBlock->getLabel_skip();
			if (anotationModeOn) std::cout << "// skip label of IF";
			std::cout << std::endl;


			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << elseBlock->getLabel_skip();
			if (anotationModeOn) std::cout << "// return label of ELSE";
			std::cout << std::endl;

		}
		else
		{
			cond->evaluate_compiler(0, myBlock->getMyEnv());

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 20 << " " << 0 << " " << " " << " " << "goto " << ifBlock->getLabel_start();
			if (anotationModeOn) std::cout << "// 条件进入if语句";
			std::cout << std::endl;
			// CHECk
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 30 << " " << " " << " " << " " << " " << "goto " << ifBlock->getLabel_skip();
			if (anotationModeOn) std::cout << "// 强制进入if语句之后";
			std::cout << std::endl;

			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << ifBlock->getLabel_rtn();
			if (anotationModeOn) std::cout << "// return label of IF";
			std::cout << std::endl;

			// IF's stack operation here // TODO-1
			{
				// stack pop operation:
				int offset = ifBlock->getMyEnv()->getVarCnt() + 1;

				// MEM[1] = -offset + 1;
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 0 << " " << -offset << " " << " " << " " << 1;
				if (anotationModeOn) std::cout << "// MEM[1] 保存 栈的offset";
				std::cout << std::endl;


				//MEM[2] = MEM[200]+MEM[1];
				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 4 << " " << 200 << " " << 1 << " " << 2;
				if (anotationModeOn) std::cout << "// MEM[2] = MEM[200] + MEM[1]";
				std::cout << std::endl;


				if (lineCounterModeOn) std::cout << LC << ": "; LC++;
				std::cout << 2 << " " << 1 << " " << 2 << " " << 200;
				if (anotationModeOn) std::cout << "// MEM[z] = MEM[1+MEM[2]],stack shift";
				std::cout << std::endl;
				// MEM[200] = MEM[MEM[200] - offset + 1];
				// stack pop end;


			}
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << ifBlock->getLabel_skip();
			if (anotationModeOn) std::cout << "// skip label of IF";
			std::cout << std::endl;
		}
	}
	else
	{
		cond->evaluate_compiler(0, myBlock->getMyEnv());
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
}



void whileStatement::execute()
{
	if (compileModeOn)
	{
		cond->evaluate_compiler(0, myBlock->getMyEnv());
		// this is wrong;


		// 条件为真 进入while，条件为假，跳转skip以躲开弹栈的代码
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 20 << " " << 0 << " " << " " << " " << "goto " << whileBlock->getLabel_start();
		if (anotationModeOn) std::cout << "// 条件跳转，进入whileBlock";
		std::cout << std::endl;

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 30 << " " << " " << " " << " " << " " << "goto " << whileBlock->getLabel_skip();
		if (anotationModeOn) std::cout << "// 强制跳转，跳到skip label处";
		std::cout << std::endl;

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << whileBlock->getLabel_rtn();
		if (anotationModeOn) std::cout << "// return label";
		std::cout << std::endl;

		// 回来之后进行栈顶移动 // stack pop operation:
		int offset = whileBlock->getMyEnv()->getVarCnt() + 1;

		// MEM[1] = -offset + 1;
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 0 << " " << -offset << " " << " " << " " << 1;
		if (anotationModeOn) std::cout << "// MEM[1] 保存 栈的offset";
		std::cout << std::endl;


		//MEM[2] = MEM[200]+MEM[1];
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 4 << " " << 200 << " " << 1 << " " << 2;
		if (anotationModeOn) std::cout << "// MEM[2] = MEM[200] + MEM[1]";
		std::cout << std::endl;


		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 2 << " " << 1 << " " << 2 << " " << 200;
		if (anotationModeOn) std::cout << "// MEM[z] = MEM[1+MEM[2]],stack shift";
		std::cout << std::endl;


		// MEM[200] = MEM[MEM[200] - offset + 1];
		// stack pop end;

		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << whileBlock->getLabel_skip() ;
		if (anotationModeOn) std::cout << "// skip label";
		std::cout << std::endl;
	}
	else
	{
		cond->evaluate_compiler(0, myBlock->getMyEnv());
		if (MEM[0])
		{
			processor.push_JmpDest(whileBlock);
			processor.push_RtnDest(myBlock);
			processor.push_RtnPos(myBlock->getCurPos());

			processor.jump();
		}
	}

}

