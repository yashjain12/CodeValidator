/*
 * lex.cpp
 *
 * CS280 - Fall 2022
 */

#include <cctype>
#include <map>

using std::map;
using namespace std;

#include "lex.h"
//Keywords mapping
LexItem id_or_kw(const string& lexeme , int linenum)
{
	map<string,Token> kwmap = {
		{ "PROGRAM", PROGRAM},
		{ "PRINT", PRINT},
		{ "INT", INT},
		{ "END", END },
		{ "FLOAT", FLOAT},
		{ "ELSE", ELSE },
		{ "IF", IF },
		{ "THEN", THEN},
		{ "TRUE", TRUE},
		{ "FALSE", FALSE},
		{ "BOOL", BOOL }
	};
	Token tt = IDENT;
	//cout << "inside id_or_kw " << lexeme << endl;
	auto kIt = kwmap.find(lexeme);
	if( kIt != kwmap.end() )
	{
		tt = kIt->second;
		if(kIt->second == TRUE || kIt->second == FALSE)
			tt = BCONST;
	}	
	
	return LexItem(tt, lexeme, linenum);
}

map<Token,string> tokenPrint = {
		{PROGRAM, "PROGRAM"},
		{PRINT, "PRINT"},
		{INT, "INT"},
		{ END, "END" },
		{ FLOAT, "FLOAT"},
		{ BOOL, "BOOL" },
		
		{ ELSE, "ELSE" },
		{ IF, "IF" },
		
		{THEN, "THEN"},
		{ TRUE, "TRUE" },
		{ FALSE, "FALSE" },
		
		{ IDENT, "IDENT" },

		{ ICONST, "ICONST" },
		{ RCONST, "RCONST" },
		{ SCONST, "SCONST" },
		{ BCONST, "BCONST" },
		
		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ MULT, "MULT" },
		{ DIV, "DIV" },
		{ ASSOP, "ASSOP" },
		{ EQUAL, "EQUAL" },
		{ GTHAN, "GTHAN" },
		{ LTHAN, "LTHAN" },
		{ AND, "AND" },
		{ OR, "OR" },
		{ NOT, "NOT" },
            
		{ COMMA, "COMMA" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },
		
		
		{ SEMICOL, "SEMICOL" },
		
		{ ERR, "ERR" },

		{ DONE, "DONE" },
};

ostream& operator<<(ostream& out, const LexItem& tok) {
	
	Token tt = tok.GetToken();
	out << tokenPrint[ tt ];
	if( tt == IDENT || tt == ICONST || tt == SCONST || tt == RCONST || tt == BCONST || tt == ERR ) {
		out << "(" << tok.GetLexeme() << ")";
	}
	return out;
}

