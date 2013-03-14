#ifndef AST_EXPRESSION_H_
#define AST_EXPRESSION_H_

#include "node.h"
#include "support.h"

class AValue;

using namespace std;

class Expression: public Node {
public:
	ClassInfo *expectedType;

	Expression() {
		this->expectedType = NULL;
	}

	virtual ~Expression() {
	}

	AValue codeGen(AstContext &astContext) {
		AValue value = gen(astContext);
		if (expectedType != NULL && !value.castTo(expectedType)) {
			throwError(this);
		}
		return value;
	}
protected:
	virtual AValue gen(AstContext &astContext)=0;

};

class LeftValueExpr: public Expression {
public:
	virtual AValue lvalueGen(AstContext &astContext)=0;
};

class IdentExpr: public LeftValueExpr {
public:
	string ident;

	IdentExpr(string &ident) {
		this->ident = ident;
	}

	AValue gen(AstContext &astContext);
	AValue lvalueGen(AstContext &astContext);
};

class BinaryOpExpr: public Expression {
public:
	Expression *leftExpr;
	Expression *rightExpr;
	int op;

	BinaryOpExpr(Expression *leftExpr, int op, Expression *rightExpr) {
		this->leftExpr = leftExpr;
		this->op = op;
		this->rightExpr = rightExpr;
	}

	AValue gen(AstContext &astContext);
};

class BinaryLogicExpr: public Expression {
public:
	Expression *leftExpr;
	Expression *rightExpr;
	int op;

	BinaryLogicExpr(Expression *leftExpr, int op, Expression *rightExpr) {
		this->leftExpr = leftExpr;
		this->op = op;
		this->rightExpr = rightExpr;
	}

	AValue gen(AstContext &astContext);
};

class PrefixOpExpr: public Expression {
public:
	int op;
	Expression *expr;

	PrefixOpExpr(int op, Expression *expr) {
		this->op = op;
		this->expr = expr;
	}

	AValue gen(AstContext &astContext);
};

class FuncInvoke: public Expression {
public:
	Expression *expr;
	string funcName;
	vector<Expression*> exprList;
	bool isConstructor;

	FuncInvoke(Expression *expr, string &funcName,
			vector<Expression*> &exprList, bool isConstructor = false) {
		this->funcName = funcName;
		this->exprList = exprList;
		this->expr = expr;
		this->isConstructor = isConstructor;
	}

	vector<AValue> multiCodeGen(AstContext &astContext);
	AValue gen(AstContext &astContext);
};

class Long: public Expression {
public:
	int64_t value;

	Long(int64_t value) {
		this->value = value;
	}

	AValue gen(AstContext &astContext);
};

class Char: public Expression {
public:
	int32_t value;
	Char(int32_t value) {
		this->value = value;
	}

	AValue gen(AstContext &astContext);
};

class Double: public Expression {
public:
	double value;

	Double(double value) {
		this->value = value;
	}

	AValue gen(AstContext &astContext);
};

class Bool: public Expression {
public:
	bool value;

	Bool(bool value) {
		this->value = value;
	}

	AValue gen(AstContext &astContext);
};

#endif
