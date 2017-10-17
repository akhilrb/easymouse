import serial
import io
import subprocess
import pyautogui
from time import sleep
from enum import IntEnum

###########################################################################################################################################
# Priority definitions for output
class Priority(IntEnum):
    SILENT = 5
    ERROR = 4
    SETTING = 3
    EVENT = 2
    GUI = 1
    ALL = 0

# Default variables
BAUD_LIST = [9600, 19200, 38400, 57600, 115200, 230400]
BAUDRATE = 9600
PORT = ""
PORT_LIST = []
serialReceiver = serial.Serial()
interv = 0.10
VerbosePriority = Priority.EVENT

###########################################################################################################################################
# Prints messages according to priority
def consolePrint(msg, priority):
    #max error length is 64 characters
    #msg = '|' + msg + ' '*(66 - len(msg)) + '|'
    if priority >= VerbosePriority:
        if priority == Priority.GUI:
            print(msg)
        else:    
            #print("\n -"+str('-'*64)+"-\n" + msg + "\n -"+str('-'*64)+"-\n")
            print(str('-'*len(msg)) + '\n' + msg + '\n' + str('-'*len(msg)))

    

# returns output of a terminal command
def cmdLine(cmd):
    process = subprocess.Popen(args = cmd, stdout = subprocess.PIPE, universal_newlines = True, shell = True)
    return process.communicate()[0]

###########################################################################################################################################
'''
----------------------------------
SERIAL RECEIVER SPECIFIC FUNCTIONS
----------------------------------
'''

# scan ports
def quickScan():
    global PORT_LIST, PORT
    for x in range(len(PORT_LIST)):
        PORT_LIST.pop(0)
    portList = str(cmdLine("python -m serial.tools.list_ports")).split("\n")
    for port in portList:
        # find and eliminate trailing whitespaces
        wsi = str(port).find(' ')
        PORT_LIST.append(str(port)[0:wsi])
    
#return choice
def choosePort():
    global PORT_LIST, PORT
    print("\nScanning Available Ports...\n---------------------")
    sleep(0.5)
    for x in range(len(PORT_LIST)):
        PORT_LIST.pop(0)
    portList = str(cmdLine("python -m serial.tools.list_ports")).split("\n")
    for port in portList:
        # find and eliminate trailing whitespaces
        wsi = str(port).find(' ')
        PORT_LIST.append(str(port)[0:wsi])
    #subtract 1 to compensate newline
    for x in range(len(PORT_LIST)-1):
        print (str(x+1)+") "+str(PORT_LIST[x]))
    print (str(len(PORT_LIST))+") Rescan")
    i = int(input("---------------------\nSelect Option #"))
    #print(PORT_LIST)
    if(i < len(PORT_LIST)):
        return PORT_LIST[i-1]
    else:
        choosePort()

# configure port specifics
def configurePort():
    global PORT, BAUDRATE, BAUD_LIST, serialReceiver
    PORT = choosePort()
    print("\nAvailable Baud Rates:\n---------------------")
    for x in range(len(BAUD_LIST)):
        print (str(x+1)+") "+str(BAUD_LIST[x]))
    i = int(input("---------------------\nSelect Option #"))
    if(i < len(BAUD_LIST)):
        BAUDRATE = BAUD_LIST[i-1]
    else:
        BAUDRATE = 115200

# start Receiver
def startReceiver():
    global PORT, BAUDRATE, serialReceiver
    configurePort()
    try:
        print("\nTrying "+ str(PORT)+" @ " + str(BAUDRATE)+" bps...\t"+ "Max Comm Speed: " + str("%.2f" %(float(BAUDRATE)/8096)) + " kBps")
        sleep(1)
        serialReceiver =  serial.Serial(PORT, BAUDRATE)
        sleep(0.5)
        consolePrint("Driver Running", Priority.EVENT)
    except serial.serialutil.SerialException as e:
        consolePrint("No device was detected on " + str(PORT), Priority.ERROR)
        startReceiver()

# restart on fail
def restartReceiver():
    global PORT, BAUDRATE, serialReceiver
    try:
        consolePrint("Attempting to restart driver automatically", Priority.EVENT)
        print("Trying "+ str(PORT)+" @ " + str(BAUDRATE)+" bps...\t"+ "Max Comm Speed: " + str("%.2f" %(float(BAUDRATE)/8096)) + " kBps")
        sleep(1)
        serialReceiver =  serial.Serial(PORT, BAUDRATE)
        consolePrint("Driver Running", Priority.EVENT)
        manageDevice(' ')
    except serial.serialutil.SerialException as e:
        consolePrint("Port collapsed. Device disconnected unexpectedly from " + str(PORT), Priority.ERROR)
        serialReceiver.close()
        startReceiver()

'''
----------------------------------
SERIAL RECEIVER SPECIFIC FUNCTIONS
----------------------------------
'''
###########################################################################################################################################
'''
-----------------
INPUT AND CONTROL
-----------------
'''

# to check different working environment variables
def setup():
    print ("Setting up variables")
    width, height = pyautogui.size()

# main manager
def manageDevice(ch):
    global interv
    nclicks = 4
    if ch == 'C':
        try:
            msg = "Clicking " + str(nclicks) + " times with interval " + str("%.2f" %interv) + " sec"
            consolePrint(msg, Priority.GUI)
            pyautogui.click(clicks = nclicks, interval = interv)
        except:
            pass
    if ch == 'c':
        try:
            x, y = pyautogui.position()
            msg = 'X: ' + str(x).rjust(4) + ' Y: ' + str(y).rjust(4)
            consolePrint(msg, Priority.GUI)
        except:
            pass
    if ch == 'I':
        interv = min(interv + 0.02, 0.50)
        msg = "Click Interval: %.2f seconds" %interv
        consolePrint(msg, Priority.SETTING)
    if ch == 'D':
        interv = max(interv - 0.02, 0.04)
        msg = "Click Interval: %.2f seconds" %interv
        consolePrint(msg, Priority.SETTING)

'''
-----------------
INPUT AND CONTROL
-----------------
'''
###########################################################################################################################################
'''
BOOT

if serialReceiver.isOpen():
    consolePrint("Serial Open", Priority.EVENT)
else:
    consolePrint("Serial Closed", Priority.EVENT)

'''

startReceiver()
#consolePrint("Press any key for more than a second to configure driver", Priority.GUI)
sleep(0.5)

if serialReceiver.isOpen():
    while 1:
        try:
            x = (serialReceiver.read()).decode("utf-8")
            manageDevice(x)
        except UnicodeDecodeError as ude:
            # maybe we entered here because of wrong message relay and not a baud mismatch
            try:
                consolePrint("Corrupt Bytes received",Priority.ERROR)
                serialReceiver =  serial.Serial(PORT, BAUDRATE)
                x = (serialReceiver.read()).decode("utf-8")
            except UnicodeDecodeError as ude:
                # this is a baud mismatch
                # ude won't be fixed until port is closed and then opened again
                consolePrint("Fatal UDError encountered", Priority.ERROR)
                serialReceiver.close()
                startReceiver()
        except Exception as e:
            # something might have happened
            consolePrint("Serial Disturbed", Priority.ERROR)
            sleep(0.25)
            # try a quick fix of switching ports
            try:
                quickScan()
                PORT = PORT_LIST[0]
                serialReceiver =  serial.Serial(PORT, BAUDRATE)
                x = (serialReceiver.read()).decode("utf-8")
            except Exception as e:
                consolePrint("Port Transferred to " + PORT, Priority.EVENT)
                restartReceiver();