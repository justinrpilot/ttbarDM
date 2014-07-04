

import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as opts

### To specify the number of events to process, just do it by command line
### e.g: cmsRun topplusdmana_cfg.py maxEvts=10


options = opts.VarParsing ('analysis')

options.register('maxEvts',
                 -1,# default value: process all events
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.int,
                 'Number of events to process')

options.register('sample',
#                 'file:/scratch/decosa/ttDM/testSample/patTuple_tlbsm_train_tlbsm_71x_v1.root',
                 'file:/scratch/decosa/ttDM/testSample/tlbsm_53x_v3_mc_10_1_qPV.root',
                 #'file:/scratch/decosa/ttDM/testSample/AOD_ttDM_53.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Sample to analyze')

options.register('outputLabel',
                 'analysisTTDM.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Output label')

options.parseArguments()


process = cms.Process("ttDManalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
# Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
# Number of maximum events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvts) )

process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
        options.sample
        )
)


# Selected leptons and jets
process.skimmedPatMuons = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuons"),
    cut = cms.string("pt > 30 && abs(eta) < 2.4")
    )

process.skimmedPatElectrons = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectrons"),
    cut = cms.string("pt > 30 && abs(eta) < 2.5")
    )

process.skimmedPatJets = cms.EDFilter(
    "PATJetSelector",
    src = cms.InputTag("goodPatJetsPFlow"),
    cut = cms.string("pt > 25 && abs(eta) < 2.4")
    )

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputLabel), closeFileFast = cms.untracked.bool(True) )


process.jets = cms.EDAnalyzer('Jets',
                              jetLabel = cms.InputTag("skimmedPatJets"),
                                )

process.muons = cms.EDAnalyzer('Muons',
                              muonLabel = cms.InputTag("skimmedPatMuons"),
                              pv   = cms.InputTag("goodOfflinePrimaryVertices"),
                              )

process.electrons = cms.EDAnalyzer('Electrons',
                              electronLabel = cms.InputTag("skimmedPatElectrons"),
                             )

process.genParticles = cms.EDAnalyzer('GenParticles',
                              genLabel = cms.InputTag("prunedGenParticles"),
                             )

process.p = cms.Path(process.skimmedPatElectrons + process.skimmedPatMuons + process.skimmedPatJets + process.jets+ process.muons+ process.electrons + process.genParticles  )

