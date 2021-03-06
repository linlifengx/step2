#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Value.h>
#include <llvm/Type.h>
#include <llvm/Function.h>
#include <llvm/BasicBlock.h>
#include <llvm/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CodeGen/CommandFlags.h>

#include "statement.h"
#include "expression.h"
#include "support.h"
#include "parser.hpp"

extern int yyparse();
extern FILE *yyin;

LLVMContext &context = getGlobalContext();
Module module("test", context);
IRBuilder<> builder(context);
DataLayout *dataLayout = NULL;
GlobalContext globalContext;

Type *int64Type = NULL;
Type *int32Type = NULL;
Type *doubleType = NULL;
Type *boolType = NULL;
Type *voidType = NULL;
ClassInfo *longClass = NULL;
ClassInfo *charClass = NULL;
ClassInfo *doubleClass = NULL;
ClassInfo *boolClass = NULL;
ClassInfo *voidClass = NULL;

Constant *int64_0 = NULL;
Constant *int32_0 = NULL;
Constant *double_0 = NULL;
Constant *bool_true = NULL;
Constant *bool_false = NULL;

Function *mainFunc = NULL;

string errorMsg;
Function *startFunc = NULL;
Program *program = NULL;

static void createSystemFunctions();
static void initGlobals();

int main(int argc, char **argv) {
	bool irOutput = false;
	bool asmOutput = false;
	bool objOutput = false;
	bool execOutput = false;
	TargetMachine::CodeGenFileType outputFileType = TargetMachine::CGFT_Null;
	char *outputFileName = NULL;
	int option;
	while ((option = getopt(argc, argv, "o:scS")) != -1) {
		switch (option) {
		case 'o':
			if (outputFileName != NULL) {
				cout << "warning: ignoring '-o " << optarg << "' because '-o "
						<< outputFileName << "' has set before" << endl;
			} else {
				outputFileName = optarg;
			}
			break;
		case 's':
			asmOutput = true;
			break;
		case 'c':
			objOutput = true;
			break;
		case 'S':
			irOutput = true;
			break;
		}
	}
	if (irOutput) {
		if (asmOutput) {
			cout << "warning: ignoring '-s' because '-S' has set" << endl;
		}
		if (objOutput) {
			cout << "warning: ignoring '-c' because '-S' has set" << endl;
		}
	} else if (asmOutput) {
		if (objOutput) {
			cout << "warning: ignoring '-c' because '-s' has set" << endl;
		}
		outputFileType = TargetMachine::CGFT_AssemblyFile;
	} else if (objOutput) {
		outputFileType = TargetMachine::CGFT_ObjectFile;
	} else {
		outputFileType = TargetMachine::CGFT_ObjectFile;
		execOutput = true;
	}

	char *inputFileName = NULL;
	for (; optind < argc; optind++) {
		if (inputFileName == NULL) {
			inputFileName = argv[optind];
		} else {
			cout << "warning: ignoring input file " << argv[optind] << endl;
		}
	}

	if (inputFileName != NULL) {
		yyin = fopen(inputFileName, "r");
		if (yyin == NULL) {
			cout << "can not open file '" << inputFileName << "'" << endl;
			exit(1);
		}
	}

	if (yyin == NULL) {
		cout << "input program>>" << endl;
	}
	yyparse();

	if (yyin != NULL) {
		fclose(yyin);
	}

	initGlobals();
	createSystemFunctions();

	program->codeGen();
	//module.dump();cout<<endl;

	InitializeNativeTarget();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmPrinters();
	InitializeAllAsmParsers();

	string opFileName;
	if (irOutput) {
		if (outputFileName == NULL) {
			if (inputFileName == NULL) {
				opFileName = "temp.ir";
			} else {
				opFileName = string(basename(inputFileName)) + ".ir";
			}
		} else {
			opFileName = outputFileName;
		}
		string errorMsg;
		tool_output_file outputFile(opFileName.c_str(), errorMsg);
		if (!errorMsg.empty()) {
			cout << errorMsg << endl;
			return 1;
		}
		outputFile.os() << module;
		outputFile.keep();
	} else {
		string errorStr;
		const Target *target = TargetRegistry::lookupTarget(
				sys::getDefaultTargetTriple(), errorStr);
		if (target == NULL) {
			cout << errorStr << endl;
			return 1;
		}
		TargetOptions targetOptions;
		TargetMachine *targetMachine = target->createTargetMachine(
				sys::getDefaultTargetTriple(), sys::getHostCPUName(), "",
				targetOptions);

		if (outputFileName == NULL) {
			if (inputFileName == NULL) {
				if (asmOutput) {
					opFileName = "temp.s";
				} else {
					opFileName = "temp.o";
				}
			} else {
				if (asmOutput) {
					opFileName = string(basename(inputFileName)) + ".s";
				} else {
					opFileName = string(basename(inputFileName)) + ".o";
				}
			}
		} else if (execOutput) {
			opFileName = string(outputFileName) + ".o";
		} else {
			opFileName = outputFileName;
		}
		string errorStr2;
		tool_output_file outputFile(opFileName.c_str(), errorStr2);
		if (!errorStr2.empty()) {
			cout << errorStr2 << endl;
			return 1;
		}
		PassManager passManager;
		passManager.add(dataLayout);
		formatted_raw_ostream fos(outputFile.os());
		targetMachine->addPassesToEmitFile(passManager, fos, outputFileType);
		passManager.run(module);
		outputFile.keep();
	}
	if (execOutput) {
		string sysapi = string(dirname(argv[0])) + "/lib/sysapi.o ";
		if (outputFileName == NULL) {
			outputFileName = "a.out";
		}
		/*string command = "ld /usr/lib/crt1.o /usr/lib/crti.o /usr/lib/crtn.o "
		 + sysapi + " /usr/lib/gcc/i486-linux-gnu/4.4.3/libgcc.a "
		 + opFileName + " -o " + outputFileName
		 + " -lc -dynamic-linker /lib/ld-linux.so.2";*/
		string command = "gcc " + sysapi + opFileName + " -o " + outputFileName;
		int status = system(command.c_str());
		command = "rm " + opFileName;
		system(command.c_str());
		return status;
	}
	return 0;
}

