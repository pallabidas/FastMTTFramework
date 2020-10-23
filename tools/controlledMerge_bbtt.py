# From truggles
# https://github.com/truggles/Z_to_TauTau_13TeV/blob/master/util/svFitMerger.py

import ROOT
import os, glob, subprocess


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

        # Merge to ~ 1000 events per file
        f = ROOT.TFile(file_,'r')
        t = f.Get(ttreePath)
	print file_,t.GetEntries()
        size = t.GetEntries()
        print size,"   ",file_
        runningSize += size
        print "running size: ",runningSize
        toMerge.append( file_ )
        if runningSize > 15000 :
            runningSize = 0
            mergeList = ["hadd", "-f", targetDir+"/%s/ntuple_%i_%s.root" % (sample, rep, channel)]
            for f in toMerge :
                mergeList.append( f )
            subprocess.call( mergeList )
            ints = []
            toMerge = []
            rep += 1
    mergeList = ["hadd", "-f", targetDir+"/%s/ntuple_%i_%s.root" % (sample, rep, channel)]
    for f in toMerge :
        mergeList.append( f )
    if len( mergeList ) > 3 : # greater than 3 means we actually have a file to merge (not empty)
        subprocess.call( mergeList )



if __name__ == '__main__' :

    #samples=["Out_VBFHTT","Out_VBFHWW","Out_WminusHTT","Out_GGHTT_v1","Out_WplusHTT","Out_GGHTT_v2","Out_ZHTT","Out_GGHWW","Out_GGZHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_GGZHLLTT","Out_GGZHQQTT","Out_GGZHNNTT","Out_ttHnonbb"] #Higgs 2016

    #samples=["Out_DY4","Out_DY1_v1","Out_EWKZLL_v1","Out_ST_tW_top","Out_W3_v2","Out_WZ_v1","Out_DY2_v1","Out_EWKZLL_v2","Out_ST_t_antitop","Out_W4_v1","Out_WZ_v2","Out_DY3","Out_EWKZNuNu_v1","Out_ST_t_top","Out_W4_v2","Out_W_v1","Out_DY_v1","Out_EWKWminus_v1","Out_EWKZNuNu_v2","Out_W4_v3","Out_W_v2","Out_DY_v2","Out_EWKWminus_v2","Out_EWKZNuNu_v3","Out_VBFHTT","Out_VBFHWW","Out_WG_v1","Out_WminusHTT","Out_EWKWminus_v3","Out_GGHTT_v1","Out_W1","Out_WG_v2","Out_WplusHTT","Out_EWKWplus_v1","Out_GGHTT_v2","Out_W2_v1","Out_WG_v3","Out_ZHTT","Out_EWKWplus_v2","Out_GGHWW","Out_W2_v2","Out_WW_v1","Out_ZZ_v1","Out_EWKWplus_v3","Out_ST_tW_antitop","Out_W3_v1","Out_WW_v2","Out_ZZ_v2","Out_GGZHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_GGZHLLTT","Out_GGZHQQTT","Out_ZZ4L","Out_ZZ2L2Q","Out_VV2L2Nu_v1","Out_VV2L2Nu_v2","Out_WW1L1Nu2Q","Out_WZ1L3Nu","Out_WZ3L1Nu","Out_WZ1L1Nu2Q","Out_WZ2L2Q","Out_TT","Out_GGZHNNTT","Out_DataC","Out_DataH","Out_DataD","Out_DataE","Out_DataBv1","Out_DataF","Out_DataBv2","Out_DataG","Out_embeddedB","Out_embeddedC","Out_embeddedD","Out_embeddedE","Out_embeddedF","Out_embeddedG","Out_embeddedH"] # All 2016

    #samples=["Out_DY2_v1","Out_DY_low","Out_W3_v1","Out_W_v1","Out_ZZ_v1","Out_DY2_v2","Out_EWKWminus","Out_W3_v2","Out_WW_v1","Out_DY3_v1","Out_EWKWplus","Out_VBFHTT","Out_W4_v1","Out_WW_v2","Out_DY3_v2","Out_EWKZLL","Out_W1_v1","Out_W4_v2","Out_WZ_v1","Out_DY1_v1","Out_DY4_v1","Out_EWKZNuNu","Out_ttHTT","Out_W2_v1","Out_WminusHTT","Out_WZ_v2","Out_DY1_v2","Out_DY_v1","Out_GGHTT","Out_TTTo2L2Nu","Out_W2_v2","Out_WplusHTT","Out_ZHTT","Out_WG","Out_GGZHNNTT","Out_GGZHQQTT","Out_GGZHLLTT","Out_GGHWW","Out_GGZHWW","Out_VBFHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_TTToSemiLeptonic_v1","Out_TTToSemiLeptonic_v2","Out_TTToHadronic_v1","Out_TTToHadronic_v2","Out_ZZ4L","Out_ZZ2L2Q","Out_VV2L2Nu","Out_WW1L1Nu2Q","Out_WZ1L3Nu","Out_WZ3L1Nu_v1","Out_WZ3L1Nu_v2","Out_WZ1L1Nu2Q","Out_WZ2L2Q","Out_ST_t_antitop","Out_ST_t_top","Out_ST_tW_antitop","Out_ST_tW_top","Out_EmbeddedA","Out_EmbeddedB","Out_EmbeddedC","Out_EmbeddedD","Out_DataA","Out_DataB","Out_DataC","Out_DataD"] # All 2018

    #samples =["Out_VBFHTT","Out_ttHTT","Out_WminusHTT","Out_GGHTT","Out_WplusHTT","Out_ZHTT","Out_GGZHNNTT","Out_GGZHQQTT","Out_GGZHLLTT","Out_GGHWW","Out_GGZHWW","Out_VBFHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW"]

    #samples=["Out_DY1","Out_DY_v2","Out_EWKZNuNu","Out_ST_t_top","Out_W1_v1","Out_W4","Out_WplusHTT","Out_DY2","Out_DYlow","Out_GGHTT","Out_TTTo2L2Nu","Out_W1_v2","Out_WW","Out_ZHTT","Out_DY3","Out_EWKWminus","Out_ST_tW_antitop","Out_TTToHadronic","Out_W2_v1","Out_WZ","Out_ZZ","Out_DY4_v1","Out_DY4_v2","Out_EWKWplus","Out_ST_tW_top_v1","Out_ST_tW_top_v2","Out_TTToSemiLeptonic","Out_W2_v2","Out_W_v1","Out_W_v2","Out_W_v3","Out_DY_v1","Out_EWKZLL","Out_ST_t_antitop_v1","Out_ST_t_antitop_v2","Out_VBFHTT","Out_W3","Out_WminusHTT","Out_WG","Out_GGZHNNTT","Out_GGZHQQTT","Out_GGZHLLTT","Out_GGHWW","Out_GGZHWW","Out_VBFHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_ttHnonbb","Out_ZZ2L2Q","Out_VV2L2Nu","Out_WW1L1Nu2Q","Out_WZ1L3Nu","Out_WZ3L1Nu","Out_WZ1L1Nu2Q","Out_WZ2L2Q","Out_ZZ4L","Out_embeddedB","Out_embeddedC","Out_embeddedD","Out_embeddedE","Out_embeddedF","Out_DataB","Out_DataC","Out_DataD","Out_DataE","Out_DataF"] # All 2017

    #samples=["Out_WplusHTT","Out_GGHTT","Out_ZHTT","Out_VBFHTT","Out_WminusHTT","Out_GGZHNNTT","Out_GGZHQQTT","Out_GGZHLLTT","Out_GGHWW","Out_GGZHWW","Out_VBFHWW","Out_WminusHWW","Out_WplusHWW","Out_ZHWW","Out_ttHnonbb"] #Higgs 2017

    samples=["Out_ttHnonbb"]

    originalDir = '/nfs_scratch/caillol/differentialmt2016_3aug'
    targetDir = '/nfs_scratch/caillol/differentialmt2016_3aug_merged'
    jobId = ''
    channel = 'xx'
    ttreePath = 'mutau_tree'
    for sample in samples :
	print sample
        mergeSample( jobId, sample, channel, ttreePath, originalDir, targetDir )


