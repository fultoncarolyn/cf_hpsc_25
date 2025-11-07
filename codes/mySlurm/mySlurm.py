#!/usr/bin/python3
import os
import sys
import getopt
import glob
import yaml
import re
from datetime import datetime
from io import StringIO
import re
import time

# =========================================================================================================
# =========================================================================================================
#
#                                     U T I L I T I E S
#
# =========================================================================================================
# =========================================================================================================


def help():

    print("")
    print("")
    print ("This script is to be run in the background.  It mimics ")
    print ("a batch run system with queues.  Its purpose is to enable")
    print ("the development of automatic restart capabilities without")
    print ("having to work on a system with those capabilities.")
    print("")
    print("")

def pack(string):
    ans = string
    for i in range(len(string),88): ans += ' '
    return ans

def bannerDisplay(header,message):
    tmp = message.split('\n')
    print 
    print 
    print ("==============================================================================================")
    print ("||                                                                                          ||")
    print ("||                                                                                          ||")
    print ("|| " + pack(header) + ' ||')
    print ("||                                                                                          ||")
    for t in tmp:
        print ('|| '+ pack(t) + ' ||' )
    print ("||                                                                                          ||")
    print ("==============================================================================================")
    print
    
def FatalError(msg):
    bannerDisplay('*** Fatal Error *** in mySlurm.py',msg)
    sys.exit()
    

# If string, s = hello(" hi " , " there " )
# this routine will return hi and there.

def findSubstrings(s):
    substring = '"'
    matches = re.finditer(substring,s)

    # List containing the indices of the double quote sign
    quotes = [match.start() for match in matches]

    ans1 =  s[quotes[0]+1:quotes[1]]
    ans2 =  s[quotes[2]+1:quotes[3]]

    return ans1, ans2


def replacePhrase(input_str,delimiter1,delimiter2,target_str):
    idx1 = input_str.find(delimiter1)
    idx2 = input_str.find(delimiter2)

    tmp = input_str[idx1+1:idx2]
    result = input_str.replace(tmp,target_str)
    return result

def timeInSeconds(time_str):

    d2s = 24*3600
    h2s = 3600
    m2s = 60

    tmp = time_str.split(':')

    if len(tmp) == 1:     return int(time_str)
    if len(tmp) == 2:     return int(tmp[0]) * m2s  + int(tmp[1]) 
    if len(tmp) == 3:     return int(tmp[1]) * h2s  + int(tmp[1]) * m2s + int(tmp[0])
    if len(tmp) == 4:     return int(tmp[2]) * d2s  + int(tmp[1]) * h2s + int(tmp[1]) * m2s + int(tmp[0])


# =========================================================================================================
# =========================================================================================================
#
#                                   M A I N   C O D E 
#
# =========================================================================================================
# =========================================================================================================


        
# ==
# ||
# || Main Program
# ||
# ==

def mySlurm(argv):

    # -
    # |
    # | Command-line arguments
    # |
    # -

    inputFile = ""
    maxTime   = -999.
    
    try:
        opts, args = getopt.getopt(argv,"h f: t:",["dir="])
      
    except:
        fatalError('Error in command-line arguments.  Try -h to see help.')

    for opt, arg in opts:
        
        if opt == '-h':
            help()
            sys.exit()
            
        elif opt == "-f":
            inputFile  = arg
            
        elif opt == "-t":
            maxTime = float(arg)
            
        elif opt == "--dir":
            srcDir   = arg

    if maxTime < 0.: FatalError("You must provide a max time in seconds.")

    
    # -
    # |
    # | Get ps -elf output
    # |
    # -

    count = 0

    while (True):
        os.system('clear')

        count += 1

        userName = 'scott'
#        jobName = 'transientDiffusion'
        jobName = 'xclock'
    
        print
        print ('Iteration             : ' + str(count))
        print ('Searching for this job: ' + jobName )
        print ('Under user name       : ' + userName)

        psCommand = "ps -elf | grep " + userName + " | grep " + jobName + " | grep -v 'grep' "
        jobStatus = os.popen(psCommand).read()
        
        if len(jobStatus) <= 0:
            print ('Job not found, nothing to do.')
            
        if len(jobStatus) > 0:
            jobStatus = jobStatus.replace('\n','')

            statusBreakdown = re.split(' +', jobStatus)
            jobID           = statusBreakdown[3]

            psCommand = "ps -p " + jobID + " -o etime | grep -v ELAPSED"
            psElapsed = os.popen(psCommand).read()
            psElapsed = psElapsed.replace('\n','')
            psSeconds = timeInSeconds(psElapsed)

            print ('Found this record     : ' + jobStatus)
            print ('Seconds running (ps)  : ' + str(psSeconds))
            print ('Max time allowed      : ' + str(maxTime))
            print

            if int(psSeconds) > int(maxTime):
                killCommand = 'kill -9 ' + str(jobID)
                print ('Max Time Exceeded:  Killing the job with: ' + killCommand)
                os.system(killCommand)

        time.sleep(2)


if __name__ == "__main__":
    mySlurm(sys.argv[1:])
