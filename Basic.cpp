/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
         	cout << ex.getMessage() << endl;
      }
   }
	
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(line);
	string s = scanner.nextToken();
	if(scanner.getTokenType(s) == 2) {
		program.addSourceLine(stringToInteger(s), line);
	} else {
		if(s == "RUN") program.excute(0, state);
		else if(s == "LIST") program.excute(1, state);
		else if(s == "CLEAR") state.clear(), program.clear();
		else if(s == "QUIT") exit(0);
		//else if(s == "HELP") program.help();
		else {
			if(s == "LET" || s == "PRINT") {
				Expression *exp = parseExp(scanner);
				int value = exp->eval(state);
				if(s == "PRINT") cout << value << endl;
				delete exp;			
			} else if(s == "INPUT") {
				int x = 0; string var = scanner.nextToken();
				bool fg = true;
				while(true) {
					fg = true;
					cout << " ? "; 
					line = getLine();
					scanner.setInput(line);
					
					if(line[0] == '-') {
						scanner.getChar();
						fg = false;
					}
					s = scanner.nextToken();
				
					if(scanner.getTokenType(s) != 2 || scanner.hasMoreTokens()) cout << "INVALID NUMBER" << endl;
					else {
						bool fg = true;
						for(int i = 0, r = s.length() ; i < r ; ++ i) {
							if(s[i] == '.') {
								cout << "INVALID NUMBER" << endl;
								fg = false;
								break;
							}
						}
						if(fg) {
							x = stringToInteger(s);
							break;
						}
					}	
				}
				if(!fg) x = -x;
				state.setValue(var, x);			
			} else {
			
			}		
		}
	}
	return;
}
