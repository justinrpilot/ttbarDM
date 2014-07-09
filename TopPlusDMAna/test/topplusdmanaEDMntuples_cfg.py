### *****************************************************************************************
### Usage:
###
### cmsRun topplusdmana_cfg.py maxEvts=N sample="mySample/sample.root" outputLabel="myoutput"
###
### Default values for the options are set:
### maxEvts     = -1
### sample      = 'file:/scratch/decosa/ttDM/testSample/tlbsm_53x_v3_mc_10_1_qPV.root'
### outputLabel = 'analysisTTDM.root'
### *****************************************************************************************
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as opts



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


process = cms.Process("ttDManalysisEDMNtuples")

process.load("FWCore.MessageService.MessageLogger_cfi")
### Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
### Number of maximum events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvts) )
### Source file
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
        options.sample
        )
)


### Selected leptons and jets
process.skimmedPatMuons = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuons"),
    cut = cms.string("pt > 60 && abs(eta) < 2.4")
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


### Asking for at least 2 jets satisfying the selection above 
process.jetFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("skimmedPatJets"),
    minNumber = cms.uint32(2),
)


process.muonUserData = cms.EDProducer(
    'MuonUserData',
    muonLabel = cms.InputTag("skimmedPatMuons"),
    pv        = cms.InputTag("goodOfflinePrimaryVertices")
    )




### Including ntuplizer 
process.load("ttbarDM.TopPlusDMAna.topplusdmedmNtuples_cff")


## ### Creating the filter path to use in order to select events
## process.filterPath = cms.Path(
##     process.jetFilter
##     )
### definition of Analysis sequence
process.analysisPath = cms.Path(
    process.skimmedPatElectrons +
    process.skimmedPatMuons +
    process.skimmedPatJets +
    process.jetFilter +
    process.muonUserData +
    process.genPart +
    process.muons +
    process.electrons +
    process.jets

)


## process.edmNtuplesOut.SelectEvents = cms.untracked.PSet(
##     SelectEvents = cms.vstring('filterPath')
##     )

process.endPath = cms.EndPath(process.edmNtuplesOut)

process.schedule = cms.Schedule(
    process.analysisPath,
    process.endPath
    )
