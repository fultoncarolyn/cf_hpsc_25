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


# Prints a user help message:

def help():

    print("")
    print("")
    print("This script keeps a command running, restarting it after")
    print("it has been killed by a system monitor.")
    print("")
    print("")

# Packs a string with blanks to be a specified width:    

def pack(string):
    ans = string
    for i in range(len(string),88): ans += ' '
    return ans

# Prints a nice banner with a message inside box:

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
    bannerDisplay('*** Fatal Error *** in keepRunning.py',msg)
    sys.exit()
    


# =========================================================================================================
# =========================================================================================================
#
#                                   M A I N   C O D E 
#
# =========================================================================================================
# =========================================================================================================

        
# ==
# ||
# || runDone:  Returns True if it finds the string indicating successfull completion in the ttyFile
# ||
# ==


def runDone(ttyFile,completionIndicator):

    try:
        f = open(ttyFile,'r')
    except:
        print("tty file ("+ttyFile+")not found.  Assuming the job is not still running...")
        return False
    
    for line in f:
        if completionIndicator in line:
            f.close()
            return True

    f.close()
    return False


        
# ==
# ||
# || Main Program
# ||
# ==

def keepRunning(argv):

    # -
    # |
    # | Command-line arguments
    # |
    # -

    yamlFile = ""
    
    try:
        opts, args = getopt.getopt(argv,"h f:")
      
    except:
        fatalError('Error in command-line arguments.  Try -h to see help.')

    for opt, arg in opts:
        
        if opt == '-h':
            help()
            sys.exit()
            
        elif opt == "-f":
            yamlFile  = arg

    if yamlFile == '' : FatalError("You must provide a yaml yaml file")
    
    # -
    # |
    # | Read input
    # |
    # -
    
    stream = open(yamlFile, 'r')
    yamlDic = yaml.load(stream,Loader=yaml.FullLoader)

    mpirun         = yamlDic['EXECUTABLE']['mpirun'        ]
    exe            = yamlDic['EXECUTABLE']['pathToExe'     ]
    initialRunArgs = yamlDic['ARGUMENTS' ]['initial'       ]
    restartRunArgs = yamlDic['ARGUMENTS' ]['restart'       ]
    ttyOutput      = yamlDic['COMPLETION']['ttyOutput'     ]
    completionStr  = yamlDic['COMPLETION']['completionStr' ]

    # -
    # |
    # | Construct the run and restart commands
    # |
    # -
    
    runCommand     = mpirun + ' ' + exe + ' ' + initialRunArgs
    restartCommand = mpirun + ' ' + exe + ' ' + restartRunArgs

    print('Running ' + runCommand)
    os.system(runCommand + ' & ')

    time.sleep(1)

    # -
    # |
    # | Infinite loop which constantly checks to see if the exe should be restarted
    # |
    # -
    
    count = 0
    while(True):

        os.system('clear')

        count += 1

        print
        print('------------------------------------------------------------------')
        print
        print('Iteration          : ' + str(count))
        print('Checking on        : ' + exe )
        print

        userName = 'scott'
        jobName = exe
    
        psCommand = "ps -elf | grep " + userName + " | grep " + jobName + " | grep -v 'grep' "
        jobStatus = os.popen(psCommand).read()
        
        print ('Searching for this job: ' + jobName )
        print ('With this command     : ' + psCommand)
        print ('Under user name       : ' + userName)
        print ('Found this record  : ' + jobStatus)
            
        
        if len(jobStatus) > 0:
            print ('It is still running :-)')
        else:
            print ('It is no longer running.   Checking its tty output to see if it finished.')
            if runDone(ttyOutput,completionStr):
                print ('It did finish.  This script (keepRunning.py) is now exiting.')
                exit(0)
            else:
                print ('It is no longer running.   Restarting it now...')
                os.system(restartCommand + ' & ')

        time.sleep(1)

    


if __name__ == "__main__":
    keepRunning(sys.argv[1:])
