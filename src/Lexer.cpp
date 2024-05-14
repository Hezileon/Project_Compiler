#include "../inc/Lexer.h"

#include <string>
#include <cctype>
Lexer::Lexer()
{
	reading_to = 0;

	bool if_is_new_object = true;
	Type currentType;
	int ch = 0;
	char temp_ch[50];
	int counter = 0;
	while((ch=getchar())!=26)
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
			else if(ch == 42 || ch == 43 || ch == 45 || ch == 47){
				if_is_new_object = false;
				currentType = Operator;
				// new a Op into the vector array and reset if_is & currentType;

				Token123* ptr;
				ptr = new Op(ch);
				Tokens.push_back(ptr);

				if_is_new_object = true;
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
				if(ch == 9 || ch == 10 || ch == 32)
				{
					// remember to put '\0' to temp_ch[counter];
					temp_ch[counter] = '\0';

					Token123* ptr = new Idf(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = true;
					counter = 0;

					// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 

				}
				else if(ch == 42 || ch == 43 || ch == 45 || ch == 47)
				{
					temp_ch[counter] = '\0';

					Token123* ptr = new Idf(temp_ch);
					Tokens.push_back(ptr);

					if_is_new_object = false;
					currentType = Operator;
					// new a Op into the vector array and reset if_is & currentType;

					Token123* ptr1;
					ptr1 = new Op(ch);
					Tokens.push_back(ptr1);

					if_is_new_object = true;
					counter = 0;
				}
				else
				{
					temp_ch[counter++] = ch;
				}
			}
			else if(currentType == Integer)
			{
				if (ch == 9 || ch == 10 || ch == 32)
				{
					// remember to put '\0' to temp_ch[counter];
					temp_ch[counter] = '\0';

					Token123* ptr;
					ptr = new Int(std::stoi(temp_ch));
					Tokens.push_back(ptr);

					// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 

					if_is_new_object = true;
					counter = 0;
				}
				else if (ch == 42 || ch == 43 || ch == 45 || ch == 47)
				{
					// remember to put '\0' to temp_ch[counter];
					temp_ch[counter] = '\0';

					Token123* ptr;
					ptr = new Int(std::stoi(temp_ch));
					Tokens.push_back(ptr);

					// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 

					if_is_new_object = false;
					currentType = Operator;
					// new a Op into the vector array and reset if_is & currentType;

					Token123* ptr1;
					ptr1 = new Op(ch);
					Tokens.push_back(ptr1);
					if_is_new_object = true;
					counter = 0;

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
		}
	}


	if(if_is_new_object == false)
	{
		if (currentType == Identifier)
		{
				temp_ch[counter] = '\0';

				Token123* ptr = new Idf(temp_ch);
				Tokens.push_back(ptr);

				if_is_new_object = true;
				counter = 0;
		}
		else if (currentType == Integer)
		{
			
				temp_ch[counter] = '\0';

				Token123* ptr;
				ptr = new Int(std::stoi(temp_ch));
				Tokens.push_back(ptr);

				// new a idf and reset if_is to correct status(do not set to false so that no ch is being left) and current type and 

				if_is_new_object = true;
				counter = 0;
		}
	}
	
}

Token123* Lexer::getToken()
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
	for(Token123* ptr: Tokens)
	{
		delete ptr;
	}
}

