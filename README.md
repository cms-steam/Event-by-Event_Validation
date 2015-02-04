Event-by-Event_Validation
=========================

Validation code for comparing offline HLT results 

There are two types of validations. One comparing event-by-event, and a faster validation comparing counts in each path. 

If you're running over remote files via xrootd or crab, get a proxy with:
   	  voms-proxy-init -voms cms

Checkout HLTrigger:
	git cms-addpkg HLTrigger/Configuration
	cd HLTrigger/Configuration/test
	 
Here there are a couple of options, you can edit the hlt files in test such as:

     OnMc_HLT_FULL.py
     OnMc_HLT_Fake.py
     OnMc_HLT_GRun.py

OR you can grab your own config specifying the menu, GT etc:

       hltGetConfiguration /dev/CMSSW_7_2_1/HLT/V118 --full --offline --mc --unprescale --process TEST --globaltag auto:run2_mc_GRun --l1-emulator 'stage1,gt' --l1Xml L1Menu_Collisions2015_25ns_v1_L1T_Scales_20101224_Imp0_0x102f.xml > hlt_stage1.py
       	   
Add the following line the process.hltOutputA module:

	'keep *_genParticles_*_*',

Run the hlt cfg of your choice either with crab or with:

    cmsRun <your_hlt_cfg.py> 

Collect the output into a text file if running with crab:

	find/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_2_3/src/HLTrigger/Configuration/test/crab_dir/res/*.root | perl -ne 'print "file:$_"' > outputA.txt

Now you've got your HLT results. Check out the validation package with:

    	   git clone git@github.com:cms-steam/Event-by-Event_Validation.git 

Process the HLT output by compiling. Then edit gen_hlt2_cfg.py to the files you want to run over and desired output names:
	scram b -j8
	cd Event-by-Event_Validation/HLTEventByEventComparison/test
	emacs genhlt2_cfg.py
	
Run the hlt pre processor:
	cmsRun genhlt2_cfg.py
## Fast validation

Edit the fast validation file to run over the output files you produced with genhlt2_cfg.py and edit the display strings for the plots and results:

	emacs fastValidation.py
	python fastValidation.py

## Slow validation
