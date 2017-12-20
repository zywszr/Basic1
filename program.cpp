/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "../StanfordCPPLib/strlib.h"
#include "program.h"
#include "statement.h"
#include "parser.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/error.h"
using namespace std;

TokenScanner scanner;

Program::Program() {
	num = 0;
	for(int i = 0 ; i < 60 ; ++ i) {
		lin[i] = nxt[i] = pre[i] = 0;
		pro[i] = NULL;	
	}
	fg = false;
	return;
}

Program::~Program() {
   	for(int i = 1 ; i <= num ; ++ i) if(pro[i]) delete pro[i];	
	return;
}

void Program::clear() {
   	for(int i = 1 ; i <= num ; ++ i) {
		lin[i] = nxt[i] = pre[i] = 0;
		if(pro[i]) delete pro[i];
		pro[i] = NULL;	
	}
	num = 0;
	return;
}

void Program::addSourceLine(int lineNumber, string line) {
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(line);
	scanner.nextToken();
	if(scanner.hasMoreTokens()) {
		int id;		
		if(Is_LineNumber(lineNumber)) id = getid(lineNumber);
		else {
			id = ++ num; lin[id] = lineNumber;
			int mn = 0, mx = 0;
			for(int i = 1 ; i < num ; ++ i) {
				if(lin[i] < lineNumber) {
					if(mn == 0 || lin[i] > lin[mn]) mn = i;				
				} else {
					if(mx == 0 || lin[i] < lin[mx]) mx = i;				
				}			
			}
			if(mn) nxt[mn] = id;
			if(mx) pre[mx] = id;
			nxt[id] = mx, pre[id] = mn;		
		}		
		str[id] = line;
		string s = scanner.nextToken();
		if(s == "LET") pro[id] = new State_LET(parseExp(scanner));
		else if(s == "REM") pro[id] = new State_REM(line);
		else if(s == "INPUT") pro[id] = new State_INPUT(parseExp(scanner));
		else if(s == "PRINT") pro[id] = new State_PRINT(parseExp(scanner));
		else if(s == "END") pro[id] = new State_END();
		else if(s == "GOTO") pro[id] = new State_GOTO(stringToInteger(scanner.nextToken()));
		else if(s == "IF") {
			string s1, s2; int k = -1, n;
			while(true) {
				s = scanner.nextToken();
				if(s == "=" || s == "<" || s == ">") {
					if(s == "=") k = 0;
					else if(s == ">") k = 1;
					else k = 2; 
					break;
				}
				s1 += s;			
			}
			while(scanner.hasMoreTokens()) {
				s = scanner.nextToken();
				if(s == "THEN") {
					n = stringToInteger(scanner.nextToken());
					break;				
				}				
				s2 += s;
			}
			scanner.setInput(s1);
			Expression *exp1 = parseExp(scanner);
			
			scanner.setInput(s2);
			Expression *exp2 = parseExp(scanner);
			pro[id] = new State_IF(exp1, exp2, k, n);		
		}	
	} else removeSourceLine(lineNumber);
	return;
}

void Program::removeSourceLine(int lineNumber) {
   	if(!Is_LineNumber(lineNumber)) return;
	int id = getid(lineNumber);
	if(nxt[id]) pre[nxt[id]] = pre[id];
	if(pre[id]) nxt[pre[id]] = nxt[id];
	nxt[id] = pre[id] = lin[id] = 0, str[id] = "";
	if(pro[id]) delete pro[id];
	pro[id] = NULL;
	for(int i = id ; i < num ; ++ i) {
		pre[i] = pre[i + 1], nxt[i] = nxt[i + 1], lin[i] = lin[i + 1];
		pro[i] = pro[i + 1], str[i] = str[i + 1];	
	}
	nxt[num] = pre[num] = lin[num] = 0, str[num] = "";
	pro[num] = NULL;
	-- num;
	for(int i = 1 ; i <= num ; ++ i) {
		if(nxt[i] > id) -- nxt[i];
		if(pre[i] > id) -- pre[i];	
	}
	return;
}

string Program::getSourceLine(int lineNumber) {
	if(Is_LineNumber(lineNumber)) return str[getid(lineNumber)];	   
	return "";  
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
   	if(!Is_LineNumber(lineNumber)) {
		
	} else {
		int id = getid(lineNumber);
		if(pro[id]) delete pro[id];
		pro[id] = stmt;
	}
	return;
}

Statement *Program::getParsedStatement(int lineNumber) {
	int id = getid(lineNumber);	
	if(pro[id]) return pro[id];
   	return NULL;
}

int Program::getFirstLineNumber() {
	if(num == 0) return -1;
	int mn = lin[1];
	for(int i = 2 ; i <= num ; ++ i) if(mn > lin[i]) mn = lin[i];    	
	return mn;
}

int Program::getNextLineNumber(int lineNumber) {
	int id = getid(lineNumber);
	if(nxt[id]) return lin[nxt[id]];
	else return -1;  
}

int Program::getid(int lineNumber) {
	for(int i = 1 ; i <= num ; ++ i) if(lin[i] == lineNumber) return i;
	return 0;
}

bool Program::Is_LineNumber(int lineNumber) {
	for(int i = 1 ; i <= num ; ++ i) if(lin[i] == lineNumber) return true;
	return false;
}

void Program::excute(int k, EvalState & state) {
	int id = getid(getFirstLineNumber());
	while(true) {
		if(!id) break;
		if(k == 1) cout << str[id] << endl;
		int type = pro[id]->typ();		
		if(type == 0) id = nxt[id];
		else if(type == 1 || type == 2 || type == 3) {
			if(!k) pro[id]->execute(state);		
			id = nxt[id];		
		} else if(type == 4) {
			int n = pro[id]->getn();
			if(!Is_LineNumber(n)) error("LINE NUMBER ERROR");
			id = getid(n);
		} else if(type == 5) {
			if(((State_IF *)pro[id])->cmp(state)) id = getid(pro[id]->getn());
			else id = nxt[id];		
		} else break;
	}	
	return;
}
