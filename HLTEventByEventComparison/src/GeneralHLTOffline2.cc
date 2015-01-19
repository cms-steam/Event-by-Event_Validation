//	GeneralHLTOffline2.cc
//
//	Module that fills a tree with trigger-by-trigger info.
//	Developed 2012-2013 by Geoffrey Smith.
//	
//	Usage: see GeneralHLTOffline2_cfi.py
//		
//

#include <memory>
#include <vector>
#include <algorithm>
#include "TH1.h"
#include "TH2.h"
#include "TH2D.h"
#include "TTree.h"
#include "TFile.h"

// user include files
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/UtilAlgos/interface/TFileDirectory.h"
// #include "CommonTools/Utils/interface/TFileDirectory.h
#include "FWCore/ServiceRegistry/interface/Service.h"
// #include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include "TMath.h"

#include "TStyle.h"

//
// class declaration
//


using std::vector;
using std::string;


class GeneralHLTOffline2 : public edm::EDAnalyzer {
   public:
      explicit GeneralHLTOffline2(const edm::ParameterSet&);
      ~GeneralHLTOffline2();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);



      // ----------member data ---------------------------


  bool debugPrint;
  bool outputPrint;
  std::string hltTag;
  
  HLTConfigProvider hltConfig_;
  
  //TH2D *genpt_vs_evt;
  TH1F *hlt_count_hist;
  TH2D *map_of_trig_to_trig_types;
  TH2D *prescale_columns;



  // -- the tree branches --
  TTree *the_tree = new TTree("the_HLT_tree_A","the_HLT_tree_A");

  char tree_path_name[500];
//  char tree_atest[];
  int tree_event;
  int tree_path_index;
  int tree_path_was_run;
  int tree_path_error;
  int tree_path_accept;
  
  
//  int tree_module_index;

  char tree_last_module_run_label[500];
  int tree_last_module_run_index;
  
  char tree_last_module_with_saved_tags_label[500];
  int tree_last_module_with_saved_tags_index;
  
  char tree_L1_module_with_saved_tags_label[500];
  int tree_L1_module_with_saved_tags_index;
  
  char tree_first_module_with_changed_objs_label[500];
  int tree_first_module_with_changed_objs_index;
  
  char tree_last_module_run_type[500];
  char tree_last_module_run_edmtype[500];
  
  int tree_L1_module_with_saved_tags_obj_ID[100];
  float tree_L1_module_with_saved_tags_obj_pt[100];
  float tree_L1_module_with_saved_tags_obj_eta[100];
  float tree_L1_module_with_saved_tags_obj_phi[100];
  
  int tree_last_module_with_saved_tags_obj_ID[100];
  float tree_last_module_with_saved_tags_obj_pt[100];
  float tree_last_module_with_saved_tags_obj_eta[100];
  float tree_last_module_with_saved_tags_obj_phi[100];
  
  int tree_first_module_with_changed_objs_obj_ID[100];
  float tree_first_module_with_changed_objs_obj_pt[100];
  float tree_first_module_with_changed_objs_obj_eta[100];
  float tree_first_module_with_changed_objs_obj_phi[100];
  
  // -----------------------
  
  int tags_saved;
  int tags_not_saved;
  int fucked_up;
  int there_were_some_sort_of_tags;
  
  int objs_check;
  
  int is_obj_there_if_key_size_is_at_least_one_obj1;
  int is_obj_there_if_key_size_is_at_least_one_obj2;
  int is_obj_there_if_key_size_is_at_least_one_obj3;
  
  int sanity_check1;
  int sanity_check2;
};



//
// constructors and destructor
//
GeneralHLTOffline2::GeneralHLTOffline2(const edm::ParameterSet& iConfig)
{
	//now do what ever initialization is needed

	debugPrint = true;
	outputPrint = false;
	
        tags_saved = 0;
	tags_not_saved = 0;
	fucked_up = 0;
	there_were_some_sort_of_tags = 0;
	objs_check = 0;
	sanity_check1 = 0;
	sanity_check2 = 0;
	
	is_obj_there_if_key_size_is_at_least_one_obj1 = 0;
	is_obj_there_if_key_size_is_at_least_one_obj2 = 0;
	is_obj_there_if_key_size_is_at_least_one_obj3 = 0;
	
	hltTag = iConfig.getParameter<std::string> ("HltProcessName");

	//hltTag = "HLT";

	edm::Service<TFileService> fs;

	TFileDirectory results = TFileDirectory( fs->mkdir("GeneralHLTOffline2") );

}


