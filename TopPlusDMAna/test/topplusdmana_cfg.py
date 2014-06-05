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

options.parseArguments()


process = cms.Process("ttDManalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
# Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
# Number of maximum events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvts) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:/scratch/decosa/ttDM/testSample/tlbsm_53x_v3_mc_10_1_qPV.root'
    )
)

# Selected leptons
process.skimmedPatMuons = cms.EDFilter(
        "PATMuonSelector",
            src = cms.InputTag("selectedPatMuons"),
            cut = cms.string("pt > 20 && abs(eta) < 2.4")
        )

process.skimmedPatElectrons = cms.EDFilter(
        "PATElectronSelector",
            src = cms.InputTag("selectedPatElectrons"),
            cut = cms.string("pt > 20 && abs(eta) < 2.5")
        )


process.topDMana = cms.EDAnalyzer('TopPlusDMAna',
                              electronLabel = cms.InputTag("skimmedPatElectrons"),
                              muonLabel = cms.InputTag("skimmedPatMuons"),
                              outputFile = cms.string("analysisTTDM.root"),
                              pv   = cms.InputTag("goodOfflinePrimaryVertices"),
                              ptmin     = cms.double(10)
)


process.p = cms.Path(process.skimmedPatMuons + process.skimmedPatElectrons + process.topDMana)
