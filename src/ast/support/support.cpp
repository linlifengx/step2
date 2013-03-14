#include "statement.h"
#include "expression.h"
#include "support.h"
#include "parser.hpp"

void throwError(Node *node) {
	cout << node->firstLine << ":" << node->firstColumn << ": error: "
			<< errorMsg << endl;
	exit(1);
}

Value* createAlloca(Type *type, BasicBlock *bb) {
	BasicBlock *currentBB = builder.GetInsertBlock();
	builder.SetInsertPoint(bb);
	Value *var = builder.CreateAlloca(type);
	builder.SetInsertPoint(currentBB);
	return var;
}

string getOperatorName(int op) {
	string name;
	if (op < 128) {
		name.push_back(op);
	} else {
		switch (op) {
		case AND:
			name = "&&";
			break;
		case OR:
			name = "||";
			break;
		case NEQUAL:
			name = "!=";
			break;
		case EQUAL:
			name = "==";
			break;
		case LE:
			name = "<=";
			break;
		case GE:
			name = ">=";
			break;
		}
	}
	return name;
}