GeneralHLTOffline2::~GeneralHLTOffline2()
{

// do anything here that needs to be done at desctruction time
// (e.g. close files, deallocate resources etc.)

}



// ------------ method called for each event  ------------
void GeneralHLTOffline2::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

if (debugPrint) std::cout << "Inside analyze - run, block, event " << iEvent.id().run() << " , " << iEvent.id().luminosityBlock() << " , " << iEvent.id().event() << " , " << std::endl;


std::cout << "  " << std::endl;

edm::Handle<edm::TriggerResults> triggerResults;
//iEvent.getByLabel(edm::InputTag("TriggerResults","", "HLT"), triggerResults);
iEvent.getByLabel(edm::InputTag("TriggerResults","", hltTag), triggerResults); 		// <- can be "reHLT", etc. controlled from config file. 

edm::Handle<trigger::TriggerEvent> aodTriggerEvent;   
//iEvent.getByLabel(edm::InputTag("hltTriggerSummaryAOD", "", "HLT"), aodTriggerEvent);
iEvent.getByLabel(edm::InputTag("hltTriggerSummaryAOD", "", hltTag), aodTriggerEvent);  // <- can be "reHLT", etc. controlled from config file.

const trigger::TriggerObjectCollection objects = aodTriggerEvent->getObjects();


if (!triggerResults.isValid())
{
	std::cout << "Trigger results not valid" << std::endl;
	return;
}

std::vector<std::string> myTriggernames = hltConfig_.triggerNames();

std::cout << myTriggernames.size() << std::endl;

  

