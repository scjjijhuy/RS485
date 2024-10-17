#ifndef __NET_FILE_PORT_H__
#define __NET_FILE_PORT_H__

/*
	获取存储空间大小
	“UFS” UFS
	“RAM” RAM
	“SD” SD card
*/
int net_file_space(char *name_pattern_in, int *free_size_out, int *total_size_out);

/*
	获取文件列表
*/
int net_file_list(char *name_pattern_in, char *list_out, int *file_amount);

/*
	删除文件
*/
int net_file_delete(char *filename_in);

/*
	上传文件
*/
int net_file_upload(char *filename_in, int file_size_in, int timeout_in, int ackmode_in ,char *payload_in);

/*
	文件下载
*/
int net_file_download(char *filename_in, int payload_size_in, char *payload_out);

/*
	打开文件
*/
int net_file_open(char *filename_in , int mode_in, int *filehandle_out);

/*
	读文件
*/
int net_file_read(int filehandle_in , int length_in, char *payload_out);

/*
	写文件
*/
int net_file_write(int filehandle_in , int length_in, int timeout_in, char *payload_in);

/*
	设置文件指针
*/
int net_file_seek(int filehandle_in , int offset_in, int position_in);

/*
	查看文件指针当前偏移量
*/
int net_file_position(int filehandle_in , int *offset_out);

/*
	截断文件
*/
int net_file_truncate(int filehandle_in);

/*
	关闭文件
*/
int net_file_close(int filehandle_in);

/*
	移动文件
*/
int net_file_move(char *srcfilename_in, char *destfilename_in, int copy_in, int overwrite_in);


#endif

