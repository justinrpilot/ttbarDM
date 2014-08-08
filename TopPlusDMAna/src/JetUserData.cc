#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"


#include "DataFormats/PatCandidates/interface/Jet.h"

// dR and dPhi
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

// Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

// trigger
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h" // gives access to the (release cycle dependent) trigger object codes

#include <vector>

using namespace reco;
using namespace edm;
using namespace std;
using namespace trigger;

class JetUserData : public edm::EDProducer {
public:
  JetUserData( const edm::ParameterSet & );   

private:
  void produce( edm::Event &, const edm::EventSetup & );
  bool isMatchedWithTrigger(const pat::Jet*, trigger::TriggerObjectCollection,int&,double&,double);

  edm::EDGetTokenT<edm::View<reco::Candidate> > jetToken_;
  edm::EDGetTokenT<edm::View<reco::Vertex> >    pvToken_;

  InputTag triggerResultsLabel_, triggerSummaryLabel_;
  InputTag hltJetFilterLabel_;
  std::string hltPath_;
  double hlt2reco_deltaRmax_;
  HLTConfigProvider hltConfig;
  int triggerBit;
 };


JetUserData::JetUserData(const edm::ParameterSet& iConfig) :
   triggerResultsLabel_(iConfig.getParameter<edm::InputTag>("triggerResults")),
   triggerSummaryLabel_(iConfig.getParameter<edm::InputTag>("triggerSummary")),
   hltJetFilterLabel_  (iConfig.getParameter<edm::InputTag>("hltJetFilter")),   //trigger objects we want to match
   hltPath_            (iConfig.getParameter<std::string>("hltPath")),
   hlt2reco_deltaRmax_ (iConfig.getParameter<double>("hlt2reco_deltaRmax"))
 {
  jetToken_ = consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("jetLabel"));
  pvToken_  = consumes<edm::View<reco::Vertex> >   (iConfig.getParameter<edm::InputTag>("pv"      ));   // "offlinePrimaryVertex"

  produces<vector<pat::Jet> >();
 }


