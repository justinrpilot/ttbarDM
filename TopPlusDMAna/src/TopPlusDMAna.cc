#include "ttbarDM/TopPlusDMAna/interface/TopPlusDMAna.h"


using namespace edm;

TopPlusDMAna::TopPlusDMAna(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

  electrons = new Electrons(iConfig);
    
  outputFile_   = iConfig.getParameter<std::string>("outputFile");
  rootFile_ = new TFile(outputFile_.c_str(),"RECREATE"); // open output file to store root-trees.


}


TopPlusDMAna::~TopPlusDMAna()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TopPlusDMAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   
   
  //electrons
  electrons->analyze(iEvent, iSetup);
  
  
  


  //in the end
  tree->Fill();
  
}


// ------------ method called once each job just before starting event loop  ------------
void 
TopPlusDMAna::beginJob()
{
  tree = new TTree("Analysis","analysis tree");
  
  
  electrons->defineBranch(tree);

  
  




  
  
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TopPlusDMAna::endJob() 
{

  rootFile_->cd();
  tree->Write();
  rootFile_->Write() ;
  rootFile_->Close() ;
  
}

// ------------ method called when starting to processes a run  ------------
void 
TopPlusDMAna::beginRun(edm::Run const&, edm::EventSetup const&)
{


}

// ------------ method called when ending the processing of a run  ------------
void 
TopPlusDMAna::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
TopPlusDMAna::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
TopPlusDMAna::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TopPlusDMAna::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TopPlusDMAna);
