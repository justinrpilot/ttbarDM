#include "ttbarDM/TopPlusDMAna/interface/Electrons.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

using namespace reco;
using namespace edm;

Electrons::Electrons(const edm::ParameterSet& iConfig):
  //  m_electronSrc = iConfig.getUntrackedParameter<edm::InputTag>("electron",edm::InputTag("selectedPatElectrons"));
  eleLabel_(iConfig.getParameter<edm::InputTag>("electronLabel"))
{
   Service<TFileService> fs;
   tree = fs->make<TTree>("Electrons","Electrons");
}

Electrons::~Electrons() {

}

void Electrons::beginJob() {
  
  tree->Branch("nElectron",&nElectron, "nElectron/I");
  tree->Branch("elePt",elePt,"elePt[nElectron]/D");
  tree->Branch("eleEta",eleEta,"eleEta[nElectron]/D");
  tree->Branch("elePhi",elePhi,"elePhi[nElectron]/D");
  tree->Branch("eleCharge",eleCharge,"eleCharge[nElectron]/I");

}


void Electrons::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  edm::Handle<std::vector<pat::Electron> > electronHandle; //to be replaced with PAT
  iEvent.getByLabel(eleLabel_, electronHandle);
  std::vector<pat::Electron> const & electrons = *electronHandle;



  nElectron = 0;

  for(std::vector<pat::Electron>::const_iterator ee = electrons.begin(); ee!= electrons.end() && nElectron < nElectronMAX ; ++ee){
    elePt[nElectron] = ee->pt();
    eleEta[nElectron] = ee->eta();
    elePhi[nElectron] = ee->phi();
    eleCharge[nElectron]  = ee->charge();
    nElectron++;
  }
  
  tree->Fill();
  
}

// ------------ method called once each job just after ending the event loop  ------------
void 
Electrons::endJob() 
{

   tree->Write();
   
}




DEFINE_FWK_MODULE(Electrons);



