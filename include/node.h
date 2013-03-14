#ifndef AST_NODE_H_
#define AST_NODE_H_

#include "common.h"

using namespace std;
using namespace llvm;

class Node {
public:
	int firstLine;
	int firstColumn;
	int lastLine;
	int lastColumn;
};

class Program: public Node {
public:
	vector<VarDef*> varDefs;
	vector<FuncDef*> funcDefs;

	void addVarDef(VarDef *varDef) {
		varDefs.push_back(varDef);
	}

	void addFuncDef(FuncDef *funcDef) {
		funcDefs.push_back(funcDef);
	}

	void codeGen();
};

class VarInit: public Node {
public:
	string varName;
	Expression *expr;

	VarInit(string &varName, Expression *expr = NULL) {
		this->varName = varName;
		this->expr = expr;
	}
};

class SimpleVarDecl: public Node {
public:
	TypeDecl *typeDecl;
	string varName;

	SimpleVarDecl(TypeDecl *typeDecl, string &varName) {
		this->typeDecl = typeDecl;
		this->varName = varName;
	}
};

class TypeDecl: public Node {
public:
	string typeName;
	unsigned dimension;

	TypeDecl(string &typeName, unsigned dimension = 0) {
		this->typeName = typeName;
		this->dimension = dimension;
	}

	ClassInfo* getClassInfo();
};

class FuncDef: public Node {
public:
	FuncDecl *funcDecl;
	StmtBlock *stmtBlock;

	FunctionInfo *functionInfo;

	FuncDef(FuncDecl *funcDecl, StmtBlock *stmtBlock) {
		this->funcDecl = funcDecl;
		this->stmtBlock = stmtBlock;
		this->functionInfo = NULL;
	}

	Function* declGen();
	void codeGen();
};

class FuncDecl: public Node {
public:
	vector<TypeDecl*> returnTypes;
	vector<SimpleVarDecl*> returnDecls;
	string funcName;
	vector<SimpleVarDecl*> argDecls;
	int style; //0 normal 1 retdecl

	FunctionInfo *functionInfo;

	FuncDecl(vector<TypeDecl*> &returnTypes, string &funcName,
			vector<SimpleVarDecl*> &argDecls) {
		this->returnTypes = returnTypes;
		this->funcName = funcName;
		this->argDecls = argDecls;
		this->style = 0;
		this->functionInfo = NULL;
	}

	FuncDecl(vector<SimpleVarDecl*> &returnDecls, string &funcName,
			vector<SimpleVarDecl*> &argDecls) {
		this->returnDecls = returnDecls;
		this->funcName = funcName;
		this->argDecls = argDecls;
		this->style = 1;
		this->functionInfo = NULL;
	}

	FunctionInfo* codeGen();
};

#endif
