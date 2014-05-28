#include "ttbarDM/TopPlusDMAna/interface/Electrons.h"

using namespace reco;


Electrons::Electrons(const edm::ParameterSet& iConfig){
  m_electronSrc = iConfig.getUntrackedParameter<edm::InputTag>("electron",edm::InputTag("gsfElectrons"));
}

Electrons::~Electrons() {

}

void Electrons::defineBranch(TTree* tree) {
  
  tree->Branch("nElectron",&nElectron, "nElectron/I");
  tree->Branch("electronpt",electronpt,"electronpt[nElectron]/F");
  tree->Branch("electroneta",electroneta,"electroneta[nElectron]/F");
  tree->Branch("electronphi",electronphi,"electronphi[nElectron]/F");
  tree->Branch("electroncharge",electroncharge,"electroncharge[nElectron]/I");

}


bool Electrons::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  edm::Handle<reco::GsfElectronCollection> electrons; //to be replaced with PAT
  iEvent.getByLabel(m_electronSrc, electrons);
  

  nElectron = 0;
  for(GsfElectronCollection::const_iterator jj = electrons->begin(); jj< electrons->end() && nElectron < nElectronMAX ; jj++){
    electronpt[nElectron] = jj->pt();
    electroneta[nElectron] = jj->eta();
    electronphi[nElectron] = jj->phi();
    electroncharge[nElectron]  = jj->charge();
        
    
    
    nElectron ++; 
  }
  
  
  return true;
}









