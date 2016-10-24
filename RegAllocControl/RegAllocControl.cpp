#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/Support/CommandLine.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/LICM.h"
#include "llvm/Transforms/Scalar/ADCE.h"
#include "llvm/Transforms/Scalar/BDCE.h"
#include "llvm/Transforms/Scalar/AlignmentFromAssumptions.h"
#include "llvm/Transforms/Scalar/ConstantHoisting.h"
//#include "llvm/Transforms/Scalar/ConstantProp.h"
#include "llvm/Transforms/Scalar/CorrelatedValuePropagation.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/DeadStoreElimination.h"
#include "llvm/Transforms/Scalar/EarlyCSE.h"
//#include "llvm/Transforms/Scalar/FlattenCFG.h"
#include "llvm/Transforms/Scalar/Float2Int.h"
#include "llvm/Transforms/Scalar/GuardWidening.h"
#include "llvm/Transforms/Scalar/GVN.h"
//#include "llvm/Transforms/Scalar/GVNHoist.h"
//#include "llvm/Transforms/Scalar/InductiveRangeCheckElimination.h"
#include "llvm/Transforms/Scalar/IndVarSimplify.h"
#include "llvm/Transforms/Scalar/JumpThreading.h"
//#include "llvm/Transforms/Scalar/LoadCombine.h"
#include "llvm/Transforms/Scalar/LoopDataPrefetch.h"
#include "llvm/Transforms/Scalar/LoopDeletion.h"
#include "llvm/Transforms/Scalar/LoopDistribute.h"
#include "llvm/Transforms/Scalar/LoopIdiomRecognize.h"
#include "llvm/Transforms/Scalar/LoopInstSimplify.h"
//#include "llvm/Transforms/Scalar/LoopInterchange.h"
//#include "llvm/Transforms/Scalar/LoopLoadElimination.h"
//#include "llvm/Transforms/Scalar/LoopRerollPass.h"
#include "llvm/Transforms/Scalar/LoopRotation.h"
//#include "llvm/Transforms/Scalar/LoopSimplify.h"
#include "llvm/Transforms/Scalar/LoopStrengthReduce.h"
#include "llvm/Transforms/Scalar/LoopUnrollPass.h"
//#include "llvm/Transforms/Scalar/LoopUnswitch.h"
//#include "llvm/Transforms/Scalar/LoopVersioningLICM.h"
#include "llvm/Transforms/Scalar/LowerAtomic.h"
#include "llvm/Transforms/Scalar/LowerExpectIntrinsic.h"



#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"

using namespace llvm;

static cl::opt<int> AggressionLevel("reg_control", cl::desc("Specify aggressiveness of register allocation"), cl::value_desc("aggression"));


namespace {
  struct RegAllocControlPass : public FunctionPass {
    static char ID;
    RegAllocControlPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      std::string funcName = F.getName();
      errs() << "Register Allocation Aggression Level " << AggressionLevel << "\n";

    }
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<LoopInfoWrapperPass>();
    }
  };
}

char RegAllocControlPass::ID = 0;
static RegisterPass<RegAllocControlPass> X("reg_control", "Register Alloc Aggressiveness Control Pass",
					   false /* Only looks at CFG */ ,
					   false /* Analysis Pass */);


static void registerRegAllocControlPass(const PassManagerBuilder &Builder,
					legacy::PassManagerBase &PM) {
  PM.add(new RegAllocControlPass());
  if (AggressionLevel > 0) 
  {
	//Scalar passes 
	PM.add(createLICMPass());
	PM.add(createAggressiveDCEPass());
	PM.add(createAlignmentFromAssumptionsPass());
	PM.add(createBitTrackingDCEPass());
	PM.add(createConstantHoistingPass()); 
	PM.add(createConstantPropagationPass());
	PM.add(createCorrelatedValuePropagationPass());
	PM.add(createDeadCodeEliminationPass());
	PM.add(createDeadStoreEliminationPass());
	PM.add(createEarlyCSEPass());
	PM.add(createFlattenCFGPass());
	PM.add(createFloat2IntPass());
	PM.add(createGuardWideningPass());
	PM.add(createGVNPass());
	PM.add(createGVNHoistPass());
	PM.add(createInductiveRangeCheckEliminationPass());
	PM.add(createIndVarSimplifyPass());
	PM.add(createJumpThreadingPass());
	PM.add(createLoadCombinePass());
	PM.add(createLoopDataPrefetchPass());	
	PM.add(createLoopDeletionPass());
	PM.add(createLoopDistributePass(true));
	PM.add(createLoopIdiomPass());
	PM.add(createLoopInstSimplifyPass());
	PM.add(createLoopInterchangePass());
	PM.add(createLoopLoadEliminationPass());
	PM.add(createLoopRerollPass());
	PM.add(createLoopRotatePass(20));
	PM.add(createLoopSimplifyCFGPass());
	PM.add(createLoopStrengthReducePass());
	PM.add(createSimpleLoopUnrollPass());
	PM.add(createLoopUnswitchPass(true));
	PM.add(createLoopVersioningLICMPass());
	PM.add(createLowerAtomicPass());
	PM.add(createLowerExpectIntrinsicPass());			
	//IPO passes
	//PM.add(createGlobalDCEPass());
  }
}


static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
 					     registerRegAllocControlPass);
