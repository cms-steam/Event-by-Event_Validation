from ROOT import TFile,TTree,TH2F, TH1F, TCanvas, gROOT, TPad, TLegend
import sys, csv, time, re, string, array, subprocess, commands, collections
gROOT.SetBatch()

def StripVersion(name):
   if re.match('.*_v[0-9]+',name):
      name = name[:name.rfind('_')]
      name = string.strip(name)
   return name

#def main():
if __name__=='__main__':
   
   #input files
   fileRef = '/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_3_0/src/DQMOffline/Trigger/test/723_stage1.root'
   fileTgt = '/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_3_0/src/DQMOffline/Trigger/test/730_stage1.root'
    #Title strings for target and reference
   target = 'tgt_CMSSW_REL'
   reference ='ref_CMSSW_REL'
   
   file_ref = TFile(fileRef)
   file_tgt = TFile(fileTgt)
   hist_ref = file_ref.Get('newHLTOffline/hlt_count_hist')
   hist_tgt = file_tgt.Get('newHLTOffline/hlt_count_hist')#eventbyevent/HLT_Tgt')

   output_log = open('summary_table.csv',"w")
   output_log.write("PATH, COUNTS IN REFERENCE, COUNTS IN TARGET, abs(TGT-REF)/REF, ERROR\n")
   
   bin_dict0 = collections.OrderedDict()#{}#{title,hits}
   bin_dict1 = collections.OrderedDict()#{}
   
   table = []
   row = []
   
   bins = hist_ref.GetNbinsX()
   
   nhist_ref = TH1F("counts","counts",bins+1,0,bins+1)
   nhist_tgt = TH1F("counts","counts",bins+1,0,bins+1)
   
   for bin in xrange(bins):
      bl0 = hist_ref.GetXaxis().GetBinLabel(bin)
      bl1 = hist_tgt.GetXaxis().GetBinLabel(bin)
      
      bin_dict0[StripVersion(bl0)]=hist_ref.GetBinContent(bin)
      bin_dict1[StripVersion(bl1)]=hist_tgt.GetBinContent(bin)
      
   i=1
   for key in bin_dict0:
      if bin_dict1.has_key(key):
         print "key == ", key   
         nhist_ref.Fill(i,bin_dict0[key])
         nhist_tgt.Fill(i,bin_dict1[key])
         nhist_ref.GetXaxis().SetBinLabel(i+1,key)
         nhist_tgt.GetXaxis().SetBinLabel(i+1,key)
       
         if bin_dict0[key] != bin_dict1[key]:
            print "key = ",key
            print bin_dict0[key]
            print bin_dict1[key]
            print "====" 
            
         if bin_dict0[key] == 0:
            if bin_dict1[key] == bin_dict0[key]:
               diff = 0.
               error = 0.
            else:
               diff = 100.
               error = bin_dict1[key] **(0.5)
         else:
            diff = abs(bin_dict1[key] - bin_dict0[key])/bin_dict0[key]
            error = diff*((1/(bin_dict1[key] + bin_dict0[key])+1/(bin_dict0[key]))**(0.5))
            
         row = [key,bin_dict0[key],bin_dict1[key],diff,error]
         table.append(row)
         i+=1
            
   table.sort(key = lambda row: row[3],reverse=True)
   for row in table:
      output_log.write("%(1)s,%(2)s,%(3)s,%(4)s,%(5)s\n"%{"1":row[0],"2":row[1],"3":row[2],"4":row[3],"5":row[4]})

   title = nhist_ref.GetName()
   can = TCanvas(title,title,5000,3000)
   pad1 = TPad("pad1","pad1",0,0.28,1,1)
   leg = TLegend(0.73,0.66,0.90,0.8)
   leg.SetFillColor(0)
   pad1.SetBottomMargin(0)
   pad1.Draw()
   pad1.cd()
   nhist_ref.SetStats(0)
   nhist_tgt.SetStats(0)
   nhist_tgt.SetLineColor(4)
   nhist_ref.SetLineColor(2)
   leg.AddEntry(nhist_ref,target)
   leg.AddEntry(nhist_tgt,reference)
    #nhist_ref.LabelsOption("a")
   nhist_ref.SetLabelSize(0.02)
    #nhist_tgt.LabelsOption("a")
   nhist_tgt.SetLabelSize(0.02)
   
   nhist_tgt.DrawCopy()
   nhist_ref.Draw("same")
   leg.Draw("same")
   can.cd()
   
   pad2 = TPad("pad2","pad2",0,0,1,0.28)
   pad2.SetTopMargin(0)
   pad2.SetBottomMargin(0.34)
   pad2.Draw()
   pad2.cd()
   nhist_tgt.Sumw2()
   nhist_tgt.SetTitle("")
   nhist_tgt.SetStats(0)
   nhist_tgt.Add(nhist_ref,-1)
   nhist_tgt.Divide(nhist_ref)
   nhist_tgt.GetYaxis().SetTitle("New-Old/Old")
   
  #nhist_ref.SetMarkerStyle(21)
   nhist_tgt.SetLineColor(1)
   nhist_tgt.GetYaxis().CenterTitle()
   nhist_tgt.GetYaxis().SetTitleSize(.065)
   nhist_tgt.GetYaxis().SetTitleOffset(.4)
   nhist_tgt.Draw("ep")
   can.SaveAs("menu_changes_by_path.png")
    #can.cd()
   
