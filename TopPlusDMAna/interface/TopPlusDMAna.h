// system include files
#include <memory>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "ttbarDM/TopPlusDMAna/interface/Electrons.h"
#include "ttbarDM/TopPlusDMAna/interface/Muons.h"



///ROOT
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

///c++
#include <boost/foreach.hpp>
#include <vector>
#include <ext/algorithm>
#include <numeric>
#include <iterator>
#include <algorithm>


class TopPlusDMAna : public edm::EDAnalyzer {
 public:
  explicit TopPlusDMAna(const edm::ParameterSet&);
  ~TopPlusDMAna();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


  Electrons* electrons; 
  Muons*     muons; 


 private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  // ----------member data ---------------------------
  
  //OUTPUT
  TFile* rootFile_;
  TTree *tree;
  std::string outputFile_;
  
      

};


