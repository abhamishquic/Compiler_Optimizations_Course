#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>

using namespace llvm;

// Simple function pass that prints dominator info
struct MyFunctionPass : PassInfoMixin<MyFunctionPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);

    DominanceFrontier &DF = FAM.getResult<DominanceFrontierAnalysis>(F);
    DF.analyze(DT);

    errs() << "Function: " << F.getName() << "\n";
    for (auto &BB : F) {
      const auto Frontier = DF.find(&BB)->second;
      errs() << "Dominance frontier of Block" << BB.getName() << ":\n";
      if (Frontier.empty())
        errs() << " empty \n";
      else {
        for (auto &F : Frontier) {
          errs() << "\t" << F->getName() << "\n";
        }
      }
    }

    return PreservedAnalyses::all();
  }
};

// Module pass that loops over functions and reuses FAM
struct MyModulePass : PassInfoMixin<MyModulePass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
    auto &FAMProxy = MAM.getResult<FunctionAnalysisManagerModuleProxy>(M);
    FunctionAnalysisManager &FAM = FAMProxy.getManager();

    errs() << "Module: " << M.getName() << "\n";

    for (Function &F : M) {
      errs() << "#############################" << "\n";
      errs() << "Function: " << F.getName() << "\n";
      if (F.isDeclaration()) {
        errs() << "is a declaration so skipping" << "\n";
        continue;
      }

      DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
      errs() << "---------------------" << "\n";
      errs() << "domination" << "\n";
      errs() << "\n";

      for (BasicBlock &BB1 : F) {
        for (auto &BB2 : F) {
          if (DT.dominates(&BB1, &BB2)) {
            errs() << left_justify(BB1.getName(), 10) << " dominates "
                   << left_justify(BB2.getName(), 10) << "\n";
          }
        }
      }
      errs() << "---------------------" << "\n";
      errs() << "parent but no domination" << "\n";
      errs() << "\n";

      for (BasicBlock &Parent : F) {
        const auto Succ = (successors(&Parent));
        for (auto *Child : Succ) {
          if (!DT.dominates(&Parent, Child)) {
            errs() << left_justify(Parent.getName(), 10) << " doesn't dominate "
                   << left_justify(Child->getName(), 10)
                   << " but is a parent \n";
          }
        }
      }
      errs() << "---------------------" << "\n";
      errs() << "Dominance frontier analysis" << "\n";
      errs() << "\n";

      DominanceFrontier &DF = FAM.getResult<DominanceFrontierAnalysis>(F);
      DF.analyze(DT);
      for (auto &BB : F) {
        // Dominance frontier of a basic block BB is the set of nodes f
        // such that
        //    - BB dominates a predecessor of F
        //    - BB doesn't strictly dominate F
        const auto Frontier = DF.find(&BB)->second;
        errs() << "Dominance frontier of Block "
               << left_justify(BB.getName(), 10) << " is: ";
        if (Frontier.empty())
          errs() << "empty";
        else {
          for (auto &F : Frontier) {
            assert(!DT.properlyDominates(&BB, F) &&
                   "Block F is in dominance frontier of block BB"
                   "But BB also dominates F");
            auto DomPred = false;
            const auto Pred = predecessors(F);
            for (auto *FPred : Pred) {
              DomPred |= DT.dominates(&BB, FPred);
            }
            assert(DomPred &&
                   "Block F is in dominance frontier of block BB - but BB "
                   "doesn't dominate any predecessor block of F");

            errs() << left_justify(F->getName(), 10) << "\t";
          }
        }
        errs() << "\n";
      }
      errs() << "---------------------" << "\n";

      /*for (BasicBlock &BB : F) {
          DomTreeNode *Node = DT.getNode(&BB);
          if (Node) {
              errs() << "    Block " << BB.getName() << " immediate dominator:
      "; if (DomTreeNode *IDom = Node->getIDom()) { errs() <<
      IDom->getBlock()->getName() << "\n"; } else { errs() << "none (entry)\n";
              }
          }
      }*/
    }

    return PreservedAnalyses::all();
  }
};

// Plugin registration
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MyPassPlugin", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "my-func-pass") {
                    FPM.addPass(MyFunctionPass());
                    return true;
                  }
                  return false;
                });

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "my-module-pass") {
                    MPM.addPass(MyModulePass());
                    return true;
                  }
                  return false;
                });
          }};
}
