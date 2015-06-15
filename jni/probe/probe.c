#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/dm-ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "blkid/blkid.h"
#include "blkid/blkidP.h"

void*devmapper_ioctl(struct dm_ioctl**io,uint32_t*size,int fd,const char*name,int request);

const char*LOG_TAG="ParagonMounterProbe";

static int is_ntfs(const char*bootsector)
{
	const char ntfs[]={'N','T','F','S',' ',' ',' ',' '};
	return memcmp(bootsector+3,ntfs,sizeof ntfs)==0;
}

static int is_exfat(const char*bootsector)
{
	static const char exfat[]={'E','X','F','A','T',' ',' ',' '};
	return memcmp(bootsector+3,exfat,sizeof exfat)==0;
}

static int is_fat32(const char*bootsector)
{
	static const char fat32[]={'F','A','T','3','2',' ',' ',' '};
	return memcmp(bootsector+82,fat32,sizeof fat32)==0;
}

enum{
	HFS_VH_OFFSET=1024,
	HFS_VH_SIZE=512,
};
struct HFSPlusVolumeHeader{
	char signature[2];
	char version[2];
};
static int is_hfsplus(const struct HFSPlusVolumeHeader*h)
{
	return h->signature[0]=='H' && h->signature[1]=='+' && h->version[0]==0 && h->version[1]==4;
}

static int is_hfsx(const struct HFSPlusVolumeHeader*h)
{
	return h->signature[0]=='H' && h->signature[1]=='X' && h->version[0]==0 && h->version[1]==5;
}

static int print_base64(const void*pdata,size_t data_len)
{
	const char*base64chars="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char c;
	unsigned int line_len=0;
	const unsigned char*data=pdata;
	size_t i;
	
	for(i=0; i<data_len; ++i){
		c=data[i]>>2&0x3f;
		if(putchar(base64chars[c])==EOF)return -1;
		line_len++;
		c=data[i]<<4&0x30;
		if(data_len==++i)break;
		c+=data[i]>>4&0xf;
		if(putchar(base64chars[c])==EOF)return -1;
		line_len++;
		c=data[i]<<2&0x3c;
		if(data_len==++i)break;
		c+=data[i]>>6&3;
		if(putchar(base64chars[c])==EOF)return -1;
		line_len++;
		c=data[i]&0x3f;
		if(putchar(base64chars[c])==EOF)return -1;
		line_len++;
	}
	if(line_len%=4){
		if(putchar(base64chars[c])==EOF)return -1;
		while(line_len++<3)
			if(putchar('=')==EOF)return -1;
	}
	return 1;
}

blkid_dev blkid_identify(const char*device,const char*tentative_fs,const char**label)
{
	blkid_dev dev;
	blkid_cache cache;
	int ret;

	__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"blkid_identify: device=%s, tentative_fs=%s",device,tentative_fs);
	if ((ret = blkid_get_cache(&cache,"/dev/null")) != 0) {
		__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"blkid_identify: error creating cache (%d)",ret);
		return 0;
	}
	dev=blkid_get_dev(cache,device,BLKID_DEV_NORMAL);
	if(!dev){
		__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"blkid_identify: %s has an unsupported type",device);
		return 0;
	}

	//check if our tentative FS coincides with blkid FS
	const char*type=blkid_get_tag_value(cache,"TYPE",device);
	if(strcmp(tentative_fs,type)){
		__android_log_print(ANDROID_LOG_WARN,LOG_TAG,"blkid_identify: unexpected type=%s",type);
		blkid_free_dev(dev);
		return 0;
	}

	//get label from blkid
	*label=blkid_get_tag_value(cache,"LABEL",device);

	return dev;
}

