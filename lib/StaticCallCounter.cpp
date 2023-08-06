//========================================================================
// FILE:
//    StaticCallCounter.cpp
//
// DESCRIPTION:
//    Counts the number of direct function calls (i.e. as seen in the source
//    code) in a file.
//
// USAGE:
//    1. Run through opt - legacy pass manager
//      opt -load <BUILD/DIR>/lib/libStaticCallCounter.so --legacy-static-cc
//      -analyze <input-llvm-file>
//    2. You can also run it through 'static':
//      <BUILD/DIR>/bin/static <input-llvm-file>
//
// License: MIT
//========================================================================
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <random>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include "StaticCallCounter.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Format.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

// 日志函数 by cyh --- start
class Logger {
  public:
    Logger(bool enabled) : enabled_ (enabled) {}

    void log(const char* format, ...) {
      if (enabled_) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
      }
    }

    void setEnabled(bool enabled) {
      enabled_ = enabled;
    }

  private:
    bool enabled_;
};
// 日志函数 by cyh --- end

//-----------------------------------------------------------------------------
// StaticCallCounter Implementation
//-----------------------------------------------------------------------------
AnalysisKey StaticCallCounter::Key;

StaticCallCounter::Result StaticCallCounter::runOnModule(Module &M) {

  errs() << "cyh hahaha" << "\n";

  llvm::DenseMap<const llvm::Function *, unsigned> Res;

  int bbID = 0;
  
  for (auto &Func : M) {

    for (auto &BB : Func) {

      errs() << "=============== " << "ID: " << bbID << " ===============" << "\n";

      errs() << BB;

      bbID++;

    }

  }

  return Res;
}

StaticCallCounter::Result
StaticCallCounter::run(llvm::Module &M, llvm::ModuleAnalysisManager &) {
  return runOnModule(M);
}

void LegacyStaticCallCounter::print(raw_ostream &OutS, Module const *) const {
  // TODO: do nothing here
  // printStaticCCResult(OutS, DirectCalls);
}

bool LegacyStaticCallCounter::runOnModule(llvm::Module &M) {

  printf("In Legacy\n");

  DirectCalls = Impl.runOnModule(M);
  return false;
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getStaticCallCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "static-cc", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerAnalysisRegistrationCallback(
                [](ModuleAnalysisManager &MAM) {
                  MAM.registerPass([&] { return StaticCallCounter(); });
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getStaticCallCounterPluginInfo();
}

//-----------------------------------------------------------------------------
// Legacy PM Registration
//-----------------------------------------------------------------------------
char LegacyStaticCallCounter::ID = 0;

// Register the pass - required for (among others) opt
RegisterPass<LegacyStaticCallCounter>
    X("legacy-static-cc", "For each function print the number of direct calls",
      true, // Doesn't modify the CFG => true
      true  // It's a pure analysis pass => true
    );

//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------
void printStaticCCResult(raw_ostream &OutS, const ResultStaticCC &DirectCalls) {
  // TODO: 这部分代码是用来打印静态分析结果的，先注释掉
  // OutS << "================================================="
  //      << "\n";
  // OutS << "LLVM-TUTOR: static analysis results\n";
  // OutS << "=================================================\n";
  // const char *str1 = "NAME";
  // const char *str2 = "#N DIRECT CALLS";
  // OutS << format("%-20s %-10s\n", str1, str2);
  // OutS << "-------------------------------------------------"
  //      << "\n";

  // // Generate a vector of captured functions, sorted alphabetically by function
  // // names. The solution implemented here is a suboptimal - a separate
  // // container with functions is created for sorting.
  // // TODO Make this more elegant (i.e. avoid creating a separate container)
  // std::vector<const Function *> FuncNames;
  // FuncNames.reserve(DirectCalls.size());
  // for (auto &CallCount : DirectCalls) {
  //   FuncNames.push_back(CallCount.getFirst());
  // }
  // std::sort(FuncNames.begin(), FuncNames.end(),
  //           [](const Function *x, const Function *y) {
  //             return (x->getName().str() < y->getName().str());
  //           });

  // // Print functions (alphabetically)
  // for (auto &Func : FuncNames) {
  //   unsigned NumDirectCalls = (DirectCalls.find(Func))->getSecond();
  //   OutS << format("%-20s %-10lu\n", Func->getName().str().c_str(),
  //                  NumDirectCalls);
  // }
}

        // // As per the comments in CallSite.h (more specifically, comments for
        // // the base class CallSiteBase), ImmutableCallSite constructor creates
        // // a valid call-site or NULL for something which is NOT a call site.
        // auto ICS = ImmutableCallSite(&Ins);

        // // Check whether the instruction is actually a call/invoke
        // if (nullptr == ICS.getInstruction()) {
        //   continue;
        // }

        // // Check whether the called function is directly invoked
        // auto DirectInvoc =
        //     dyn_cast<Function>(ICS.getCalledValue()->stripPointerCasts());
        // if (nullptr == DirectInvoc) {
        //   continue;
        // }

        // // Update the count for the particular call
        // auto CallCount = Res.find(DirectInvoc);
        // if (Res.end() == CallCount) {
        //   CallCount = Res.insert(std::make_pair(DirectInvoc, 0)).first;
        // }
        // ++CallCount->second;



        // 尝试使用以下两种方法去识别常数，都没用
        // if(auto *CI = dyn_cast<ConstantInt>(&Ins)) {
        //   errs() << "CI: " << CI->getValue() << "\n";
        // }
        // if(auto *C = dyn_cast<Constant>(&Ins)) {
        //   errs() << "C: " << *C << "\n";
        // }

        // for(auto &cyhoperand = Ins.value_op_begin(); cyhoperand != Ins.value_op_end(); cyhoperand++) {
        //   if (isa<Constant>(&cyhoperand))
        //     Constant *C = cast<Constant>(&cyhoperand);
        //     errs() << "C: " << *C << "\n";
        // }

        // 尝试使用这种方法去识别常数，能够识别C程序中的常数，但是也会识别出很多无关的东西
        // for (auto operand = Ins.operands().begin(); operand != Ins.operands().end(); ++operand) {
        //   if(auto *CI = dyn_cast<ConstantInt>(operand))
        //     errs() << "CI: " << CI->getValue() << "\n";
        //     // errs() << "cyh operand: " << operand->get() << "\n";
        // }

        // 下面这个分支会捕捉到 if, if-else, for循环，while循环，三元运算符。
        // TODO: 可以想想怎么把 if-else 和 三元运算符 分离出来
        // else if (auto *BI = dyn_cast<BranchInst>(&Ins)) {
        //   if (BI->isConditional()) {
        //     // This is an if-else branch
        //     // Perform any analysis or transformations here
        //     errs() << "This is an if-else branch" << "\n";
        //   }
        // }