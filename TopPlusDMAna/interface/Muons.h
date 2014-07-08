#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// Muons
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

class Muons  : public edm::EDAnalyzer {
 public:

  explicit Muons(const edm::ParameterSet&);
  ~Muons();

   private:

  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;  
  
  edm::InputTag muLabel_;
  edm::InputTag pvLabel_;

  TTree *tree;
  
  static const int nMuonMAX = 100;
  int nMuon;
  double muPt[nMuonMAX];
  double muEta[nMuonMAX];
  double muPhi[nMuonMAX];
  int muCharge[nMuonMAX];
  double muPFiso[nMuonMAX];
  bool muisSoft[nMuonMAX];
  bool muisLoose[nMuonMAX];
  bool muisTight[nMuonMAX];

  
  
};