for (int j=0; j<(int)myTriggernames.size(); j++)
{
	if (tree_event==8999&&debugPrint) std::cout << "debug 14" << std::endl;
	
	unsigned int path__index = hltConfig_.triggerIndex(myTriggernames[j]);

	
	if (tree_event==8999) std::cout << "debug 15" << std::endl;
	
	if (tree_event==4000&&debugPrint)
	{
		std::cout << "trigger name iter: " << j << "path index: " << path__index  << std::endl;
		
	}
	
	if(triggerResults->accept(path__index))
	{
		hlt_count_hist->Fill(j);
	}

	if (tree_event==8999&&debugPrint) std::cout << "debug 16" << std::endl;

		
	tree_event = iEvent.id().event();
	tree_path_index = path__index;
	tree_path_was_run = triggerResults->wasrun(path__index);
	tree_path_error = triggerResults->error(path__index);
	tree_path_accept = triggerResults->accept(path__index);
	
	if (tree_event==8999&&debugPrint) std::cout << "debug 17 " << std::endl;
	
	strcpy (tree_path_name,myTriggernames[j].c_str());
	
	if (tree_event==8999&&debugPrint) std::cout << "debug 18 " << std::endl;
	
	tree_last_module_run_index = triggerResults->index(path__index);
		
	if (tree_event==8999&&debugPrint) std::cout << "debug 19 " << std::endl;
		
	std::vector<std::string> the_saved_tag_modules = hltConfig_.saveTagsModules(path__index);
	
	if (tree_event==8999&&debugPrint)
	{
		std::cout << "debug 20 " << std::endl;
		std::cout << path__index << "  " << tree_last_module_run_index << "  " << std::endl;
	}
	
	std::string mod_label = hltConfig_.moduleLabel(path__index,tree_last_module_run_index);
	
	if (tree_event==8999&&debugPrint) std::cout << mod_label << std::endl;
	
	if (tree_event==8999&&debugPrint) std::cout << "debug 21 " << std::endl;
	
	strcpy (tree_last_module_run_label,mod_label.c_str());
	
	std::string mod_type = hltConfig_.moduleType(mod_label);
	//std::string mod_edm_type = hltConfig_.moduleEDMType(mod_label);
	
	strcpy (tree_last_module_run_type,mod_type.c_str());
	//strcpy (tree_last_module_run_edmtype,mod_edm_type.c_str());
	
	
	if (tree_event==8999&&debugPrint) std::cout << "debug 22 " << std::endl;
	
	
	strcpy (tree_last_module_with_saved_tags_label,"none");
  	tree_last_module_with_saved_tags_index = -99;
  
  	strcpy (tree_L1_module_with_saved_tags_label,"none");
  	tree_L1_module_with_saved_tags_index = -99;
	
	
	if (tree_event==8999&&debugPrint) std::cout << "debug 23 " << std::endl;
	
//	std::cout << " debug: in trigger loop, " << j << std::endl;
	
	for (int obj_init=0; obj_init<100; obj_init++)
	{

		tree_L1_module_with_saved_tags_obj_ID[obj_init] = -99;
		tree_L1_module_with_saved_tags_obj_pt[obj_init] = -99.;
		tree_L1_module_with_saved_tags_obj_eta[obj_init] = -99.;
		tree_L1_module_with_saved_tags_obj_phi[obj_init] = -99.;

		tree_last_module_with_saved_tags_obj_ID[obj_init] = -99;
		tree_last_module_with_saved_tags_obj_pt[obj_init] = -99.;
		tree_last_module_with_saved_tags_obj_eta[obj_init] = -99.;
		tree_last_module_with_saved_tags_obj_phi[obj_init] = -99.;
		
		tree_first_module_with_changed_objs_obj_ID[obj_init] = -99;
  		tree_first_module_with_changed_objs_obj_pt[obj_init] = -99.;
  		tree_first_module_with_changed_objs_obj_eta[obj_init] = -99.;
  		tree_first_module_with_changed_objs_obj_phi[obj_init] = -99.;
		
  
	} 
	
	if (tree_event==8999&&debugPrint) std::cout << "debug 24 " << std::endl;
	
//	std::cout << " debug: initialize tree vars " << std::endl;
	
	
	if (the_saved_tag_modules.size()>0)   //// fill tree with saved module object info
	{
						
		// first loop over modules backwards to find last modules that do various things...
		for (int mods = (the_saved_tag_modules.size() - 1); mods>=0; mods--)
		{

			
			if (tree_event==8999) std::cout << mods << std::endl;

			//edm::InputTag moduleWhoseResultsWeWant(the_saved_tag_modules[mods], "", "HLT");
			edm::InputTag moduleWhoseResultsWeWant(the_saved_tag_modules[mods], "", hltTag);
			
			if (tree_event==8999) std::cout << "debug 1" << std::endl;
			
			
			strcpy (tree_last_module_with_saved_tags_label,the_saved_tag_modules[mods].c_str());			
			
			if (tree_event==8999) std::cout << "debug 2" << std::endl;
			
			tree_last_module_with_saved_tags_index = hltConfig_.moduleIndex(path__index,the_saved_tag_modules[mods]);

			if (tree_event==8999) std::cout << "debug 3" << std::endl;

			unsigned int indexOfModuleInAodTriggerEvent = aodTriggerEvent->filterIndex(moduleWhoseResultsWeWant);

			if (tree_event==8999) std::cout << "debug 4" << std::endl;
			
			if (indexOfModuleInAodTriggerEvent < aodTriggerEvent->sizeFilters())
			{

				if (tree_event==8999) std::cout << "debug 5" << std::endl;

				const trigger::Keys &keys = aodTriggerEvent->filterKeys( indexOfModuleInAodTriggerEvent );

				if (tree_event==8999) std::cout << "debug 6" << std::endl;
				
				if (objs_check<(int)keys.size()) objs_check = keys.size();
				
				for ( size_t iKey = 0; iKey < keys.size(); iKey++ )
				{
				
					if (tree_event==8999) std::cout << "debug 7" << std::endl;
					
					trigger::TriggerObject foundObject = objects[keys[iKey]];					
					
					if (tree_event==8999) std::cout << "debug 8" << std::endl;

					int object_number = iKey;
					
					if (tree_event==8999) std::cout << "debug 9" << std::endl;

					tree_last_module_with_saved_tags_obj_ID[object_number] = foundObject.id();
					tree_last_module_with_saved_tags_obj_pt[object_number] = foundObject.pt();
					tree_last_module_with_saved_tags_obj_eta[object_number] = foundObject.eta();
					tree_last_module_with_saved_tags_obj_phi[object_number] = foundObject.phi();
					
					//if ((iKey==0)&&(foundObject.pt()<0)) is_obj_there_if_key_size_is_at_least_one++;
					
					if ((foundObject.id()!=-99)&&(foundObject.pt()<0.)) sanity_check1++;

				}

				

				if (tree_last_module_with_saved_tags_obj_pt[2]!=-99.) is_obj_there_if_key_size_is_at_least_one_obj3++;
				if (tree_last_module_with_saved_tags_obj_pt[1]!=-99.) is_obj_there_if_key_size_is_at_least_one_obj2++;
				if (tree_last_module_with_saved_tags_obj_pt[0]!=-99.)
				{
					is_obj_there_if_key_size_is_at_least_one_obj1++;
					break;
				}

				if ((tree_last_module_with_saved_tags_obj_pt[0]==-99.)&&(tree_last_module_with_saved_tags_obj_pt[1]!=-99.||tree_last_module_with_saved_tags_obj_pt[2]!=-99.||tree_last_module_with_saved_tags_obj_pt[3]!=-99.||tree_last_module_with_saved_tags_obj_pt[4]!=-99.)) sanity_check2++; 

			}			
			
		}


		int numL1objs = 0;

		if (tree_event==8999) std::cout << "debug 10" << std::endl;


		//edm::InputTag moduleWithL1Seed(the_saved_tag_modules[0], "", "HLT");
		edm::InputTag moduleWithL1Seed(the_saved_tag_modules[0], "", hltTag);
		strcpy (tree_L1_module_with_saved_tags_label,the_saved_tag_modules[0].c_str());			
		tree_L1_module_with_saved_tags_index = hltConfig_.moduleIndex(path__index,the_saved_tag_modules[0]);

		if (tree_event==8999) std::cout << "debug 11" << std::endl;

		unsigned int indexOfModuleInAodTriggerEvent2 = aodTriggerEvent->filterIndex(moduleWithL1Seed);

		if (tree_event==8999) std::cout << "debug 12" << std::endl;

		if (indexOfModuleInAodTriggerEvent2 < aodTriggerEvent->sizeFilters())
		{

			const trigger::Keys &keys2 = aodTriggerEvent->filterKeys(
			indexOfModuleInAodTriggerEvent2 );

			numL1objs = keys2.size();

			for ( size_t iKey2 = 0; iKey2 < keys2.size(); iKey2++ )
			{
				trigger::TriggerObject foundObject2 = objects[keys2[iKey2]];					

				int object_number2 = iKey2;

				tree_L1_module_with_saved_tags_obj_ID[object_number2] = foundObject2.id();
				tree_L1_module_with_saved_tags_obj_pt[object_number2] = foundObject2.pt();
				tree_L1_module_with_saved_tags_obj_eta[object_number2] = foundObject2.eta();
				tree_L1_module_with_saved_tags_obj_phi[object_number2] = foundObject2.phi();
				
							
			}

		}



		// now loop over modules in correct order to find first modules that do various things...
		for (unsigned int mods=0; mods<the_saved_tag_modules.size(); mods++)
		{

			edm::InputTag moduleWhoseResultsWeWant(the_saved_tag_modules[mods], "", hltTag);
			
			strcpy (tree_first_module_with_changed_objs_label,the_saved_tag_modules[mods].c_str());			
						
			tree_first_module_with_changed_objs_index = hltConfig_.moduleIndex(path__index,the_saved_tag_modules[mods]);

			unsigned int indexOfModuleInAodTriggerEvent = aodTriggerEvent->filterIndex(moduleWhoseResultsWeWant);
			
			if (indexOfModuleInAodTriggerEvent < aodTriggerEvent->sizeFilters())
			{

				const trigger::Keys &keys3 = aodTriggerEvent->filterKeys( indexOfModuleInAodTriggerEvent );
				
				int numobjects = keys3.size();
				
				for ( size_t iKey = 0; iKey < keys3.size(); iKey++ )
				{
									
					trigger::TriggerObject foundObject = objects[keys3[iKey]];					
					
					int object_number = iKey;
					
					tree_first_module_with_changed_objs_obj_ID[object_number] = foundObject.id();
					tree_first_module_with_changed_objs_obj_pt[object_number] = foundObject.pt();
					tree_first_module_with_changed_objs_obj_eta[object_number] = foundObject.eta();
					tree_first_module_with_changed_objs_obj_phi[object_number] = foundObject.phi();
					
				}


				int maxobjs = std::max(numL1objs,numobjects);

				bool isdiff = false;

				for (int ojbs_compare=0; ojbs_compare<maxobjs; ojbs_compare++)
				{					
					if (tree_first_module_with_changed_objs_obj_ID[ojbs_compare]!=tree_L1_module_with_saved_tags_obj_ID[ojbs_compare]) isdiff = true;
					if (tree_first_module_with_changed_objs_obj_pt[ojbs_compare]!=tree_L1_module_with_saved_tags_obj_pt[ojbs_compare]) isdiff = true;
					if (tree_first_module_with_changed_objs_obj_eta[ojbs_compare]!=tree_L1_module_with_saved_tags_obj_eta[ojbs_compare]) isdiff = true;
					if (tree_first_module_with_changed_objs_obj_phi[ojbs_compare]!=tree_L1_module_with_saved_tags_obj_phi[ojbs_compare]) isdiff = true;
					
				}
				
				if (!isdiff) break;
				
				

			}			
			
		}
		
		
		if (tree_event==8999) std::cout << "debug 13" << std::endl;

	}
	

	
	///////////////////////////////////////////////////////////////
	//////////// FILL THE TREE ////////////////////////////////////
	///////////////////////////////////////////////////////////////
	the_tree->Fill();	///////////////////////////////////////    <<<------------------ NOTE that the tree is filled once per TRIGGER; not once per EVENT !!! 
	


}  // end of loop over triggers