void output_dm_deps()
{
	struct dm_ioctl*io_list=0,*io_deps=0;
	uint32_t size_list,size_deps;
	uint32_t i;

	const int fd=open("/dev/device-mapper",O_RDWR);
	if(fd==-1){
		__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"output_dm_deps(): error opening devmapper (%s)",strerror(errno));
		return;
	}

	//get device list
	const struct dm_name_list*list=devmapper_ioctl(&io_list,&size_list,fd,0,DM_LIST_DEVICES);
	if(!list)
		return;

	//iterate over devices
	while(1){
		//get device dependencies
		const struct dm_target_deps*deps=devmapper_ioctl(&io_deps,&size_deps,fd,list->name,DM_TABLE_DEPS);
		if(!deps)
			break;
		__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"output_dm_deps(): name=%s (/dev/block/dm-%u) deps count %u",list->name,minor(list->dev),deps->count);

		//iterate over device dependencies
		for(i=0;i<deps->count;i++){
			__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"output_dm_deps(): \tdev=0x%llx %u:%u",(long long)deps->dev[i],major(deps->dev[i]),minor(deps->dev[i]));
			if(printf("%u:%u/%s\n",major(deps->dev[i]),minor(deps->dev[i]),list->name)<0)
				break;
		}
		if(!list->next)
			break;
		list=(const struct dm_name_list*)((char*)list+list->next);
	}

	free(io_list);
	free(io_deps);
	close(fd);
}

#define SEP " "
#define UNKNOWN_FS "?"
#define NO_LABEL "?" //must be an invalid Base64 sequence
#define UNDETECTED UNKNOWN_FS SEP NO_LABEL SEP

int main(int argc,char**argv)
{
	int i;
	char buf[HFS_VH_OFFSET+HFS_VH_SIZE];
	int fd=-1;
	blkid_dev dev=0;

	__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"probe: argc=%d",argc);

	//output help when called without parameters
	if(argc==1){
		fprintf(stderr,"Empty probe parameters.\n");
		printf("Input: <device> ...\n");
		printf("Output: <Filesystem> <? or volume label in Base64 UTF-8> ...\n");
		printf("Output: <device maj:min>/<device mapper name>\n");
		printf("Output: ...\n");
		return 1;
	}

	for(i=1;i<argc;i++){
		fd=open(argv[i],O_RDONLY);
		if(fd==-1){
			const int e=errno;
			__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"probe: error opening '%s' errno %d %s",argv[i],e,strerror(e));
			if(printf(UNDETECTED)<0)break;
			continue;
		}
		const ssize_t len=read(fd,buf,sizeof buf);
		if(len==-1){
			const int e=errno;
			__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"probe: error reading '%s' errno %d %s",argv[i],e,strerror(e));
			if(printf(UNDETECTED)<0)break;
		}else{
			const struct HFSPlusVolumeHeader*h=(const struct HFSPlusVolumeHeader*)(buf+HFS_VH_OFFSET);
			const char*tentative_fs;
			const char*blkid_fs=0;
			const char*label;

			//detect FS using simple method
			if(is_ntfs(buf)){
				tentative_fs="NTFS";
				blkid_fs="ntfs";
			}else if(is_hfsplus(h)){
				tentative_fs="HFS+";
				blkid_fs="hfsplus";
			}else if(is_hfsx(h)){
				tentative_fs="HFSX";
				blkid_fs="hfsplus";
			}else if(is_exfat(buf)){
				tentative_fs="exFAT";
				if(printf("%s" SEP NO_LABEL SEP,tentative_fs)<0)break;
				//blkid_fs="take exFAT code from util-linux";
			}else if(is_fat32(buf)){
				tentative_fs="VFAT";
				blkid_fs="vfat";
			}else{
				if(printf(UNDETECTED)<0)break;
			}

			if(blkid_fs){
				dev=blkid_identify(argv[i],blkid_fs,&label);
				if(!dev){
					__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"probe: found %s, but blkid did not detect it",tentative_fs);
					if(printf(UNDETECTED)<0)break;
				}else{
					//get label and print the result
					if(label && label[0]){
						__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"probe: %s is %s with label %s",argv[i],tentative_fs,label);
						if(printf("%s" SEP,tentative_fs)<0)break;
						if(print_base64(label,strlen(label))<0)break;
						if(printf(SEP)<0)break;
					}else{
						__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"probe: %s is %s without label",argv[i],tentative_fs);
						if(printf("%s" SEP NO_LABEL SEP,tentative_fs)<0)break;
					}

					blkid_free_dev(dev);
					dev=0;
				}
			}
		}
		if(close(fd))
			assert(0);
		fd=-1;
	}
	if(dev)
		blkid_free_dev(dev);
	if(fd!=-1 && close(fd))
		assert(0);

	if(printf("\n")>0)
		output_dm_deps();
	return 0;
}
