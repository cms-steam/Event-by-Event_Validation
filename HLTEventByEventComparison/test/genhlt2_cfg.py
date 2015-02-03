import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("Demo")

process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'POSTLS170_V5::All'
process.prefer("GlobalTag")

fileList = FileUtils.loadListFromFile('/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_2_3/src/HLTrigger/Configuration/test/outputA.txt')
readFile = cms.untracked.vstring(*fileList)
#readFile = 'file:/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_2_0_pre8/src/HLTrigger/Configuration/test/outputA.root'

process.maxEvents = cms.untracked.PSet(
    	input = cms.untracked.int32(-1)
)


process.source = cms.Source("PoolSource",
    	fileNames = cms.untracked.vstring(readFile))
         #'file:/uscms_data/d3/muell149/HLTONLINE/CMSSW_6_2_5/src/HLTrigger/Configuration/test/outputA.root'
    #))

#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange(
#    '199812:70-199812:80'
#)

process.load("Event-by-Event_Validation.HLTEventByEventComparison.GeneralHLTOffline2_cfi")

#import FWCore.ParameterSet.Config as cms
#process.newHLTOffline = cms.EDAnalyzer("GeneralHLTOffline2",
#    HltProcessName = cms.string("HLTGRun") ## or ReHLT , HLTGRun, ...
#)
#process.genparticles = cms.EDAnalyzer("QuickGenInfo")

process.load("Event-by-Event_Validation.HLTEventByEventComparison.QuickGenInfo_cfi")

process.newHLTOffline.HltProcessName = 'TEST' # use 'HLT' for centrally produced samples. default is 'HLTGRun'.


process.TFileService = cms.Service("TFileService",
                                   #fileName = cms.string("620_pre7_special_charlieOnLine_with_geninfo_lastfilter.root")
                                   fileName = cms.string("723_stage1.root")
                                   )

process.p = cms.Path(process.newHLTOffline+process.genparticles)


# summary
process.options = cms.untracked.PSet(
	wantSummary = cms.untracked.bool(True)
	)

outfile = open('config.py','w')
print >> outfile,process.dumpPython()
outfile.close()
																				           
