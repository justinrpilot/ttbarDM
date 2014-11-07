#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "TFile.h"
#include "TTree.h"

#include<vector>

//using namespace reco;
using namespace edm;
using namespace std;


class  DMAnalysisTreeMaker : public edm::EDAnalyzer 
{
public:
  explicit DMAnalysisTreeMaker( const edm::ParameterSet & );   

private:
  virtual void analyze(const edm::Event &, const edm::EventSetup & );
  vector<string> additionalVariables(string);
  string makeName(string label,string pref,string var);
  std::vector<edm::ParameterSet > physObjects;
  std::vector<edm::InputTag > variablesFloat, variablesInt, singleFloat, singleInt;
  edm::InputTag lhes_;
  TTree * treesBase;
  map<string, TTree * > treesByCategory;
  std::vector<string> names;
  map< string , float[50] > vfloats_values;
  map< string , int[50] > vints_values;
  map< string , string > obs_to_obj;
  map< string , string > obj_to_pref;
  
  map< string , float > float_values;
  map< string , int > int_values;
  map< string , int > sizes;

  map< string , bool > got_label; 
  map< string , int > max_instances; 

  map<string, edm::Handle<std::vector<float> > > h_floats;
  map<string, edm::Handle<std::vector<int> > > h_ints;
  map<string, edm::Handle<float> > h_float;
  map<string, edm::Handle<int> >h_int;

  string mu_label, ele_label, jets_label, boosted_tops_label, met_label ; 

};


