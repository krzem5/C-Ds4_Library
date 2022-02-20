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



void _ds4_deinit(ds4_device_t* d){
	CancelIo(d->_fh);
}



_Bool _ds4_init(ds4_raw_device_t* p,ds4_device_t* o){
	o->_fh=CreateFileA(p->name,(GENERIC_WRITE|GENERIC_READ),FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);
	if (o->_fh==INVALID_HANDLE_VALUE){
		return 0;
	}
	return 1;
}



_Bool _ds4_recv_data(ds4_device_t* d){
	OVERLAPPED oe;
	memset(&oe,0,sizeof(OVERLAPPED));
	if (!ReadFile(d->_fh,d->_in_bf,64,NULL,&oe)&&GetLastError()!=ERROR_IO_PENDING){
		ASSERT(!"Device Error!");
		return 0;
	}
	DWORD sz=0;
	if (!GetOverlappedResult(d->_fh,&oe,&sz,TRUE)){
		return 0;
	}
	return (sz==64);
}



void _ds4_send_data(ds4_device_t* d){
	OVERLAPPED oe;
	memset(&oe,0,sizeof(OVERLAPPED));
	if (!WriteFile(d->_fh,d->_out_bf,32,NULL,&oe)&&GetLastError()!=ERROR_IO_PENDING){
		ASSERT(!"Device Error!");
		return;
	}
	DWORD sz=0;
	if (!GetOverlappedResult(d->_fh,&oe,&sz,TRUE)){
		d->_last_cfg=DS4_CONFIG_FORCE_UPDATE;
	}
}



void ds4_deinit(void){
}



ds4_raw_device_t* ds4_enumerate_usb(ds4_raw_device_count_t* l){
	ds4_raw_device_t* o=NULL;
	ds4_raw_device_count_t ol=0;
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
		size_t len=strlen(dev_i->DevicePath)+1;
		char* name=malloc(len);
		memcpy(name,dev_i->DevicePath,len);
		ol++;
		o=realloc(o,ol*sizeof(ds4_raw_device_t));
		(o+ol-1)->name=name;
_cleanup_handle:
		CloseHandle(fh);
_cleanup_loop:
		free(dev_i);
	}
	SetupDiDestroyDeviceInfoList(dev_list);
	*l=ol;
	return o;
}



void ds4_init(void){
}
