#ifndef COMMON_H_
#define COMMON_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <llvm/IRBuilder.h>
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Constants.h>
#include <llvm/Type.h>

class ClassInfo;
class FunctionInfo;
class AValue;
class AFunction;
class AstContext;
class GlobalContext;

class Node;
class Program;
class VarInit;
class SimpleVarDecl;
class TypeDecl;
class FuncDef;
class FuncDecl;

class Statement;
class StmtBlock;
class VarDef;
class VarAssi;
class MultiVarAssi;
class SimpleStmtList;
class ExprStmt;
class IfElseStmt;
class ForStmt;
class ReturnStmt;
class BreakStmt;
class ContinueStmt;

class Expression;
class LeftValueExpr;
class IdentExpr;
class BinaryOpExpr;
class BinaryLogicExpr;
class PrefixOpExpr;
class FuncInvoke;
class Long;
class Char;
class Double;
class Bool;

using namespace std;

#endif
