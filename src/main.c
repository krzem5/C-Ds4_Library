#include <ds4_library.h>
#include <stdlib.h>
#include <stdio.h>



static uint32_t _hue_color(void){
	static uint16_t hue=0;
	uint8_t t=(hue>>8)/43;
	uint8_t v=((hue>>8)-(t*43))*6;
	hue+=75;
	if (t==0){
		return DS4_MAKE_COLOR(255,v,0);
	}
	if (t==1){
		return DS4_MAKE_COLOR(255-v,255,0);
	}
	if (t==2){
		return DS4_MAKE_COLOR(0,255,v);
	}
	if (t==3){
		return DS4_MAKE_COLOR(0,255-v,255);
	}
	if (t==4){
		return DS4_MAKE_COLOR(v,0,255);
	}
	return DS4_MAKE_COLOR(255,0,255-v);
}



static void _print_ds4(ds4_device_t* d){
	printf("Buttons:");
	if (d->buttons&DS4_BUTTON_UP){
		printf(" up");
	}
	if (d->buttons&DS4_BUTTON_DOWN){
		printf(" down");
	}
	if (d->buttons&DS4_BUTTON_LEFT){
		printf(" left");
	}
	if (d->buttons&DS4_BUTTON_RIGHT){
		printf(" right");
	}
	if (d->buttons&DS4_BUTTON_L1){
		printf(" L1");
	}
	if (d->buttons&DS4_BUTTON_R1){
		printf(" R1");
	}
	if (d->buttons&DS4_BUTTON_L2){
		printf(" L2");
	}
	if (d->buttons&DS4_BUTTON_R2){
		printf(" R2");
	}
	if (d->buttons&DS4_BUTTON_L3){
		printf(" L3");
	}
	if (d->buttons&DS4_BUTTON_R3){
		printf(" R3");
	}
	if (d->buttons&DS4_BUTTON_CROSS){
		printf(" cross");
	}
	if (d->buttons&DS4_BUTTON_CIRCLE){
		printf(" circle");
	}
	if (d->buttons&DS4_BUTTON_SQURARE){
		printf(" squrare");
	}
	if (d->buttons&DS4_BUTTON_TRIANGLE){
		printf(" triangle");
	}
	if (d->buttons&DS4_BUTTON_OPTIONS){
		printf(" options");
	}
	if (d->buttons&DS4_BUTTON_SHARE){
		printf(" share");
	}
	if (d->buttons&DS4_BUTTON_PS){
		printf(" PS");
	}
	if (d->buttons&DS4_BUTTON_TOUCHPAD){
		printf(" touchpad");
	}
	printf("\nSticks: left=<%d %d> right=<%d %d>\nTriggers: L2=%u R2=%u\nBattery: %u\nAcceleration: %.2f %.2f %.2f\nAngular Velocity: %.2f %.2f %.2f\nTouch#0:",d->lx,d->ly,d->rx,d->ry,d->l2,d->r2,d->battery,d->acc_x,d->acc_y,d->acc_z,d->avel_x,d->avel_y,d->avel_z);
	if (d->touch[0].id==DS4_NO_DATA){
		printf("\nTouch#1:");
	}
	else{
		printf(" id=%u pos=<%u %u>\nTouch#1:",d->touch[0].id,d->touch[0].x,d->touch[0].y);
	}
	if (d->touch[1].id==DS4_NO_DATA){
		putchar('\n');
	}
	else{
		printf(" id=%u pos=<%u %u>\n",d->touch[1].id,d->touch[1].x,d->touch[1].y);
	}
	putchar('\n');
}



int main(int argc,const char** argv){
	ds4_init();
	ds4_raw_device_count_t len;
	ds4_raw_device_t* dev_l=ds4_enumerate_usb(&len);
	ds4_device_t dev;
	for (ds4_raw_device_count_t i=0;i<len;i++){
		printf("Device: %s\n",*(dev_l+i));
		if (!i){
			if (!ds4_connect_device(dev_l+i,&dev)){
				printf("Failed to connect to device!\n");
			}
		}
	}
	ds4_free_enumeration(dev_l,len);
	if (len){
		while (1){
			ds4_update(&dev);
			_print_ds4(&dev);
			dev.color=_hue_color();
			dev.rumble_small=dev.l2;
			dev.rumble_big=dev.r2;
			ds4_send_config(&dev);
			if (dev.buttons&DS4_BUTTON_PS){
				break;
			}
		}
		ds4_disconnect_device(&dev);
	}
	ds4_deinit();
	return 0;
}
