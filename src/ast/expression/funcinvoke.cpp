#include "expression.h"
#include "support.h"

vector<AValue> FuncInvoke::multiCodeGen(AstContext &astContext) {
	AValue object;
	AFunction funcV = astContext.getFunc(funcName);

	if (funcV.llvmFunc == NULL) {
		throwError(this);
	}

	vector<ClassInfo*> &argClasses = funcV.funcInfo->argClasses;
	if (exprList.size() < argClasses.size()) {
		errorMsg = "too few arguments to function '" + funcName + "''";
		throwError(this);
	} else if (exprList.size() > argClasses.size()) {
		errorMsg = "too many arguments to function '" + funcName + "'";
		throwError(this);
	}

	vector<AValue> argValues;
	vector<Value*> argllvmValues;

	for (unsigned i = 0; i < exprList.size(); i++) {
		exprList[i]->expectedType = argClasses[i];
		AValue v = exprList[i]->codeGen(astContext);
		argValues.push_back(v);
		argllvmValues.push_back(v.llvmValue);
	}

	Value *callResult = NULL;
	if (argValues.size() == 0) {
		callResult = builder.CreateCall(funcV.llvmFunc);
	} else {
		ArrayRef<Value*> args(argllvmValues);
		callResult = builder.CreateCall(funcV.llvmFunc, args);
	}

	vector<AValue> resultValues;
	vector<ClassInfo*> &resultClasses = funcV.funcInfo->returnClasses;
	if (resultClasses.size() == 0) {
		resultValues.push_back(AValue(NULL, voidClass));
	} else if (resultClasses.size() == 1) {
		resultValues.push_back(AValue(callResult, resultClasses[0]));
	} else {
		Value *alloc = createAlloca(funcV.funcInfo->returnType,
				astContext.allocBB);
		builder.CreateStore(callResult, alloc);
		for (unsigned i = 0; i < resultClasses.size(); i++) {
			Value *element = builder.CreateStructGEP(alloc, i);
			AValue v(builder.CreateLoad(element), resultClasses[i]);
			resultValues.push_back(v);
		}
	}
	return resultValues;
}

AValue FuncInvoke::gen(AstContext &astContext) {
	vector<AValue> resultValues = multiCodeGen(astContext);
	return resultValues[0];
}
