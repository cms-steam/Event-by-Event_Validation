#include "TFile.h"
#include "TChain.h"
#include "THStack.h"
#include "TF1.h"
#include "TH1.h"
#include "TH3.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TAxis.h"
#include "TList.h"
#include "TLatex.h"
#include "TLine.h"
#include "TObject.h"
#include "TDirectory.h"
#include "TGraphAsymmErrors.h"
#include "TKey.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <exception>
#include <cmath> 
#include <iomanip>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sstream>
#include <map>
#include <iterator>


void makePlots_hlt_eventbyevent_comparison( std::string inputFile_="input.root", std::string target_label_="Target", std::string outputSuffix_="_out" ) {

  TFile *file = new TFile(inputFile_.c_str());

  std::string dirprefix = "Images" + outputSuffix_;
  struct stat st;
  if( stat(dirprefix.c_str(),&st) != 0 )  mkdir(dirprefix.c_str(),0777);

  std::string label_ref = "Reference";
  std::string label_tgt = target_label_;

  TH1D* h_ref = (TH1D*)file->Get("eventbyevent/HLT_Ref");
  TH1D* h_tgt = (TH1D*)file->Get("eventbyevent/HLT_Tgt");
  TH1D* h_tgt_sep = (TH1D*)file->Get("eventbyevent/HLT_Tgt_sep");

  std::map<std::string, int> hlt_counts_per_path_ref_;
  std::map<std::string, int> hlt_counts_per_path_tgt_;
  std::map<double, std::string> hlt_counts_per_path_diff_;

  std::vector<std::string> hlt_common_paths;

  TAxis * axis = h_ref->GetXaxis();

  // loop over target paths
  for( int iPath=0; iPath<h_tgt_sep->GetNbinsX(); iPath++ ){
    std::string name(h_tgt_sep->GetXaxis()->GetBinLabel(iPath+1));
    
    int bin = axis->FindBin(name.c_str());

    if( bin<1 ) continue;

    double cnt_ref = h_ref->GetBinContent(bin);
    double cnt_tgt = h_tgt->GetBinContent(bin);

    int diff = int( cnt_ref - cnt_tgt );
    double rel_diff = double(diff);

    if( cnt_ref>0 )      rel_diff = fabs(cnt_tgt - cnt_ref)/cnt_ref;
    else if( cnt_tgt>0 ) rel_diff = fabs(cnt_tgt - cnt_ref)/cnt_tgt;
    else                 rel_diff = 0.;

    // printf("%s\t %d\t %d\t %d\t %.3f \n",
    // 	   name.c_str(), int(cnt_ref), int(cnt_tgt), diff, rel_diff ); 

    hlt_counts_per_path_ref_[name] = int(cnt_ref);
    hlt_counts_per_path_tgt_[name] = int(cnt_tgt);

    hlt_counts_per_path_diff_[rel_diff] = name;

    if( rel_diff>0.15 ) hlt_common_paths.push_back(name);
  }


  std::map<double, std::string>::reverse_iterator rit;
  for( rit=hlt_counts_per_path_diff_.rbegin(); rit!=hlt_counts_per_path_diff_.rend(); ++rit )
    std::cout << "path: " << rit->second << "\t" << rit->first << "\t" << hlt_counts_per_path_ref_[rit->second] << "\t" << hlt_counts_per_path_tgt_[rit->second] << std::endl;


  int numPaths = int( hlt_counts_per_path_ref_.size() );
  TH1D* h_hlt_ref = new TH1D("h_hlt_ref",";HLT path", numPaths, 0, numPaths );
  TH1D* h_hlt_tgt = new TH1D("h_hlt_tgt",";HLT path", numPaths, 0, numPaths );
  TH1D* h_hlt_diff = new TH1D("h_hlt_diff",";HLT path", numPaths, 0, numPaths );

  int iHLT=0;
  for( std::map<std::string, int>::iterator it=hlt_counts_per_path_ref_.begin(); it!=hlt_counts_per_path_ref_.end(); ++it ){
    iHLT++;
    std::string name = it->first;

    double cnt_ref = double(hlt_counts_per_path_ref_[name]);
    double cnt_tgt = double(hlt_counts_per_path_tgt_[name]);

    double rel_diff = 1.0;
    if( cnt_ref>0 )      rel_diff = (cnt_tgt - cnt_ref)/cnt_ref;
    else if( cnt_tgt>0 ) rel_diff = (cnt_tgt - cnt_ref)/cnt_tgt;
    else                 rel_diff = 0.;

    h_hlt_ref->SetBinContent(iHLT,cnt_ref);
    h_hlt_tgt->SetBinContent(iHLT,cnt_tgt);
    h_hlt_diff->SetBinContent(iHLT,rel_diff);

    h_hlt_ref->GetXaxis()->SetBinLabel(iHLT,name.c_str());
    h_hlt_diff->GetXaxis()->SetBinLabel(iHLT,name.c_str());
  }


  h_hlt_ref->SetStats(0);

  h_hlt_ref->SetLineColor(kBlue);
  h_hlt_tgt->SetLineColor(kRed);

  if( true ){
    // TLegend *legend = new TLegend(0.255,0.78,0.92,0.89);
    TLegend *legend = new TLegend(0.255,0.92,0.92,0.99);

    legend->SetFillColor(kWhite);
    legend->SetLineColor(kWhite);
    legend->SetShadowColor(kWhite);
    legend->SetTextFont(42);
    legend->SetTextSize(0.035);

    legend->SetNColumns(2);

    legend->AddEntry(h_hlt_ref,label_ref.c_str(),"l");
    legend->AddEntry(h_hlt_tgt,label_tgt.c_str(),"l");


    //// Original
    // double ratioMin = -1.2;
    // double ratioMax = 1.2;
    double ratioMin = -0.6;
    double ratioMax = 0.6;

    //Hack to get it plotted with ratio plot
    TCanvas* myC = new TCanvas("myC", "myC", 1200,700);

    gStyle->SetPadBorderMode(0);
    gStyle->SetFrameBorderMode(0);
    Float_t small = 1.e-5;
    myC->Divide(1,2,small,small);
    const float padding=1e-5; const float ydivide=0.3;
    myC->GetPad(1)->SetPad( padding, ydivide + padding , 1-padding, 1-padding);
    myC->GetPad(2)->SetPad( padding, padding, 1-padding, ydivide-padding);
    myC->GetPad(1)->SetLeftMargin(.11);
    myC->GetPad(2)->SetLeftMargin(.11);
    myC->GetPad(1)->SetRightMargin(.05);
    myC->GetPad(2)->SetRightMargin(.05);
    myC->GetPad(1)->SetBottomMargin(.3);
    myC->GetPad(2)->SetBottomMargin(.3);
    myC->GetPad(1)->Modified();
    myC->GetPad(2)->Modified();
    myC->cd(1);
    gPad->SetBottomMargin(small);
    gPad->Modified();



    TH1D* myRatio = (TH1D*)h_hlt_diff->Clone("myRatio");

    myRatio->SetStats(0);
    myRatio->Sumw2();
    myRatio->SetLineColor(kBlack);
    myRatio->SetMarkerColor(kBlack);

    myRatio->SetMinimum(ratioMin);
    myRatio->SetMaximum(ratioMax);
    //myRatio->GetYaxis()->SetNdivisions(50000+404);
    myRatio->GetYaxis()->SetNdivisions(20000+505);
    myRatio->GetYaxis()->SetLabelSize(0.1); //make y label bigger
    myRatio->GetXaxis()->SetLabelSize(0.1); //make y label bigger
    myRatio->GetXaxis()->SetTitleOffset(1.1);
    myRatio->GetXaxis()->SetTitle(h_hlt_diff->GetXaxis()->GetTitle()); //make y label bigger
    myRatio->GetXaxis()->SetLabelSize(0.12);
    myRatio->GetXaxis()->SetLabelOffset(0.04);
    myRatio->GetXaxis()->SetTitleSize(0.12);
    // myRatio->GetYaxis()->SetTitle("Data/MC");
    myRatio->GetYaxis()->SetTitle("New - Old / Old");
    myRatio->GetYaxis()->SetTitleSize(0.1);
    myRatio->GetYaxis()->SetTitleOffset(.45);
    myC->cd(2);
    gPad->SetTopMargin(small);
    gPad->SetTickx();
    gPad->Modified();

    myC->cd(1);

    h_hlt_ref->GetYaxis()->SetTitle("Number of Events");
    h_hlt_ref->GetYaxis()->SetTitleSize(0.05);
    h_hlt_ref->GetYaxis()->SetTitleOffset(.95);

    h_hlt_ref->Draw();
    h_hlt_tgt->Draw("same");

    legend->Draw();

    double xmin = h_hlt_ref->GetBinLowEdge(1);
    double xmax = h_hlt_ref->GetBinLowEdge(h_hlt_ref->GetNbinsX()) + h_hlt_ref->GetBinWidth(h_hlt_ref->GetNbinsX());

    myC->cd(2);

    myRatio->SetLineWidth(2);

    myRatio->Draw("hist");

    TLine* myLine;
    myLine = new TLine(xmin, 0, xmax, 0);
    myLine->Draw();

    std::string outputFile = dirprefix + "/hlt_path_comparison" + outputSuffix_ + ".pdf";
    myC->Print(outputFile.c_str());

    delete legend;
    delete myC;
    delete myLine;
  }


  if( true ){
    int numCommonPaths = int(hlt_common_paths.size());

    for( int iPath=0; iPath<numCommonPaths; iPath++ ){
      std::cout << " ====================== " << std::endl;
      std::string name = hlt_common_paths[iPath];
      std::string pathName_ref = "eventbyevent/h_path_ref_" + name;
      std::string pathName_tgt = "eventbyevent/h_path_tgt_" + name;

      std::string diffname = "h_path_diff_" + name;

      //std::cout << " path is " << pathName_ref << std::endl;

      if( name=="HLT_LogMonitor" ) continue;

      TH1D* h_hlt_filt_ref = (TH1D*)file->Get(pathName_ref.c_str());
      TH1D* h_hlt_filt_tgt = (TH1D*)file->Get(pathName_tgt.c_str());
      TH1D* h_hlt_filt_diff = (TH1D*)h_hlt_filt_ref->Clone(diffname.c_str());

      bool printOut = true;
      for( int iBin=0; iBin<h_hlt_filt_tgt->GetNbinsX(); iBin++ ){
	double cnt_ref = h_hlt_filt_ref->GetBinContent(iBin+1);
	double cnt_tgt = h_hlt_filt_tgt->GetBinContent(iBin+1);

	double rel_diff = 1.0;
	if( cnt_ref>0 )      rel_diff = (cnt_tgt - cnt_ref)/cnt_ref;
	else if( cnt_tgt>0 ) rel_diff = (cnt_tgt - cnt_ref)/cnt_tgt;
	else                 rel_diff = 0.;

	h_hlt_filt_diff->SetBinContent(iBin+1,rel_diff);

	//if( fabs(rel_diff)>0.0002 ) printOut = true;
      }

      if( printOut ){
	double cnt_ref = double(hlt_counts_per_path_ref_[name]);
	double cnt_tgt = double(hlt_counts_per_path_tgt_[name]);

	double rel_diff = 1.0;
	if( cnt_ref>0 )      rel_diff = (cnt_tgt - cnt_ref)/cnt_ref;
	else if( cnt_tgt>0 ) rel_diff = (cnt_tgt - cnt_ref)/cnt_tgt;
	else                 rel_diff = 0.;

	printf("%s: relative difference = %.3f,\t reference = %d,\t target = %d \n",
	       name.c_str(), rel_diff, hlt_counts_per_path_ref_[name], hlt_counts_per_path_tgt_[name]);

	for( int iBin=0; iBin<h_hlt_filt_tgt->GetNbinsX(); iBin++ ){
	  double my_cnt_ref = h_hlt_filt_ref->GetBinContent(iBin+1);
	  double my_cnt_tgt = h_hlt_filt_tgt->GetBinContent(iBin+1);

	  double my_rel_diff = 1.0;
	  if( my_cnt_ref>0 )      my_rel_diff = (my_cnt_tgt - my_cnt_ref)/my_cnt_ref;
	  else if( my_cnt_tgt>0 ) my_rel_diff = (my_cnt_tgt - my_cnt_ref)/my_cnt_tgt;
	  else                   my_rel_diff = 0.;

	  printf("\t %s \t %.3f \t %.0f \t %.0f \n",
		 h_hlt_filt_ref->GetXaxis()->GetBinLabel(iBin+1), my_rel_diff, my_cnt_ref, my_cnt_tgt);
	}
      }

      h_hlt_filt_ref->SetStats(0);

      h_hlt_filt_ref->SetLineColor(kBlue);
      h_hlt_filt_tgt->SetLineColor(kRed);

      //TLegend *legend = new TLegend(0.255,0.78,0.92,0.89);
      TLegend *legend = new TLegend(0.255,0.92,0.92,0.99);

      legend->SetFillColor(kWhite);
      legend->SetLineColor(kWhite);
      legend->SetShadowColor(kWhite);
      legend->SetTextFont(42);
      legend->SetTextSize(0.035);

      legend->SetNColumns(2);

      legend->AddEntry(h_hlt_filt_ref,label_ref.c_str(),"l");
      legend->AddEntry(h_hlt_filt_tgt,label_tgt.c_str(),"l");


      double ratioMin = -1.2;
      double ratioMax = 1.2;

      //Hack to get it plotted with ratio plot
      TCanvas* myC = new TCanvas("myC", "myC", 600,700);

      gStyle->SetPadBorderMode(0);
      gStyle->SetFrameBorderMode(0);
      Float_t small = 1.e-5;
      myC->Divide(1,2,small,small);
      const float padding=1e-5; const float ydivide=0.3;
      myC->GetPad(1)->SetPad( padding, ydivide + padding , 1-padding, 1-padding);
      myC->GetPad(2)->SetPad( padding, padding, 1-padding, ydivide-padding);
      myC->GetPad(1)->SetLeftMargin(.11);
      myC->GetPad(2)->SetLeftMargin(.11);
      myC->GetPad(1)->SetRightMargin(.05);
      myC->GetPad(2)->SetRightMargin(.05);
      myC->GetPad(1)->SetBottomMargin(.3);
      myC->GetPad(2)->SetBottomMargin(.3);
      myC->GetPad(1)->Modified();
      myC->GetPad(2)->Modified();
      myC->cd(1);
      gPad->SetBottomMargin(small);
      gPad->Modified();



      TH1D* myRatio = (TH1D*)h_hlt_filt_diff->Clone();

      myRatio->SetStats(0);
      myRatio->Sumw2();
      myRatio->SetLineColor(kBlack);
      myRatio->SetMarkerColor(kBlack);

      myRatio->SetMinimum(ratioMin);
      myRatio->SetMaximum(ratioMax);
      myRatio->GetYaxis()->SetNdivisions(50000+404);
      myRatio->GetYaxis()->SetLabelSize(0.1); //make y label bigger
      myRatio->GetXaxis()->SetLabelSize(0.1); //make y label bigger
      myRatio->GetXaxis()->SetTitleOffset(1.1);
      myRatio->GetXaxis()->SetTitle(h_hlt_diff->GetXaxis()->GetTitle()); //make y label bigger
      myRatio->GetXaxis()->SetLabelSize(0.12);
      myRatio->GetXaxis()->SetLabelOffset(0.04);
      myRatio->GetXaxis()->SetTitleSize(0.12);
      // myRatio->GetYaxis()->SetTitle("Data/MC");
      myRatio->GetYaxis()->SetTitle("New - Old / Old");
      myRatio->GetYaxis()->SetTitleSize(0.1);
      myRatio->GetYaxis()->SetTitleOffset(.45);
      myC->cd(2);
      gPad->SetTopMargin(small);
      gPad->SetTickx();
      gPad->Modified();

      myC->cd(1);

      h_hlt_filt_ref->GetYaxis()->SetTitle("Number of Events");
      h_hlt_filt_ref->GetYaxis()->SetTitleSize(0.05);
      h_hlt_filt_ref->GetYaxis()->SetTitleOffset(.95);

      h_hlt_filt_ref->Draw();
      h_hlt_filt_tgt->Draw("same");

      legend->Draw();

      double xmin = h_hlt_filt_ref->GetBinLowEdge(1);
      double xmax = h_hlt_filt_ref->GetBinLowEdge(h_hlt_filt_ref->GetNbinsX()) + h_hlt_filt_ref->GetBinWidth(h_hlt_filt_ref->GetNbinsX());

      myC->cd(2);

      myRatio->SetLineWidth(2);

      myRatio->Draw("hist");

      TLine* myLine;
      myLine = new TLine(xmin, 0, xmax, 0);
      myLine->Draw();

      std::string outputFile = dirprefix + "/hlt_path_" + name + outputSuffix_ + ".pdf";
      myC->Print(outputFile.c_str());

      delete legend;
      delete myC;
      delete myLine;
    }
    std::cout << " ====================== " << std::endl;

  }

  // close file
  file->Close();

}