std::cout << "end of trigger loop" << std::endl;

 
}   
   
 
void GeneralHLTOffline2::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{   
   // do something here ??  yes.
   
	edm::Service<TFileService> newfs;
  
	bool changed = true;
	
	if (hltConfig_.init(iRun, iSetup, hltTag, changed))
	{
		if(debugPrint) std::cout << "HLT config with process name " << hltTag << " successfully extracted" << std::endl;
	}
	else 
	{
		if (debugPrint) std::cout << "Warning, didn't find process HLT" << std::endl;
	}

	std::cout << " HLTConfig processName " << hltConfig_.processName() << " tableName " << hltConfig_.tableName() << " size of table (paths): " << hltConfig_.size() << std::endl;  // " globalTag: " << hltConfig_.globalTag() << std::endl;


	std::vector<std::string> myTriggernames = hltConfig_.triggerNames();

	hlt_count_hist = newfs->make<TH1F>("hlt_count_hist", "Counts/Path", myTriggernames.size(), -0.5, myTriggernames.size() - 0.5);
	//genpt_vs_evt = newfs->make<TH2D>("genpt_vs_evt","genpt_vs_evt",10000,0,10000,10000,0,100);
	map_of_trig_to_trig_types = newfs->make<TH2D>("map_of_trig_to_trig_types", "path-to-dataset map", myTriggernames.size(), -0.5, myTriggernames.size() - 0.5,50,-0.5,49.5);
	
	unsigned int num_prescale_sets = hltConfig_.prescaleSize();
	
	prescale_columns = newfs->make<TH2D>("prescale_columns", "prescale columns", num_prescale_sets, -0.5, num_prescale_sets - 0.5, myTriggernames.size(), -0.5, myTriggernames.size() - 0.5);

	for (unsigned int k=0; k<myTriggernames.size(); k++)
	{
		hlt_count_hist->GetXaxis()->SetBinLabel(k+1, myTriggernames[k].c_str());  // Geoff
		map_of_trig_to_trig_types->GetXaxis()->SetBinLabel(k+1, myTriggernames[k].c_str());
		prescale_columns->GetYaxis()->SetBinLabel(k+1, myTriggernames[k].c_str());
	}


	const std::vector<std::string> &nameStreams = hltConfig_.streamNames();
  	bool streamAfound = false;
  	std::vector<std::string>::const_iterator si = nameStreams.begin();
  	std::vector<std::string>::const_iterator se = nameStreams.end();
  	for ( ; si != se; ++si) 
	{
    		if ((*si) == "A") 
		{
      			streamAfound = true;
      			break;
    		}
  	}

  	if (streamAfound) 
	{
		const std::vector<std::string> &datasetNames =  hltConfig_.streamContent("A");
		if (debugPrint) std::cout << "Number of Stream A datasets " << datasetNames.size() << std::endl;
    		for (unsigned int i = 0; i < datasetNames.size(); i++)
		{
      			
			map_of_trig_to_trig_types->GetYaxis()->SetBinLabel(i+1,datasetNames[i].c_str());
			
			const std::vector<std::string> &datasetPaths = hltConfig_.datasetContent(datasetNames[i]);

        		for (unsigned int iPath = 0; iPath < datasetPaths.size(); iPath++)
			{
                    		map_of_trig_to_trig_types->Fill(map_of_trig_to_trig_types->GetXaxis()->FindBin(datasetPaths[iPath].c_str())-1,i);
			}
		}
        }
	
	
	
	
	for (unsigned int presc_it=0; presc_it<num_prescale_sets; presc_it++)
	{
		for (unsigned int trigs=0; trigs<myTriggernames.size(); trigs++)
		{
	
			unsigned int prescale = hltConfig_.prescaleValue(presc_it, myTriggernames[trigs]);

			prescale_columns->Fill(presc_it,trigs,prescale);
		
		}
	}
	

}
   
