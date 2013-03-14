#ifndef AST_SUPPORT_H_
#define AST_SUPPORT_H_

#include "common.h"

using namespace std;
using namespace llvm;

extern LLVMContext &context;
extern Module module;
extern IRBuilder<> builder;
extern DataLayout *dataLayout;
extern GlobalContext globalContext;

extern Type *int64Type;
extern Type *int32Type;
extern Type *doubleType;
extern Type *boolType;
extern Type *voidType;
extern ClassInfo *longClass;
extern ClassInfo *charClass;
extern ClassInfo *doubleClass;
extern ClassInfo *boolClass;
extern ClassInfo *voidClass;

extern Constant *int64_0;
extern Constant *int32_0;
extern Constant *double_0;
extern Constant *bool_true;
extern Constant *bool_false;

extern Function *mainFunc;

extern string errorMsg;

extern void throwError(Node *node);
extern string getOperatorName(int op);
extern Value* createAlloca(Type *type, BasicBlock *bb);

class ClassInfo {
public:
	string name;
	Type *llvmType;

	ClassInfo(string name, Type *llvmType) {
		this->name = name;
		this->llvmType = llvmType;
	}

	Constant* getInitial();

	bool isBoolType();
	bool isLongType();
	bool isDoubleType();
	bool isCharType();
};

class FunctionInfo {
public:
	string name;
	Function *llvmFunction;

	Type *returnType;
	int returnNum;
	vector<ClassInfo*> returnClasses;
	vector<ClassInfo*> argClasses;
	int style;  //0 normal 1 returns decl 2 constructor

	FunctionInfo(string name, Function *llvmFunction,
			vector<ClassInfo*> &returnClasses, vector<ClassInfo*> &argClasses,
			int style = 0) {
		this->name = name;
		this->llvmFunction = llvmFunction;
		this->returnClasses = returnClasses;
		this->argClasses = argClasses;
		this->style = style;

		this->returnNum = returnClasses.size();
		this->returnType = llvmFunction->getReturnType();
	}

	FunctionInfo() {
		this->llvmFunction = NULL;
		this->returnType = NULL;
		this->style = 0;
		this->returnNum = 0;
	}
};

class AValue {
public:
	Value *llvmValue;
	ClassInfo *clazz;

	AValue(Value *llvmValue = NULL, ClassInfo *clazz = NULL, bool isReadOnly =
			false) {
		this->llvmValue = llvmValue;
		this->clazz = clazz;
	}

	bool castTo(ClassInfo *destClazz);
	bool isBool();
	bool isLong();
	bool isChar();
	bool isDouble();
};

class AFunction {
public:
	Value *llvmFunc;
	FunctionInfo *funcInfo;

	AFunction(Value *llvmFunc = NULL, FunctionInfo *funcInfo = NULL) {
		this->llvmFunc = llvmFunc;
		this->funcInfo = funcInfo;
	}

};

class AstContext {
public:
	AstContext *superior;
	map<string, AValue> varTable;
	FunctionInfo *currentFunc;
	BasicBlock *allocBB;
	BasicBlock *breakOutBB;
	BasicBlock *continueBB;
	vector<Value*> *returnVars;
	Value *returnAlloc;

	explicit AstContext(AstContext *superior = NULL) {
		this->superior = superior;
		if (superior != NULL) {
			this->currentFunc = superior->currentFunc;
			this->allocBB = superior->allocBB;
			this->breakOutBB = superior->breakOutBB;
			this->continueBB = superior->continueBB;
			this->returnVars = superior->returnVars;
			this->returnAlloc = superior->returnAlloc;
		} else {
			this->currentFunc = NULL;
			this->allocBB = NULL;
			this->breakOutBB = NULL;
			this->continueBB = NULL;
			this->returnVars = NULL;
			this->returnAlloc = NULL;
		}
	}

	bool addVar(string &name, AValue avalue);
	AValue getVar(string &name);
	AFunction getFunc(string &name);
};

class GlobalContext {
public:
	map<string, ClassInfo*> classTable;
	map<string, FunctionInfo*> functionTable;
	map<string, AValue> varTable;

	bool addClass(ClassInfo *clazz);
	bool addFunction(FunctionInfo *func);
	bool addVar(string &name, AValue avalue);
	ClassInfo* getClass(string &name);
	AFunction getFunctionV(string &name);
	AValue getVar(string &name);
};

#endif
