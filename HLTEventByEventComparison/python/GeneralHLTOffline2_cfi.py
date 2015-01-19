import FWCore.ParameterSet.Config as cms

newHLTOffline = cms.EDAnalyzer("GeneralHLTOffline2",
    HltProcessName = cms.string("HLT2013") ## or ReHLT , HLTGRun, ...
)
