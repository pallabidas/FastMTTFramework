# From truggles
# https://github.com/truggles/Z_to_TauTau_13TeV/blob/master/util/svFitMerger.py

import ROOT
import os, glob, subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--channel', '-c', default=None, help='Output name')
parser.add_argument('--year', '-y', default=None, help='Output name')
args = parser.parse_args()



# Check if directory exists, make it if not
def checkDir( dirName ) :
    if not os.path.exists( dirName ) : os.makedirs( dirName )




def mergeSample( jobId, sample, channel, ttreePath, originalDir, targetDir ) :
    #files = glob.glob(originalDir+'/%s%s_*_%s.root' % (jobId, sample, channel) )
    print originalDir+'/%s/*.root' % (sample)
    files = glob.glob(originalDir+'/%s/*.root' % (sample) )
    checkDir( targetDir+'/%s' % (sample) )

    rep = 0
    runningSize = 0
    toMerge = []
    ints = []
    for file_ in files :

        # Merge to ~ 20000 events per file
        f = ROOT.TFile(file_,'r')
        t = f.Get(ttreePath)
	print file_,t.GetEntries()
        size = t.GetEntries()
        print size,"   ",file_
        runningSize += size
        print "running size: ",runningSize
        toMerge.append( file_ )
        if runningSize > 20000 :
            runningSize = 0
            # mergeList = ["hadd", "-f", targetDir+"/%s/ntuple_%i_%s.root" % (sample, rep, channel)]
            mergeList = ["hadd", "-f", targetDir+"/%s/%s_%i.root" % (sample, sample, rep)]
            for f in toMerge :
                mergeList.append( f )
            subprocess.call( mergeList )
            ints = []
            toMerge = []
            rep += 1
#    mergeList = ["hadd", "-f", targetDir+"/%s/ntuple_%i_%s.root" % (sample, rep, channel)]
    mergeList = ["hadd", "-f", targetDir+"/%s/%s_%i.root" % (sample, sample, rep)]
    for f in toMerge :
        mergeList.append( f )
    if len( mergeList ) > 3 : # greater than 3 means we actually have a file to merge (not empty)
        subprocess.call( mergeList )



if __name__ == '__main__' :

    samples2016=["Out_DY1_v1","Out_DY2_v1","Out_DY3","Out_DY4","Out_DY_v1","Out_DY_v2","Out_W1","Out_W2_v1","Out_W2_v2","Out_W3_v1","Out_W3_v2","Out_W4_v1","Out_W4_v2","Out_W4_v3","Out_W_v1","Out_W_v2","Out_ST_t_antitop","Out_ST_t_top","Out_ST_tW_antitop","Out_ST_tW_top","Out_TT","Out_VV2L2Nu_v1","Out_VV2L2Nu_v2","Out_WW1L1Nu2Q","Out_WZ1L1Nu2Q","Out_WZ1L3Nu","Out_WZ2L2Q","Out_WZ3L1Nu","Out_ZZ2L2Q","Out_ZZ4L","Out_GGHTT_v1","Out_GGHTT_v2","Out_GGHWW","Out_GGZHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_VBFHTT","Out_VBFHWW","Out_WminusHTT","Out_WplusHTT","Out_ZHTT","Out_GGZHLLTT","Out_GGZHNNTT","Out_GGZHQQTT","Out_ttHnonbb","Out_ggH_bbtt15","Out_vbf_bbtt15","Out_ggH_bbtt20","Out_vbf_bbtt20","Out_ggH_bbtt25","Out_vbf_bbtt25","Out_ggH_bbtt30","Out_vbf_bbtt30","Out_ggH_bbtt35","Out_vbf_bbtt35","Out_ggH_bbtt40","Out_vbf_bbtt40","Out_ggH_bbtt45","Out_vbf_bbtt45","Out_ggH_bbtt50","Out_vbf_bbtt50","Out_ggH_bbtt55","Out_vbf_bbtt55","Out_ggH_bbtt60","Out_vbf_bbtt60","Out_embeddedB","Out_embeddedC","Out_embeddedD","Out_embeddedE","Out_embeddedF","Out_embeddedG","Out_embeddedH","Out_DataBv1","Out_DataBv2","Out_DataC","Out_DataD","Out_DataE","Out_DataF","Out_DataG","Out_DataH"] 

    samples2017=["Out_DY1","Out_DY2","Out_DY3","Out_DY4_v1","Out_DY4_v2","Out_DY_v1","Out_DY_v2","Out_W1_v1","Out_W1_v2","Out_W2_v1","Out_W2_v2","Out_W3","Out_W4","Out_W_v1","Out_W_v2","Out_W_v3","Out_ST_t_antitop_v1","Out_ST_t_antitop_v2","Out_ST_t_top","Out_ST_tW_antitop","Out_ST_tW_top_v1","Out_ST_tW_top_v2","Out_WW","Out_WZ","Out_ZZ","Out_VV2L2Nu","Out_WW1L1Nu2Q","Out_WZ1L1Nu2Q","Out_WZ1L3Nu","Out_WZ2L2Q","Out_WZ3L1Nu","Out_ZZ2L2Q","Out_ZZ4L","Out_TTTo2L2Nu","Out_TTToHadronic","Out_TTToSemiLeptonic","Out_GGHTT","Out_GGHWW","Out_GGZHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_VBFHTT","Out_VBFHWW","Out_WminusHTT","Out_WplusHTT","Out_ZHTT","Out_GGZHLLTT","Out_GGZHNNTT","Out_GGZHQQTT","Out_ttHnonbb","Out_ggH_bbtt12","Out_vbf_bbtt12","Out_ggH_bbtt20","Out_vbf_bbtt20","Out_ggH_bbtt30","Out_vbf_bbtt30","Out_ggH_bbtt40","Out_vbf_bbtt40","Out_ggH_bbtt50","Out_vbf_bbtt50","Out_ggH_bbtt60","Out_vbf_bbtt60","Out_embeddedB","Out_embeddedC","Out_embeddedD","Out_embeddedE","Out_embeddedF","Out_DataB","Out_DataC","Out_DataD","Out_DataE","Out_DataF"] 