void GeneralHLTOffline2::endRun(edm::Run const&, edm::EventSetup const&)
{
	if (debugPrint) std::cout << " endRun called " << std::endl; 
}   
   

   
 // ------------ method called once each job just before starting event loop  ------------
void GeneralHLTOffline2::beginJob()
{
	/// add branches to the tree:

        the_tree = new TTree("the_HLT_tree_A","the_HLT_tree_A");
	
	the_tree->Branch("path_name",tree_path_name,"path_name/C");  // self explanitory

  
  	the_tree->Branch("event",&tree_event,"event/I");  // self explanitory
	
	the_tree->Branch("path_index",&tree_path_index,"path_index/I");  // self explanitory
	the_tree->Branch("path_was_run",&tree_path_was_run,"path_was_run/I");  // self explanitory
	the_tree->Branch("path_error",&tree_path_error,"path_error/I");  // self explanitory
	the_tree->Branch("path_accept",&tree_path_accept,"path_accept/I");  // self explanitory




	the_tree->Branch("last_module_run_index",&tree_last_module_run_index,"last_module_run_index/I"); // index of the trigger module that made the decision to fire or not
	the_tree->Branch("last_module_run_label",tree_last_module_run_label,"last_module_run_label/C"); // name of the trigger module that made the decision to fire or not
	
	the_tree->Branch("last_module_with_saved_tags_label",tree_last_module_with_saved_tags_label,"last_module_with_saved_tags_label/C");  // if the trigger module that made the decision== a module that had saved tags, what is its label
	the_tree->Branch("last_module_with_saved_tags_index",&tree_last_module_with_saved_tags_index,"last_module_with_saved_tags_index/I");  // if the trigger module that made the decision== a module that had saved tags, what is its index
	
	the_tree->Branch("L1_module_with_saved_tags_label",tree_L1_module_with_saved_tags_label,"L1_module_with_saved_tags_label/C");  // if the trigger module that made the decision== a L1 module that had saved tags, what is its label
	the_tree->Branch("L1_module_with_saved_tags_index",&tree_L1_module_with_saved_tags_index,"L1_module_with_saved_tags_index/I");  // if the trigger module that made the decision== a L1 module that had saved tags, what is its index
	
	the_tree->Branch("first_module_with_changed_objs_label",tree_first_module_with_changed_objs_label,"first_module_with_changed_objs_label/C");
	the_tree->Branch("first_module_with_changed_objs_index",&tree_first_module_with_changed_objs_index,"first_module_with_changed_objs_index/I");	
	
	
//	the_tree->Branch("saved_tags_object_info_available",&tree_saved_tags_object_info_available,"saved_tags_object_info_available/I"); // did we fill the tree with info about the trigger object(s) from the above module, yes or no	

	the_tree->Branch("last_module_run_type",tree_last_module_run_type,"tree_last_module_run_type/C");		// EDFilter used by last run module
//	the_tree->Branch("last_module_run_edmtype",tree_last_module_run_edmtype,"tree_last_module_run_edmtype/C");

	
	the_tree->Branch("last_module_with_saved_tags_object_obj_ID",&tree_last_module_with_saved_tags_obj_ID,"last_module_with_saved_tags_object_obj_ID[100]/I");  // ID of object(s) from saved tags module, given saved_tags_object_info_available == true above
	the_tree->Branch("last_module_with_saved_tags_object_obj_pt",&tree_last_module_with_saved_tags_obj_pt,"last_module_with_saved_tags_object_obj_pt[100]/F");  // pt of object(s) from saved tags module, given saved_tags_object_info_available == true above
	the_tree->Branch("last_module_with_saved_tags_object_obj_eta",&tree_last_module_with_saved_tags_obj_eta,"last_module_with_saved_tags_object_obj_eta[100]/F");  // eta of object(s) from saved tags module, given saved_tags_object_info_available == true above
	the_tree->Branch("last_module_with_saved_tags_object_obj_phi",&tree_last_module_with_saved_tags_obj_phi,"last_module_with_saved_tags_object_obj_phi[100]/F");  // phi of object(s) from saved tags module, given saved_tags_object_info_available == true above
	
	
	the_tree->Branch("L1_module_with_saved_tags_object_obj_ID",&tree_L1_module_with_saved_tags_obj_ID,"L1_module_with_saved_tags_object_obj_ID[100]/I");  // ID of object(s) from saved tags module, given saved_tags_object_info_available == true above
	the_tree->Branch("L1_module_with_saved_tags_object_obj_pt",&tree_L1_module_with_saved_tags_obj_pt,"L1_module_with_saved_tags_object_obj_pt[100]/F");  // pt of object(s) from saved tags module, given saved_tags_object_info_available == true above
	the_tree->Branch("L1_module_with_saved_tags_object_obj_eta",&tree_L1_module_with_saved_tags_obj_eta,"L1_module_with_saved_tags_object_obj_eta[100]/F");  // eta of object(s) from saved tags module, given saved_tags_object_info_available == true above
	the_tree->Branch("L1_module_with_saved_tags_object_obj_phi",&tree_L1_module_with_saved_tags_obj_phi,"L1_module_with_saved_tags_object_obj_phi[100]/F");
	
	
	the_tree->Branch("first_module_with_changed_objs_obj_ID", &tree_first_module_with_changed_objs_obj_ID,"first_module_with_changed_objs_obj_ID[100]/I");
	the_tree->Branch("first_module_with_changed_objs_obj_pt", &tree_first_module_with_changed_objs_obj_pt,"first_module_with_changed_objs_obj_pt[100]/F");
	the_tree->Branch("first_module_with_changed_objs_obj_eta", &tree_first_module_with_changed_objs_obj_eta,"first_module_with_changed_objs_obj_eta[100]/F");
	the_tree->Branch("first_module_with_changed_objs_obj_phi", &tree_first_module_with_changed_objs_obj_phi,"first_module_with_changed_objs_obj_phi[100]/F");
	
	
	
//	the_tree->Branch("object_number",&tree_object_number,"object_number/I");  // given saved_tags_object_info_available == true, which object are we talking about	
//	the_tree->Branch("test_float_array",&test_float_array,"test_float_array[10]/F");

}

// ------------ method called once each job just after ending the event loop  ------------
void GeneralHLTOffline2::endJob() 
{
/*
	std::cout << " endJob() " << std::endl; 
        std::cout << "modules that made the decision and had saved tags: " << tags_saved << std::endl;
	std::cout << "modules that made the decision and did not have saved tags: " << tags_not_saved << std::endl;
	std::cout << "times that something was not right: " << fucked_up << std::endl;
	std::cout << "times that there was at least one saved tag: " << there_were_some_sort_of_tags << std::endl;
*/

	std::cout << "objs_check: " << objs_check << std::endl;
	std::cout << "the following 3 numbers should be in decreasing order: " << is_obj_there_if_key_size_is_at_least_one_obj1 << " " << is_obj_there_if_key_size_is_at_least_one_obj2 << " " << is_obj_there_if_key_size_is_at_least_one_obj3 << std::endl;
	std::cout << "if this is greater than 0, then there were times when only partial info about an object was recorded: " << sanity_check1 << std::endl;
	std::cout << "if this is greater than 0, then there were times when the first key index was not filled but another key index was: " << sanity_check2 << std::endl;

}  
   
   
DEFINE_FWK_MODULE(GeneralHLTOffline2);
