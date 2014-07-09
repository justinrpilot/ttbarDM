import FWCore.ParameterSet.Config as cms
import copy


basic =  cms.EDProducer(
    "CandViewNtpProducer",
    src=cms.InputTag("skimmedPatMuons"),
    lazyParser=cms.untracked.bool(True),
    prefix=cms.untracked.string("mu"),
    eventInfo=cms.untracked.bool(True),
    variables = cms.VPSet(
    cms.PSet(
    tag = cms.untracked.string("Mass"),
    quantity = cms.untracked.string("mass")
    ),
    cms.PSet(
    tag = cms.untracked.string("Pt"),
    quantity = cms.untracked.string("pt")
    ),
    cms.PSet(
    tag = cms.untracked.string("Eta"),
    quantity = cms.untracked.string("eta")
    ),
    cms.PSet(
    tag = cms.untracked.string("Phi"),
    quantity = cms.untracked.string("phi")
    ),
    cms.PSet(
    tag = cms.untracked.string("E"),
    quantity = cms.untracked.string("energy")
    ),
    cms.PSet(
    tag = cms.untracked.string("Charge"),
    quantity = cms.untracked.string("charge")
    ),
    )
    )

muonVars = (
    
    cms.PSet(
    tag = cms.untracked.string("isSoftMuon"),
    quantity = cms.untracked.string("isSoftMuon")
    ),
    cms.PSet(
    tag = cms.untracked.string("isLooseMuon"),
    quantity = cms.untracked.string("isLooseMuon")
    ),
    cms.PSet(
    tag = cms.untracked.string("isTightMuon"),
    quantity = cms.untracked.string("isTightMuon")
    ),
    )

jetVars = (
    
    cms.PSet(
    tag = cms.untracked.string("CSVbtag"),
    quantity = cms.untracked.string("bDiscriminator(\"combinedSecondaryVertexBJetTags\")")
    ),
    cms.PSet(
    tag = cms.untracked.string("PartonFlavour"),
    quantity = cms.untracked.string("partonFlavour")
    ),
    )

    
muons = copy.deepcopy(basic)
#muons.variables += muonVars
muons.src = cms.InputTag("skimmedPatMuons")

electrons = copy.deepcopy(basic)
electrons.src = cms.InputTag("skimmedPatElectrons")

jets = copy.deepcopy(basic)
jets.variables += jetVars
jets.src = cms.InputTag("skimmedPatJets")


edmNtuplesOut = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string('TTbarDMEdmNtuples.root'),
    outputCommands = cms.untracked.vstring(
    "drop *",
    "keep *_muons_*_*",
    "keep *_electrons_*_*",
    "keep *_jets_*_*"
    
    ),
    dropMetaData = cms.untracked.string('ALL'),
    )
