#include "tokens.hpp"
#include <iostream>
#include <string.h>
#include <string>
#include <vector> 
using namespace std;

extern struct loc_t {int from; int to; } yyloc;

const char* getTokenType(int token_num)
{
	switch(token_num)
	{
		case INT: return "INT";
		case BYTE: return "BYTE";
    case B: return "B";
    case BOOL: return "BOOL";
    case AND: return "AND";
    case OR: return "OR";
    case NOT: return "NOT";
    case TRUE: return "TRUE";
    case FALSE: return "FALSE";
    case RETURN: return "RETURN";
    case IF: return "IF";
    case ELSE: return "ELSE";
    case WHILE: return "WHILE";
    case BREAK: return "BREAK";
    case CONTINUE: return "CONTINUE";
    case SC: return "SC";
    case LPAREN: return "LPAREN";
    case RPAREN: return "RPAREN";
    case LBRACE: return "LBRACE";
    case RBRACE: return "RBRACE";
    case ASSIGN: return "ASSIGN";
    case RELOP: return "RELOP";
    case BINOP: return "BINOP";
    case COMMENT: return "COMMENT";
		case ID: return "ID";
    case NUM: return "NUM";
    case STRING: return "STRING";
    case UNCLOSED_STRING: return "UNCLOSED_STRING";
	}
  return "Failed";
}

void checkLegalEscape(string subStr)
{
  size_t pos = subStr.find("\\");
  while (pos != std::string::npos) 
  {
    // cout << "entere while loop. pos= " << pos;
    char c = subStr[pos+1]; //maybe +2?
    if ((c != '\\') && (c != '\"') && (c != 'n') && (c != 'r') && (c != 't') && (c != '0') && (c != 'x'))
    {
      cout << "Error undefined escape sequence " << c << endl;
      exit(0);
    }
    pos = subStr.find("\\", pos+2); //start looking from the last replacement position
  }
}

string trimNullTerminator(string subStr)
{
  size_t pos = subStr.find("\\0");
  if (pos != string::npos)
  {
    subStr = subStr.substr(0, pos);
  }

  pos = subStr.find("\\x00");
  if (pos != string::npos)
  {
    subStr = subStr.substr(0, pos);
  }
  
  return subStr;
}


string matchEscapeSeq(string subStr)
{
  // updating escape sequences to relevent represantaion
  vector<string> old_patterns{"\\n", "\\r", "\\t", "\\\\","\\\""};
  vector<string> new_patterns{"\n", "\r", "\t", "\\","\""}; // need to check \t \r \0..
  vector<string>::iterator new_it = new_patterns.begin();

  for (vector<string>::iterator old_it=old_patterns.begin(); old_it!=old_patterns.end(); ++old_it) 
  {
    std::string replace_word = *old_it;
    std::string replace_by = *new_it; // replace_word.substr(1, replace_word.size());
    size_t pos = subStr.find(replace_word);
    while (pos != std::string::npos) 
    {
      // cout << "enterd while loop, replace word is: " << replace_word << ". replace by is: " << replace_by<< endl;
      subStr.replace(pos, replace_word.size(), replace_by); 
      pos = subStr.find(replace_word, pos + replace_by.size()); //start looking from the last replacement position
    }
    ++new_it;
  }
  return subStr;
}

string matchHexSeq(string subStr)
{
  // converting hex representations to hascii
  size_t old_loc = subStr.find("\\x"); // find begining of hex rep
  while (old_loc != std::string::npos)
  {
    // cout << "entered loop. old loc = " << old_loc << endl;
    string hex_str = subStr.substr(old_loc+2, 2); // find the hex value
    char c0 = hex_str[0];
    char c1 = hex_str[1];
    unsigned long ascii_code = 0;
    bool legal_hex = 0;
    if((isdigit(c0)) || (isalpha(c0) && (toupper(c0) >= 'A' && toupper(c0) <= 'F')))
    {
      if((isdigit(c1)) || (isalpha(c1) && (toupper(c1) >= 'A' && toupper(c1) <= 'F')))
      {
        ascii_code = stoul(hex_str, nullptr, 16); // convert from hex to ascii code
        if (ascii_code <= 127)
        {
          legal_hex = 1;
        }
      }
    }

    if (not legal_hex)
    {
      cout << "Error undefined escape sequence x" << hex_str << endl;
      exit(0);
    }

    string ascii_str(1, ascii_code); // convert from ascii code to str
    subStr.replace(old_loc, 4, ascii_str); 
    old_loc = subStr.find("\\x");
  }
  return subStr;
}


void handleString(const char* token_type)
{
  if (strlen(yytext)==2) //empty string ""
  {
    std::cout << yylineno << " " << token_type << std::endl;
  }
  else
  {
    std::string subStr = strndup(yytext + 1, strlen(yytext)-2); //remove the ""
    checkLegalEscape(subStr);
    subStr = trimNullTerminator(subStr);
    subStr = matchEscapeSeq(subStr);
    subStr = matchHexSeq(subStr);

    std::cout << yylineno << " " << token_type << " " << subStr << std::endl;
  }
}

void printRequestedLine(int token_num)
{
  const char* token_type = getTokenType(token_num);
  switch(token_num)
	{
		case COMMENT: 
      std::cout << yylineno << " " << token_type << " //" << std::endl;
      break;

    case STRING:
      handleString(token_type);
      break;
    case UNCLOSED_STRING:
      cout << "Error unclosed string" << endl;
      exit(0);

    default:
      std::cout << yylineno << " " << token_type << " " << yytext << std::endl;
      break;
  }
}

int main()
{
	int token;
	while(token = yylex()) 
	{
    if (token == -1)
    {
      std::cout << "Error " << yytext << std::endl;
      exit(0);
    }
		printRequestedLine(token);
	}
	return 0;
}