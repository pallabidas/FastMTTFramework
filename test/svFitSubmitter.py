
import sys, getopt

import os
import sys
import glob
import pwd
import argparse
import errno
import socket
import signal
import logging
import math

def get_parser(desc):
    parser = argparse.ArgumentParser(description=desc)

    return parser

def parse_command_line(argv):
    parser = get_parser("Run the desired analyzer on FSA n-tuples")

    parser.add_argument('-s','--submit',action='store_true',help='Submit jobs to condor')
    parser.add_argument('-dr','--dryrun',action='store_true',help='Create jobs but dont submit')
    parser.add_argument('-t','--test',action='store_true',help='Do a test (output test.root)')
    parser.add_argument('-jn','--jobName',nargs='?',type=str,const='',help='Job Name for condor submission')
    parser.add_argument('-d','--customDir',nargs='?',type=str,const='',help='Custom input directory')
    parser.add_argument('-sd','--sampledir',nargs='?',type=str,const='',help='The Sample Input directory')
    parser.add_argument('-es','--doES',nargs='?',type=str,const='',help='Doing TES / EES shifts?')
    parser.add_argument('-ues','--doUES',nargs='?',type=str,const='',help='Doing TES / EES shifts?')
    parser.add_argument('-res','--doRES',nargs='?',type=str,const='',help='Doing TES / EES shifts?')
    parser.add_argument('-jes','--doJES',nargs='?',type=str,const='',help='Doing TES / EES shifts?')
    parser.add_argument('-year','--year',nargs='?',type=str,const='',help='Doing TES / EES shifts?')
    parser.add_argument('-channel','--channel',type=str)
    args = parser.parse_args(argv)

    return args

print "svfit standalone submit"


def main(argv=None):
    '''
    Submit a job using farmoutAnalysisJobs --fwklite
    '''
    print "svfit standalone submit"
    args = parse_command_line(argv)
    jobName = args.jobName
    channel = args.channel
    period = args.year
    dryrun = args.dryrun
    sampledir = args.sampledir
    sample_name = os.path.basename(sampledir)
    print "sample_name:",sample_name
    if sample_name == '' :
        print "SAMPLE_NAME not defined, check for trailing '/' on sampledir path"
        return
    if 'uwlogin' in socket.gethostname():
        sample_dir = '/data/%s/%s/%s' % (pwd.getpwuid(os.getuid())[0], jobName, sample_name)
    else:
        sample_dir = '/nfs_scratch/%s/%s/%s' % (pwd.getpwuid(os.getuid())[0], jobName, sample_name)
    submit_dir = '%s/submit' % (sample_dir)
    if os.path.exists(submit_dir):
        print('Submission directory exists for %s %s.' % (jobName, sample_name))
            
    dag_dir = '%s/dags/dag' % (sample_dir)
    os.system('mkdir -p %s' % (os.path.dirname(dag_dir)))
    os.system('mkdir -p %s' % (dag_dir+'inputs'))

    output_dir = '/store/user/%s/%s/%s/'\
        % (pwd.getpwuid(os.getuid())[0], jobName, sample_name)

    filelist = ['%s/%s' % (sampledir, x) for x in os.listdir(sampledir)]
    numfiles = len(filelist)
    totalsize = sum([os.path.getsize(f) for f in filelist])
    averagesize = totalsize/numfiles
    filesperjob = 1
    input_name = '%s/%s.txt' % (dag_dir+'inputs', sample_name)
    with open(input_name,'w') as file:
        for f in filelist:
            file.write('%s\n' % f.replace('/hdfs','',1))

    bash_name = '%s/%s_%s_%s.sh' % (dag_dir+'inputs', channel, period, sample_name)
    bashScript = "#!/bin/bash\n value=$(<$INPUT)\n echo \"$value\"\n"
    if args.channel=="em":
       bashScript += '$CMSSW_BASE/bin/$SCRAM_ARCH/SVFitStandAloneFSATauDM_emu_norecoil inputfile=$value newOutputFile=1.0 newFile=\'$OUTPUT\'' #% (channel, sample_name, period)
    if args.channel=="et":
       bashScript += '$CMSSW_BASE/bin/$SCRAM_ARCH/SVFitStandAloneFSATauDM_etau_norecoil inputfile=$value newOutputFile=1.0 newFile=\'$OUTPUT\'' #% (channel, sample_name, period)
    if args.channel=="mt":
       bashScript += '$CMSSW_BASE/bin/$SCRAM_ARCH/SVFitStandAloneFSATauDM_mutau_norecoil inputfile=$value newOutputFile=1.0 newFile=\'$OUTPUT\'' #% (channel, sample_name, period)
    if args.channel=="tt":
       bashScript += '$CMSSW_BASE/bin/$SCRAM_ARCH/SVFitStandAloneFSATauDM_tautau_norecoil inputfile=$value newOutputFile=1.0 newFile=\'$OUTPUT\'' #% (channel, sample_name, period)
    if args.channel=="all":
        bashScript += '$CMSSW_BASE/bin/$SCRAM_ARCH/SVFit inputfile=$value newOutputFile=1.0 newFile=\'$OUTPUT\'' #% (channel, sample_name, period)
    if args.doES : doES = "doES="+args.doES
    else : doES = ''
    bashScript += ' %s' % (doES)
    if args.doUES : doUES = "doUES="+args.doUES
    else : doUES = ''
    bashScript += ' %s' % (doUES)
    if args.doRES : doRES = "doRES="+args.doRES
    else : doRES = ''
    bashScript += ' %s' % (doRES)
    if args.doJES : doJES = "doJES="+args.doJES
    else : doJES = ''
    bashScript += ' %s' % (doJES)
    if args.year : year = "year="+args.year
    else : year = ''
    bashScript += ' %s' % (year)
    bashScript += '\n'
    with open(bash_name,'w') as file:
        file.write(bashScript)
    os.system('chmod +x %s' % bash_name)
                
# create farmout command
    farmoutString = 'farmoutAnalysisJobs --infer-cmssw-path --fwklite --input-file-list=%s' % (input_name)
    farmoutString += ' --submit-dir=%s --output-dag-file=%s --output-dir=%s' % (submit_dir, dag_dir, output_dir)
    farmoutString += ' --input-files-per-job=%i %s %s' % (filesperjob, jobName, bash_name)
    farmoutString += ' --opsys=CentOS7'
                
    if not args.dryrun:
        print('Submitting %s' % sample_name)
        os.system(farmoutString)
    else:
        print farmoutString
        
    return


if __name__ == '__main__':
    main()
