#!/usr/bin/python3

import os

import sys
import getopt
import glob
import struct

# ==
# ||
# || Main Program
# ||
# ==

def sb(argv):

    os.system('clear')
    
    print()
    print( '------------------------------------')
    print( '   sb.py for nPEx = 2 nPEy = 2      ')
    print( '------------------------------------')
    print()
    # -
    # |
    # | User arguments
    # |
    # -

    readFile = 'NULL'
    nPtsx    = 0
    
    try:
        opts, args = getopt.getopt(argv,"h f: x: y:")
      
    except:
       print( 'Error in command-line arguments.')
       exit(0)

    for opt, arg in opts:
        
        if opt == "-h":
            print( './sb.py -f <binary file to be read>')
            print( 'For in-class demo, try: ./sb_py3.py -f output.bin -x 10 -y 10' )
            exit(0)
        
        if opt == "-x":
            nPtsx = int(arg)
        
        if opt == "-y":
            nPtsy = int(arg)
        
        elif opt == "-f":
            readFile = arg


    numPts = nPtsx * nPtsy
    
    print()
    print('Read user specified file: ' )
    print('------------------------------------')
    print()
    print('(o) File name = ' + readFile)
    print('(o) Output here is hard-coded for nPEx = 2 and nPEy = 2')
    print('(o) Number of points to be read = ',numPts)
    print()
    print()

    file = open(readFile, "rb")



    count = 0
    rowcount = 0
    for i in range(0,numPts):
        Num  = file.read(4)
        floatValue = struct.unpack('f',Num)[0]
        print ("{:5.1f}".format(floatValue),end='')

        count += 1

        if count == nPtsx/2:
            print (' | ' ,end='')

        if count == nPtsx:
            print ()
            count = 0
            rowcount += 1
            if rowcount == nPtsy/2:
                print (nPtsx*'------')

            

    print ()
    file.close()


    
    
    print
    print

    

if __name__ == "__main__":
    sb(sys.argv[1:])



