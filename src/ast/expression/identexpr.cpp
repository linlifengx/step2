#include "expression.h"
#include "support.h"

AValue IdentExpr::lvalueGen(AstContext &astContext) {
	AValue var = astContext.getVar(ident);
	if (var.llvmValue == NULL) {
		throwError(this);
	}
	return var;
}

AValue IdentExpr::gen(AstContext &astContext) {
	AValue value = lvalueGen(astContext);
	value.llvmValue = builder.CreateLoad(value.llvmValue);
	return value;
}
