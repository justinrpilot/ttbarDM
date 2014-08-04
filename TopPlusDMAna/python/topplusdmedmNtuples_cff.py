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
#   cms.PSet(
#        tag = cms.untracked.string("dB"),
#        quantity = cms.untracked.string("userFloat('dB')")
#   ),
#   cms.PSet(
#        tag = cms.untracked.string("dBPV2D"),
#        quantity = cms.untracked.string("userFloat('dBPV2D')")
#   ),
#   cms.PSet(
#        tag = cms.untracked.string("dBPV3D"),
#        quantity = cms.untracked.string("userFloat('dBPV3D')")
#   ),
#   cms.PSet(
#        tag = cms.untracked.string("dBBS2D"),
#        quantity = cms.untracked.string("userFloat('dBBS2D')")
#   ),
#   cms.PSet(
#        tag = cms.untracked.string("dBBS3D"),
#        quantity = cms.untracked.string("userFloat('dBBS3D')")
#   ),
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
### the following variables need have track embedded in the pat::muon
    cms.PSet(
    tag = cms.untracked.string("IsLooseMuon"),
    quantity = cms.untracked.string("isLooseMuon")
    ),
    cms.PSet(
    tag = cms.untracked.string("IsSoftMuon"),
    quantity = cms.untracked.string("userFloat('isSoftMuon')")
    ),
    cms.PSet(
    tag = cms.untracked.string("IsTightMuon"),
    quantity = cms.untracked.string("userFloat('isTightMuon')")
    ),
## variables used in ID
## https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon_selection
   ### LOOSE
   cms.PSet(
    tag = cms.untracked.string("IsPFMuon"),
    quantity = cms.untracked.string("isPFMuon")
   ),
   cms.PSet(
    tag = cms.untracked.string("IsGlobalMuon"),
    quantity = cms.untracked.string("isGlobalMuon")
   ),   
   cms.PSet(
    tag = cms.untracked.string("IsTrackerMuon"),
    quantity = cms.untracked.string("isTrackerMuon")
   ),   
   ### TIGHT
   cms.PSet(
    tag = cms.untracked.string("GlbTrkNormChi2"),
    quantity = cms.untracked.string("? globalTrack.isNonnull ? globalTrack.normalizedChi2 : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("NumberValidMuonHits"),
    quantity = cms.untracked.string("? globalTrack.isNonnull ? globalTrack.hitPattern.numberOfValidMuonHits : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("NumberMatchedStations"),
    quantity = cms.untracked.string("numberOfMatchedStations")
   ),
   cms.PSet(
    tag = cms.untracked.string("NumberValidPixelHits"),
    quantity = cms.untracked.string("? innerTrack.isNonnull ? innerTrack.hitPattern.numberOfValidPixelHits : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("NumberTrackerLayers"),
    quantity = cms.untracked.string("? track.isNonnull ? track.hitPattern.trackerLayersWithMeasurement : 0")
   ),
   ### SOFT
   cms.PSet(
    tag = cms.untracked.string("NumberOfValidTrackerHits"),
    quantity = cms.untracked.string("? innerTrack.isNonnull ? innerTrack.hitPattern.numberOfValidTrackerHits : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("NumberOfPixelLayers"),
    quantity = cms.untracked.string("? innerTrack.isNonnull ? innerTrack.hitPattern.pixelLayersWithMeasurement : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("InTrkNormChi2"),
    quantity = cms.untracked.string("? innerTrack.isNonnull ? innerTrack.normalizedChi2 : 0")
   ),
## variables used in isolation
## https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Accessing_PF_Isolation_from_reco   
   cms.PSet(
    tag = cms.untracked.string("SumChargedHadronPt"),
    quantity = cms.untracked.string("pfIsolationR04().sumChargedHadronPt")
   ),
   cms.PSet(
    tag = cms.untracked.string("SumNeutralHadronPt"),
    quantity = cms.untracked.string("pfIsolationR04().sumNeutralHadronEt")
   ),
   cms.PSet(
    tag = cms.untracked.string("SumPhotonPt"),
    quantity = cms.untracked.string("pfIsolationR04().sumPhotonEt")
   ),
   cms.PSet(
    tag = cms.untracked.string("SumPUPt"),
    quantity = cms.untracked.string("pfIsolationR04().sumPUPt")
   ),
### genLepton
   cms.PSet(
    tag = cms.untracked.string("GenMuonPx"),
    quantity = cms.untracked.string("? genParticleRef.isNonnull ? genLepton.px : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("GenMuonPy"),
    quantity = cms.untracked.string("? genParticleRef.isNonnull ? genLepton.py : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("GenMuonPz"),
    quantity = cms.untracked.string("? genParticleRef.isNonnull ? genLepton.pz : 0")
   ),
   cms.PSet(
    tag = cms.untracked.string("GenMuonE"),
    quantity = cms.untracked.string("? genParticleRef.isNonnull ? genLepton.energy : 0")
   ),
### trigger matching
   cms.PSet(
    tag = cms.untracked.string("TrigMuonPt"),
    quantity = cms.untracked.string("userFloat('TrigMuonPt')")
   ),
   cms.PSet(
    tag = cms.untracked.string("TrigMuonEta"),
    quantity = cms.untracked.string("userFloat('TrigMuonEta')")
   ),
   cms.PSet(
    tag = cms.untracked.string("TrigMuonPhi"),
    quantity = cms.untracked.string("userFloat('TrigMuonPhi')")
   ),
   cms.PSet(
    tag = cms.untracked.string("TrigMuonE"),
    quantity = cms.untracked.string("userFloat('TrigMuonE')")
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
electronVars = (
    ###Cut-based ID variables
  cms.PSet(
        tag = cms.untracked.string("Iso03"),
        quantity = cms.untracked.string("userFloat('iso03')")
   ),
   cms.PSet(
        tag = cms.untracked.string("D0"),
        quantity = cms.untracked.string("userFloat('d0')")
   ),
   cms.PSet(
        tag = cms.untracked.string("Dz"),
        quantity = cms.untracked.string("userFloat('dz')")
        ),
   cms.PSet(
        tag = cms.untracked.string("dEtaIn"),
        quantity = cms.untracked.string("deltaEtaSuperClusterTrackAtVtx")
        ),
   cms.PSet(
        tag = cms.untracked.string("dPhiIn"),
        quantity = cms.untracked.string("deltaPhiSuperClusterTrackAtVtx")
        ),
   cms.PSet(
        tag = cms.untracked.string("HoE"),
        quantity = cms.untracked.string("hcalOverEcal")
        ),
   cms.PSet(
        tag = cms.untracked.string("full5x5siee"),
        quantity = cms.untracked.string("full5x5_sigmaEtaEta")
        ),
   cms.PSet(
        tag = cms.untracked.string("ooEmooP"),
        quantity = cms.untracked.string("userFloat('ooEmooP')")
        ),
   cms.PSet(
        tag = cms.untracked.string("expectedMissInHits"),
        quantity = cms.untracked.string("gsfTrack().trackerExpectedHitsInner().numberOfLostHits()")
        ),
   cms.PSet(
        tag = cms.untracked.string("pssConVeto"),
        quantity = cms.untracked.string("passConversionVeto")
        )
   
)


electrons = copy.deepcopy(basic)
electrons.variables += electronVars
electrons.prefix = cms.untracked.string("el")
electrons.src = cms.InputTag("electronUserData")


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
