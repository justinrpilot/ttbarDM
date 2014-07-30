#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"


// dR and dPhi                                                                                                                                                              
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

// Muons
//#include "ttbarDM/TopPlusDMAna/interface/Muons.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonPFIsolation.h"

// Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include<vector>

using namespace reco;
using namespace edm;
using namespace std;


class  MuonUserData : public edm::EDProducer {
public:
  MuonUserData( const edm::ParameterSet & );   

private:
  void produce( edm::Event &, const edm::EventSetup & );
  InputTag muLabel_, pvLabel_;
 };


MuonUserData::MuonUserData(const edm::ParameterSet& iConfig):
   muLabel_(iConfig.getParameter<edm::InputTag>("muonLabel")),
   pvLabel_(iConfig.getParameter<edm::InputTag>("pv"))   // "offlinePrimaryVertex"
 {
  produces<std::vector<pat::Muon> >();
 }


void MuonUserData::produce( edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  //PV
  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  iEvent.getByLabel(pvLabel_, pvHandle);

  //Muons
  edm::Handle<std::vector<pat::Muon> > muonHandle;
  iEvent.getByLabel(muLabel_, muonHandle);

  //if(!pvHandle.isValid()) {
    //    std::cout << " - No Primary Vertex" << std::endl;
  //}
  //size_t nPV=pvHandle->size();
  //std::cout<<"Number of primary vertices: "<<nPV<<std::endl;
  
  const reco::Vertex& PV= pvHandle->front();

  auto_ptr<vector<pat::Muon> > muonColl( new vector<pat::Muon> (*muonHandle) );

  for (unsigned int i = 0; i< muonColl->size();++i){
    pat::Muon & m = (*muonColl)[i];

    // muon ID
    bool isTightMuon = m.isTightMuon(PV);
    bool isSoftMuon  = m.isSoftMuon(PV) ;
    
    // impact parameters
    double d0    = m.dB (pat::Muon::PV2D);
    double d0err = m.edB(pat::Muon::PV2D);
    double dz    = fabs(m.vz()-PV.z());

    // isolation (delta beta corrections)
    // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Accessing_PF_Isolation_from_reco
    double sumChargedHadronPt = m.pfIsolationR04().sumChargedHadronPt;
    double sumNeutralHadronPt = m.pfIsolationR04().sumNeutralHadronEt;
    double sumPhotonPt        = m.pfIsolationR04().sumPhotonEt;
    double sumPUPt            = m.pfIsolationR04().sumPUPt;
    double pt                 = m.pt();
    double iso04 = sumChargedHadronPt+TMath::Max(0.,sumNeutralHadronPt+sumPhotonPt-0.5*sumPUPt)/pt;

    m.addUserFloat("isSoftMuon",  isSoftMuon);
    m.addUserFloat("isTightMuon", isTightMuon);
    m.addUserFloat("d0",          d0);
    m.addUserFloat("d0err",       d0err);
    m.addUserFloat("dz",          dz);
    m.addUserFloat("iso04",       iso04);

  }

 iEvent.put( muonColl );

}

// ------------ method called once each job just after ending the event loop  ------------


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(MuonUserData);
