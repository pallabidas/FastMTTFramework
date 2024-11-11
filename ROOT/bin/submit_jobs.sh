count="1"

[ -d output ] || mkdir output
[ -d error ] || mkdir error
[ -d log ] || mkdir log

dataset=$1
file=input_${dataset}.list

[ -d /eos/user/p/pdas/HAA_svfit/${1} ] || mkdir /eos/user/p/pdas/HAA_svfit/${1}
while read -r line; do

echo count $count

input_filename=$(echo $line | awk '{print $1}')

cat>Job_${1}_${count}.sh<<EOF
#!/bin/bash
cd /afs/cern.ch/work/p/pdas/haa/svfit/CMSSW_13_3_0/src/FastMTTFramework/ROOT/bin/
cmsenv

$CMSSW_BASE/bin/$SCRAM_ARCH/SVFit inputfile=${input_filename} newOutputFile=1.0 newFile=/eos/user/p/pdas/HAA_svfit/${1}/out_${count}.root

EOF

chmod 755 Job_${1}_${count}.sh

sed 's/template/'${1}_${count}'/g' < template_condor_script > condor_${1}_${count}

condor_submit condor_${1}_${count}

count=$[$count+1]

done < $file

exit 0
