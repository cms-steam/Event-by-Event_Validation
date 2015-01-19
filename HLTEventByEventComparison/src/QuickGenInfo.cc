//	QuickGenInfo.cc
//
//	Simple module to extract genparticle info and save to tree
//	Created 2013 by Geoffrey Smith
//	
//	usage: 
//	genparticles = cms.EDAnalyzer("QuickGenInfo")	
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
/*
using namespace edm;
using namespace reco;
using namespace std; 
*/

using std::vector;
using std::string;


class QuickGenInfo : public edm::EDAnalyzer {
   public:
      explicit QuickGenInfo(const edm::ParameterSet&);
      ~QuickGenInfo();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);



      // ----------member data ---------------------------


  bool debugPrint;
  bool outputPrint;
  
  TH1F *genparticles_size;
  
  // -- the tree branches --
  TTree *the_tree;
  
  int tree_event;
  int tree_gen_ID[100];
  int tree_gen_parent_ID[100];
  int tree_gen_grandparent_ID[100];
  float tree_gen_px[100];
  float tree_gen_py[100];
  float tree_gen_pz[100]; 
  float tree_gen_pt[100]; 
  float tree_gen_energy[100];
  float tree_gen_eta[100];
  float tree_gen_phi[100];
  float tree_gen_charge[100];
  

   // -----------------------
  
};



//
// constructors and destructor
//
QuickGenInfo::QuickGenInfo(const edm::ParameterSet& iConfig)
{
	//now do what ever initialization is needed

	debugPrint = true;
	outputPrint = false;

	edm::Service<TFileService> fs;

	TFileDirectory results = TFileDirectory( fs->mkdir("QuickGenInfo") );

}


QuickGenInfo::~QuickGenInfo()
{

// do anything here that needs to be done at desctruction time
// (e.g. close files, deallocate resources etc.)

}



// ------------ method called for each event  ------------
void QuickGenInfo::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
//   using namespace edm;
//   using std::string;
//   using namespace reco;
   

if (debugPrint) std::cout << "Inside analyze - run, block, event " << iEvent.id().run() << " , " << iEvent.id().luminosityBlock() << " , " << iEvent.id().event() << " , " << std::endl;

		   
std::cout << "  " << std::endl;
std::cout << "Gen particle info for event number " << iEvent.id().event() << ": " << std::endl;
std::cout << "  " << std::endl;

edm::Handle<reco::GenParticleCollection> genParticles;
//edm::Handle<vector<reco::GenParticle> > genParticles;
iEvent.getByLabel("genParticles", genParticles);

std::cout << "after getting collection ..." << std::endl;

for (int obj_init=0; obj_init<100; obj_init++)
{

	tree_gen_ID[obj_init] = -99;
	tree_gen_parent_ID[obj_init] = -99;
	tree_gen_grandparent_ID[obj_init] = -99;
	tree_gen_px[obj_init] = -99.;
	tree_gen_py[obj_init] = -99.;
	tree_gen_pz[obj_init] = -99.; 
	tree_gen_pt[obj_init] = -99.; 
	tree_gen_energy[obj_init] = -99.;
	tree_gen_eta[obj_init] = -99.;
	tree_gen_phi[obj_init] = -99.;
}


tree_event = iEvent.id().event();

std::cout << "after init ..." << std::endl;

//int gensize = genParticles->size();
//genparticles_size->Fill(gensize);

int counter = 0;
int counter_specific = 0;

