#include "ttbarDM/TopPlusDMAna/interface/Muons.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

// dR and dPhi                                                                                                                                                                       
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

// Muons
#include "DataFormats/MuonReco/interface/MuonPFIsolation.h"

// Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

using namespace reco;
using namespace edm;

Muons::Muons(const edm::ParameterSet& iConfig):
  //edm::ParameterSet conf = iConfig.getParameter<edm::ParameterSet>("muonBlock");
  muLabel_(iConfig.getParameter<edm::InputTag>("muonLabel")),
  pvLabel_(iConfig.getParameter<edm::InputTag>("pv"))   // "offlinePrimaryVertex"
 {
   Service<TFileService> fs;
   tree = fs->make<TTree>("Muons","Muons");
 }

Muons::~Muons() {

}

void 
Muons::beginJob() {
 
  tree->Branch("nMuon",    &nMuon,      "nMuon/I");
  tree->Branch("muPt",    muPt,     "muPt[nMuon]/D");
  tree->Branch("muEta",   muEta,    "muEta[nMuon]/D");
  tree->Branch("muPhi",   muPhi,    "muPhi[nMuon]/D");
  tree->Branch("muPFiso",   muPFiso,    "muPFiso[nMuon]/D");
  tree->Branch("muCharge",muCharge, "muCharge[nMuon]/I");
  tree->Branch("muisSoft", muisSoft,  "muisSoft[nMuon]/O");
  tree->Branch("muisLoose", muisLoose,  "muisLoose[nMuon]/O");
  tree->Branch("muisTight", muisTight,  "muisTight[nMuon]/O");

}


void Muons::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  //PV
  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  iEvent.getByLabel(pvLabel_, pvHandle);

  //if(!pvHandle.isValid()) {
    //    std::cout << " - No Primary Vertex" << std::endl;
  //}
  //size_t nPV=pvHandle->size();
  //std::cout<<"Number of primary vertices: "<<nPV<<std::endl;
  const reco::Vertex& PV= pvHandle->front();

  //Muons
  edm::Handle<std::vector<pat::Muon> > muonHandle;
  iEvent.getByLabel(muLabel_, muonHandle);
  std::vector<pat::Muon> const & muons = *muonHandle;  
   nMuon = 0;
  for(std::vector<pat::Muon>::const_iterator mm = muons.begin(); mm!= muons.end() && nMuon < nMuonMAX ; ++mm) {

    float pfIso = 0.;

    //const pat::Muon* mu = &(*mm);
    bool isTight = mm->isTightMuon(PV);
    //    bool isTight = mu -> isTightMuon(mu, vertex);
    bool isSoft = mm->isSoftMuon(PV);
    bool isLoose = mm->isLooseMuon();
    // std::cout<<"Muon Pt"<<mm->pt()<<std::endl;
    muPt    [nMuon] = mm->pt();
    muEta   [nMuon] = mm->eta();
    muPhi   [nMuon] = mm->phi();
    muCharge[nMuon] = mm->charge();    
    muPFiso[nMuon]  = pfIso;
    muisSoft[nMuon]  = isSoft;
    muisLoose[nMuon]  = isLoose;
    muisTight[nMuon]  = isTight;

    //std::cout<<"Filling muons"<<std::endl;
    nMuon ++; 

    // (mm.pfIsolationR04().sumChargedHadronPt + std::max(mm.pfIsolationR04().sumNeutralHadronEt + mm.pfIsolationR04().sumPhotonEt - 0.5*mm.pfIsolationR04().sumPUPt, 0.) ) / mm.pt(); // from PAT
  }
  
  tree->Fill();

}

// ------------ method called once each job just after ending the event loop  ------------
void 
Muons::endJob() 
{

   tree->Write();
   
}



DEFINE_FWK_MODULE(Muons);
