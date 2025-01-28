# FastMTTFramework

## Introduction
This repository is for running the `FastMTT` algorithm for estimating the full di-tau mass on samples with two visible taus. Depending on the decay modes of the taus, there are different settings to pass to the
`FastMTT` function. 

If the input consists of a `TTree` with events in all three channels (`mutau`, `etau`, and `emu`) together, with a branch called `channel` that is equal to `0` for `mutau`, `1` for `etau`, and `2` for `emu`, use 
the flag `--channel=all`, which will call the executable in `ROOT/bin/SVFit.cc`. 

If the input has folders `mutau/event_tree`, `etau/event_tree`, and `emu/event_tree`, this will also work as long as `channel` is still defined as above, since the script automatically loops through all objects in the input `.ROOT` file.

This combined executable assumes that the input `TTree` already has the nominal lepton energy scales applied, and the lepton up/down shifts already in the input `TTree` as well.

The older scheme, which has a different executable for each channel, computes the nominal and up/down lepton energy scales on top of the input branches.

In the [LUNA framework](https://gitlab.cern.ch/skkwan/lunaFramework), this step is meant to be run after `postprocessing`, and before the DNN evaluation.

Features to be added:
- Benchmark runtimes with systematics enabled

## Setup (do only once)
```bash
cmsrel CMSSW_13_3_0
cd CMSSW_13_3_0/src
cmsenv
git clone https://github.com/SVfit/ClassicSVfit TauAnalysis/ClassicSVfit -b fastMTT_21_06_2018
git clone https://github.com/SVfit/SVfitTF TauAnalysis/SVfitTF
git clone https://github.com/cecilecaillol/FastMTTFramework
cd $CMSSW_BASE/src
scram b -j 12
```

## Submit condor jobs in lxplus

Use the `FastMTTFramework/ROOT/bin/submit_jobs.sh` file with input argument being the dataset name, corresponding to `input_${1}.list` that has the input root files listed.

```bash
voms-proxy-init --voms=cms --valid=194:00
cmsenv
cd FastMTTFramework/ROOT/bin/
./submit_jobs.sh TTTo2L2Nu
```
