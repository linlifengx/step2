#include "statement.h"
#include "expression.h"
#include "support.h"

FunctionInfo* FuncDecl::codeGen() {
	vector<ClassInfo*> returnInfos;
	vector<Type*> returnllvmTypes;
	if (style == 0) {
		for (unsigned i = 0; i < returnTypes.size(); i++) {
			ClassInfo *clazz = returnTypes[i]->getClassInfo();
			if (clazz == NULL) {
				throwError(this);
			}
			returnInfos.push_back(clazz);
			returnllvmTypes.push_back(clazz->llvmType);
		}
	} else {
		for (unsigned i = 0; i < returnDecls.size(); i++) {
			ClassInfo *clazz = returnDecls[i]->typeDecl->getClassInfo();
			if (clazz == NULL) {
				throwError(this);
			}
			returnInfos.push_back(clazz);
			returnllvmTypes.push_back(clazz->llvmType);
		}

	}

	Type *returnType = NULL;
	if (returnllvmTypes.size() == 0) {
		returnType = builder.getVoidTy();
	} else if (returnllvmTypes.size() == 1) {
		returnType = returnllvmTypes[0];
	} else {
		ArrayRef<Type*> typesArray(returnllvmTypes);
		returnType = StructType::create(context, typesArray);
	}

	vector<ClassInfo*> argInfos;
	vector<Type*> argllvmTypes;
	for (unsigned i = 0; i < argDecls.size(); i++) {
		SimpleVarDecl *argDecl = argDecls[i];
		ClassInfo *clazz = argDecl->typeDecl->getClassInfo();
		if (clazz == NULL) {
			throwError(argDecl);
		}
		argInfos.push_back(clazz);
		argllvmTypes.push_back(clazz->llvmType);
	}

	FunctionType *functionType = NULL;
	if (argllvmTypes.size() == 0) {
		functionType = FunctionType::get(returnType, false);
	} else {
		ArrayRef<Type*> argTypeArrayRef(argllvmTypes);
		functionType = FunctionType::get(returnType, argTypeArrayRef, false);
	}
	Function *function = Function::Create(functionType,
			Function::ExternalLinkage, funcName + "_sp", &module);
	FunctionInfo *functionInfo = new FunctionInfo(funcName, function,
			returnInfos, argInfos, style);
	if (!globalContext.addFunction(functionInfo)) {
		throwError(this);
	}

	this->functionInfo = functionInfo;
	return functionInfo;
}

Function* FuncDef::declGen() {
	functionInfo = funcDecl->codeGen();
	if (funcDecl->funcName == "main") {
		if (functionInfo->returnClasses.size() != 1
				|| functionInfo->returnClasses[0] != longClass
				|| functionInfo->argClasses.size() != 0) {
			errorMsg =
					"main function must be of type 'long main(char[][] args)'";
			throwError(funcDecl);
		}
	}
	return functionInfo->llvmFunction;
}

void FuncDef::codeGen() {
	Function *function = functionInfo->llvmFunction;
	vector<ClassInfo*> &returnClasses = functionInfo->returnClasses;
	vector<ClassInfo*> &argClasses = functionInfo->argClasses;
	AstContext astContext;

	BasicBlock *allocBB = BasicBlock::Create(context, "alloc", function);
	BasicBlock *entryBB = BasicBlock::Create(context, "entry", function);
	astContext.allocBB = allocBB;
	builder.SetInsertPoint(allocBB);
	unsigned i = 0;
	for (Function::arg_iterator ai = function->arg_begin();
			ai != function->arg_end(); ai++, i++) {
		SimpleVarDecl *argDecl = funcDecl->argDecls[i];
		ClassInfo *argClazz = argClasses[i];
		Value *alloc = builder.CreateAlloca(argClazz->llvmType);
		builder.CreateStore(ai, alloc);
		if (!astContext.addVar(argDecl->varName, AValue(alloc, argClazz))) {
			throwError(argDecl);
		}
	}

	vector<Value*> returnVars;
	if (functionInfo->returnNum > 0) {
		Value *retAlloc = builder.CreateAlloca(functionInfo->returnType);
		astContext.returnAlloc = retAlloc;
		for (i = 0; i < functionInfo->returnNum; i++) {
			ClassInfo *retClazz = returnClasses[i];
			Value *retElement = NULL;
			if (functionInfo->returnNum == 1) {
				retElement = retAlloc;
			} else {
				retElement = builder.CreateStructGEP(retAlloc, i);
			}
			builder.CreateStore(retClazz->getInitial(), retElement);
			returnVars.push_back(retElement);
			if (funcDecl->style == 1) {
				SimpleVarDecl *retDecl = funcDecl->returnDecls[i];
				if (!astContext.addVar(retDecl->varName,
						AValue(retElement, retClazz))) {
					throwError(retDecl);
				}
			}
		}
	}
	astContext.returnVars = &returnVars;

	astContext.currentFunc = functionInfo;
	builder.SetInsertPoint(entryBB);
	stmtBlock->codeGen(astContext);

	if (functionInfo->returnNum == 0) {
		builder.CreateRetVoid();
	} else {
		builder.CreateRet(builder.CreateLoad(astContext.returnAlloc));
	}

	builder.SetInsertPoint(allocBB);
	builder.CreateBr(entryBB);
}
