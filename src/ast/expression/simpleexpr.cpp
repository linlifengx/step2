#include "expression.h"
#include "support.h"

AValue PrefixOpExpr::gen(AstContext &astContext) {
	AValue val = expr->codeGen(astContext);
	if (op == '-') {
		if (val.isDouble()) {
			val.llvmValue = builder.CreateFNeg(val.llvmValue);
			return val;
		} else if (val.isLong() || val.isChar()) {
			val.llvmValue = builder.CreateNeg(val.llvmValue);
			return val;
		}
	} else if (op == '!') {
		if (val.isBool()) {
			val.llvmValue = builder.CreateNot(val.llvmValue);
			return val;
		}
	}
	errorMsg = "invalid argument type '" + val.clazz->name + "' to unary '"
			+ getOperatorName(op) + "' expression";
	throwError(this);
	return val;
}

AValue Long::gen(AstContext &astContext) {
	return AValue(ConstantInt::getSigned(int64Type, value), longClass);
}

AValue Char::gen(AstContext &astContext) {
	return AValue(ConstantInt::getSigned(int32Type, value), charClass);
}

AValue Double::gen(AstContext &astContext) {
	return AValue(ConstantFP::get(doubleType, value), doubleClass);
}

AValue Bool::gen(AstContext &astContext) {
	return AValue(builder.getInt1(value), boolClass);
}
