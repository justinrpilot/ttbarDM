#include "ttbarDM/TopPlusDMAna/interface/Jets.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

using namespace reco;
using namespace edm;

Jets::Jets(const edm::ParameterSet& iConfig):
  //  m_electronSrc = iConfig.getUntrackedParameter<edm::InputTag>("electron",edm::InputTag("selectedPatJets"));
  jetLabel_(iConfig.getParameter<edm::InputTag>("jetLabel"))
{
   Service<TFileService> fs;
   tree = fs->make<TTree>("Jets","Jets");
}

Jets::~Jets() {

}

void Jets::beginJob() {
  
  tree->Branch("nJet",&nJet, "nJet/I");
  tree->Branch("jetPt",jetPt,"jetPt[nJet]/D");
  tree->Branch("jetEta",jetEta,"jetEta[nJet]/D");
  tree->Branch("jetPhi",jetPhi,"jetPhi[nJet]/D");
  tree->Branch("jetCSV",jetPhi,"jetCSV[nJet]/D");
  tree->Branch("jetCharge",jetCharge,"jetCharge[nJet]/I");

}


void Jets::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  edm::Handle<std::vector<pat::Jet> > jetHandle; //to be replaced with PAT
  iEvent.getByLabel(jetLabel_, jetHandle);
  std::vector<pat::Jet> const & jet = *jetHandle;

  nJet = 0;

  for(std::vector<pat::Jet>::const_iterator jj = jet.begin(); jj!= jet.end() && nJet < nJetMAX ; ++jj){
    jetPt[nJet] = jj->pt();
    jetEta[nJet] = jj->eta();
    jetPhi[nJet] = jj->phi();
    jetCSV[nJet] = jj->bDiscriminator("combinedSecondaryVertexBJetTags");
    jetCharge[nJet]  = jj->charge();
    jetPartonFlavour[nJet]  = jj->partonFlavour();
    nJet++;
  }
  
  tree->Fill();

 }

// ------------ method called once each job just after ending the event loop  ------------
void 
Jets::endJob() 
{

   tree->Write();
   
}

DEFINE_FWK_MODULE(Jets);








