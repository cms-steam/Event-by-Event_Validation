Event-by-Event_Validation
=========================

Validation code for comparing offline HLT results 

There are two types of validations. One comparing event-by-event, and a faster validation comparing counts in each path. 

Run the hlt cfg of your choice. Add the following line the process.hltOutputA module:
    
    'keep *_genParticles_*_*',

Collect the output into a text file:
	
	find/afs/cern.ch/user/m/muell149/work/HLTONLINE/CMSSW_7_2_3/src/HLTrigger/Configuration/test/crab_dir/res/*.root | perl -ne 'print "file:$_"' > outputA.txt

## Fast validation


## Slow validation