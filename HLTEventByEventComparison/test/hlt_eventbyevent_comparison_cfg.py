import FWCore.ParameterSet.Config as cms

##################################################################

# options
scenario=2  # 0 = Fall13, 25 ns; 1 = Fall13, 50 ns; 2 = Spring14, 25 ns;

inputFile = 'input.root'
outputFile = 'output.root'

if scenario==0:
    inputFile = 'file:/uscms_data/d2/dpuigh/TPG/EventByEvent/CMSSW_7_1_0_pre8/src/outputA25nsFall13_1000.root'
    outputFile = 'histo_hlt_eventbyevent_comparison_Fall13_25ns.root'
elif scenario==1:
    inputFile = 'file:/uscms_data/d2/dpuigh/TPG/EventByEvent/CMSSW_7_1_0_pre8/src/outputA50nsFall13_1000.root'
    outputFile = 'histo_hlt_eventbyevent_comparison_Fall13_50ns.root'
elif scenario==2:
    inputFile = 'file:/uscms_data/d2/dpuigh/TPG/EventByEvent/CMSSW_7_1_0_pre8/src/outputA25nsSpring14_1000.root'
    outputFile = 'histo_hlt_eventbyevent_comparison_Spring14_25ns.root'


##################################################################

process = cms.Process("EbyE")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger.cerr.threshold = 'INFO'
#process.MessageLogger.categories.append('EbyE')
#process.MessageLogger.cerr.INFO = cms.untracked.PSet(
#        limit = cms.untracked.int32(-1)
#        )

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
    )

process.source = cms.Source("PoolSource",
                            # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
            inputFile,
        )
    )

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'PRE_LS171V9A::All'

process.eventbyevent = cms.EDAnalyzer('HLTEventByEventComparison',
                                      ReferenceHLTProcessName = cms.string("HLT"),
                                      TargetHLTProcessName = cms.string("HLT2013")
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(outputFile)
    )


process.p = cms.Path(process.eventbyevent)
