### *****************************************************************************************
### Usage:
###
### cmsRun topplusdmanaEDMntuples_cfg.py maxEvts=N sample="mySample/sample.root" version="71" outputLabel="myoutput"
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
#                 'file:/afs/cern.ch/user/d/decosa/public/forTTDMteam/patTuple_tlbsm_train_tlbsm_71x_v1.root',
#                 'file:/afs/cern.ch/work/o/oiorio/public/xDM/patTuple_tlbsm_train_tlbsm_71x_v1.root',
                 'file:/tmp/oiorio/patTuple_tlbsm_train_tlbsm_71x_v1.root',
#                 'file:/afs/cern.ch/user/d/decosa/public/forTTDMteam/tlbsm_53x_v3_mc_10_1_qPV.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Sample to analyze')

options.register('version',
                 #'53',
                 '71',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'ntuple version (53 or 71)')

options.register('outputLabel',
                 'analysisTTDM.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Output label')

options.register('isData',
                 False,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Is data?')

options.register('LHE',
                 False,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Keep LHEProducts')

options.parseArguments()

if(options.isData):options.LHE = False

process = cms.Process("ttDManalysisEDMNtuples")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories.append('HLTrigReport')
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

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:startup_GRun')
process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
for pset in process.GlobalTag.toGet.value():
    pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
    #   Fix for multi-run processing:
    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )
    

### Selected leptons and jets
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

if options.version=="53" :
    jetLabel="goodPatJetsPFlow"
elif options.version=="71" :
    jetLabel="goodPatJets"
process.skimmedPatJets = cms.EDFilter(
    "CandViewSelector",
    src = cms.InputTag(jetLabel),
#    src = cms.InputTag("goodPatJetsPFlow"), # 53x
#    src = cms.InputTag("goodPatJets"), # 71x    
    cut = cms.string("(( pt > 25 && mass < 20.) || mass > 20. ) && abs(eta) < 4.")
)


### Asking for at least 2 jets satisfying the selection above 
process.jetFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("skimmedPatJets"),
    minNumber = cms.uint32(2),
    filter = cms.bool(True)
)


process.muonUserData = cms.EDProducer(
    'MuonUserData',
    muonLabel = cms.InputTag("skimmedPatMuons"),
    pv        = cms.InputTag("goodOfflinePrimaryVertices"),
#    pv        = cms.InputTag("offlinePrimaryVertices"),
    ### TTRIGGER ###
    triggerResults     = cms.InputTag("TriggerResults","","HLT"),
    triggerSummary     = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    hltMuonFilter      = cms.InputTag("hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f40QL3crIsoRhoFiltered0p15"),
    hltPath            = cms.string("HLT_IsoMu40_eta2p1_v11"),
    hlt2reco_deltaRmax = cms.double(0.1),
    mainROOTFILEdir    = cms.string("../data/")
)

process.jetUserData = cms.EDProducer(
    'JetUserData',
    jetLabel  = cms.InputTag("skimmedPatJets"),
    pv        = cms.InputTag("goodOfflinePrimaryVertices"),
    ### TTRIGGER ###
    triggerResults     = cms.InputTag("TriggerResults","","HLT"),
    triggerSummary     = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    hltJetFilter       = cms.InputTag("hltSixCenJet20L1FastJet"),
    hltPath            = cms.string("HLT_QuadJet60_DiJet20_v6"),
    hlt2reco_deltaRmax = cms.double(0.2)
)

process.electronUserData = cms.EDProducer(
    'ElectronUserData',
    eleLabel = cms.InputTag("skimmedPatElectrons"),
    pv        = cms.InputTag("goodOfflinePrimaryVertices"),
    triggerResults = cms.InputTag("TriggerResults"),
    triggerSummary = cms.InputTag("hltTriggerSummaryAOD"),
    hltElectronFilter  = cms.InputTag("hltL1sL1Mu3p5EG12ORL1MuOpenEG12L3Filtered8"),  ##trigger matching code to be fixed!
    hltPath             = cms.string("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL")
)


if options.version == "71":
    from PhysicsTools.CandAlgos.EventShapeVars_cff import *
    process.eventShapePFVars = pfEventShapeVars.clone()
    process.eventShapePFVars.src = cms.InputTag("particleFlow")
    
    process.eventShapePFJetVars = pfEventShapeVars.clone()
    process.eventShapePFJetVars.src = cms.InputTag("skimmedPatJets")

    process.centrality = cms.EDProducer("CentralityUserData",
                                        src = cms.InputTag("skimmedPatJets")
                                        )                                    

### Including ntuplizer 
process.load("ttbarDM.TopPlusDMAna.topplusdmedmNtuples_cff")


#add rootplizer

process.TFileService = cms.Service("TFileService", fileName = cms.string("TTBar.root"))

process.load("ttbarDM.TopPlusDMAna.topplusdmedmRootTreeMaker_cff")


### definition of Analysis sequence
process.analysisPath = cms.Path(
    process.skimmedPatElectrons +
    process.skimmedPatMuons +
    process.skimmedPatJets 

)
if options.version == "71":
    process.analysisPath+=process.eventShapePFVars
    process.analysisPath+=process.eventShapePFJetVars 
    process.analysisPath+=process.centrality 

#process.analysisPath+=process.jetFilter


process.analysisPath+=process.muonUserData
process.analysisPath+=process.jetUserData
process.analysisPath+=process.electronUserData

process.analysisPath+=process.genPart

process.analysisPath+=process.muons

process.analysisPath+=process.electrons
process.analysisPath+=process.jets
process.analysisPath+=process.met

process.analysisPath+=process.DMTreesDumper

### Creating the filter path to use in order to select events
process.filterPath = cms.Path(
    process.jetFilter
    )


### keep info from LHEProducts if they are stored in PatTuples
if(options.LHE):
    process.LHEUserData = cms.EDProducer("LHEUserData",
        lheLabel = cms.InputTag("source")
        )
    process.analysisPath+=process.LHEUserData
    process.edmNtuplesOut.outputCommand+=(' *_LHE*_*_*')

### end LHE products     



process.edmNtuplesOut.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('filterPath')
    )

process.fullPath = cms.Schedule(
    process.analysisPath,
    process.filterPath
     )

process.endPath = cms.EndPath(process.edmNtuplesOut)

## process.outpath = cms.Schedule(
##     process.analysisPath,
##     process.endPath
##     )
