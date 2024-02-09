import os

originalDir="/hdfs/store/user/skkwan/fastmtt_2024-02-06-10h24m"
rescueDir="/hdfs/store/user/skkwan/fastmtt_2024-02-07-13h45m_rescue"

for subdir, dirs, files in os.walk(rescueDir):
 
    directoryName = os.path.basename(subdir)
    targetDir=os.path.join(originalDir, directoryName)
    command="cp {}/*.root {}".format(subdir, targetDir)
    print(command)
    os.system(command)

