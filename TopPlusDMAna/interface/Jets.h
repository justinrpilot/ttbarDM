#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


//Pat Jets
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

class Jets : public edm::EDAnalyzer  {
 
 public:
  explicit Jets(const edm::ParameterSet&);
  ~Jets();

  
 private:

  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;  

  edm::InputTag jetLabel_;
  
  TTree *tree;

  static const int nJetMAX = 100;
  int nJet;
  double jetPt[nJetMAX];
  double jetEta[nJetMAX];
  double jetPhi[nJetMAX];
  double jetCSV[nJetMAX];
  int jetCharge[nJetMAX];
  int jetPartonFlavour[nJetMAX];

  
  
};

