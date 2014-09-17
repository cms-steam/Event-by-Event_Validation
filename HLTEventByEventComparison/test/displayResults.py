#shebang
import sys 
import os
import getopt
import subprocess
import commands
import re
import string
import jinja2 
import csv

from os.path import expanduser

def makeList(csv_file_name):
   count = 0
   list_of_lists = []

   try:
      with open(csv_file_name,'rb') as f:
         reader = csv.reader(f)
         for row in reader:
            if count > 0:
               var = int(row[9])
               row[9] = var
            list_of_lists.append(row)
            count += 1

         list_of_lists.sort(key = lambda row: row[9],reverse=True)         
         f.close()
   except:
      print 'Could not open file: ', csv_file_name

   return list_of_lists


def main(argv):
   
   
   if len(argv) > 2:
      print "Error, please specify valid input directory"
      sys.exit()
   elif len(argv) <= 1 :
      print "Usage: python displayResults.py <workingdir> title"
      sys.exit()

   title = argv[1]

   if '/' in argv[0][-1]:
      output_dir = argv[0]
   else:
      output_dir = argv[0]+'/'
   data_dir = output_dir+'data/'
   
   with open(data_dir+'summary.log','rb') as f:
      lines = f.readlines()
   f.close()

   all_types = makeList(data_dir+'allDecayTypes.csv')
   all_had = makeList(data_dir+'AllHad.csv')
   double_ele = makeList(data_dir+'DoubleEle.csv')
   double_mu = makeList(data_dir+'DoubleMu.csv')
   double_tau = makeList(data_dir+'DoubleTau.csv')
   single_ele = makeList(data_dir+'SingleEle.csv')
   single_mu = makeList(data_dir+'SingleMu.csv')
   single_tau = makeList(data_dir+'SingleTau.csv')
   ele_mu = makeList(data_dir+'EleMu.csv')
   ele_tau = makeList(data_dir+'EleTau.csv')
   mu_tau = makeList(data_dir+'MuTau.csv')

   #begin templating
   env = jinja2.Environment(loader=jinja2.FileSystemLoader(os.path.dirname(__file__)))
   env.tests["sum"] = lambda s: s == "Total"
   template = env.get_template('template.html')
   openfile = output_dir+'index.html'
   f = open(openfile, 'w')
   try:
      f.write(template.render(
            ids = title,
            overview = lines,
            allTypes = all_types,
            allHad = all_had,
            doubleEle = double_ele,
            doubleMu = double_mu,
            doubleTau = double_tau,
            singleEle = single_ele,
            singleMu = single_mu,
            singleTau = single_tau,
            eleMu = ele_mu,
            eleTau = ele_tau,
            muTau = mu_tau
      ).encode('utf-8'))
   finally:
      f.close()
   print "after templating "

if __name__=='__main__':
   main(sys.argv[1:])
