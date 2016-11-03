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
#include "llvm/Transforms/Scalar/CorrelatedValuePropagation.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/DeadStoreElimination.h"
#include "llvm/Transforms/Scalar/EarlyCSE.h"
#include "llvm/Transforms/Scalar/Float2Int.h"
#include "llvm/Transforms/Scalar/GuardWidening.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/IndVarSimplify.h"
#include "llvm/Transforms/Scalar/JumpThreading.h"
#include "llvm/Transforms/Scalar/LoopDataPrefetch.h"
#include "llvm/Transforms/Scalar/LoopDeletion.h"
#include "llvm/Transforms/Scalar/LoopDistribute.h"
#include "llvm/Transforms/Scalar/LoopIdiomRecognize.h"
#include "llvm/Transforms/Scalar/LoopInstSimplify.h"
#include "llvm/Transforms/Scalar/LoopRotation.h"
#include "llvm/Transforms/Scalar/LoopStrengthReduce.h"
#include "llvm/Transforms/Scalar/LoopUnrollPass.h"
#include "llvm/Transforms/Scalar/LowerAtomic.h"
#include "llvm/Transforms/Scalar/LowerExpectIntrinsic.h"
#include "llvm/Transforms/Scalar/LowerGuardIntrinsic.h"
#include "llvm/Transforms/Scalar/MemCpyOptimizer.h"
#include "llvm/Transforms/Scalar/MergedLoadStoreMotion.h"
#include "llvm/Transforms/Scalar/NaryReassociate.h"
#include "llvm/Transforms/Scalar/PartiallyInlineLibCalls.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SCCP.h"
#include "llvm/Transforms/Scalar/Sink.h"
#include "llvm/Transforms/Scalar/SpeculativeExecution.h"
#include "llvm/Transforms/Scalar/SROA.h"
#include "llvm/Transforms/Scalar/TailRecursionElimination.h"

#include "llvm/Transforms/Utils/AddDiscriminators.h"
#include "llvm/Transforms/Utils/BreakCriticalEdges.h"
#include "llvm/Transforms/Utils/LCSSA.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/LoopVersioning.h"
#include "llvm/Transforms/Utils/LowerInvoke.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/Transforms/Utils/SimplifyInstructions.h"
#include "llvm/Transforms/Utils/SymbolRewriter.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"

#include "llvm/Transforms/Vectorize.h"
#include "llvm/Transforms/Vectorize/SLPVectorizer.h"

#include "llvm/Transforms/ObjCARC.h"

#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include "llvm/Transforms/IPO/ConstantMerge.h"
#include "llvm/Transforms/IPO/CrossDSOCFI.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"

#include "llvm/Transforms/Instrumentation.h"

#include "llvm/Transforms/Coroutines.h"

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

	//	PM.add(createLoopRerollPass());

	PM.add(createLoopRotatePass(20));
	PM.add(createLoopSimplifyCFGPass());

	//	PM.add(createLoopStrengthReducePass());

	PM.add(createLoopUnswitchPass(true));
	PM.add(createLoopVersioningLICMPass());
	PM.add(createLowerAtomicPass());
	PM.add(createLowerExpectIntrinsicPass());			
	PM.add(createLowerGuardIntrinsicPass());
	PM.add(createMemCpyOptPass());
	PM.add(createMergedLoadStoreMotionPass());

	//	PM.add(createNaryReassociatePass());

	//PM.add(createPlaceSafepointsPass());

	//	PM.add(createReassociatePass());

	//	PM.add(createDemoteRegisterToMemoryPass());

	//PM.add(createRewriteStatepointsForGCPass());
	PM.add(createScalarizerPass());
	PM.add(createSCCPPass());
	PM.add(createSeparateConstOffsetFromGEPPass(NULL,true));
	//PM.add(createSimplificationPass(int, ste::func)); 			
	PM.add(createSinkingPass());
	PM.add(createSpeculativeExecutionPass());

	//	PM.add(createSROAPass());

	PM.add(createStraightLineStrengthReducePass());
	PM.add(createStructurizeCFGPass(false));
	PM.add(createTailCallEliminationPass());

	//Utils Passes
	PM.add(createAddDiscriminatorsPass());
	PM.add(createBreakCriticalEdgesPass());	
	PM.add(createInstructionNamerPass());
	PM.add(createLCSSAPass());
	PM.add(createLoopSimplifyPass());
	PM.add(createLoopVersioningPass());
	PM.add(createLowerInvokePass());
	PM.add(createLowerSwitchPass());

	//	PM.add(createPromoteMemoryToRegisterPass());

	//PM.add(createMetaRenamerPass());
	//PM.add(createNameAnonGlobalPass());
	PM.add(createInstructionSimplifierPass());
	PM.add(createRewriteSymbolsPass());
	PM.add(createUnifyFunctionExitNodesPass());

	//Vectorize passes
	//PM.add(createBBVectorizePass(NULL);
	PM.add(createLoadStoreVectorizerPass());
	PM.add(createSLPVectorizerPass());
	
	//ObjCARC passes
	//PM.add(createObjCARCAPElimPass());
	PM.add(createObjCARCContractPass());
	PM.add(createObjCARCExpandPass());	
	PM.add(createObjCARCOptPass());
	//PM.add(createPAEvalPass());

	//IPO passes
	//PM.add(createAlwaysInlinerLegacyPass(true));
	//PM.add(createArgumentPromotionPass(100));
	//PM.add(createBarrierNoopPass());
	//PM.add(createConstantMergePass());
	//PM.add(createCrossDSOCFIPass());
	//PM.add(createDeadArgEliminationPass());
	//PM.add(createGlobalDCEPass());

	//Instrumentation passes
	//PM.add(createAddressSanitizerModulePass(true,true));
	PM.add(createBoundsCheckingPass());
	//PM.add(createPGOIndirectCallPromotionLegacyPass(true));
	//PM.add(createMemorySanitizerPass(50));
	//PM.add(createPGOInstrumentationGenLegacyPass());
	//PM.add(createThreadSanitizerPass());

	//InstCombine passes
	//	PM.add(createInstructionCombiningPass(true));

	//Coroutines passes
	//	PM.add(createCoroCleanupPass());
	//	PM.add(createCoroEarlyPass());
	//	PM.add(createCoroElidePass());
	//PM.add(createCoroSplitPass());
	
  }
  if (AggressionLevel < 0) {
    PM.add(createInstructionCombiningPass(true));
    PM.add(createLoopRerollPass());
    PM.add(createDemoteRegisterToMemoryPass());

  }
  if (AggressionLevel > 0) {
    PM.add(createLICMPass());
    PM.add(createGVNPass());
    PM.add(createSimpleLoopUnrollPass());
    PM.add(createGVNHoistPass());
    PM.add(createPartiallyInlineLibCallsPass());
  }
  if (AggressionLevel > 1) {
    PM.add(createSROAPass());
    PM.add(createNaryReassociatePass());
    PM.add(createReassociatePass());
    PM.add(createLoopStrengthReducePass());
    PM.add(createPromoteMemoryToRegisterPass());
  }

}


static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
 					     registerRegAllocControlPass);
