#include "statement.h"
#include "support.h"

Constant* ClassInfo::getInitial() {
	if (llvmType == int64Type) {
		return int64_0;
	} else if (llvmType == int32Type) {
		return int32_0;
	} else if (llvmType == doubleType) {
		return double_0;
	} else if (llvmType == boolType) {
		return bool_false;
	} else {
		errorMsg = "can't init a var of " + name + " class";
		return NULL;
	}
}

bool ClassInfo::isBoolType() {
	return this == boolClass || llvmType == boolType;
}

bool ClassInfo::isLongType() {
	return this == longClass || llvmType == int64Type;
}

bool ClassInfo::isDoubleType() {
	return this == doubleClass || llvmType == doubleType;
}

bool ClassInfo::isCharType() {
	return this == charClass || llvmType == int32Type;
}
