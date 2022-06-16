# FastMTTFramework

(Instructions modified from original)

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

(Not necessary for me) Merge skimmed files to have 20k events per file: 

```
cd FastMTTFramework/tools
mkdir /nfs_scratch/user/caillol/haabbtt_et2016_7sep_merged #edit name to match your area and samples
# edit controlledMerge_bbtt.py to change the location of your skimmed files (originalDir and targetDir)
python controlledMerge_bbtt.py --channel=mt --year=2018
```

Copy the merged files to hdfs to be read by Condor:
https://github.com/skkwan/ToolRoom/blob/master/fileManipulationAndFirstLooks/copyFilesFromCERNtoUW.md

https://github.com/skkwan/ToolRoom/blob/master/fileManipulationAndFirstLooks/gfalCopyDirectoriesIntelligently.py

```
cp -r /nfs_scratch/skkwan/hToAA/mt2018/Apr-04-2022-18h26m-DataMC2018_JERsys_withTES_merged /hdfs/store/user/skkwan/hToAA/skims/mt2018/.
```

Setup proxy:

```
 voms-proxy-init --voms=cms --valid=48:00
```

Submit jobs to condor:

```
cd FastMTTFramework/test
python prepare_submit.py --channel=mt --year=2018 #edit name of area in code
sh do_submit_et2016.sh
```

