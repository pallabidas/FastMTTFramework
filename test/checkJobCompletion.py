# lookForErrorFiles.py
# Usage:
#   cmsenv
#   cd ${CMSSW_BASE}/src/lunaFramework
#   source venv-LUNA/bin/activate
#   python lookForErrorsFiles.py --rootdir=/nfs_scratch/skkwan/fastmtt_all2018/ --dryrun


import argparse
import os
import re
import sys
import shutil

commandTemplate="python svFitSubmitter.py -sd FILEGOESHERE -es=0 -ues=0 -res=0 -jes=0  -year=2018  --channel='all' --jobName fastmtt_2024-02-06-10h24m_rescue"

parser = argparse.ArgumentParser(
    description='Check a folder of Condor log files for failed jobs (jobs that do not have the "normal termination" string.'
)

parser.add_argument(
    "--execute", help="Create new rescue *.csvs.", dest="execute", action="store_true"
)
parser.add_argument(
    "--dryrun",
    help="Only check log files, do not create new rescue *.csv.",
    dest="execute",
    action="store_false",
)
parser.add_argument(
    "--rootdir", help="Directory with Condor logs to scan", required=True
)
parser.set_defaults(execute=False)

args = parser.parse_args()

# The string in the .out ffile that indicates a succesful job
outfile_proofstring = "dashboard_completion called with exit code 0"
outfile_proofstring2 = "Found key=event_tree"
outfile_alsoOK = "exiting as though successful"

badlogsfile = "badlogs.csv"
os.system("rm {}".format(badlogsfile))

rescueScript = "do_rescue_submit_all2018.sh"
os.system("rm {}".format(rescueScript))

# ---- End of setup --------------

# Loop through log files in the directory
for subdir, dirs, files in os.walk(args.rootdir):
    directoryName = os.path.basename(subdir)

    # Loop through the Condor log files
    if "postprocessed_ntuple" not in directoryName:
        continue

    for filename in files:
        if ".out" not in filename:
            continue
        logpath = os.path.join(subdir, filename)
        # print("Checking {}".format(logpath))
        with open(logpath) as flog:
            content = flog.read()
            hasStandardProofstrings = (outfile_proofstring in content) and (outfile_proofstring2 in content)
            hasDuplicateFileProofstring = (outfile_alsoOK in content)
            isLogValid = (hasStandardProofstrings or hasDuplicateFileProofstring)
            if not isLogValid:
                print(">>> proofstring {}, {}, or {} not found in {}".format(outfile_proofstring, outfile_proofstring2, outfile_alsoOK, logpath))
                with open(badlogsfile, "a") as badlogs:
                    badlogs.write("{}\n".format(logpath))
            
                # In this directory, get the .inputs file
                inputpath = logpath.replace(".out", ".inputs")
                with open(inputpath, "r") as fIn:
                    inputContent = fIn.read()
                    thisFilePath = inputContent.replace("root://cmsxrootd.hep.wisc.edu/", "/hdfs").replace("\n", "")
                    with open(rescueScript, "a") as rescue:
                        rescue.write(commandTemplate.replace("FILEGOESHERE", thisFilePath))
                        rescue.write("\n")

