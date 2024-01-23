import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--channel', '-c', default=None, required=True)
parser.add_argument('--year', '-y', default=None, required=True)

args = parser.parse_args()

if __name__ == "__main__":

    names2016=[
       "DY1_v1","1","0","1","0",
       "DY2_v1","1","0","1","0",
       "DY3","1","0","1","0",
       "DY4","1","0","1","0",
       "DY_v1","1","0","1","0",
       "DY_v2","1","0","1","0",
       "W1","1","0","1","0",
       "W2_v1","1","0","1","0",
       "W2_v2","1","0","1","0",
       "W3_v1","1","0","1","0",
       "W3_v2","1","0","1","0",
       "W4_v1","1","0","1","0",
       "W4_v2","1","0","1","0",
       "W4_v3","1","0","1","0",
       "W_v1","1","0","1","0",
       "W_v2","1","0","1","0",
       "ST_t_antitop","1","1","0","1",
       "ST_t_top","1","1","0","1",
       "ST_tW_antitop","1","1","0","1",
       "ST_tW_top","1","1","0","1",
       "TT","1","1","0","1",
       "VV2L2Nu_v1","1","1","0","1",
       "VV2L2Nu_v2","1","1","0","1",
       "WW1L1Nu2Q","1","1","0","1",
       "WZ1L1Nu2Q","1","1","0","1",
       "WZ1L3Nu","1","1","0","1",
       "WZ2L2Q","1","1","0","1",
       "WZ3L1Nu","1","1","0","1",
       "ZZ2L2Q","1","1","0","1",
       "ZZ4L","1","1","0","1",
       "GGHTT_v1","1","0","1","0",
       "GGHTT_v2","1","0","1","0",
       "GGHWW","1","0","1","0",
       "GGZHWW","1","1","0","1",
       "WminusHWW","1","1","0","1",
       "WplusHWW","1","1","0","1",
       "ZHWW","1","1","0","1",
       "VBFHTT","1","0","1","0",
       "VBFHWW","1","0","1","0",
       "WminusHTT","1","1","0","1",
       "WplusHTT","1","1","0","1",
       "ZHTT","1","1","0","1",
       "GGZHLLTT","1","1","0","1",
       "GGZHNNTT","1","1","0","1",
       "GGZHQQTT","1","1","0","1",
       "ttHnonbb","1","1","0","1",
       "ggH_bbtt15","1","0","1","0",
       "vbf_bbtt15","1","0","1","0",
       "ggH_bbtt20","1","0","1","0",
       "vbf_bbtt20","1","0","1","0",
       "ggH_bbtt25","1","0","1","0",
       "vbf_bbtt25","1","0","1","0",
       "ggH_bbtt30","1","0","1","0",
       "vbf_bbtt30","1","0","1","0",
       "ggH_bbtt35","1","0","1","0",
       "vbf_bbtt35","1","0","1","0",
       "ggH_bbtt40","1","0","1","0",
       "vbf_bbtt40","1","0","1","0",
       "ggH_bbtt45","1","0","1","0",
       "vbf_bbtt45","1","0","1","0",
       "ggH_bbtt50","1","0","1","0",
       "vbf_bbtt50","1","0","1","0",
       "ggH_bbtt55","1","0","1","0",
       "vbf_bbtt55","1","0","1","0",
       "ggH_bbtt60","1","0","1","0",
       "vbf_bbtt60","1","0","1","0",
       "embeddedB","2","0","0","0",
       "embeddedC","2","0","0","0",
       "embeddedD","2","0","0","0",
       "embeddedE","2","0","0","0",
       "embeddedF","2","0","0","0",
       "embeddedG","2","0","0","0",
       "embeddedH","2","0","0","0",
       "DataBv1","0","0","0","0",
       "DataBv2","0","0","0","0",
       "DataC","0","0","0","0",
       "DataD","0","0","0","0",
       "DataE","0","0","0","0",
       "DataF","0","0","0","0",
       "DataG","0","0","0","0",
       "DataH","0","0","0","0",
    ]

    names2017=[
       "DY1","1","0","1","0",
       "DY2","1","0","1","0",
       "DY3","1","0","1","0",
       "DY4_v1","1","0","1","0",
       "DY4_v2","1","0","1","0",
       "DY_v1","1","0","1","0",
       "DY_v2","1","0","1","0",
       "W1_v1","1","0","1","0",
       "W1_v2","1","0","1","0",
       "W2_v1","1","0","1","0",
       "W2_v2","1","0","1","0",
       "W3","1","0","1","0",
       "W4","1","0","1","0",
       "W_v1","1","0","1","0",
       "W_v2","1","0","1","0",
       "W_v3","1","0","1","0",
       "ST_t_antitop_v1","1","1","0","1",
       "ST_t_antitop_v2","1","1","0","1",
       "ST_t_top","1","1","0","1",
       "ST_tW_antitop","1","1","0","1",
       "ST_tW_top_v1","1","1","0","1",
       "ST_tW_top_v2","1","1","0","1",
       "WW","1","1","0","1",
       "WZ","1","1","0","1",
       "ZZ","1","1","0","1",
       "VV2L2Nu","1","1","0","1",
       "WW1L1Nu2Q","1","1","0","1",
       "WZ1L1Nu2Q","1","1","0","1",
       "WZ1L3Nu","1","1","0","1",
       "WZ2L2Q","1","1","0","1",
       "WZ3L1Nu","1","1","0","1",
       "ZZ2L2Q","1","1","0","1",
       "ZZ4L","1","1","0","1",
       "TTTo2L2Nu","1","1","0","1",
       "TTToHadronic","1","1","0","1",
       "TTToSemiLeptonic","1","1","0","1",
       "GGHTT","1","0","1","0",
       "GGHWW","1","0","1","0",
       "GGZHWW","1","1","0","1",
       "WminusHWW","1","1","0","1",
       "WplusHWW","1","1","0","1",
       "ZHWW","1","1","0","1",
       "VBFHTT","1","0","1","0",
       "VBFHWW","1","0","1","0",
       "WminusHTT","1","1","0","1",
       "WplusHTT","1","1","0","1",
       "ZHTT","1","1","0","1",
       "GGZHLLTT","1","1","0","1",
       "GGZHNNTT","1","1","0","1",
       "GGZHQQTT","1","1","0","1",
       "ttHnonbb","1","1","0","1",
       "ggH_bbtt12","1","0","1","0",
       "vbf_bbtt12","1","0","1","0",
       "ggH_bbtt20","1","0","1","0",
       "vbf_bbtt20","1","0","1","0",
       "ggH_bbtt30","1","0","1","0",
       "vbf_bbtt30","1","0","1","0",
       "ggH_bbtt40","1","0","1","0",
       "vbf_bbtt40","1","0","1","0",
       "ggH_bbtt50","1","0","1","0",
       "vbf_bbtt50","1","0","1","0",
       "ggH_bbtt60","1","0","1","0",
       "vbf_bbtt60","1","0","1","0",
       "embeddedB","2","0","0","0",
       "embeddedC","2","0","0","0",
       "embeddedD","2","0","0","0",
       "embeddedE","2","0","0","0",
       "embeddedF","2","0","0","0",
       "DataB","0","0","0","0",
       "DataC","0","0","0","0",
       "DataD","0","0","0","0",
       "DataE","0","0","0","0",
       "DataF","0","0","0","0"
    ]

    names2018=[
        ["DY1JetsToLL", "1","0","1","0"],
        ["DY2JetsToLL", "1","0","1","0"],
        ["DY3JetsToLL", "1","0","1","0"],
        ["DY4JetsToLL", "1","0","1","0"],
        ["DYJetsToLL_M-50",  "1","0","1","0"],
        ["DYJetsToLL_M-10to50", "1","0","1","0"],

        ["GluGluZH_HToWW_ZTo2L", "1","1","0","1"],
        ["GluGluZH_HToWWTo2L2Nu", "1","1","0","1"],

        ["GluGluHToTauTau", "1","0","1","0"],
        ["GluGluHToWWTo2L2Nu", "1","0","1","0"],  

        ["HWminusJ_HToWW", "1","1","0","1"],
        ["HWplusJ_HToWW",  "1","1","0","1"],
        ["HZJ_HToWW",      "1","1","0","1"],

        ["ST_t-channel_antitop", "1","1","0","1"],
        ["ST_t-channel_top",     "1","1","0","1"],
        ["ST_tW_antitop",        "1","1","0","1"],
        ["ST_tW_top",            "1","1","0","1"],

        ["SUSYGluGluToHToAA_AToBB_AToTauTau_M-45", "1","0","1","0"],
        ["SUSYVBFHToAA_AToBB_AToTauTau_M-45",      "1","0","1","0"],

        ["ttHTobb",    "1","1","0","1"],
        ["ttHToNonbb", "1","1","0","1"],

        ["TTTo2L2Nu",             "1","1","0","1"],
        ["TTToHadronic",          "1","1","0","1"],
        ["TTToSemiLeptonic",      "1","1","0","1"],

        ["VBFHToTauTau",    "1","0","1","0"],

        ["VBFHToWWTo2L2Nu", "1","0","1","0"],

        ["VVTo2L2Nu", "1","1","0","1"],

        ["W1JetsToLNu", "1","0","1","0"],
        ["W2JetsToLNu", "1","0","1","0"],
        ["W3JetsToLNu", "1","0","1","0"],
        ["W4JetsToLNu", "1","0","1","0"],
        ["WJetsToLNu",  "1","0","1","0"],
        ["WminusHToTauTau", "1","1","0","1"], 
        ["WplusHToTauTau",  "1","1","0","1"],

        ["WZTo2Q2L", "1","1","0","1"],
        ["WZTo3LNu", "1","1","0","1"],

        ["ZHToTauTau", "1","1","0","1"],

        ["ZZTo2Q2L", "1","1","0","1"],
        ["ZZTo4L",   "1","1","0","1"],
        ["Embedded-Run2018A-MuTau", "2","0","0","0"],
        ["Embedded-Run2018B-MuTau", "2","0","0","0"],
        ["Embedded-Run2018C-MuTau", "2","0","0","0"],
        ["Embedded-Run2018D-MuTau", "2","0","0","0"],
        ["SingleMuon-Run2018A", "0","0","0","0"],
        ["SingleMuon-Run2018B", "0","0","0","0"],
        ["SingleMuon-Run2018C", "0","0","0","0"],
        ["SingleMuon-Run2018D", "0","0","0","0"],

        ["Embedded-Run2018A-ElTau", "2","0","0","0"],
        ["Embedded-Run2018B-ElTau", "2","0","0","0"],
        ["Embedded-Run2018C-ElTau", "2","0","0","0"],
        ["Embedded-Run2018D-ElTau", "2","0","0","0"],
        ["EGamma-Run2018A", "0","0","0","0"],
        ["EGamma-Run2018B", "0","0","0","0"],
        ["EGamma-Run2018C", "0","0","0","0"],
        ["EGamma-Run2018D", "0","0","0","0"],


        ["Embedded-Run2018A-EMu", "2","0","0","0"],
        ["Embedded-Run2018B-EMu", "2","0","0","0"],
        ["Embedded-Run2018C-EMu", "2","0","0","0"],
        ["Embedded-Run2018D-EMu", "2","0","0","0"],
        ["MuonEG-Run2018A", "0","0","0","0"],
        ["MuonEG-Run2018B", "0","0","0","0"],
        ["MuonEG-Run2018C", "0","0","0","0"],
        ["MuonEG-Run2018D", "0","0","0","0"], 

    ]

    names=names2016
    if args.year=="2017":
      names=names2017
    if args.year=="2018":
      names=names2018

    all_File = open("do_submit_"+args.channel+args.year+".sh" , 'w')
    line=""
