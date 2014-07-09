import FWCore.ParameterSet.Config as cms
import copy

#### basic set of variables which are commons to all the objects
basic =  cms.EDProducer(
    "CandViewNtpProducer",
    src=cms.InputTag("skimmedPatMuons"),
    lazyParser=cms.untracked.bool(True),
    prefix=cms.untracked.string("basic"),
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

### muon variables 
muonVars = (

    cms.PSet(
    tag = cms.untracked.string("isSoftMuon"),
    quantity = cms.untracked.string("userFloat('isSoftMuon')")
    ),
    cms.PSet(
    tag = cms.untracked.string("isLooseMuon"),
    quantity = cms.untracked.string("userFloat('isLooseMuon')")
    ),
    cms.PSet(
    tag = cms.untracked.string("isTightMuon"),
    quantity = cms.untracked.string("userFloat('isTightMuon')")
    ),
    )

### jet variables
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


### copying the muon set of variables from basic,
### adding the set of variable which are related to muons only
muons = copy.deepcopy(basic)
muons.variables += muonVars
muons.prefix = cms.untracked.string("mu")
muons.src = cms.InputTag("muonUserData")

###electrons
electrons = copy.deepcopy(basic)
electrons.prefix = cms.untracked.string("el")
electrons.src = cms.InputTag("skimmedPatElectrons")

###jets
jets = copy.deepcopy(basic)
jets.variables += jetVars
jets.prefix = cms.untracked.string("jet")
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
