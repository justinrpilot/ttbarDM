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

    bool isTightMuon = 0;
    bool isSoftMuon  = 0;
    bool isLooseMuon = 0;
   
       
    isTightMuon = m.isTightMuon(PV);
    isSoftMuon  = m.isSoftMuon(PV);
    isLooseMuon = m.isLooseMuon();
    
    m.addUserFloat("isSoftMuon",  isSoftMuon);
    m.addUserFloat("isLooseMuon", isLooseMuon);
    m.addUserFloat("isTightMuon", isTightMuon);

  }

 iEvent.put( muonColl );

}

// ------------ method called once each job just after ending the event loop  ------------


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(MuonUserData);