LexItem
getNextToken(istream& in, int& linenum)
{
	enum TokState { START, INID, INSTRING, ININT, INFLOAT, INCOMMENT } lexstate = START;
	string lexeme;
	char ch, nextch, nextchar;
	Token tt;
	bool decimal = false;
	       
	//cout << "in getNestToken" << endl;
    while(in.get(ch)) {
    	//cout << "in while " << ch << endl;
		switch( lexstate ) {
		case START:
			if( ch == '\n' )
			{
				linenum++;
				//cout << linenum << endl;
			}	
                
			if( isspace(ch) )
				continue;

			lexeme = ch;

			if( isalpha(ch) || ch == '_') {
				if(isalpha(ch)) lexeme = toupper(ch);
				lexstate = INID;
				//cout << "in ID " << endl;
			}
			
			else if( ch == '\"' ) {
				lexstate = INSTRING;
				
			}
			
			else if( isdigit(ch) ) {
				lexstate = ININT;
			}
			else if( ch == '/' ) {
				nextchar = in.peek();
				if(nextchar == '*')
				{
					lexeme += ch;
					lexstate = INCOMMENT;
					in.get(ch);
				}
					
				else
				{
					tt = DIV;
					return LexItem(tt, lexeme, linenum);
					
				}	
			}
						
			else {
				tt = ERR;
				switch( ch ) {
				case '+':
					tt = PLUS;
                    break;  
					
				case '-':
					tt = MINUS;
                    break; 
					
				case '*':
					tt = MULT;
					break;

				/*case '/':
					tt = DIV;
					break;*/
					
				case '=':
					nextchar = in.peek();
					if(nextchar == '='){
						in.get(ch);
						tt = EQUAL;
						break;
					}
					tt = ASSOP;
					break;
				
				case '(':
					tt = LPAREN;
					break;			
				case ')':
					tt = RPAREN;
					break;
				
				case ';':
					tt = SEMICOL;
					break;
					
				case ',':
					tt = COMMA;
					break;
					
				case '>':
					tt = GTHAN;
					break;
				
				case '<':
					tt = LTHAN;
					break;
				/*	
				case '.':
					nextch = in.peek();
					if(isdigit(nextch)){
						lexstate = INFLOAT;
						decimal = true;
						continue;
					}
					else {
						lexeme += nextch;
						return LexItem(ERR, lexeme, linenum);
						//cout << "Here what?" << endl;
					}*/
					
				case '&':
					nextchar = in.peek();
					if(nextchar == '&'){
						in.get(ch);
						tt = AND;
						break;
					}
					else
					{
						lexeme += nextch;
						return LexItem(ERR, lexeme, linenum);
					}
										
				case '|':
					nextchar = in.peek();
					if(nextchar == '|'){
						in.get(ch);
						tt = OR;
						break;
					}
					else
					{
						lexeme += nextch;
						return LexItem(ERR, lexeme, linenum);
					}
										
				case '!':
					tt = NOT;
					break;
													
				}
				
				return LexItem(tt, lexeme, linenum);
			}
			break;

		case INID:
			if( isalpha(ch) || isdigit(ch) || ch == '_' || ch == '@' ) {
				if(isalpha(ch))
					ch = toupper(ch);
				//cout << "in id continued" << ch << endl;
				lexeme += ch;
			}
			else {
				in.putback(ch);
				//cout << lexeme << endl;
				return id_or_kw(lexeme, linenum);
				
			}
			break;

		case INSTRING:
                          
			if( ch == '\n' ) {
				return LexItem(ERR, lexeme, linenum);
			}
			lexeme += ch;
			if( ch == '\"' ) {
				lexeme = lexeme.substr(1, lexeme.length()-2);
				return LexItem(SCONST, lexeme, linenum);
			}
			break;

		case ININT:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else if(ch == '.') {
				lexstate = INFLOAT;
				in.putback(ch);
			}
			else {
				in.putback(ch);
				return LexItem(ICONST, lexeme, linenum);
			}
			break;
		
		case INFLOAT:
				
			if( ch == '.' && isdigit(in.peek()) && !decimal) {
				lexeme += ch; decimal = true;
				
			}
			else if(ch == '.' && !isdigit(in.peek()) && !decimal){
				lexeme += ch;
				return LexItem(ERR, lexeme, linenum);
			}
			else if(isdigit(ch) && decimal){
				lexeme += ch;
			}
			
			else if(ch == '.' && decimal){
				lexeme += ch;
				return LexItem(ERR, lexeme, linenum);
			}
			else {
				in.putback(ch);
				return LexItem(RCONST, lexeme, linenum);
			}
			
			break;
		
					
		case INCOMMENT:
			if(ch == '\n')
				linenum++;
			else if( ch == '*' ) {
               nextch = in.peek();
               if (nextch == '/')
				{
					in.get(ch);
					lexstate = START;
				}
					
			}
			break;
		}

	}//end of while loop
	
	if(lexstate == INCOMMENT)
	{
		cout << endl;
		cout << "Missing a comment end delimiters '*/' at line " << linenum << endl;
		//return LexItem(ERR, lexeme, linenum);
	}
	if( in.eof() )
		return LexItem(DONE, "", linenum);
	return LexItem(ERR, "some strange I/O error", linenum);
}





