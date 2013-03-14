#include "statement.h"
#include "support.h"

void Program::codeGen() {
	// func decl gen
	for (unsigned i = 0; i < funcDefs.size(); i++) {
		funcDefs[i]->declGen();
	}

	// create main func and global var gen
	FunctionType *mainFuncType = FunctionType::get(int64Type, false);
	mainFunc = Function::Create(mainFuncType, Function::ExternalLinkage,
			"start_program", &module);
	builder.SetInsertPoint(BasicBlock::Create(context, "entry", mainFunc));
	for (unsigned i = 0; i < varDefs.size(); i++) {
		varDefs[i]->globalGen();
	}
	string mainStr = "main";
	AFunction mainF = globalContext.getFunctionV(mainStr);
	Function *mainf = (Function*) mainF.llvmFunc;
	if (mainf == NULL) {
		cout << errorMsg << endl;
		builder.CreateRet(int64_0);
	} else {
		builder.CreateRet(builder.CreateCall(mainf));
	}

	// function gen
	for (unsigned i = 0; i < funcDefs.size(); i++) {
		funcDefs[i]->codeGen();
	}
}

ClassInfo* TypeDecl::getClassInfo() {
	ClassInfo *classInfo = globalContext.getClass(typeName);
	if (classInfo == NULL) {
		throwError(this);
	}
	return classInfo;
}
