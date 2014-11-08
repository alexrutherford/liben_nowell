import subprocess
import sys,os

indices=[i for i in range(1,50)]

fileName=''

for i in indices:

    fileName='OUT_'+str(i)+'.out'

    print fileName
    
    subprocess.call(['./put_file_in_db.exe',fileName])
	
    print ['./put_file_in_db.exe',fileName]
