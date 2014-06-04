#ifndef MUONS_H
#define MUONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"

// Muons
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

class Muons {
 public:

  Muons(const edm::ParameterSet&);
  virtual ~Muons();

  void defineBranch(TTree* tree);
  bool analyze(const edm::Event&, const edm::EventSetup&);

 private:
  
  bool isLooseMuon(const pat::Muon*);
  bool isSoftMuon (const pat::Muon*, const reco::Vertex*);
  bool isTightMuon(const pat::Muon*, const reco::Vertex*);
  
  edm::InputTag muLabel_;
  edm::InputTag pvLabel_;
  float ptmin_;
  
  
  static const int nMuonMAX = 100;
  int nMuon;
  float muPt[nMuonMAX];
  float muEta[nMuonMAX];
  float muPhi[nMuonMAX];
  int muCharge[nMuonMAX];
  float muPFiso[nMuonMAX];
  bool muisSoft[nMuonMAX];
  bool muisLoose[nMuonMAX];
  bool muisTight[nMuonMAX];

  
  
};

#endif
