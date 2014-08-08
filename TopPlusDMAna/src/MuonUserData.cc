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

// trigger
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h" // gives access to the (release cycle dependent) trigger object codes



#include<vector>

using namespace reco;
using namespace edm;
using namespace std;
using namespace trigger;

class  MuonUserData : public edm::EDProducer {
public:
  MuonUserData( const edm::ParameterSet & );   

private:
  void produce( edm::Event &, const edm::EventSetup & );
  bool isMatchedWithTrigger(const pat::Muon, trigger::TriggerObjectCollection,int&,double&,double);

  InputTag muLabel_, pvLabel_;
  InputTag triggerResultsLabel_, triggerSummaryLabel_;
  InputTag hltMuonFilterLabel_;
  std::string hltPath_;
  double hlt2reco_deltaRmax_;
  HLTConfigProvider hltConfig;
  int triggerBit;
 };


MuonUserData::MuonUserData(const edm::ParameterSet& iConfig):
   muLabel_(iConfig.getParameter<edm::InputTag>("muonLabel")),
   pvLabel_(iConfig.getParameter<edm::InputTag>("pv")),   // "offlinePrimaryVertex"
   triggerResultsLabel_(iConfig.getParameter<edm::InputTag>("triggerResults")),
   triggerSummaryLabel_(iConfig.getParameter<edm::InputTag>("triggerSummary")),
   hltMuonFilterLabel_ (iConfig.getParameter<edm::InputTag>("hltMuonFilter")),   //trigger objects we want to match
   hltPath_            (iConfig.getParameter<std::string>("hltPath")),
   hlt2reco_deltaRmax_ (iConfig.getParameter<double>("hlt2reco_deltaRmax"))
 {
  produces<std::vector<pat::Muon> >();
 }

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
void MuonUserData::produce( edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  //PV
  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  iEvent.getByLabel(pvLabel_, pvHandle);
  const reco::Vertex& PV= pvHandle->front();

  //Muons
  edm::Handle<std::vector<pat::Muon> > muonHandle;
  iEvent.getByLabel(muLabel_, muonHandle);
  auto_ptr<vector<pat::Muon> > muonColl( new vector<pat::Muon> (*muonHandle) );

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
  trigger::TriggerObjectCollection MuonLegObjects;

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
      if (lable.Contains(hltMuonFilterLabel_.label())) {

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
	  MuonLegObjects.push_back(TO);	  
	  //	  cout << "   " << i << " " << VIDS[i] << "/" << KEYS[i] << ": "
	  //	       << TO.id() << " " << TO.pt() << " " << TO.eta() << " " << TO.phi() << " " << TO.mass()
	  //	       << endl;
	}
      }
    }
  }
  //  std::cout << "----> MuonLegObjects: " << MuonLegObjects.size() << " <--> RECO : " << muonColl->size() << std::endl;
  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////  /////////
  for (size_t i = 0; i< muonColl->size(); i++){
    pat::Muon & m = (*muonColl)[i];

    // muon ID
    bool isTightMuon = m.isTightMuon(PV);
    bool isSoftMuon  = m.isSoftMuon(PV) ;
    
    // impact parameters
    double d0    = m.dB ();
    double d0err = m.edB();
    double dz    = fabs(m.vz()-PV.z());

    // isolation (delta beta corrections)
    // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Accessing_PF_Isolation_from_reco
    double sumChargedHadronPt = m.pfIsolationR04().sumChargedHadronPt;
    double sumNeutralHadronPt = m.pfIsolationR04().sumNeutralHadronEt;
    double sumPhotonPt        = m.pfIsolationR04().sumPhotonEt;
    double sumPUPt            = m.pfIsolationR04().sumPUPt;
    double pt                 = m.pt();
    double iso04 = sumChargedHadronPt+TMath::Max(0.,sumNeutralHadronPt+sumPhotonPt-0.5*sumPUPt)/pt;

    // trigger matched 
    int idx       = -1;
    double deltaR = -1.;
    bool isMatched2trigger = isMatchedWithTrigger(m, MuonLegObjects, idx, deltaR, hlt2reco_deltaRmax_) ;
    double hltEta = ( isMatched2trigger ? MuonLegObjects[0].eta()    : -999.);
    double hltPhi = ( isMatched2trigger ? MuonLegObjects[0].phi()    : -999.);
    double hltPt  = ( isMatched2trigger ? MuonLegObjects[0].pt()     : -999.);
    double hltE   = ( isMatched2trigger ? MuonLegObjects[0].energy() : -999.);
    
    m.addUserFloat("isSoftMuon",  isSoftMuon);
    m.addUserFloat("isTightMuon", isTightMuon);
    m.addUserFloat("d0",          d0);
    m.addUserFloat("d0err",       d0err);
    m.addUserFloat("dz",          dz);
    m.addUserFloat("iso04",       iso04);

    m.addUserFloat("HLTmuonEta",   hltEta);
    m.addUserFloat("HLTmuonPhi",   hltPhi);
    m.addUserFloat("HLTmuonPt",    hltPt);
    m.addUserFloat("HLTmuonE",     hltE);
    m.addUserFloat("HLTmuonDeltaR",deltaR);

    /*
    //     **** DEBUG dB ****
    double dB_     = m.dB ();
    double dB_PV2D = m.dB (pat::Muon::PV2D);
    double dB_PV3D = m.dB (pat::Muon::PV3D);
    double dB_BS2D = m.dB (pat::Muon::BS2D);
    double dB_BS3D = m.dB (pat::Muon::BS3D);

    m.addUserFloat("dB",    dB_);
    m.addUserFloat("dBPV2D",dB_PV2D);
    m.addUserFloat("dBPV3D",dB_PV3D);
    m.addUserFloat("dBBS2D",dB_BS2D);
    m.addUserFloat("dBBS3D",dB_BS3D);
    */
  }

 iEvent.put( muonColl );

}

// ------------ method called once each job just after ending the event loop  ------------

bool
MuonUserData::isMatchedWithTrigger(const pat::Muon p, trigger::TriggerObjectCollection triggerObjects, int& index, double& deltaR, double deltaRmax = 0.1)
{
  for (size_t i = 0 ; i < triggerObjects.size() ; i++){
    float dR = sqrt(pow(triggerObjects[i].eta()-p.eta(),2)+ pow(acos(cos(triggerObjects[i].phi()-p.phi())),2)) ;
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
DEFINE_FWK_MODULE(MuonUserData);
