#!python3.6
# -*- coding: utf-8 -*-
# Version sollte vor dem Encoding kommen

from ctypes import *
from ctypes.wintypes import *


GENERIC_READ = 0x80000000
GENERIC_WRITE = 0x40000000
FILE_ATTRIBUTE_NORMAL = 0x80
DEVICE_UNKNOWN = 0x22
FILE_ANY_ACCESS = 0
METHOD_NEITHER = 0x3
PAGE_EXECUTE_READWRITE = 0x40
INVALID_HANDLE_VALUE = HANDLE(-1).value


DeviceType = DEVICE_UNKNOWN
Access = FILE_ANY_ACCESS
Method = METHOD_NEITHER
			
DRIVERNAME = b"\\\\.\\KernelControl"

VirtualProtect = windll.kernel32.VirtualProtect
VirtualProtect.argtypes = [c_void_p, DWORD, DWORD, c_void_p]
VirtualProtect.restype = BOOL

DeviceIoControl = windll.kernel32.DeviceIoControl
DeviceIoControl.argtypes = [HANDLE, DWORD, c_char_p, DWORD, c_char_p, DWORD, POINTER(WORD), c_void_p]
DeviceIoControl.restype = BOOL

CreateFileA = windll.kernel32.CreateFileA
CreateFileA.argtypes = [c_char_p, DWORD, DWORD, c_void_p, DWORD, DWORD, HANDLE]
CreateFileA.restype = HANDLE

print("\nVersuche ein Handle zu erlangen...")

hDevice = CreateFileA (
	DRIVERNAME,
	GENERIC_WRITE | GENERIC_READ,
	0,
	None,
	3,
	FILE_ATTRIBUTE_NORMAL,
	None
)

if hDevice == INVALID_HANDLE_VALUE:
	print("Konnte Handle für '%s' nicht öffnen" % DRIVERNAME.decode("ascii"))
	print("Errorcode: %i" % GetLastError())
	input()
	#exit(1)
else:
	print("Handle für den Treiber geöffnet: %i" % hDevice)



def send2Driver(ControlCode, data, lData, output, lOutput):
	returnedBytes = WORD()
	status = DeviceIoControl (
		hDevice,
		ControlCode,		
		data,	
		lData,		# Grösse des Buffers
		output,			
		lOutput,			
		byref(returnedBytes),
		None
	)
	return (status, returnedBytes.value)




""" Hier kommen alle unterstützen Funktionen rein """

IOCTL_TriggerTripleFault = 0
IOCTL_ExecuteUsercodeAddress = 0
IOCTL_ThreadSleep = 0
IOCTL_ClearSMP = 0


def TriggerTripleFault():
    send2Driver(IOCTL_TriggerTripleFault, None, 0, None, 0)


def ClearSMP():
    send2Driver(IOCTL_ClearSMP, None, 0, None, 0)


def ExecuteUsercodeAddress():

    payload = b"\xC3"    # Einfacher Return
    lpOldProt = cast(pointer((BYTE*100)()), c_void_p)

    # Alloziiere Speicher und mache ihn ausführbar.
    memory = c_char_p(payload)
    status = VirtualProtect(memory, sizeof(memory), PAGE_EXECUTE_READWRITE, lpOldProt)
    if not status:
        print("Konnte Code nicht erstellen. Errorcode ist %i" % GetLastError())
        exit(1)

    # Sende das Ganze an den Treiber
    result = send2Driver(IOCTL_ExecuteUsercodeAddress, memory, len(payload), None, 0)
    if not result[0]:
        print("Konnte Payload nicht an Treiber senden. Errorcode: %i" % GetLastError())
        exit(1)


def ThreadSleep(tu):
        
    time = DWORD(tu)
    result = send2Driver(IOCTL_ThreadSleep, time, sizeof(DWORD), None, 0)
    if not result[0]:
        print("Konnte IOCTL nicht an Treiber senden: %i" % GetLastError())
        exit(1)



from sys import argv

try:
    arg = argv[1]
    if arg == "-t": TriggerTripleFault()
    elif arg == "-e": ExecuteUsercodeAddress()
    elif arg == "-t": ThreadSleep(100)
    elif arg == "-s": ClearSMP()
    else: print("Invalide Option")
except Exception as e:
    print("Fehler aufgetreten")
    print(e)