void initGlobals() {
	dataLayout = new DataLayout(&module);

	int64Type = builder.getInt64Ty();
	int32Type = builder.getInt32Ty();
	doubleType = builder.getDoubleTy();
	boolType = builder.getInt1Ty();
	voidType = builder.getVoidTy();

	int64_0 = ConstantInt::getSigned(int64Type, 0);
	int32_0 = ConstantInt::getSigned(int32Type, 0);
	double_0 = ConstantFP::get(doubleType, 0);
	bool_true = builder.getInt1(true);
	bool_false = builder.getInt1(false);

	longClass = new ClassInfo("long", int64Type);
	charClass = new ClassInfo("char", int32Type);
	doubleClass = new ClassInfo("double", doubleType);
	boolClass = new ClassInfo("bool", boolType);
	voidClass = new ClassInfo("void", NULL);

	globalContext.addClass(longClass);
	globalContext.addClass(charClass);
	globalContext.addClass(doubleClass);
	globalContext.addClass(boolClass);
	globalContext.addClass(voidClass);
}

void createSystemFunctions() {
	vector<Type*> argllvmTypes;
	vector<ClassInfo*> argClasses;
	vector<ClassInfo*> emptyClasses;
	FunctionType *funcType = NULL;
	Constant *func = NULL;

	//create print long func
	argllvmTypes.push_back(int64Type);
	argClasses.push_back(longClass);
	funcType = FunctionType::get(voidType, ArrayRef<Type*>(argllvmTypes),
			false);
	func = module.getOrInsertFunction("printL", funcType);
	FunctionInfo *printfL = new FunctionInfo("printL", (Function*) func,
			emptyClasses, argClasses);

	//create print char func
	argllvmTypes.clear();
	argClasses.clear();
	argllvmTypes.push_back(int32Type);
	argClasses.push_back(charClass);
	funcType = FunctionType::get(voidType, ArrayRef<Type*>(argllvmTypes),
			false);
	func = module.getOrInsertFunction("printC", funcType);
	FunctionInfo *printfC = new FunctionInfo("printC", (Function*) func,
			emptyClasses, argClasses);

	//create print double func
	argllvmTypes.clear();
	argClasses.clear();
	argllvmTypes.push_back(doubleType);
	argClasses.push_back(doubleClass);
	funcType = FunctionType::get(voidType, ArrayRef<Type*>(argllvmTypes),
			false);
	func = module.getOrInsertFunction("printD", funcType);
	FunctionInfo *printfD = new FunctionInfo("printD", (Function*) func,
			emptyClasses, argClasses);

	//create print bool func
	argllvmTypes.clear();
	argClasses.clear();
	argllvmTypes.push_back(boolType);
	argClasses.push_back(boolClass);
	funcType = FunctionType::get(voidType, ArrayRef<Type*>(argllvmTypes),
			false);
	func = module.getOrInsertFunction("printB", funcType);
	FunctionInfo *printfB = new FunctionInfo("printB", (Function*) func,
			emptyClasses, argClasses);

	//create println func
	argllvmTypes.clear();
	argClasses.clear();
	funcType = FunctionType::get(voidType, false);
	func = module.getOrInsertFunction("println", funcType);
	FunctionInfo *println = new FunctionInfo("println", (Function*) func,
			emptyClasses, emptyClasses);

	globalContext.addFunction(printfL);
	globalContext.addFunction(printfC);
	globalContext.addFunction(printfD);
	globalContext.addFunction(printfB);
	globalContext.addFunction(println);
}

