/*
*\Author:  O.Iorio
*
*
*
*\version  $Id: DMBaseTreeMaker 
*/
// This analyzer dumps the edmNtuples to bare root trees
//
//
//
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TFile.h"
#include "TTree.h"

#include<vector>

//using namespace reco;
using namespace edm;
using namespace std;


class  DMTreeMaker : public edm::EDAnalyzer 
{
public:
  explicit DMTreeMaker( const edm::ParameterSet & );   

private:
  virtual void analyze(const edm::Event &, const edm::EventSetup & );
  std::vector<edm::ParameterSet > physObjects;
  std::vector<edm::InputTag > variablesFloat, variablesInt, singleFloat, singleInt;
  TTree * treesBase;
  std::vector<string> names;
  map< string , float[50] > vfloats_values;
  map< string , int[50] > vints_values;
  map< string , string > obs_to_obj;
  
  map< string , float > float_values;
  map< string , int > int_values;
  map< string , int > sizes;

  map< string , bool > got_label; 
  map< string , int > max_instances; 

  map<string, edm::Handle<std::vector<float> > > h_floats;
  map<string, edm::Handle<std::vector<int> > > h_ints;
  map<string, edm::Handle<float> > h_float;
  map<string, edm::Handle<int> >h_int;

};


DMTreeMaker::DMTreeMaker(const edm::ParameterSet& iConfig){

  physObjects = iConfig.template getParameter<std::vector<edm::ParameterSet> >("physicsObjects");
  
  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();
  
  Service<TFileService> fs;
  TFileDirectory DMTrees = fs->mkdir( "systematics_trees" );

  treesBase = new TTree("TreeBase", "TreeBase");
  
  for (;itPsets!=physObjects.end();++itPsets){ 
    int maxI = itPsets->getUntrackedParameter< int >("maxInstances",10);
    variablesFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesF"); 
    variablesInt = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesI"); 
    singleFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("singleF"); 
    singleInt = itPsets->template getParameter<std::vector<edm::InputTag> >("singleI"); 
    string namelabel = itPsets->getParameter< string >("label");
    std::vector<edm::InputTag >::const_iterator itF = variablesFloat.begin();
    std::vector<edm::InputTag >::const_iterator itI = variablesInt.begin();
    std::vector<edm::InputTag >::const_iterator itsF = singleFloat.begin();
    std::vector<edm::InputTag >::const_iterator itsI = singleInt.begin();

    stringstream max_instance_str;
    max_instance_str<<maxI;
    max_instances[namelabel]=maxI;
    string nameobs = namelabel;
    for (;itF != variablesFloat.end();++itF){
      
      string name=itF->instance()+itF->label();
      string nameshort=itF->instance();
      if(nameobs!=itF->label())cout<<" warning! label not matching the module! check if members of pset are consistent. If intentional , ignore this warning";
      cout << "nameobs "<< nameobs<< " name " << name <<" nameshort " <<nameshort << " strsizw "<< (nameshort+"["+max_instance_str.str()+"]/F").c_str()<<endl;
      treesBase->Branch(nameshort.c_str(), &vfloats_values[name],(nameshort+"["+max_instance_str.str()+"]/F").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
    }
    for (;itI != variablesInt.end();++itI){
      string name=itI->instance()+itI->label();
      string nameshort=itF->instance();
      treesBase->Branch(nameshort.c_str(), &vints_values[name],(nameshort+"["+max_instance_str.str()+"]/I").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
    }  
    names.push_back(nameobs);
    treesBase->Branch((nameobs+"_size").c_str(), &sizes[nameobs]);

    //Initialize single pset objects
    for (;itsF != singleFloat.end();++itsF){
      string name=itsF->instance()+itsF->label();
      string nameshort=itsF->instance();
      treesBase->Branch(nameshort.c_str(), &float_values[name]);
    }
    for (;itsI != singleInt.end();++itsI){
      string name=itsI->instance()+itsI->label();
      string nameshort=itsI->instance();
      treesBase->Branch(nameshort.c_str(), &int_values[name]);
    }

  }
}

void DMTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();
  
  //Part 1 taking the obs values from the edm file
  for (;itPsets!=physObjects.end();++itPsets){ 
    variablesFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesF"); 
    variablesInt = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesI"); 
    singleFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("singleF"); 
    singleInt = itPsets->template getParameter<std::vector<edm::InputTag> >("singleI"); 
    std::vector<edm::InputTag >::const_iterator itF = variablesFloat.begin();
    std::vector<edm::InputTag >::const_iterator itI = variablesInt.begin();
    std::vector<edm::InputTag >::const_iterator itsF = singleFloat.begin();
    std::vector<edm::InputTag >::const_iterator itsI = singleInt.begin();
    string namelabel = itPsets->getParameter< string >("label");
    size_t maxInstance=(size_t)max_instances[namelabel];
 
    //Vectors of floats
    for (;itF != variablesFloat.end();++itF){
      string name=itF->instance()+itF->label();
      string nameobs=itF->label();
      float tmp =1.0;
      iEvent.getByLabel(*(itF),h_floats[name]);
      for (size_t fi = 0;fi < maxInstance ;++fi){
	if(fi <h_floats[name]->size()){tmp = h_floats[name]->at(fi);}
	else { tmp = -9999.; }
	vfloats_values[name][fi]=tmp;
      }
      sizes[nameobs]=h_floats[name]->size();
    }
    
    //Vectors of ints
    for (;itI != variablesInt.end();++itI){
      string name=itI->instance()+itI->label();
      int tmp = 1;
      iEvent.getByLabel(*(itI),h_ints[name]);
      for (size_t fi = 0;fi < maxInstance;++fi){
	if(fi <h_ints[name]->size()){tmp = h_ints[name]->at(fi);}
	else { tmp = -9999.; }
	vints_values[name][fi]=tmp;
      }
    }  
    
    //Single floats/ints
    for (;itsF != singleFloat.end();++itsF){
      string name=itsF->instance()+itsF->label();
      iEvent.getByLabel(*(itsF),h_float[name]);
      float_values[name]=*h_float[name];
    }
    for (;itsI != singleInt.end();++itsI){
      string name=itsI->instance()+itsI->label();
      iEvent.getByLabel(*(itsI),h_int[name]);
      int_values[name]=*h_int[name];
    }
    
  }
  treesBase->Fill(); 
}

//DMTreeMaker::~DMTreeMaker(const edm::ParameterSet& iConfig)
// ------------ method called once each job just after ending the event loop  ------------


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(DMTreeMaker);
