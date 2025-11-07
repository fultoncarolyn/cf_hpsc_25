#!/usr/bin/python3

import os

import sys
import getopt
import glob



def WriteOnePCfile(PElist,timeList,varName):

    g = open('pc_' + varName,'w')
    
    for time in sorted(timeList):
        
        print ( 'splot \\' , file = g)
        for PE in sorted(PElist):
            fileName = varName+'_' + str(PE) + '_' + str(time) + '.plt'
            if os.path.isfile(fileName):
               print ( '"'+fileName+'" w l lw 3 , \\' , file = g)
        print ( '' , file = g)
        print ( 'pause .1' , file = g)
        
    g.close()


# ==
# ||
# || Main Program
# ||
# ==

def writePlotCmd(argv):



    # List of all ptcl files
    
    ptclList = glob.glob("phi*.plt")

    # Parse the filenames counting the number of PEs for this run

    PElist = []
    timeList = []
    
    for i in range(0,len(ptclList)):
        name = ptclList[i];
        tmp = name.split('_')

        PE  = tmp[1]
        if PE not in PElist:
            PElist.append(PE)

        tmp2 = tmp[2].split(".")
        time = int(tmp2[0])
        if time not in timeList:
            timeList.append(int(time))

            
    WriteOnePCfile(PElist,timeList,'phi')
#   WriteOnePCfile(PElist,timeList,'e1')
#   WriteOnePCfile(PElist,timeList,'e2')
#   WriteOnePCfile(PElist,timeList,'m1')
#   WriteOnePCfile(PElist,timeList,'m2')
#   WriteOnePCfile(PElist,timeList,'Mf1')
#   WriteOnePCfile(PElist,timeList,'Mf2')


if __name__ == "__main__":
    writePlotCmd(sys.argv[1:])
