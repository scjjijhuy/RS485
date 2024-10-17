#ifndef __NET_FILE_PORT_H__
#define __NET_FILE_PORT_H__

/*
	��ȡ�洢�ռ��С
	��UFS�� UFS
	��RAM�� RAM
	��SD�� SD card
*/
int net_file_space(char *name_pattern_in, int *free_size_out, int *total_size_out);

/*
	��ȡ�ļ��б�
*/
int net_file_list(char *name_pattern_in, char *list_out, int *file_amount);

/*
	ɾ���ļ�
*/
int net_file_delete(char *filename_in);

/*
	�ϴ��ļ�
*/
int net_file_upload(char *filename_in, int file_size_in, int timeout_in, int ackmode_in ,char *payload_in);

/*
	�ļ�����
*/
int net_file_download(char *filename_in, int payload_size_in, char *payload_out);

/*
	���ļ�
*/
int net_file_open(char *filename_in , int mode_in, int *filehandle_out);

/*
	���ļ�
*/
int net_file_read(int filehandle_in , int length_in, char *payload_out);

/*
	д�ļ�
*/
int net_file_write(int filehandle_in , int length_in, int timeout_in, char *payload_in);

/*
	�����ļ�ָ��
*/
int net_file_seek(int filehandle_in , int offset_in, int position_in);

/*
	�鿴�ļ�ָ�뵱ǰƫ����
*/
int net_file_position(int filehandle_in , int *offset_out);

/*
	�ض��ļ�
*/
int net_file_truncate(int filehandle_in);

/*
	�ر��ļ�
*/
int net_file_close(int filehandle_in);

/*
	�ƶ��ļ�
*/
int net_file_move(char *srcfilename_in, char *destfilename_in, int copy_in, int overwrite_in);


#endif

