/* Implementation of Recursive-Descent Parser
 * parse.cpp
 * Programming Assignment 3
 * Fall 2022
*/

#include "parseInt.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults;
queue<Value> * ValQue;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog ::= PROGRAM IDENT StmtList END PROGRAM
bool Prog(istream& in, int& line)
{
	bool f1;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			
			f1 = StmtList(in, line); 
			
			if(f1) {
				tok = Parser::GetNextToken(in, line);
				if( tok == END){
					tok = Parser::GetNextToken(in, line);
					if(tok != PROGRAM){
						ParseError(line, "Missing PROGRAM Keyword.");
						return false;
					}
					return true;
				}
				else
				{
					ParseError(line, "Missing END of Program.");
					return false;
				}
				
			}
			else
			{
				ParseError(line, "Incorrect Program Body.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}//End of Prog

//StmtList ::= Stmt; { Stmt; }
bool StmtList(istream& in, int& line){
	bool status;
		
	LexItem tok;
	
	status = Stmt(in, line);
		
	while(status)
	{
		tok = Parser::GetNextToken(in, line);
		if(tok != SEMICOL)
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}
			
		status = Stmt(in, line);
	}
			
	tok = Parser::GetNextToken(in, line);
	if(tok == END )
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else if(tok == ELSE)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else 
	{
		ParseError(line, "Syntactic error in Program Body.");
		return false;
	}
	
}//End of StmtList function

//Stmt ::= DeclStmt | ControlStmt
bool Stmt(istream& in, int& line){
	bool status=false;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case INT: case FLOAT: case BOOL:
		Parser::PushBackToken(t);
		status = DeclStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Declaration Statement.");
			return status;
		}
		break;
	case IF: case PRINT: case IDENT: 
		Parser::PushBackToken(t);
		status = ControlStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect control Statement.");
			return status;
		}
		break;
	default:
		Parser::PushBackToken(t);
	}
	return status;
}//End of Stmt function

//DeclStmt ::= ( INT | FLOAT | BOOL ) VarList
bool DeclStmt(istream& in, int& line)
{
	LexItem t;
	bool status;
	
    
	t = Parser::GetNextToken(in, line);
	if(t == INT || t == FLOAT || t == BOOL)
	{
		status = VarList(in, line, t);
		
		if (!status)
		{
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return status;
		}
	}
	else
	{
		ParseError(line, "Incorrect Declaration Type.");
		return false;
	}
	
	return true;
		
}//End of DeclStmt

//VarList:= Var {,Var}
bool VarList(istream& in, int& line, LexItem& type) {
	bool status = false;
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		//set IDENT lexeme to the type tok value
		identstr = tok.GetLexeme();
 	if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		SymTable.insert({identstr, type.GetToken()});
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = VarList(in, line, type);
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;	
}//End of VarList
	

//ControlStmt ::= AssigStmt | IfStmt | PrintStmt 
bool ControlStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		break;
		
	default:
		Parser::PushBackToken(t);
		return false;
	}
    
	return status;
}//End of ControlStmt


//PrintStmt:= PRINT (ExpreList) 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
    
    ValQue = new queue<Value>;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list after Print");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
    while (!(*ValQue).empty()) {
        cout << (*ValQue).front();
        ValQue->pop();
    }
    cout <<endl;
	return ex;
}//End of PrintStmt