void JetUserData::produce( edm::Event& iEvent, const edm::EventSetup& iSetup) {
    

  //PV
  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  iEvent.getByToken(pvToken_, pvHandle);
  //  const reco::Vertex& PV= pvHandle->front();

  //Jets
  edm::Handle<edm::View<reco::Candidate> > jetHandle;
  iEvent.getByToken(jetToken_, jetHandle);
  auto_ptr<vector<pat::Jet> > jetColl(new std::vector<pat::Jet>() );


  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////
  // TRIGGER (this is not really needed ...)
  bool changedConfig = false;
  if (!hltConfig.init(iEvent.getRun(), iSetup, "HLT", changedConfig)) {
    std::cout << "Initialization of HLTConfigProvider failed!!" << std::endl;
    return;
  }

  if (changedConfig){
    std::cout << "the current menu is " << hltConfig.tableName() << std::endl;
    triggerBit = -1;
    for (size_t j = 0; j < hltConfig.triggerNames().size(); j++) {
      //      std::cout << "hltConfig.triggerNames()[" << j << "]: " << hltConfig.triggerNames()[j] << std::endl;
      if (TString(hltConfig.triggerNames()[j]).Contains(hltPath_)) triggerBit = j;
    }
    if (triggerBit == -1) std::cout << "HLT path not found" << std::endl;
  }

  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByLabel(triggerResultsLabel_, triggerResults);
  if (size_t(triggerBit) < triggerResults->size() )
    if (triggerResults->accept(triggerBit))
      std::cout << "event pass : " << hltPath_ << std::endl;

  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////
  // TRIGGER MATCHING
  trigger::TriggerObjectCollection JetLegObjects;

  edm::Handle<trigger::TriggerEvent> triggerSummary;
  iEvent.getByLabel(triggerSummaryLabel_, triggerSummary);

  // Results from TriggerEvent product - Attention: must look only for
  // modules actually run in this path for this event!
  const unsigned int triggerIndex(hltConfig.triggerIndex(hltPath_));
  const vector<string>& moduleLabels(hltConfig.moduleLabels(triggerIndex));
  const unsigned int moduleIndex(triggerResults->index(triggerIndex));
  for (unsigned int j=0; j<=moduleIndex; ++j) {
    const string& moduleLabel(moduleLabels[j]);
    const string  moduleType(hltConfig.moduleType(moduleLabel));
    // check whether the module is packed up in TriggerEvent product
    const unsigned int filterIndex(triggerSummary->filterIndex(InputTag(moduleLabel,"","HLT")));
    if (filterIndex<triggerSummary->sizeFilters()) {
      //      cout << " 'L3' filter in slot " << j << " - label/type " << moduleLabel << "/" << moduleType << endl;
      TString lable = moduleLabel.c_str();
      if (lable.Contains(hltJetFilterLabel_.label())) {

	const trigger::Vids& VIDS (triggerSummary->filterIds(filterIndex));
	const trigger::Keys& KEYS(triggerSummary->filterKeys(filterIndex));
	const size_type nI(VIDS.size());
	const size_type nK(KEYS.size());
	assert(nI==nK);
	const size_type n(max(nI,nK));
	//	cout << "   " << n  << " accepted TRIGGER objects found: " << endl;
	const trigger::TriggerObjectCollection& TOC(triggerSummary->getObjects());
	for (size_type i=0; i!=n; ++i) {
	  const trigger::TriggerObject& TO(TOC[KEYS[i]]);
	  JetLegObjects.push_back(TO);	  
	  //	  cout << "   " << i << " " << VIDS[i] << "/" << KEYS[i] << ": "
	  //	       << TO.id() << " " << TO.pt() << " " << TO.eta() << " " << TO.phi() << " " << TO.mass()
	  //	       << endl;
	}
      }
    }
  }
  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////



  for ( edm::View<reco::Candidate>::const_iterator thisJet = jetHandle->begin(), lastJet = jetHandle->end(); 
	thisJet != lastJet; ++thisJet){
    
    const pat::Jet* jet = dynamic_cast< const pat::Jet* >( &(*thisJet) );
    
    
    //Individual jet operations to be added here
    // trigger matched 
    int idx       = -1;
    double deltaR = -1.;
    bool isMatched2trigger = isMatchedWithTrigger(jet, JetLegObjects, idx, deltaR, hlt2reco_deltaRmax_) ;
    double hltEta = ( isMatched2trigger ? JetLegObjects[0].eta()    : 0.);
    double hltPhi = ( isMatched2trigger ? JetLegObjects[0].phi()    : 0.);
    double hltPt  = ( isMatched2trigger ? JetLegObjects[0].pt()     : 0.);
    double hltE   = ( isMatched2trigger ? JetLegObjects[0].energy() : 0.);
    
    
    //	jetColl->push_back( *jet);
    jet->addUserFloat("HLTjetEta",   hltEta);
    jet->addUserFloat("HLTjetPhi",   hltPhi);
    jet->addUserFloat("HLTjetPt",    hltPt);
    jet->addUserFloat("HLTjetE",     hltE);
    jet->addUserFloat("HLTjetDeltaR",deltaR);


  }

 iEvent.put( jetColl );

}

// ------------ method called once each job just after ending the event loop  ------------
bool
JetUserData::isMatchedWithTrigger(const pat::Jet* p, trigger::TriggerObjectCollection triggerObjects, int& index, double& deltaR, double deltaRmax = 0.2)
{
  for (size_t i = 0 ; i < triggerObjects.size() ; i++){
    float dR = sqrt(pow(triggerObjects[i].eta()-p->eta(),2)+ pow(acos(cos(triggerObjects[i].phi()-p->phi())),2)) ;
    //    std::cout << "dR: " << dR << std::endl;
    if (dR<deltaRmax) {
      deltaR = dR;
      index  = i;
      return true;
    }
  }
  return false;
}


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(JetUserData);
