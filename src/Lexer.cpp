#include "../inc/Lexer.h"

#include <string>
#include <cctype>

void KeyWordspecialCase(bool& if_is_new_object,Type& currentType, int& ch, char* temp_ch, int& counter, std::vector<Token*>& Tokens)
{
	char last_ch = temp_ch[counter - 1];
	bool ifIsSpecialCase = (last_ch == '=' && ch == '=') ||
		(last_ch == '>' && ch == '=') || 
		(last_ch == '<' && ch == '=') || 
		(last_ch == '!' && ch == '=') ||
		(last_ch == '-' && ch == '=') ||
		(last_ch == '*' && ch == '=') ||
		(last_ch == '/' && ch == '=') ||
		(last_ch == '+' && ch == '=') ;
	if(ifIsSpecialCase)
	{
		temp_ch[counter++] = ch;
		temp_ch[counter] = '\0';
		Token* ptr;
		ptr = new Op(temp_ch);
		Tokens.push_back(ptr);

		if_is_new_object = true;
	}
}

Lexer::Lexer()
{
	reading_to = 0;

	bool if_is_new_object = true;
	Type currentType;
	int ch = 0;
	char temp_ch[50];
	int counter = 0;
	while((ch=getchar())!=26 && ch != EOF)
	{
		if(if_is_new_object)
		{
			if(isdigit(ch))
			{
				if_is_new_object = false;
				currentType = Integer;
				memset(temp_ch, '\0', sizeof(temp_ch));
				counter = 0;

				// put first integer to temp_ch;
				temp_ch[counter++] = ch;
			}
			else if(isKeyWord(ch)){
				if_is_new_object = false;
				currentType = Operator;

				memset(temp_ch, '\0', sizeof(temp_ch));
				counter = 0;

				// put first integer to temp_ch;
				temp_ch[counter++] = ch;

				/*
				// new a Op into the vector array and reset if_is & currentType;

				Token123* ptr;
				ptr = new Op(ch);
				Tokens.push_back(ptr);

				if_is_new_object = true;
				*/
			}
			else if(isalpha(ch))
			{
				if_is_new_object = false;
				currentType = Identifier;
				memset(temp_ch, '\0', sizeof(temp_ch));
				counter = 0;

				temp_ch[counter++] = ch;
			}
		}
		else
		{
			if(currentType == Identifier)
			{
				if(isBlank(ch))//遇到空格-> stop
				{
					// remember to put '\0' to temp_ch[counter];
					temp_ch[counter] = '\0';

					Token* ptr = new Idf(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = true;
					counter = 0;

					// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 
				}
				else if(isKeyWord(ch))//遇到keyWord ->stop 并切换至下一个type!!!
				{
					temp_ch[counter] = '\0';
					Token* ptr = new Idf(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = true;
					// new a Op into the vector array and reset if_is & currentType;

					if_is_new_object = false;
					currentType = Operator;

					memset(temp_ch, '\0', sizeof(temp_ch));
					counter = 0;
					temp_ch[counter++] = ch;
					// put first integer to temp_ch;
				}
				else
				{
					temp_ch[counter++] = ch;
				}
			}
			else if(currentType == Integer)
			{
				if (isBlank(ch))//遇到空格 ->stop
				{
					// remember to put '\0' to temp_ch[counter];
					temp_ch[counter] = '\0';

					Token* ptr;
					ptr = new Int(std::stoi(temp_ch));
					Tokens.push_back(ptr);

					// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 

					if_is_new_object = true;
					counter = 0;
				}
				else if (isKeyWord(ch))//遇到keyWord -> stop 并切换至下一个type!!!
				{
					// remember to put '\0' to temp_ch[counter];
					temp_ch[counter] = '\0';

					Token* ptr;
					ptr = new Int(std::stoi(temp_ch));
					Tokens.push_back(ptr);

					if_is_new_object = true;
					// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 

					if_is_new_object = false;
					currentType = Operator;

					memset(temp_ch, '\0', sizeof(temp_ch));
					counter = 0;
					temp_ch[counter++] = ch;
					// put first integer to temp_ch;

				}
				else if(isalpha(ch))
				{
					std::cerr << "something is wrong";
				}
				else
				{
					temp_ch[counter++] = ch;
				}
			}
			else if(currentType = Operator)
			{
				if (isBlank(ch))//遇到空格 ->stop
				{
					// remember to put '\0' to temp_ch[counter];
					temp_ch[counter] = '\0';

					Token* ptr;
					ptr = new Op(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = true;
					counter = 0;
				}
				else if (isalpha(ch))//遇到字母 ->stop 并切换至下一个type
				{
					temp_ch[counter] = '\0';

					Token* ptr;
					ptr = new Op(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = true;
					// next type begin:
					if_is_new_object = false;
					currentType = Identifier;

					memset(temp_ch, '\0', sizeof(temp_ch));
					counter = 0;
					temp_ch[counter++] = ch;

				}
				else if (isdigit(ch))//遇到数字 ->stop 并切换至下一个type
				{
					temp_ch[counter] = '\0';
					Token* ptr;
					ptr = new Op(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = true;
					// next type begin:
					if_is_new_object = false;
					currentType = Integer;

					memset(temp_ch, '\0', sizeof(temp_ch));
					counter = 0;
					temp_ch[counter++] = ch;
				}
				else if (isKeyWord(ch))//继续读取...
				{
					KeyWordspecialCase(if_is_new_object, currentType, ch, temp_ch, counter, Tokens);

					temp_ch[counter] = '\0';
					Token* ptr;
					ptr = new Op(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = true;
				}
			}
		}
	}


	if(if_is_new_object == false)
	{
		if (currentType == Identifier)
		{
				temp_ch[counter] = '\0';

				Token* ptr = new Idf(temp_ch);
				Tokens.push_back(ptr);

				if_is_new_object = true;
				counter = 0;
		}
		else if (currentType == Integer)
		{
			
				temp_ch[counter] = '\0';

				Token* ptr;
				ptr = new Int(std::stoi(temp_ch));
				Tokens.push_back(ptr);

				// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 

				if_is_new_object = true;
				counter = 0;
		}
		else if(currentType == Operator)
		{
			KeyWordspecialCase(if_is_new_object, currentType, ch, temp_ch, counter, Tokens);

			temp_ch[counter] = '\0';
			Token* ptr;
			ptr = new Op(temp_ch);
			Tokens.push_back(ptr);

			if_is_new_object = true;
		}
	}
}

Token* Lexer::getToken()
{
	if (reading_to == Tokens.size()) { return nullptr; }
	else
	{
		return Tokens[reading_to++];
	}
}

void Lexer::reset_reading()
{
	reading_to = 0;
}

Lexer::~Lexer()
{
	for(Token* ptr: Tokens)
	{
		delete ptr;
	}
}

Idf::Idf(const Idf& identifier)
{
	if (this != &identifier)
	{
		delete[] str;

		str = new char[identifier.len + 1];
		len = identifier.len;
		strcpy(str, identifier.str);

	}
}

Idf& Idf::operator=(const Idf& identifier)
{
	if(this != &identifier)
	{
		delete[] str;

		str = new char[identifier.len + 1];
		len = identifier.len;
		strcpy(str, identifier.str);
	}
	return *this;
}
bool Idf::operator==(const char* ch) const
{
	int result = strcmp(str, ch);
	return (result == 0);
}

Int::Int(const Int& integer)
{
	this->I = integer.I;
}
Int& Int::operator=(const Int& integer)
{
	if(this->I != integer.I)
	{
		this->I = integer.I;
	}
	return *this;
}
Op::Op(const Op& operator_)
{
	if(&operator_ != this)
	{
		delete[] OP;

		OP = new char(strlen(operator_.OP) + 1);
		strcpy(OP, operator_.OP);
	}
}
Op& Op::operator=(const Op& operator_)
{
	if(operator_.OP != OP)
	{
		delete[] OP;

		OP = new char(strlen(operator_.OP) + 1);
		strcpy(OP, operator_.OP);
	}
	return *this;
}







/*Lexer's test and running
 *
 *	
*/