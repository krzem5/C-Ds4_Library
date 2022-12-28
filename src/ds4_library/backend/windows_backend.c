#include <ds4_library.h>
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <hidclass.h>
#include <hidsdi.h>
#include <string.h>



#define SONY_VENDOR_ID 0x054c
#define DS4_PRODUT_ID1 0x054c
#define DS4_PRODUT_ID2 0x09cc



void _ds4_deinit(const ds4_device_t* device){
	CancelIo(device->_handle);
}



void* _ds4_init(const ds4_raw_device_t* raw){
	HANDLE h=CreateFileA(*raw,(GENERIC_WRITE|GENERIC_READ),FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,0);
	return (h==INVALID_HANDLE_VALUE?NULL:(void*)h);
}



_Bool _ds4_recv_data(ds4_device_t* device){
	DWORD sz;
	return (ReadFile((HANDLE)(device->_handle),device->_in_buffer,64,&sz,NULL)&&sz==64);
}



void _ds4_send_data(const ds4_device_t* device){
	DWORD sz;
	WriteFile((HANDLE)(device->_handle),device->_out_buffer,32,&sz,NULL);
}



void ds4_deinit(void){
}



ds4_raw_device_t* ds4_enumerate_usb(ds4_raw_device_count_t* count){
	ds4_raw_device_t* out=NULL;
	ds4_raw_device_count_t length=0;
	GUID i_guid=GUID_DEVINTERFACE_HID;
	HDEVINFO dev_list=SetupDiGetClassDevsA(&i_guid,NULL,NULL,DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);
	uint32_t dev_list_idx=0;
	while (1){
		SP_DEVICE_INTERFACE_DATA dev={
			sizeof(SP_DEVICE_INTERFACE_DATA)
		};
		if (!SetupDiEnumDeviceInterfaces(dev_list,NULL,&i_guid,dev_list_idx,&dev)){
			break;
		}
		dev_list_idx++;
		DWORD sz;
		if (!SetupDiGetDeviceInterfaceDetailA(dev_list,&dev,NULL,0,&sz,NULL)&&GetLastError()!=ERROR_INSUFFICIENT_BUFFER){
			continue;
		}
		SP_DEVICE_INTERFACE_DETAIL_DATA_A* dev_i=malloc(sz);
		dev_i->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
		if (!SetupDiGetDeviceInterfaceDetailA(dev_list,&dev,dev_i,sz,NULL,NULL)){
			goto _cleanup_loop;
		}
		DWORD j=0;
		while (1){
			SP_DEVINFO_DATA dev_info={
				sizeof(SP_DEVINFO_DATA)
			};
			if (!SetupDiEnumDeviceInfo(dev_list,j,&dev_info)){
				goto _cleanup_loop;
			}
			j++;
			char dev_name[MAX_PATH];
			if (!SetupDiGetDeviceRegistryPropertyA(dev_list,&dev_info,SPDRP_CLASS,NULL,(PBYTE)dev_name,MAX_PATH,NULL)||strcmp(dev_name,"HIDClass")){
				continue;
			}
			if (SetupDiGetDeviceRegistryPropertyA(dev_list,&dev_info,SPDRP_DRIVER,NULL,(PBYTE)dev_name,MAX_PATH,NULL)){
				break;
			}
		}
		HANDLE fh=CreateFileA(dev_i->DevicePath,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);
		if (fh==INVALID_HANDLE_VALUE){
			goto _cleanup_loop;
		}
		HIDD_ATTRIBUTES attr={
			sizeof(HIDD_ATTRIBUTES)
		};
		if (!HidD_GetAttributes(fh,&attr)||attr.VendorID!=SONY_VENDOR_ID||(attr.ProductID!=DS4_PRODUT_ID1&&attr.ProductID!=DS4_PRODUT_ID2)){
			goto _cleanup_handle;
		}
		length++;
		out=realloc(out,length*sizeof(ds4_raw_device_t));
		size_t len=strlen(dev_i->DevicePath)+1;
		*(out+length-1)=malloc(len);
		memcpy(*(out+length-1),dev_i->DevicePath,len);
_cleanup_handle:
		CloseHandle(fh);
_cleanup_loop:
		free(dev_i);
	}
	SetupDiDestroyDeviceInfoList(dev_list);
	*count=length;
	return out;
}



void ds4_init(void){
}
