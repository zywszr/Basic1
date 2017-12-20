/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface specified in Chapter 17, which is an
 * excellent model for the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include "evalstate.h"
#include "exp.h"
#include "../StanfordCPPLib/error.h"

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

   Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

   virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

   virtual void execute(EvalState & state) = 0;
   
   virtual int getn() { return 0; }
   virtual int typ() { return -1; }

};

/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

class State_REM:public Statement {
public:
	State_REM(string _s):s(_s) { return; }
	~State_REM() { return; }
	void execute(EvalState & state) { return; }
	int typ() { return 0; }
private:
	string s;  		
};

class State_LET:public Statement {
public:
	State_LET(Expression *_s) {
		s = _s;
		return;	
	}
	~State_LET() {
		if(s) delete s;
		return;	
	}	
	void execute(EvalState & state) {
		int x = s->eval(state);
		return;			
	}
	int typ() { return 1; }
private:
	Expression *s;
};

class State_PRINT:public Statement {
public:
	State_PRINT(Expression *_s) {
		s = _s;
		return;	
	}	
	~State_PRINT() {
		if(s) delete s;
		return;	
	}
	void execute(EvalState & state) {
		cout << s->eval(state) << endl;
		return;
	}
	int typ() { return 2; }
private:
	Expression *s;
};

class State_INPUT:public Statement {
public:
	State_INPUT(Expression *_s) {
		s = _s;
		return;	
	}
	~State_INPUT() {
		if(s) delete s;
		return;	
	}
	void execute(EvalState & state) {
		cout <<	" ? ";
		int x; cin >> x;
		state.setValue(((IdentifierExp *)s)->getName(), x);
		return;		
	}
	int typ() { return 3; }
private:
	Expression *s;
};

class State_END:public Statement {
public:
	State_END() { return; }
	~State_END() { return; }
	void execute(EvalState & state) { return; }
	int typ() { return 6; }
};

class State_GOTO:public Statement {
public:
	State_GOTO(int _n):n(_n) { return; }
	~State_GOTO() { return; }
	void execute(EvalState & state) { return; }
	int getn() { return n; }
	int typ() { return 4; }	
private:
	int n;
};

class State_IF:public Statement {
public:
	State_IF(Expression *_x, Expression *_y, int _k, int _n):k(_k), n(_n) {
		x = _x, y = _y;
		return;	
	}
	~State_IF() {
		if(x) delete x;
		if(y) delete y;	
		return;	
	}
	void execute(EvalState & state) {
		bool fg = false;
		if(k == 0) {
			if(x->eval(state) == y->eval(state)) fg = true;		
		} else if(k == 1) {
			if(x->eval(state) > y->eval(state)) fg = true;	
		} else {
			if(x->eval(state) < y->eval(state)) fg = true;		
		}
		return;			
	}
	bool cmp(EvalState & state) {
		bool fg = false;
		if(k == 0) {
			if(x->eval(state) == y->eval(state)) fg = true;		
		} else if(k == 1) {
			if(x->eval(state) > y->eval(state)) fg = true;	
		} else {
			if(x->eval(state) < y->eval(state)) fg = true;		
		}
		return fg;	
	}
	int typ() { return 5; }		
	int getn() { return n; }
private:
	Expression *x, *y;
	int k, n;	
};

#endif
