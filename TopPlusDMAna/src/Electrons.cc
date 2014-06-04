#include "ttbarDM/TopPlusDMAna/interface/Electrons.h"

using namespace reco;
using namespace edm;

Electrons::Electrons(const edm::ParameterSet& iConfig):
  //  m_electronSrc = iConfig.getUntrackedParameter<edm::InputTag>("electron",edm::InputTag("selectedPatElectrons"));
  eleLabel_(iConfig.getParameter<edm::InputTag>("electronLabel"))
{
}

Electrons::~Electrons() {

}

void Electrons::defineBranch(TTree* tree) {
  
  tree->Branch("nElectron",&nElectron, "nElectron/I");
  tree->Branch("elePt",elePt,"elePt[nElectron]/F");
  tree->Branch("eleEta",eleEta,"eleEta[nElectron]/F");
  tree->Branch("elePhi",elePhi,"elePhi[nElectron]/F");
  tree->Branch("eleCharge",eleCharge,"eleCharge[nElectron]/I");

}


bool Electrons::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  edm::Handle<std::vector<pat::Electron> > electronHandle; //to be replaced with PAT
  iEvent.getByLabel(eleLabel_, electronHandle);
  std::vector<pat::Electron> const & electrons = *electronHandle;

  nElectron = 0;

  for(std::vector<pat::Electron>::const_iterator ee = electrons.begin(); ee!= electrons.end() && nElectron < nElectronMAX ; ee++){
    elePt[nElectron] = ee->pt();
    eleEta[nElectron] = ee->eta();
    elePhi[nElectron] = ee->phi();
    eleCharge[nElectron]  = ee->charge();
    nElectron++;
  }
  
  
  return true;
}









