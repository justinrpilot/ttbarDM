#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

//GenParticle
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

class GenParticles  : public edm::EDAnalyzer {
 public:

  explicit GenParticles(const edm::ParameterSet&);
  ~GenParticles();

  
 private:

  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;  
  
  TTree *tree;
  
  edm::InputTag genLabel_;
  
  
  static const int nGenMAX = 100;
  int nGen;
  double genPartPx[nGenMAX];
  double genPartPy[nGenMAX];
  double genPartPz[nGenMAX];
  double genPartE[nGenMAX];
  int genPartID[nGenMAX];
  int genPartStatus[nGenMAX];
  int genPartMomID[nGenMAX];
  
};