DMAnalysisTreeMaker::DMAnalysisTreeMaker(const edm::ParameterSet& iConfig){
  
  mu_label = iConfig.getParameter<std::string >("muLabel");
  ele_label = iConfig.getParameter<std::string >("eleLabel");
  jets_label = iConfig.getParameter<std::string >("jetsLabel");
  boosted_tops_label = iConfig.getParameter<std::string >("boostedTopsLabel");
  met_label = iConfig.getParameter<std::string >("metLabel");

  physObjects = iConfig.template getParameter<std::vector<edm::ParameterSet> >("physicsObjects");
  
  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();
  
  Service<TFileService> fs;
  TFileDirectory DMTrees = fs->mkdir( "systematics_trees" );

  treesBase = new TTree("TreeBase", "TreeBase");
  
  lhes_ = iConfig.getParameter<edm::InputTag>( "lhes" );
  

  for (;itPsets!=physObjects.end();++itPsets){ 
    int maxI = itPsets->getUntrackedParameter< int >("maxInstances",10);
    variablesFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesF"); 
    variablesInt = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesI"); 
    singleFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("singleF"); 
    singleInt = itPsets->template getParameter<std::vector<edm::InputTag> >("singleI"); 
    string namelabel = itPsets->getParameter< string >("label");
    string nameprefix = itPsets->getParameter< string >("prefix");
    std::vector<edm::InputTag >::const_iterator itF = variablesFloat.begin();
    std::vector<edm::InputTag >::const_iterator itI = variablesInt.begin();
    std::vector<edm::InputTag >::const_iterator itsF = singleFloat.begin();
    std::vector<edm::InputTag >::const_iterator itsI = singleInt.begin();
    
    stringstream max_instance_str;
    max_instance_str<<maxI;
    max_instances[namelabel]=maxI;
    string nameobs = namelabel;
    string prefix = nameprefix;
    for (;itF != variablesFloat.end();++itF){
      
      string name=itF->instance()+"_"+itF->label();
      string nameinstance=itF->instance();
      string nameshort=itF->instance();
      //if(nameobs!=itF->label())cout<<" warning! label not matching the module! check if members of pset are consistent. If intentional , ignore this warning";
      //cout << "nameobs "<< nameobs<< " name " << name <<" nameshort " <<nameshort << " strsizw "<< (nameshort+"["+max_instance_str.str()+"]/F").c_str()<<endl;
      treesBase->Branch(nameshort.c_str(), &vfloats_values[name],(nameshort+"["+max_instance_str.str()+"]/F").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
      obj_to_pref[nameobs] = prefix;
      cout << " branching name "<< name<< " for obs "<< nameobs << " instance "<< nameinstance << endl;
    }
    
    for (;itI != variablesInt.end();++itI){
      string name=itI->instance()+"_"+itI->label();
      string nameshort=itF->instance();
      treesBase->Branch(nameshort.c_str(), &vints_values[name],(nameshort+"["+max_instance_str.str()+"]/I").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
      obj_to_pref[nameobs] = prefix;
    }  
    
    if (variablesFloat.size()>0){
      string nameshortv = namelabel;
      vector<string> extravars = additionalVariables(nameshortv);
      for(size_t addv = 0; addv < extravars.size();++addv){
	string name = nameshortv+extravars.at(addv);
	treesBase->Branch(name.c_str(), &vfloats_values[name],(name+"["+max_instance_str.str()+"]/F").c_str());
	obs_to_obj[name] = nameobs;
      obj_to_pref[nameobs] = prefix;
      }
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
  string nameshortv= "Event";
  vector<string> extravars = additionalVariables(nameshortv);
  for(size_t addv = 0; addv < extravars.size();++addv){
    string name = nameshortv+"_"+extravars.at(addv);
    treesBase->Branch(name.c_str(), &float_values[name],(name+"/F").c_str());
  }
}

void DMAnalysisTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();

  edm::Handle<LHEEventProduct > lhes;
  iEvent.getByLabel(lhes_, lhes);

  
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
      string name=itF->instance()+"_"+itF->label();
      //string namelabel;
      float tmp =1.0;
      iEvent.getByLabel(*(itF),h_floats[name]);
      //      cout << "name "<< name <<endl;
      for (size_t fi = 0;fi < maxInstance ;++fi){
	if(fi <h_floats[name]->size()){tmp = h_floats[name]->at(fi);}
	else { tmp = -9999.; }
	//cout << " setting name "<< name<< " at instance "<< fi <<" to value "<< tmp <<endl;
	vfloats_values[name][fi]=tmp;
      }
      sizes[namelabel]=h_floats[name]->size();
      //      cout<< " size for "<< namelabel <<" is then "<< sizes[namelabel]<<endl; 
    }
    
    //Vectors of ints
    for (;itI != variablesInt.end();++itI){
      string name=itI->instance()+"_"+itI->label();
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
  //Part 2: selection and analysis-level changes
  //Electrons:
  for(int el = 0;el < max_instances[ele_label] ;++el){
    string pref = obj_to_pref[ele_label];
    float pt = vfloats_values[makeName(ele_label,pref,"Pt")][el];
    float eta = vfloats_values[makeName(ele_label,pref,"Eta")][el];
    bool isTightElectron = pt>30 && fabs(eta)<2.1;
    bool isLooseElectron= pt>20;
    //    cout << " ele_label "<< ele_label <<  makeName(ele_label,pref,"Pt") << " eta is "<< endl;
    //cout <<" makename with pt "<<   makeName(ele_label,pref,"Pt")<<endl;
    //    pt = pt+1.;
    if(isTightElectron){
      ++float_values["Event_nTightElectrons"];
    }
    if(isLooseElectron){
      ++float_values["Event_nLooseElectrons"]; 
    }
  } 
  for(int el = 0;el < max_instances[mu_label] ;++el){
    string pref = obj_to_pref[mu_label];
    float pt = vfloats_values[makeName(mu_label,pref,"Pt")][el];
    float eta = vfloats_values[makeName(mu_label,pref,"Eta")][el];
    bool isTightMuon = pt>30 && fabs(eta)<2.1;
    bool isLooseMuon= pt>20;
    //    cout << " ele_label "<< mu_label <<  makeName(mu_label,pref,"Pt") << " eta is "<< endl;
    //cout <<" makename with pt "<<   makeName(ele_label,pref,"Pt")<<endl;
    //    pt = pt+1.;
    if(isTightMuon){
      ++float_values["Event_nTightMuons"];
    }
    if(isLooseMuon){
      ++float_values["Event_nLooseMuons"]; 
    }
  }
  //Jets:
  for(int el = 0;el < max_instances[jets_label] ;++el){
    string pref = obj_to_pref[jets_label];
    float pt = vfloats_values[makeName(jets_label,pref,"Pt")][el];
    float eta = vfloats_values[makeName(jets_label,pref,"Eta")][el];
    bool isTightJet = pt>40 && fabs(eta)<4.7;
    bool isTightJetCSVT = isTightJet && (vfloats_values[makeName(jets_label,pref,"CSV")][el]>0.7);
    //    bool isTightJetCSVT = isTightJet && (vfloats_values[makeName(jets_label,pref,"IsCSVT")][el]>0.);
    bool isLooseJet= pt>20;
    //    cout << " jets_label "<< jets_label <<  makeName(jets_label,pref,"Pt") << " eta is "<< endl;
    //cout <<" makename with pt "<<   makeName(jets_label,pref,"Pt")<<endl;
    //    pt = pt+1.;
    if(isTightJet){
      ++float_values["Event_nTightJets"];
      if(isTightJetCSVT)++float_values["Event_nCSVTJets"];
    }
    if(isLooseJet){
      ++float_values["Event_nLooseJets"]; 
    }
  } 
  float weightsign = lhes->hepeup().XWGTUP;
  float LHEWeightSign = weightsign/fabs(weightsign);
  float_values["Event_LHEWeightSign"]=LHEWeightSign;

  //Part 3: filling the additional variables
  treesBase->Fill(); 
  //Reset event weights/#objects
  string nameshortv= "Event";
  vector<string> extravars = additionalVariables(nameshortv);
  for(size_t addv = 0; addv < extravars.size();++addv){
    string name = nameshortv+"_"+extravars.at(addv);
    float_values[name]=0;
  }
      
}
string DMAnalysisTreeMaker::makeName(string label,string pref,string var){
  return pref+var+"_"+label;
}

vector<string> DMAnalysisTreeMaker::additionalVariables(string object){
  vector<string> addvar;
  bool ismuon=object.find("muon")!=std::string::npos;
  bool iselectron=object.find("electron")!=std::string::npos;
  bool ismet=object.find("met")!=std::string::npos;
  bool isjet=object.find("jet")!=std::string::npos;
  bool isevent=object.find("Event")!=std::string::npos;
  if(ismuon || iselectron){
    addvar.push_back("SFTrigger");
    addvar.push_back("SFReco");
    addvar.push_back("isQCD");
    addvar.push_back("isTightOffline");
    addvar.push_back("isLooseOffline");
  }
  if(ismet){
    addvar.push_back("CorrPt");
    addvar.push_back("CorrPhi");
  }
  if(isjet){
    addvar.push_back("CorrPt");
    addvar.push_back("CorrEta");
    addvar.push_back("CorrPhi");
    addvar.push_back("CorrE");
    addvar.push_back("CorrMass");
    addvar.push_back("CorrNJets");
    addvar.push_back("CorrPartonFlavour");
  }
  if(isevent){
    addvar.push_back("nTightMuons");
    addvar.push_back("nLooseMuons");
    addvar.push_back("nTightElectrons");
    addvar.push_back("nLooseElectrons");
    addvar.push_back("nElectronsSF");
    
    addvar.push_back("nMuonsSF");
    addvar.push_back("nCSVTJets");
    addvar.push_back("nCSVMJets");
    addvar.push_back("nCSVLJets");
    addvar.push_back("nTightJets");
    addvar.push_back("nLooseJets");
    addvar.push_back("bWeight1TCSVT");
    addvar.push_back("bWeight1TCSVM");
    addvar.push_back("bWeight1TCSVL");
    addvar.push_back("bWeight2TCSVT");
    addvar.push_back("bWeight2TCSVM");
    addvar.push_back("bWeight2TCSVL");
    addvar.push_back("LHEWeightSign");
  }
  return addvar;
}
//DMAnalysisTreeMaker::~DMAnalysisTreeMaker(const edm::ParameterSet& iConfig)
// ------------ method called once each job just after ending the event loop  ------------


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(DMAnalysisTreeMaker);