for(size_t i = 0; i < genParticles->size(); ++ i)
{
	
	int st = (*genParticles)[i].status();
	
	if ((counter<100)&&(st==3)) // status == 3 only
	{


		tree_gen_ID[counter] = (*genParticles)[i].pdgId();

	//     int st = p.status();  

		tree_gen_px[counter] = (*genParticles)[i].px();
		tree_gen_py[counter] = (*genParticles)[i].py();
		tree_gen_pz[counter] = (*genParticles)[i].pz();
		tree_gen_pt[counter] = (*genParticles)[i].pt();
		tree_gen_energy[counter]  = (*genParticles)[i].energy();
		tree_gen_eta[counter]  = (*genParticles)[i].eta();
		tree_gen_phi[counter]  = (*genParticles)[i].phi();
		tree_gen_charge[counter]  = (*genParticles)[i].charge();

		//const reco::Candidate & p = (*genParticles)[i];

		if ((*genParticles)[i].mother())
		{
			const reco::Candidate * mom = (*genParticles)[i].mother();
			tree_gen_parent_ID[counter] = mom->pdgId();
			
			if (mom->mother())
			{
				const reco::Candidate * gramma = mom->mother();
				tree_gen_grandparent_ID[i] = gramma->pdgId();
			}
		}
		
		/*
		const reco::Candidate * gramma = mom->mother();	
		tree_gen_grandparent_ID[i] = gramma->pdgId();
		*/

	     // , eta = p.eta(), phi = p.phi(), mass = p.mass();
	     // double vx = p.vx(), vy = p.vy(), vz = p.vz();
	     // int charge = p.charge();
	     // int n = p.numberOfDaughters();
	//     for(size_t j = 0; j < n; ++ j) {
	//       const Candidate * d = p.daughter( j );
	//       int dauId = d->pdgId();
	       // . . . 
	//     }

	//	std::cout << " pdgid " << id << " pt " << pt << std::endl;
	//	printf("pdgid: %-12i px: %-15f py: %-15f pz: %-15f pt: %-15f E: %-15f eta: %-15f phi: %-15f \n",id,px,py,pz,pt,E,eta,phi);
	//	genpt_vs_evt->Fill((float)iEvent.id().event(),pt);
	
		counter++;

	}
	
	if (st==2) counter_specific++; // if we saved status==2, about how many / event would that be
	

}

std::cout << counter << std::endl;
	
genparticles_size->Fill(counter_specific);	
	
the_tree->Fill();	
	
	

 
}   
   
 
void QuickGenInfo::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{   
   // do something here ??  yes.
   
	edm::Service<TFileService> newfs;
	
	genparticles_size = newfs->make<TH1F>("genparticles_size", "size of genparticles collection per evt", 1000, 0, 1000);


}
   
void QuickGenInfo::endRun(edm::Run const&, edm::EventSetup const&)
{
	if (debugPrint) std::cout << " endRun called " << std::endl; 
}   
   

   
 // ------------ method called once each job just before starting event loop  ------------
void QuickGenInfo::beginJob()
{
	std::cout << "begin job" <<  std::endl;

	the_tree = new TTree("the_gen_tree","the_gen_tree");
	
	the_tree->Branch("event",&tree_event,"event/I");
//	the_tree->Branch("status",&tree_status,"status[100]/I");
	the_tree->Branch("gen_ID",&tree_gen_ID,"gen_ID[100]/I");
	the_tree->Branch("gen_parent_ID",&tree_gen_parent_ID,"gen_parent_ID[100]/I");
	the_tree->Branch("gen_grandparent_ID",&tree_gen_grandparent_ID,"gen_grandparent_ID[100]/I");
	the_tree->Branch("gen_px",&tree_gen_px,"gen_px[100]/F");
	the_tree->Branch("gen_py",&tree_gen_py,"gen_py[100]/F");
	the_tree->Branch("gen_pz",&tree_gen_pz,"gen_pz[100]/F");
	the_tree->Branch("gen_pt",&tree_gen_pt,"gen_pt[100]/F");
	the_tree->Branch("gen_energy",&tree_gen_energy,"gen_energy[100]/F");
	the_tree->Branch("gen_eta",&tree_gen_eta,"gen_eta[100]/F");
	the_tree->Branch("gen_phi",&tree_gen_phi,"gen_phi[100]/F");
	the_tree->Branch("gen_charge",&tree_gen_charge,"gen_charge[100]/F");
		


}

// ------------ method called once each job just after ending the event loop  ------------
void QuickGenInfo::endJob() 
{

	std::cout << " endJob called " << std::endl;

}  
   
   
DEFINE_FWK_MODULE(QuickGenInfo);
