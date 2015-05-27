#include <android/log.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/dm-ioctl.h>

extern const char*LOG_TAG;

void*devmapper_ioctl(struct dm_ioctl**io,uint32_t*size,int fd,const char*name,int request)
{
	if(!*io){
		*size=sizeof**io+1024;
		*io=malloc(*size);
		if(!*io){
			__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"devmapper_ioctl(%x): malloc(%u) faled",request,*size);
			return 0;
		}
	}

	do{
		//reinit structure
		memset(*io,0,*size);
		(*io)->data_size=*size;
		(*io)->data_start=sizeof*io;
		(*io)->version[0]=4;
		(*io)->version[1]=0;
		(*io)->version[2]=0;
		(*io)->flags=0;
		if(name && strlcpy((*io)->name,name,sizeof(*io)->name)>=sizeof(*io)->name)
			return 0;

		if(ioctl(fd,request,*io)){
			__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"devmapper_ioctl(%x): ioctl failed (%s)",request,strerror(errno));
			return 0;
		}else if((*io)->flags & DM_BUFFER_FULL_FLAG){
			const uint32_t new_size=*size*2;
			__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"devmapper_ioctl(%x): ioctl buffer full. Current size %u. New size %u.",request,*size,new_size);
			if(new_size<*size)
				return 0;
			void*new_buffer=realloc(*io,new_size);
			if(io==new_buffer){
				__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"devmapper_ioctl(%x): realloc() failed",request);
				return 0;
			}
			*io=new_buffer;
			*size=new_size;
		}
	}while((*io)->flags & DM_BUFFER_FULL_FLAG);
	return(char*)*io+(*io)->data_start;
}