#    for j in range(0,len(name)/5):
#       line=line+"python svFitSubmitter.py -sd /hdfs/store/user/caillol/haabbtt_"+args.channel+args.year+"_7sep_merged/Out_"+name[j*5]+" -es="+name[j*5+1]+" -ues="+name[j*5+2]+" -res="+name[j*5+3]+" -jes="+name[j*5+4]+" -year="+args.year+" --channel='"+args.channel+"' --jobName haabbtt_"+args.channel+args.year+"_svfitted_7sep" + "\n"

    resProcesses = ""
    for processInfo in names:
        process = processInfo[0]
        doES    = processInfo[1]
        doUES   = processInfo[2]
        doRes   = processInfo[3]
        doJES   = processInfo[4] 

        doES = "0"
        doUES = "0"
        doRes = "0"
        doJES = "0"

        line=line+"python svFitSubmitter.py -sd /hdfs/store/user/skkwan/hToA1A2/benchmark/"+process + " -es=" + doES + " -ues=" + doUES + " -res=" + doRes + " -jes=" + doJES + "  -year=" + str(args.year) + "  --channel='" + args.channel + "' --jobName fastmtt_" + args.channel + str(args.year) + "\n" 

        if (doRes == "1"):
            resProcesses = resProcesses + ", " + process


    all_File.write(line)
    all_File.close()

