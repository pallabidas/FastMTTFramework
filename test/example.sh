###########################
# Submit jobs to run FastMTT.
# Usage: 
#   bash do_submit.sh
#
# Tips:
#   jobName determines the output folder name: /hdfs/store/user/skkwan/jobName
###########################


# All channels
python svFitSubmitter.py -sd /hdfs/store/user/skkwan/hToA1A2/test -es=0 -ues=0 -res=0 -jes=0  -year=2018  --channel='all' --jobName svfit_nominal_single_script

# # Cross-check 
# python svFitSubmitter.py -sd /hdfs/store/user/skkwan/hToA1A2/test -es=0 -ues=0 -res=0 -jes=0  -year=2018  --channel='mt' --jobName test_svfit_nominal_sync_mutau

# python svFitSubmitter.py -sd /hdfs/store/user/skkwan/hToA1A2/test -es=0 -ues=0 -res=0 -jes=0  -year=2018  --channel='et' --jobName test_svfit_nominal_sync_etau

# python svFitSubmitter.py -sd /hdfs/store/user/skkwan/hToA1A2/test -es=0 -ues=0 -res=0 -jes=0  -year=2018  --channel='em' --jobName test_svfit_nominal_sync_emu
