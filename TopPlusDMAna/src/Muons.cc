#include "ttbarDM/TopPlusDMAna/interface/Muons.h"

// dR and dPhi                                                                                                                                                                       
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

// Muons
#include "DataFormats/MuonReco/interface/MuonPFIsolation.h"

// Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"


Muons::Muons(const edm::ParameterSet& iConfig){
  edm::ParameterSet conf = iConfig.getParameter<edm::ParameterSet>("muonBlock");
  muonLabel_ = conf.getParameter<edm::InputTag>("muon"); // "patMuons";
  pvLabel_   = conf.getParameter<edm::InputTag>("pv");   // "offlinePrimaryVertex"
  ptmin_     = conf.getParameter<double>       ("ptmin");
}

Muons::~Muons() {

}

void Muons::defineBranch(TTree* tree) {

  tree->Branch("nMuon",    &nMuon,      "nMuon/I");
  tree->Branch("muonpt",    muonpt,     "muonpt[nMuon]/F");
  tree->Branch("muoneta",   muoneta,    "muoneta[nMuon]/F");
  tree->Branch("muonphi",   muonphi,    "muonphi[nMuon]/F");
  tree->Branch("muoncharge",muoncharge, "muoncharge[nMuon]/I");
  tree->Branch("muonpfiso", muonpfiso,  "muonpfiso[nMuon]/F");

}


bool Muons::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  iEvent.getByLabel(pvLabel_, pvHandle);
  if(!pvHandle.isValid()) {
    std::cout << " - No Primary Vertex" << std::endl;
    return true;
  }
  size_t nPV=pvHandle->size();
  const reco::Vertex* vertex=&pvHandle->front();

  edm::Handle<std::vector<pat::Muon> > muons;
  iEvent.getByLabel(muonLabel_, muons);
  

  nMuon = 0;
  for(std::vector<pat::Muon>::const_iterator imuon = muons->begin(), emuon = muons->end(); 
      imuon != emuon; ++imuon) {

    if (imuon->pt() < ptmin_) continue;
    if (fabs(imuon->eta() ) < 2.4 ) continue;

    float pfIso = 0.;
    // (imuon.pfIsolationR04().sumChargedHadronPt + std::max(imuon.pfIsolationR04().sumNeutralHadronEt + imuon.pfIsolationR04().sumPhotonEt - 0.5*imuon.pfIsolationR04().sumPUPt, 0.) ) / imuon.pt(); // from PAT
    const pat::Muon* mu = &(*imuon);
    bool isTight = isTightMuon(mu, vertex);
    bool isLoose = isLooseMuon(mu);

    muonpt    [nMuon] = imuon->pt();
    muoneta   [nMuon] = imuon->eta();
    muonphi   [nMuon] = imuon->phi();
    muoncharge[nMuon] = imuon->charge();    
    muonpfiso[nMuon]  = pfIso;

    nMuon ++; 
  }
  
  
  return true;
}

bool
Muons::isLooseMuon(const pat::Muon* muon) {
  return muon->isPFMuon() && (muon->isGlobalMuon() || muon->isTrackerMuon());
}

bool
Muons::isSoftMuon(const pat::Muon* muon, const reco::Vertex* vertex) {
  return  muon::isGoodMuon(*muon, muon::TMOneStationTight)
    && muon->track()->hitPattern().trackerLayersWithMeasurement()       > 5
    && muon->innerTrack()->hitPattern().pixelLayersWithMeasurement()    > 1
    && muon->muonBestTrack()->normalizedChi2()                          < 1.8
    && muon->innerTrack()->dxy(vertex->position())                      < 3.
    && muon->innerTrack()->dz(vertex->position())                       < 30.
    ;
}
bool 
Muons::isTightMuon(const pat::Muon* muon, const reco::Vertex* vertex) {
  return  muon->isGlobalMuon()
    && muon->isPFMuon()
    && muon->muonBestTrack()->normalizedChi2()                          < 10.
    && (muon->globalTrack().isNonnull() ? muon->globalTrack()->hitPattern().numberOfValidMuonHits() : -1)        > 0
    && muon->numberOfMatchedStations()                                  > 1
    && fabs(muon->muonBestTrack()->dxy(vertex->position()))             < 0.2
    && fabs(muon->muonBestTrack()->dz(vertex->position()))              < 0.5
    && muon->innerTrack()->hitPattern().numberOfValidPixelHits()        > 0
    && muon->track()->hitPattern().trackerLayersWithMeasurement()       > 5
    ;
}

