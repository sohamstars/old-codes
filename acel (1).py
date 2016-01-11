
import smbus
import sys
import getopt
import time
import os
import subprocess
bus=smbus.SMBus(1)
c=[ 0., 0., 0. ]
f=open("dat1.txt","w")


def init():
    
    bus.write_byte_data(0x53,0x2D,0x00);  
    bus.write_byte_data(0x53,0x2D,0x16);  
    bus.write_byte_data(0x53,0x2D,0x08);
    bus.write_byte_data(0x53,0x31,0x00);
   

def read():
    
    e=[ 0, 0, 0 ]
    d=[ 0, 0, 0, 0, 0, 0 ]
    d[0]=(bus.read_byte_data(0x53,0x32))
    d[1]=(bus.read_byte_data(0x53,0x33))
    e[0]=((d[1]<<8) | d[0])
    if(e[0]>511):
        e[0]=-1*(65535-e[0])
    c[0]=(e[0])*3.5/1000*9.8
    

    d[2]=(bus.read_byte_data(0x53,0x34))
    d[3]=(bus.read_byte_data(0x53,0x35))
    e[1]=((d[3]<<8) | d[2])
    if(e[1]>511):
        e[1]=-1*(65535-e[1])
    c[1]=(e[1])*3.9/1000*9.8

    d[4]=(bus.read_byte_data(0x53,0x36))
    d[5]=(bus.read_byte_data(0x53,0x37))
    e[2]=((d[5]<<8) | d[4])
    if(e[2]>511):
        e[2]=-1*(65535-e[2])
    c[2]=(e[2])*4.3/1000*9.8
    
        
   

def main():

    init()

    #BluetoothAddr="10:68:3F:5D:2C:95"      #Nexus4
    BluetoothAddr="90:C1:15:3D:C7:12"       #address of the mobile
    getChannelCmd="sdptool browse "+BluetoothAddr+" | sed -n '/OBEX Object Push/,/Version/p' | grep 'Channel' | awk '{ printf( \"%s\", $2) }'"
                                        #create channel between bluetooth and android app
    

    time.sleep(1)                       #timesleep(1) added for waiting to receive reading from accelerometer
    i=0
    while (i<10):
        
        read()
        st=repr(c)                      # Readings received converted into string
        f.write(st+"\n")                # 10 readings read and saved in file
        time.sleep(1)
        i=i+1
    f.close()                           #file dat1.txt closed

    time.sleep(15)                      #waiting for key.txt

    from subprocess import call
    call(["gcc","rc4.c","-o","rc4"])        #rc4 exec file created
    from subprocess import call
    call(["./rc4","/home/pi/key.txt","/home/pi/dat1.txt","/home/pi/dat2.txt"])


    #dat2.txt in bus
    out=os.popen(getChannelCmd).read()      #permission from os
    
    sendFileCmd="obexftp --nopath --noconn --uuid none --bluetooth "+BluetoothAddr+" --channel "+out+" --put /home/pi/dat2.txt"
    
    os.system(sendFileCmd)      


    from subprocess import call
    call(["rm","/home/pi/key.txt"])
   
    from subprocess import call
    call(["rm","/home/pi/dat2.txt"])  

    print"congratulations"


if __name__ == "__main__":
    main()

    
