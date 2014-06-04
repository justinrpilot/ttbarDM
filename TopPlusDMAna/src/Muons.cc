#include "ttbarDM/TopPlusDMAna/interface/Muons.h"

// dR and dPhi                                                                                                                                                                       
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

// Muons
#include "DataFormats/MuonReco/interface/MuonPFIsolation.h"

// Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"


Muons::Muons(const edm::ParameterSet& iConfig):
  //edm::ParameterSet conf = iConfig.getParameter<edm::ParameterSet>("muonBlock");
  muLabel_(iConfig.getParameter<edm::InputTag>("muonLabel")),
  pvLabel_(iConfig.getParameter<edm::InputTag>("pv")),   // "offlinePrimaryVertex"
  ptmin_(iConfig.getParameter<double>("ptmin"))
{}

Muons::~Muons() {

}

void Muons::defineBranch(TTree* tree) {

  tree->Branch("nMuon",    &nMuon,      "nMuon/I");
  tree->Branch("muPt",    muPt,     "muPt[nMuon]/F");
  tree->Branch("muEta",   muEta,    "muEta[nMuon]/F");
  tree->Branch("muPhi",   muPhi,    "muPhi[nMuon]/F");
  tree->Branch("muPFiso",   muPFiso,    "muPFiso[nMuon]/F");
  tree->Branch("muCharge",muCharge, "muCharge[nMuon]/I");
  tree->Branch("muisSoft", muisSoft,  "muisSoft[nMuon]/O");
  tree->Branch("muisLoose", muisLoose,  "muisLoose[nMuon]/O");
  tree->Branch("muisTight", muisTight,  "muisTight[nMuon]/O");

}


bool Muons::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  iEvent.getByLabel(pvLabel_, pvHandle);

  if(!pvHandle.isValid()) {
    std::cout << " - No Primary Vertex" << std::endl;
    return true;
  }
  size_t nPV=pvHandle->size();
  std::cout<<"Number of primary vertices: "<<nPV<<std::endl;
  const reco::Vertex* vertex=&pvHandle->front();

  edm::Handle<std::vector<pat::Muon> > muonHandle;
  iEvent.getByLabel(muLabel_, muonHandle);
  std::vector<pat::Muon> const & muons = *muonHandle;  

  nMuon = 0;
  for(std::vector<pat::Muon>::const_iterator imuon = muons.begin(); imuon!= muons.end() && nMuon < nMuonMAX ; ++imuon) {

    //    if (imuon->pt() < ptmin_) continue;
    //if (fabs(imuon->eta() ) < 2.4 ) continue;

    float pfIso = 0.;
    // (imuon.pfIsolationR04().sumChargedHadronPt + std::max(imuon.pfIsolationR04().sumNeutralHadronEt + imuon.pfIsolationR04().sumPhotonEt - 0.5*imuon.pfIsolationR04().sumPUPt, 0.) ) / imuon.pt(); // from PAT
    const pat::Muon* mu = &(*imuon);
    bool isTight = isTightMuon(mu, vertex);
    bool isSoft = isSoftMuon(mu, vertex);
    bool isLoose = isLooseMuon(mu);

    muPt    [nMuon] = imuon->pt();

    muEta   [nMuon] = imuon->eta();
    muPhi   [nMuon] = imuon->phi();
    muCharge[nMuon] = imuon->charge();    
    muPFiso[nMuon]  = pfIso;
    muisSoft[nMuon]  = isSoft;
    muisLoose[nMuon]  = isLoose;
    muisTight[nMuon]  = isTight;


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

