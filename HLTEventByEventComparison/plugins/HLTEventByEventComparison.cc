// -*- C++ -*-
//
// Package:    Event-by-Event_Validation/HLTEventByEventComparison
// Class:      HLTEventByEventComparison
// 
/**\class HLTEventByEventComparison HLTEventByEventComparison.cc Event-by-Event_Validation/HLTEventByEventComparison/plugins/HLTEventByEventComparison.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Darren Puigh
//         Created:  Thu, 26 Jun 2014 11:05:31 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"

//
// class declaration
//

class HLTEventByEventComparison : public edm::EDAnalyzer {
   public:
      explicit HLTEventByEventComparison(const edm::ParameterSet&);
      ~HLTEventByEventComparison();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

  //virtual void beginRun(edm::Run&, edm::EventSetup&) override;
      virtual void beginRun(edm::Run const& iRun,edm::EventSetup const& iSetup) override;
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------

  edm::EDGetTokenT <edm::TriggerResults> triggerResultsTokenRef;
  edm::EDGetTokenT <edm::TriggerResults> triggerResultsTokenTgt;

  HLTConfigProvider hlt_config_ref_;
  HLTConfigProvider hlt_config_tgt_;

  std::string hltTagRef;
  std::string hltTagTgt;

  std::vector<std::string> hlt_triggerNames_ref_;
  std::vector<std::string> hlt_triggerNames_tgt_;

  // Histograms
  TH1D *h_hlt_ref;
  TH1D *h_hlt_tgt;
  TH1D *h_hlt_tgt_sep;

  std::map<std::string, int> hlt_counts_per_path_ref_;
  std::map<std::string, int> hlt_counts_per_path_tgt_;

  std::map<std::string, int> hlt_counts_per_path_ref_ref_;
  std::map<std::string, int> hlt_counts_per_path_ref_tgt_;

  int numEvents_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HLTEventByEventComparison::HLTEventByEventComparison(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
  hltTagRef = iConfig.getParameter<std::string> ("ReferenceHLTProcessName");
  hltTagTgt = iConfig.getParameter<std::string> ("TargetHLTProcessName");

  triggerResultsTokenRef = consumes <edm::TriggerResults> (edm::InputTag(std::string("TriggerResults"), std::string(""), hltTagRef));
  triggerResultsTokenTgt = consumes <edm::TriggerResults> (edm::InputTag(std::string("TriggerResults"), std::string(""), hltTagTgt));

  numEvents_ = 0;
}


HLTEventByEventComparison::~HLTEventByEventComparison()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
HLTEventByEventComparison::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   numEvents_++;
   
  // Access Trigger Results: Reference
  edm::Handle<edm::TriggerResults> triggerResultsRef;
  iEvent.getByToken(triggerResultsTokenRef, triggerResultsRef);

  // Access Trigger Results: Target
  edm::Handle<edm::TriggerResults> triggerResultsTgt;
  iEvent.getByToken(triggerResultsTokenTgt, triggerResultsTgt);

  if( !triggerResultsRef.isValid() ){
    std::cout << "Trigger results not valid for tag " << hltTagRef << std::endl;
    return;
  }
  if( !triggerResultsTgt.isValid() ){
    std::cout << "Trigger results not valid for tag " << hltTagTgt << std::endl;
    return;
  }


  for( unsigned int iPath=0; iPath<hlt_triggerNames_ref_.size(); iPath++ ){
    std::string name = hlt_triggerNames_ref_[iPath];
    unsigned int index = hlt_config_ref_.triggerIndex(name);

    if( index >= triggerResultsRef->size() ) continue;

    bool accept = triggerResultsRef->accept(index);

    std::string pathNameNoVer = hlt_config_ref_.removeVersion(name);

    if( accept ) hlt_counts_per_path_ref_[name] += 1;
    if( accept ) hlt_counts_per_path_ref_ref_[pathNameNoVer] += 1;

    if( accept ){
      TAxis * axis = h_hlt_ref->GetXaxis();
      if( !axis ) continue;
      int bin_num = axis->FindBin(pathNameNoVer.c_str());
      int bn = bin_num - 1;
      h_hlt_ref->Fill(bn, 1);
    }
  }


  for( unsigned int iPath=0; iPath<hlt_triggerNames_tgt_.size(); iPath++ ){
    std::string name = hlt_triggerNames_tgt_[iPath];
    unsigned int index = hlt_config_tgt_.triggerIndex(name);

    if( index >= triggerResultsTgt->size() ) continue;

    bool accept = triggerResultsTgt->accept(index);

    std::string pathNameNoVer = hlt_config_tgt_.removeVersion(name);

    if( accept ) hlt_counts_per_path_tgt_[name] += 1;
    if( accept ) hlt_counts_per_path_ref_tgt_[pathNameNoVer] += 1;

    if( accept ){
      TAxis * axis = h_hlt_tgt->GetXaxis();
      if( !axis ) continue;
      int bin_num = axis->FindBin(pathNameNoVer.c_str());
      int bn = bin_num - 1;
      h_hlt_tgt->Fill(bn, 1);
    }


    if( accept ){
      TAxis * axis = h_hlt_tgt_sep->GetXaxis();
      if( !axis ) continue;
      int bin_num = axis->FindBin(pathNameNoVer.c_str());
      int bn = bin_num - 1;
      h_hlt_tgt_sep->Fill(bn, 1);
    }
  }



}


// ------------ method called once each job just before starting event loop  ------------
void 
HLTEventByEventComparison::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HLTEventByEventComparison::endJob() 
{

  // report on triggers fired
  std::cout << "***************************************************************" << std::endl;
  std::cout << "  Summary for Reference HLT: Total number of events = " << numEvents_ << std::endl;
  for( std::map<std::string, int>::const_iterator iter = hlt_counts_per_path_ref_.begin(); iter != hlt_counts_per_path_ref_.end(); iter++){
    std::string name = iter->first;
    double eff = 100*double(iter->second)/double(numEvents_);
    printf("\t %s,\t %d,\t %.1f \n",name.c_str(), iter->second, eff);
  }
  std::cout << "***************************************************************" << std::endl;
  std::cout << "  Summary for Target HLT: Total number of events = " << numEvents_ << std::endl;
  for( std::map<std::string, int>::const_iterator iter = hlt_counts_per_path_tgt_.begin(); iter != hlt_counts_per_path_tgt_.end(); iter++){
    std::string name = iter->first;
    double eff = 100*double(iter->second)/double(numEvents_);
    printf("\t %s,\t %d,\t %.1f \n",name.c_str(), iter->second, eff);
  }
  std::cout << "***************************************************************" << std::endl;




  // report on triggers fired
  std::cout << "***************************************************************" << std::endl;
  std::cout << "  Summary for HLT Comparison: Total number of events = " << numEvents_ << std::endl;
  for( std::map<std::string, int>::const_iterator iter = hlt_counts_per_path_ref_ref_.begin(); iter != hlt_counts_per_path_ref_ref_.end(); iter++){
    std::string name = iter->first;
    int counts_ref = hlt_counts_per_path_ref_ref_[name];
    int counts_tgt = hlt_counts_per_path_ref_tgt_[name];

    double eff_ref = ( numEvents_>0 ) ? 100*double(counts_ref)/double(numEvents_) : 0;
    double eff_tgt = ( numEvents_>0 ) ? 100*double(counts_tgt)/double(numEvents_) : 0;

    if( abs(counts_ref - counts_tgt)>0.2*counts_ref ) 
      printf("\t %s,\t %d,\t %.1f,\t %d,\t %.1f <======== significant \n", name.c_str(), counts_ref, eff_ref, counts_tgt, eff_tgt);
    else printf("\t %s,\t %d,\t %.1f,\t %d,\t %.1f \n", name.c_str(), counts_ref, eff_ref, counts_tgt, eff_tgt);

  }
  std::cout << "***************************************************************" << std::endl;

}

// ------------ method called when starting to processes a run  ------------
void 
HLTEventByEventComparison::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{

  bool refchanged = true;
  bool tgtchanged = true;
  if (!hlt_config_ref_.init(iRun, iSetup, hltTagRef, refchanged)) {
    std::cout << "Warning, didn't find process HLT,\t" << hltTagRef << std::endl;
    return;
  }

  if (!hlt_config_tgt_.init(iRun, iSetup, hltTagTgt, tgtchanged)) {
    std::cout << "Warning, didn't find process HLT,\t" << hltTagTgt << std::endl;
    return;
  }

  std::vector<std::string> triggerNames_ref = hlt_config_ref_.triggerNames();
  std::vector<std::string> triggerNames_tgt = hlt_config_tgt_.triggerNames();

  hlt_triggerNames_ref_.clear();
  hlt_triggerNames_tgt_.clear();

  std::string prefix = "HLT_";
  for( unsigned int iPath=0; iPath<triggerNames_ref.size(); iPath++ ){
    std::string name = triggerNames_ref[iPath];
    if( name.compare(0, prefix.length(), prefix) == 0 ) hlt_triggerNames_ref_.push_back(name);
  }
  for( unsigned int iPath=0; iPath<triggerNames_tgt.size(); iPath++ ){
    std::string name = triggerNames_tgt[iPath];
    if( name.compare(0, prefix.length(), prefix) == 0 ) hlt_triggerNames_tgt_.push_back(name);
  }

  unsigned int numHLT_ref = hlt_triggerNames_ref_.size();
  unsigned int numHLT_tgt = hlt_triggerNames_tgt_.size();

  edm::Service<TFileService> fs;
  h_hlt_ref = fs->make<TH1D>("HLT_Ref",";HLT path", numHLT_ref , 0 , numHLT_ref );
  h_hlt_tgt = fs->make<TH1D>("HLT_Tgt",";HLT path", numHLT_ref , 0 , numHLT_ref );
  h_hlt_tgt_sep = fs->make<TH1D>("HLT_Tgt_sep",";HLT path", numHLT_tgt , 0 , numHLT_tgt );

  for( unsigned int iPath=0; iPath<numHLT_ref; iPath++ ){
    std::string pathNameNoVer = hlt_config_ref_.removeVersion(hlt_triggerNames_ref_[iPath]);
    int jPath = iPath+1;
    if( h_hlt_ref ){
      TAxis * axis = h_hlt_ref->GetXaxis();
      if( axis ) axis->SetBinLabel(jPath, pathNameNoVer.c_str());
    }
    if( h_hlt_tgt ){
      TAxis * axis = h_hlt_tgt->GetXaxis();
      if( axis ) axis->SetBinLabel(jPath, pathNameNoVer.c_str());
    }
  }


  for( unsigned int iPath=0; iPath<numHLT_tgt; iPath++ ){
    std::string pathNameNoVer = hlt_config_tgt_.removeVersion(hlt_triggerNames_tgt_[iPath]);
    int jPath = iPath+1;
    if( h_hlt_tgt_sep ){
      TAxis * axis = h_hlt_tgt_sep->GetXaxis();
      if( axis ) axis->SetBinLabel(jPath, pathNameNoVer.c_str());
    }
  }

  std::cout << "=====================" << std::endl;
  std::cout << "  Reference HLT " << std::endl;
  for( unsigned int iPath=0; iPath<numHLT_ref; iPath++ ){
    std::string name = hlt_triggerNames_ref_[iPath];
    std::string pathNameNoVer = hlt_config_ref_.removeVersion(name);
    std::cout << "\t" << name << "\t" << pathNameNoVer << std::endl;
  }

  std::cout << "=====================" << std::endl;
  std::cout << "  Target HLT " << std::endl;
  for( unsigned int iPath=0; iPath<numHLT_tgt; iPath++ ){
    std::string name = hlt_triggerNames_tgt_[iPath];
    std::string pathNameNoVer = hlt_config_tgt_.removeVersion(name);
    std::cout << "\t" << name << "\t" << pathNameNoVer << std::endl;
  }
  std::cout << "=====================" << std::endl;


  // Zero out map
  for( unsigned int iPath=0; iPath<numHLT_ref; iPath++ ){
    std::string name = hlt_triggerNames_ref_[iPath];
    hlt_counts_per_path_ref_[name] = 0;
  }
  for( unsigned int iPath=0; iPath<numHLT_tgt; iPath++ ){
    std::string name = hlt_triggerNames_tgt_[iPath];
    hlt_counts_per_path_tgt_[name] = 0;
  }


  // Zero out map
  for( unsigned int iPath=0; iPath<numHLT_ref; iPath++ ){
    std::string name = hlt_triggerNames_ref_[iPath];
    std::string pathNameNoVer = hlt_config_tgt_.removeVersion(name);

    hlt_counts_per_path_ref_ref_[pathNameNoVer] = 0;
    hlt_counts_per_path_ref_tgt_[pathNameNoVer] = 0;
  }

}


// ------------ method called when ending the processing of a run  ------------
/*
void 
HLTEventByEventComparison::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
HLTEventByEventComparison::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
HLTEventByEventComparison::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HLTEventByEventComparison::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HLTEventByEventComparison);
