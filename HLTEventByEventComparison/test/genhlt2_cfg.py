import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'START61_V11::All'
process.prefer("GlobalTag")



process.maxEvents = cms.untracked.PSet(
    	input = cms.untracked.int32(1000)
)


process.source = cms.Source("PoolSource",
    	fileNames = cms.untracked.vstring(
         #'file:/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_1_0_pre8/src/HLTrigger/Configuration/test/outputA.root'
    'file:/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_1_0_pre8/src/HLTrigger/Configuration/test/fall13_50ns/outputA_Fal13_50ns.root'
    ))

#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange(
#    '199812:70-199812:80'
#)

process.load("DQMOffline.Trigger.GeneralHLTOffline2_cfi")
process.load("DQMOffline.Trigger.QuickGenInfo_cfi")

process.newHLTOffline.HltProcessName = 'HLTGRun' # use 'HLT' for centrally produced samples. default is 'HLTGRun'.


process.TFileService = cms.Service("TFileService",
                                   #fileName = cms.string("620_pre7_special_charlieOnLine_with_geninfo_lastfilter.root")
                                   fileName = cms.string("Fal13_50ns_new_1k.root")
                                   )

process.p = cms.Path(process.newHLTOffline+process.genparticles)


# summary
process.options = cms.untracked.PSet(
	wantSummary = cms.untracked.bool(True)
	)

outfile = open('config.py','w')
print >> outfile,process.dumpPython()
outfile.close()
																				           
