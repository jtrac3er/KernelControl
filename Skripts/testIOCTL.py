# -*- coding: utf-8 -*-
#!python3.6

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

TRIGGER_TRIPLE_FAULT = 0x800 
EXECUTE_USERCODE_ADDRESS = 0x801			
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
	print("Konnte Handle f�r '%s' nicht �ffnen" % DRIVERNAME.decode("ascii"))
	print("Errorcode: %i" % GetLastError())
	input()
	exit(1)
else:
	print("Handle f�r den Treiber ge�ffnet: %i" % hDevice)



def send2Driver(ControlCode, data, lData, output, lOutput):
	returnedBytes = WORD()
	status = DeviceIoControl (
		hDevice,
		ControlCode,		
		data,	
		lData,		# Gr�sse des Buffers
		output,			
		lOutput,			
		byref(returnedBytes),
		None
	)
	return (status, returnedBytes.value)




""" Hier kommen alle unterst�tzen Funktionen rein """

IOCTL_TriggerTripleFault = 0
IOCTL_ExecuteUsercodeAddress = 0
IOCTL_ThreadSleep = 0
IOCTL_ClearSMP = 0


def TriggerTripleFault():
    pass

def ExecuteUsercodeAddress():
    pass

def ThreadSleep():
    pass

def ClearSMP():
    pass
