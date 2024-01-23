# FastMTTFramework

This repository is for running the `FastMTT` algorithm for estimating the full di-tau mass on samples with two visible taus. Depending on the decay modes of the taus, there are different settings to pass to the
`FastMTT` function. 

If the input consists of one `TTree` with events in all three channels (`mutau`, `etau`, and `emu`) together, with a branch called `channel` that is equal to `0` for `mutau`, `1` for `etau`, and `2` for `emu`, use 
the flag `--channel=all`, which will call the executable in `ROOT/bin/SVFit.cc`. 

This single executable assumes that the input `TTree` already has the nominal lepton energy scales applied, and the lepton up/down shifts already in the input `TTree` as well.

The older scheme, which has a different executable for each channel, computes the nominal and up/down lepton energy scales on top of the input branches.

## Setup (do only once)
```bash
cmsrel CMSSW_9_4_4
cd CMSSW_9_4_4/src
cmsenv
git clone https://github.com/SVfit/ClassicSVfit TauAnalysis/ClassicSVfit -b fastMTT_21_06_2018
git clone https://github.com/SVfit/SVfitTF TauAnalysis/SVfitTF
git clone https://github.com/cecilecaillol/FastMTTFramework
cd $CMSSW_BASE/src
scram b -j 4
```

## Each time we change the input files
Copy the merged files to hdfs to be read by Condor:
https://github.com/skkwan/ToolRoom/blob/master/fileManipulationAndFirstLooks/copyFilesFromCERNtoUW.md

https://github.com/skkwan/ToolRoom/blob/master/fileManipulationAndFirstLooks/gfalCopyDirectoriesIntelligently.py

## Each time we change the .cc files

First make sure that `ROOT/bin/BuildFile.xml` includes the `.cc`.

```bash
# Recompile
scram b -j 8
```

## To run the example to submit one file

```bash
voms-proxy-init --voms=cms --valid=194:00
cmsenv
cd test/
bash example.sh
```

## To submit over all 2018 samples

```bash
voms-proxy-init --voms=cms --valid=194:00
cmsenv
cd test/
python prepare_submit.py --channel=all --year=2018  
# The above step makes a .sh file which we can run
bash do_submit_all2018.sh
```

## Comments on farmoutAnalysisJob behaviour

- The Condor `.sub` file is called `submit` and is located in `/nfs_scratch/`, e.g. 
`/nfs_scratch/skkwan/fastmtt_all2018/TTTo2L2Nu/submit/all_2018_TTTo2L2Nu-postprocessed_ntuple_TTTo2L2Nu_15/submit`
