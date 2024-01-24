"""
Usage: python haddHistograms.py --dir [directory]
where [directory] is some timestamped directory.

Add --recreate (FALSE by default) to force re-creation of existing postprocessed_ntuple_{sample}.root
Add --execute to execute all the bash commands.

Hadds files in [dir]/[sample]/*.root into [dir]/sample.root.
"""

import argparse
import os
import sys


parser = argparse.ArgumentParser(
    description="Hadd files from a directory with sub-directories"
)
# parser.add_argument('--year',    dest='year',    help='Year of dataset', required=True)
parser.add_argument("--dir", dest="rootdir", help="Path to directory", required=True)
parser.add_argument("--recreate", dest="recreate", action="store_true")
parser.add_argument("--execute", dest="execute", action="store_true")
parser.set_defaults(recreate=False)
parser.set_defaults(execute=False)

args = parser.parse_args()

if not args.rootdir.endswith("/"):
    args.rootdir += "/"

tempDir = "/nfs_scratch/skkwan/"

print("Directory to use: " + args.rootdir)

targetFiles = []
for subdir, dirs, files in os.walk(args.rootdir):
    sampleName = os.path.basename(os.path.normpath(subdir))
    outputFile = tempDir + "info_svfit_postprocessed_" + sampleName + ".root"

    if sampleName == os.path.basename(args.rootdir):
        print(">>> Skipping " + sampleName)
        continue

    targetFiles.append(outputFile)
    targetFile = args.rootdir + sampleName + "/*postprocessed_ntuple_*.root"
    # if os.path.exists(outputFile) and not args.recreate:
    #     print(outputFile + " exists, do not recreate")
    #     continue
    command = "hadd -f -T " + outputFile + " " + targetFile
    command2 = "mv {} {}".format(outputFile, args.rootdir)
    print(command)
    print(command2)
    if args.execute:
        os.system(command)
        os.system(command2)

if not args.execute:
    print("--execute flag was not set: did not execute any commands")
