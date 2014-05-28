#ifndef ELECTRONS_H
#define ELECTRONS_H

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
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"


#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

class Electrons {
 public:

  Electrons(const edm::ParameterSet&);
  virtual ~Electrons();

  void defineBranch(TTree* tree);
  bool analyze(const edm::Event&, const edm::EventSetup&);
  edm::ESHandle<TransientTrackBuilder> trackBuilder_;
  

 private:

  edm::InputTag m_electronSrc;
  
  
  static const int nElectronMAX = 100;
  int nElectron;
  float electronpt[nElectronMAX];
  float electroneta[nElectronMAX];
  float electronphi[nElectronMAX];
  int electroncharge[nElectronMAX];

  
  
};

#endif
