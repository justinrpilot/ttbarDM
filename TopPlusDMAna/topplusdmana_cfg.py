import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
# Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:tlbsm_53x_v3_mc_10_1_qPV.root'
    )
)

#pat.selectedMuons = cms.EDFilter()

process.topDMana = cms.EDAnalyzer('TopPlusDMAna',
                              electronLabel = cms.InputTag("selectedPatElectrons"),
                              muonLabel = cms.InputTag("selectedPatMuons"),
                              outputFile = cms.string("analysisTTDM.root"),
                              pv   = cms.InputTag("goodOfflinePrimaryVertices"),
                              ptmin     = cms.double(10)
)


process.p = cms.Path(process.topDMana)
