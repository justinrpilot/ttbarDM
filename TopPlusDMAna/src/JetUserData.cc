#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"


#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"


#include <vector>

using namespace reco;
using namespace edm;
using namespace std;


class JetUserData : public edm::EDProducer {
public:
  JetUserData( const edm::ParameterSet & );   

private:
  void produce( edm::Event &, const edm::EventSetup & );
  edm::EDGetTokenT<edm::View<reco::Candidate> > jetLabel_;
 };


JetUserData::JetUserData(const edm::ParameterSet& iConfig)
 {
  jetLabel_ = consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("jetLabel"));
  produces<vector<pat::Jet> >();
 }


void JetUserData::produce( edm::Event& iEvent, const edm::EventSetup& iSetup) {
    

  //Jets
  edm::Handle<edm::View<reco::Candidate> > jetHandle;
  iEvent.getByToken(jetLabel_, jetHandle);


  auto_ptr<vector<pat::Jet> > jetColl(new std::vector<pat::Jet>() );

  for ( edm::View<reco::Candidate>::const_iterator thisJet = jetHandle->begin(); thisJet != jetHandle->end(); ++thisJet){

  	const pat::Jet* jet = dynamic_cast< const pat::Jet* >( &(*thisJet) );


	//Individual jet operations to be added here
	

	jetColl->push_back( *jet);

  }

 iEvent.put( jetColl );

}

// ------------ method called once each job just after ending the event loop  ------------


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(JetUserData);