//IfStmt:= IF (Expr) then StmtList [Else StmtList] END IF
bool IfStmt(istream& in, int& line) {
	bool ex = false, status ; 
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	Value retVal;
	ex = Expr(in, line,retVal);
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	if (!retVal.IsBool()) {
    return false;
  }
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	if (retVal.GetBool()) {
	  t = Parser::GetNextToken(in, line);
  	if(t != THEN)
  	{
  		ParseError(line, "If Statement Syntax Error");
  		return false;
  	}
  	status = StmtList(in, line);
  	if(!status)
  	{
  		ParseError(line, "Missing Statement for If-Stmt Then-Part");
  		return false;
  	}
  	t = Parser::GetNextToken(in, line);
    if (t == ELSE) {
      while (t != END) {
        t = Parser::GetNextToken(in, line);
      }
    }
  }
  else {
    while (t != END && t != ELSE) {
      t = Parser::GetNextToken(in, line);
    }
    if (t == ELSE) {
      status = StmtList(in, line);
		  if(!status)
  		{
  			ParseError(line, "Missing Statement for If-Stmt Else-Part");
  			return false;
  		}
        
  		else
  		{
  			t = Parser::GetNextToken(in, line);
  		}
    }
  }
	
	if( t == END){
		t = Parser::GetNextToken(in, line);
		if(t != IF){
			ParseError(line, "Missing closing keywords of IF statement.");
			return false;
		}
		return true;
	}
	else
	{
		ParseError(line, "Missing END of IF.");
		return false;
	}
	
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem & idtok)
{
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
		identstr = tok.GetLexeme();
		//cout<< "in assigstmt var: " << identstr << endl;
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
        idtok = LexItem(tok.GetToken(),identstr,line);
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) { //remember you have to incorporate type checking, eg bool a; then a = 3 cant be true, for this you have to check SymTable
	
	bool varstatus = false, status = false;
	LexItem t;
	LexItem idtok;
    Value retVal;
	varstatus = Var( in, line, idtok);
	
    if (idtok.GetToken() != IDENT) {
        return false;
    }
    
	if (varstatus){
		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line, retVal);
			
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
      if (retVal.IsErr()) {
        return false;
       }     
            map<string, Token>::iterator itsym;
            bool isDefined = false;
            map<string, Value>::iterator ittemp;
            
            itsym = SymTable.find(idtok.GetLexeme());
            ittemp = TempsResults.find(idtok.GetLexeme());
            if (ittemp != TempsResults.end()) {isDefined = true;}
            if (itsym == SymTable.end()) {
                return false;
            }
            else {
                if (itsym->second == INT) {
                    if (retVal.IsInt()) {
                      if (isDefined) {
                        ittemp->second = retVal;
                      }
                      else {
                        TempsResults.insert({idtok.GetLexeme(), retVal});
                      }
                    }
                    else if (retVal.IsReal()) {
                      if (isDefined) {
                        retVal = Value((int)(retVal.GetReal()));
                        ittemp->second = retVal;
                      }
                      else {
                        retVal = Value((int)(retVal.GetReal())); //might have to recheck this
                        TempsResults.insert({idtok.GetLexeme(), retVal});
                      }
                    }
                    else {
                        return false;
                    }
                }
                if (itsym->second == FLOAT) {
                    if (retVal.IsReal()) {//or is int
                      if (isDefined) {
                        ittemp->second = retVal;
                      }
                      else {
                        TempsResults.insert({idtok.GetLexeme(),retVal});
                      }
                    }
                    else if (retVal.IsInt()) {
                      if (isDefined) {
                        float fl = (float)(retVal.GetInt());
                        retVal = Value(fl);
                        ittemp->second = retVal;
                      }
                      else {
                        float fl = (float)(retVal.GetInt());
                        retVal = Value(fl);
                        TempsResults.insert({idtok.GetLexeme(), retVal});
                      }
                    }
                    else {
                        return false;
                    }
                }
                if (itsym->second == BOOL) {
                    if (retVal.IsBool()) {
                      if (isDefined) {
                        ittemp->second = retVal;
                      }
                      else {
                        TempsResults.insert({idtok.GetLexeme(), retVal});
                      }
                    }
                    else {
                        return false;
                    }
                }
            }
            
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
    
    Value retVal;
	
	status = Expr(in, line,retVal);
    
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
    ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		
		status = ExprList(in, line);
		
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool t1 = LogANDExpr(in, line, val1);
	LexItem tok;
		
	if( !t1 ) {
		return false;
	}
	
    retVal = val1;
    
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == OR ) 
	{
		t1 = LogANDExpr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		retVal = retVal||val2;
		if (retVal.IsErr()) {
            ParseError(line, "Illegal OR expression");
            return false;
        }
        
        tok = Parser::GetNextToken(in, line);
        
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr/LogORExpr

//LogANDExpr ::= EqualExpr { && EqualExpr }
bool LogANDExpr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
    LexItem tok;
	bool t1 = EqualExpr(in, line,val1);
		
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == AND ) 
	{
		t1 = EqualExpr(in, line,val2);
        
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		retVal = retVal&&val2;
        if (retVal.IsErr()) {
             ParseError(line, "Illegal And expression");
            return false;
        }
        
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of LogANDExpr

//EqualExpr ::= RelExpr [ == RelExpr ]
bool EqualExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
    Value val1, val2;
	bool t1 = RelExpr(in, line,val1);
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == EQUAL ) 
	{
		t1 = RelExpr(in, line,val2);
        retVal = val1==val2; //revisit this line
        if (retVal.IsErr()) {
             ParseError(line, "Illegal equal expression");
            return false;
        }
        
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == EQUAL)
		{
			ParseError(line, "Illegal Equality Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of EqualExpr

//RelExpr ::= AddExpr [ ( < | > ) AddExpr ]
bool RelExpr(istream& in, int& line, Value & retVal) { //might have error because retval is set to val1 (which is initially an integer) and then changed to a boolean type (int < int or int > int)
	LexItem tok;
    Value val1, val2;
	bool t1 = AddExpr(in, line,val1);
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == LTHAN || tok == GTHAN) 
	{
		t1 = AddExpr(in, line, val2);
        if (tok == LTHAN) {
            retVal = retVal<val2;
        }
        if (tok == GTHAN) {
            retVal = retVal>val2;
          }
    if (retVal.IsErr()) {
            return false;
        }
        
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		
		if(tok == LTHAN || tok == GTHAN)
		{
			ParseError(line, "Illegal Relational Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of RelExpr

//AddExpr :: MultExpr { ( + | - ) MultExpr }
bool AddExpr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool t1 = MultExpr(in, line, val1);
	
    LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
        
		t1 = MultExpr(in, line, val2);
        if (tok == PLUS) {
            retVal = retVal+val2;
        }
        if (tok == MINUS) {
            retVal = retVal-val2;
        }
        if (retVal.IsErr()) {
            return false;
        }
        
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//MultExpr ::= UnaryExpr { ( * | / ) UnaryExpr }
bool MultExpr(istream& in, int& line, Value & retVal) {
	//ValQue2 = new queue<Value>; //create new queue of objects

    Value val1, val2;
	
    bool t1 = UnaryExpr(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		t1 = UnaryExpr(in, line, val2);
		if (tok == MULT) {
            retVal = retVal*val2;
        }
        if (tok == DIV) {
            retVal = retVal/val2;
        }
        if (retVal.IsErr()) {
            return false;
        }
        
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//UnaryExpr ::= ( - | + | ! ) PrimaryExpr | PrimaryExpr
bool UnaryExpr(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
  else if (t == NOT) {
    sign = 2;
  }
	else
		Parser::PushBackToken(t);
		
	status = PrimaryExpr(in, line, sign, retVal);
     if (sign == 2) {
        if (!retVal.IsBool()) {
          return false;
        }
       retVal = !retVal;
    }
  
    if (sign == -1) {
        
        if (!retVal.IsInt() && !retVal.IsReal()) {
            return false;
        }
        if (retVal.IsInt()) {
            retVal = -retVal.GetInt();
        }
        if (retVal.IsReal()) {
            retVal = -retVal.GetReal();
        }
    }
	return status;
}//End of UnaryExpr



//PrimaryExpr ::= IDENT | ICONST | RCONST | SCONST | BCONST | ( Expr )
bool PrimaryExpr(istream& in, int& line, int sign, Value & retVal) { //might have to deal with cases with parenthesis 
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if( tok == IDENT ) {
		
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undeclared Variable");
			return false;	
		}
        map<string, Value>::iterator ittemps;
        ittemps = TempsResults.find(lexeme);
        if(ittemps == TempsResults.end()) {
            return false;
        }
        else {
            retVal = ittemps->second;
        }
		return true;
	}
	else if( tok == ICONST ) {
        retVal = Value(stoi(tok.GetLexeme()));
		return true;
	}
	else if( tok == SCONST ) {
        retVal = Value(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {
    float a = stod(tok.GetLexeme());
		retVal = Value(a);
		return true;
	}
	else if( tok == BCONST ) {
        if (tok.GetLexeme() == "TRUE") {
            retVal =Value(true);
        }
        if (tok.GetLexeme() == "FALSE") {
            retVal = Value(false);
        }
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal); //check again
        if (retVal.IsErr()) {
            return false;
        }
        
		if( !ex ) {
			ParseError(line, "Missing expression after Left Parenthesis");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing right Parenthesis after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}