#     samples2018=["Out_DY1_v1","Out_DY1_v2","Out_DY2_v1","Out_DY2_v2","Out_DY3_v1","Out_DY3_v2","Out_DY4_v1","Out_DY_v1","Out_GGHTT","Out_ST_t_antitop","Out_ST_t_top","Out_ST_tW_antitop","Out_ST_tW_top","Out_TTTo2L2Nu","Out_TTToHadronic_v1","Out_TTToHadronic_v2","Out_TTToSemiLeptonic_v1","Out_TTToSemiLeptonic_v2","Out_VBFHTT","Out_VV2L2Nu","Out_W1_v1","Out_W2_v1","Out_W2_v2","Out_W3_v1","Out_W3_v2","Out_W4_v1","Out_W4_v2","Out_W_v1","Out_WZ2L2Q","Out_WZ3LNu_v1","Out_WZ3LNu_v2","Out_WminusHTT","Out_WplusHTT","Out_ZHTT","Out_ZZ2L2Q","Out_ZZ4L","Out_GGZHLLTT","Out_GGZHNNTT","Out_GGZHQQTT","Out_ttHnonbb","Out_GGHWW","Out_GGZHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_VBFHWW","Out_ggH_bbtt12","Out_vbf_bbtt12","Out_ggH_bbtt20","Out_vbf_bbtt20","Out_ggH_bbtt30","Out_vbf_bbtt30","Out_ggH_bbtt40","Out_vbf_bbtt40","Out_ggH_bbtt50","Out_vbf_bbtt50","Out_ggH_bbtt60","Out_vbf_bbtt60","Out_wh_bbtt40","Out_zh_bbtt40","Out_embeddedA","Out_embeddedB","Out_embeddedC","Out_embeddedD","Out_DataA","Out_DataB","Out_DataC","Out_DataD"] #2018 EDIT

#    samples2018=["DY1JetsToLL"]

    samples2018=["DY1JetsToLL", "DY2JetsToLL", "DY3JetsToLL", "DY4JetsToLL", "DYJetsToLL", "DYJetsToLL_M-10to50", "Embedded-Run2018A-MuTau", "Embedded-Run2018B-MuTau", "Embedded-Run2018C-MuTau", "Embedded-Run2018D-MuTau", "ggZH_HToTauTau_ZToLL", "ggZH_HToTauTau_ZToNuNu", "ggZH_HToTauTau_ZToQQ", "GluGluHToTauTau", "GluGluHToWWTo2L2Nu", "GluGluZH_HToWW", "HWminusJ_HToWW", "HWplusJ_HToWW", "HZJ_HToWW", "SingleMuon-Run2018A", "SingleMuon-Run2018B", "SingleMuon-Run2018C", "SingleMuon-Run2018D", "ST_t-channel_antitop", "ST_t-channel_top", "ST_tW_antitop", "ST_tW_top", "SUSYGluGluToHToAA_AToBB_AToTauTau_M-45", "SUSYVBFHToAA_AToBB_AToTauTau_M-45", "ttHTobb", "ttHToNonbb", "TTTo2L2Nu", "TTToHadronic", "TTToHadronic-ext2", "TTToSemiLeptonic", "TTToSemiLeptonic-ext3", "VBFHToTauTau", "VBFHToWWTo2L2Nu", "VVTo2L2Nu", "W1JetsToLNu", "W2JetsToLNu", "W3JetsToLNu", "W4JetsToLNu", "WJetsToLNu", "WminusHToTauTau", "WplusHToTauTau", "WZTo2L2Q", "WZTo3LNu", "ZHToTauTau", "ZZTo2L2Q", "ZZTo4L"]

    samples=samples2016
    if args.year=="2017":
       samples=samples2017
    if args.year=="2018":
       samples=samples2018

    folder='Apr-04-2022-18h26m-DataMC2018_JERsys_withTES'
    originalDir = '/hdfs/store/user/skkwan/hToAA/skims/'+args.channel+args.year+'/'+folder
    targetDir = '/nfs_scratch/skkwan/hToAA/mergedskims/'+args.channel+args.year+'/'+folder+'_merged'

    jobId = ''
    channel = 'xx'
    ttreePath = 'etau_tree' 
    if args.channel=="em":
       ttreePath = 'emu_tree'
    if args.channel=="mt":
       ttreePath = 'mutau_tree'
    if args.channel=="tt":
       ttreePath = 'tautau_tree'

    for sample in samples :
	print sample
        mergeSample( jobId, sample, channel, ttreePath, originalDir, targetDir )


