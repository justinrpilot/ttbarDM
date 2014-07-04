#include "ttbarDM/TopPlusDMAna/interface/GenParticles.h"
#include <Math/VectorUtil.h>
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

using namespace reco;
using namespace edm;

GenParticles::GenParticles(const edm::ParameterSet& iConfig):
  //  m_electronSrc = iConfig.getUntrackedParameter<edm::InputTag>("electron",edm::InputTag("selectedPatGenParticles"));
  genLabel_(iConfig.getParameter<edm::InputTag>("genLabel"))
{
   Service<TFileService> fs;
   tree = fs->make<TTree>("GenParticles","GenParticles");
}

GenParticles::~GenParticles() {

}

void GenParticles::beginJob() {
 
  // WIMP
  tree->Branch("nGen",&nGen, "nGen/I");
  tree->Branch("genPartPx",genPartPx, "genPartPx[nGen]/D");
  tree->Branch("genPartPy",genPartPy, "genPartPy[nGen]/D");
  tree->Branch("genPartPz",genPartPz, "genPartPz[nGen]/D");
  tree->Branch("genPartE", genPartE, "genPartE[nGen]/D");
  tree->Branch("genPartID", genPartPz, "genPartID[nGen]/I");
  tree->Branch("genPartStatus", genPartStatus, "genPartStatus[nGen]/I");
  tree->Branch("genPartMomID", genPartMomID, "genPartMomID[nGen]/I");

}


void GenParticles::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    
  edm::Handle<std::vector<reco::GenParticle> > genHandle; //to be replaced with PAT
  iEvent.getByLabel(genLabel_, genHandle);
  std::vector<reco::GenParticle> const & gens = *genHandle;

  nGen = 0;

  for(std::vector<reco::GenParticle>::const_iterator gg = gens.begin(); gg!= gens.end() && nGen < nGenMAX ; ++gg){
        if(gg->status() == 3){
          genPartPx[nGen] = gg->px();
          genPartPy[nGen] = gg->py();
          genPartPz[nGen] = gg->pz();
          genPartE[nGen]  = gg->energy();
          genPartID[nGen]  = gg->pdgId();
          genPartStatus[nGen]  = gg->status();
          if(gg->numberOfMothers()==0)genPartMomID[nGen] = 0;
	  else genPartMomID[nGen] = gg->mother(0)->pdgId();
	  nGen++;
      }
  }
  
 tree->Fill();

 }

// ------------ method called once each job just after ending the event loop  ------------
void 
GenParticles::endJob() 
{

   tree->Write();
   
}


DEFINE_FWK_MODULE(GenParticles);
