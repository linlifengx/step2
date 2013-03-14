#include "node.h"
#include "support.h"

bool AstContext::addVar(string& name, AValue value) {
	if (varTable[name].llvmValue != NULL) {
		errorMsg = "redefine variable named '" + name + "'";
		return false;
	}
	varTable[name] = value;
	return true;
}

AValue AstContext::getVar(string& name) {
	AValue var = varTable[name];
	if (var.llvmValue == NULL && superior != NULL) {
		return superior->getVar(name);
	}
	if (var.llvmValue == NULL) {
		var = globalContext.getVar(name);
	}
	if (var.llvmValue == NULL) {
		errorMsg = "undeclared identifier '" + name + "'";
	}
	return var;
}

AFunction AstContext::getFunc(string& name) {
	return globalContext.getFunctionV(name);
}

bool GlobalContext::addClass(ClassInfo *clazz) {
	if (classTable[clazz->name] != NULL) {
		errorMsg = "redefine type named '" + clazz->name + "'";
		return false;
	}
	classTable[clazz->name] = clazz;
	return true;
}

bool GlobalContext::addFunction(FunctionInfo *func) {
	if (functionTable[func->name] != NULL) {
		errorMsg = "redefine function named '" + func->name + "'";
		return false;
	}
	functionTable[func->name] = func;
	return true;
}

bool GlobalContext::addVar(string &name, AValue value) {
	if (varTable[name].llvmValue != NULL) {
		errorMsg = "redefine variable named '" + name + "'";
		return false;
	}
	varTable[name] = value;
	return true;
}

ClassInfo* GlobalContext::getClass(string &name) {
	ClassInfo *type = classTable[name];
	if (type == NULL) {
		if (name == "void") {
			errorMsg = "variable has incomplete type 'void'";
		} else {
			errorMsg = "undeclared type '" + name + "'";
		}
	}
	return type;
}

AFunction GlobalContext::getFunctionV(string &name) {
	FunctionInfo *funcInfo = functionTable[name];
	if (funcInfo == NULL) {
		errorMsg = "undeclared function '" + name + "'";
		return AFunction();
	} else {
		return AFunction(funcInfo->llvmFunction, funcInfo);
	}
}

AValue GlobalContext::getVar(string &name) {
	AValue var = varTable[name];
	if (var.llvmValue == NULL) {
		errorMsg = "undeclared identifier '" + name + "'";
	}
	return var;
}
