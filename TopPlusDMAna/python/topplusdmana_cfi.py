import FWCore.ParameterSet.Config as cms

topPlusDM = cms.EDAnalyzer('TopPlusDMAna',
   muonBlock = cms.PSet(
    muon  = cms.InputTag("selectedMuons"),
    pv    = cms.InputTag("offlinePrimaryVertices"),
    ptmin = cms.double(5.)
   ) 
)
