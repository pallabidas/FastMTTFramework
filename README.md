# FastMTTFramework

```

cmsrel CMSSW_9_4_4
cd CMSSW_9_4_4/src
cmsenv
git clone https://github.com/SVfit/ClassicSVfit TauAnalysis/ClassicSVfit -b fastMTT_21_06_2018
git clone https://github.com/SVfit/SVfitTF TauAnalysis/SVfitTF
git clone https://github.com/cecilecaillol/FastMTTFramework
cd $CMSSW_BASE/src
scram b -j 4
```

Merge skimmed files to have 20k events per file: 

```
cd FastMTTFramework/tools
mkdir /nfs_scratch/user/caillol/haabbtt_et2016_7sep_merged #edit name to match your area and samples
# edit controlledMerge_bbtt.py to change the location of your skimmed files (originalDir and targetDir)
python controlledMerge_bbtt.py --channel=et --year=2016 
```

Copy the merged files to hdfs to be read by Condor:

```
cp -r /nfs_scratch/user/caillol/haabbtt_et2016_7sep_merged /hdfs/store/user/caillol/.
```

Setup proxy:

```
 voms-proxy-init --voms=cms --valid=48:00
```

Submit jobs to condor:

```
cd FastMTTFramework/test
python prepare_submit.py --channel=et --year=2016 #edit name of area in code
sh do_submit_et2016.sh
```

