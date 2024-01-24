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
outfile_alsoOK = "exiting as though successful"

rescuefile = "rescue.csv"
os.system("rm {}".format(rescuefile))

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
        print("Checking {}".format(logpath))
        with open(logpath) as flog:
            if not ((outfile_proofstring in flog.read()) or (outfile_alsoOK in flog.read())):
                print(">>> proofstring {} not found in {}".format(outfile_proofstring, logpath))
                with open("rescue.csv", "a") as rescuefile:
                    rescuefile.write("{}\n".format(logpath))

