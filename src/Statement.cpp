#include "../inc/Statement.h"

#include "../inc/Expression.h"

assignment::assignment(char* _idf, char* _op, expression* _exp)
	: idf(_idf),op(_op),exp(_exp)
{}

int assignment::execute()
{
	return assignValueToIdf(idf,exp->evaluate());
}

output::output(char* _op, char* _idf)
	:op(_op), idf(_idf)
{}

int output::execute()
{
	std::cout << findIdfValue(idf) << std::endl;
	return findIdfValue(idf);
}


seqStatement::seqStatement(Lexer* lexer)
{
	for(Token* tk_1 = lexer->getToken(); 
		!(tk_1->getType() == Operator && strcmp(tk_1->getOp(), "EOF") == 0); 
		tk_1 = lexer->getToken())
	{
		// We treat "Output" as a special kind of KeyWord;
		if (tk_1->getType() == Identifier && (strcmp(tk_1->getIdf(), "Output") == 0 || strcmp(tk_1->getIdf(), "output") == 0))
		{
			
			Token* tk_2 = lexer->getToken();
			statement* ptr = new output{ tk_1->getIdf(),tk_2->getIdf() };
			seq.push_back(ptr);

			tk_1 = lexer->getToken();
			if (tk_1->getType() == Operator && strcmp(tk_1->getOp(), ";") == 0)
			{}
			else
			{
				std::cerr << "(Generate Output Statement ERROR) Expected ; but receive " << tk_1->getOp() << " instead" << std::endl;
			}
		}
		else
		{
			Token* tk_2 = lexer->getToken();
			if(tk_2->getType()== Operator && strcmp(tk_2->getOp(),"=")==0)
			{
				statement* ptr = new assignment{tk_1->getIdf(),tk_2->getOp(),parseExpression(lexer)};
				seq.push_back(ptr);

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
		}
	}
}

int seqStatement::execute()
{
	for(statement* p:seq)
	{
		p->execute();
	}
	return 0;
}


seqStatement::~seqStatement()
{
	for (statement* p : seq)
	{
		delete p;
	}
}
