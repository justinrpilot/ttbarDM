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

met =  cms.EDProducer(
    "CandViewNtpProducer",
    src=cms.InputTag("patMETPF"),
    lazyParser=cms.untracked.bool(True),
    prefix=cms.untracked.string("met"),
    eventInfo=cms.untracked.bool(True),
    variables = cms.VPSet(
    cms.PSet(
    tag = cms.untracked.string("Pt"),
    quantity = cms.untracked.string("pt")
    ),
    cms.PSet(
    tag = cms.untracked.string("Px"),
    quantity = cms.untracked.string("px")
    ),
    cms.PSet(
    tag = cms.untracked.string("Py"),
    quantity = cms.untracked.string("py")
    ),
    cms.PSet(
    tag = cms.untracked.string("Phi"),
    quantity = cms.untracked.string("phi")
    ),
    )
    )

### muon variables 
muonVars = (
   cms.PSet(
        tag = cms.untracked.string("Iso03"),
        quantity = cms.untracked.string("userFloat('iso03')")
   ),
   cms.PSet(
        tag = cms.untracked.string("D0"),
        quantity = cms.untracked.string("userFloat('d0')")
   ),
   cms.PSet(
        tag = cms.untracked.string("D0err"),
        quantity = cms.untracked.string("userFloat('d0err')")
   ),
   cms.PSet(
        tag = cms.untracked.string("Dz"),
        quantity = cms.untracked.string("userFloat('dz')")
        ),
   cms.PSet(
        tag = cms.untracked.string("Dzerr"),
        quantity = cms.untracked.string("userFloat('dzerr')")
        ),
### the following variables have been commented in the c++ code
### because current version of B2G pattuple does not have track embedded in the pat::muon
### and this makes the code crashing !!!!
### [I tried to exploit the isNull() method, but w/o succeding :( ]   
   cms.PSet(
    tag = cms.untracked.string("IsSoftMuon"),
    quantity = cms.untracked.string("userFloat('isSoftMuon')")
    ),
   cms.PSet(
    tag = cms.untracked.string("IsLooseMuon"),
    quantity = cms.untracked.string("userFloat('isLooseMuon')")
    ),
   cms.PSet(
    tag = cms.untracked.string("IsTightMuon"),
    quantity = cms.untracked.string("userFloat('isTightMuon')")
    ),
   )

### jet variables
jetVars = (
    
    cms.PSet(
    tag = cms.untracked.string("CSVbtag"),
    quantity = cms.untracked.string("bDiscriminator(\"combinedSecondaryVertexBJetTags\")")
    ),
#    cms.PSet(
#    tag = cms.untracked.string("PartonFlavour"),
#    quantity = cms.untracked.string("partonFlavour")
#    ),
    )

genPartVars = (
    cms.PSet(
    tag = cms.untracked.string("ID"),
    quantity = cms.untracked.string("pdgId")
    ),
    cms.PSet(
    tag = cms.untracked.string("Status"),
    quantity = cms.untracked.string("status")
    ),
    cms.PSet(
    tag = cms.untracked.string("MomID"),
    quantity = cms.untracked.string("?numberOfMothers>0 ? mother(0).pdgId : 0")
    ),
    )


    
### copying the muon set of variables from basic,
### adding the set of variable which are related to muons only
muons = copy.deepcopy(basic)
muons.variables += muonVars
muons.prefix = cms.untracked.string("mu")
muons.src = cms.InputTag("muonUserData")
#muons.src = cms.InputTag("skimmedPatMuons")

###electrons
electrons = copy.deepcopy(basic)
electrons.prefix = cms.untracked.string("el")
electrons.src = cms.InputTag("skimmedPatElectrons")

###jets
jets = copy.deepcopy(basic)
jets.variables += jetVars
jets.prefix = cms.untracked.string("jet")
jets.src = cms.InputTag("skimmedPatJets")

###genPart
genPart = copy.deepcopy(basic)
genPart.variables += genPartVars
genPart.prefix = cms.untracked.string("genPart")
genPart.src = cms.InputTag("prunedGenParticles")


###event variables
eventShapeVar = (
   cms.PSet(
        tag = cms.untracked.string("isotropy"),
        quantity = cms.untracked.string("isotropy")
   ),
   cms.PSet(
        tag = cms.untracked.string("circularity"),
        quantity = cms.untracked.string("circularity")
   ),
   cms.PSet(
        tag = cms.untracked.string("sphericity"),
        quantity = cms.untracked.string("sphericity")
   ),
   cms.PSet(
        tag = cms.untracked.string("aplanarity"),
        quantity = cms.untracked.string("aplanarity")
   ),
   cms.PSet(
        tag = cms.untracked.string("thrust"),
        quantity = cms.untracked.string("thrust")
   ),
)

edmNtuplesOut = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string('TTbarDMEdmNtuples.root'),
    outputCommands = cms.untracked.vstring(
    "drop *",
    "keep *_genPart_*_*",
    "keep *_muons_*_*",
    "keep *_electrons_*_*",
    "keep *_jets_*_*",
    "keep *_eventShape*_*_*",
    "keep *_*_*centrality*_*",
    "keep *_met_*_*"
    ),
    dropMetaData = cms.untracked.string('ALL'),
    )
