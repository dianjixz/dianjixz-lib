#ifndef __PTS_CREAT_H__
#define __PTS_CREAT_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef enum {
	PTMX_STOP = 0,
	PTMX_RUN = 1
} ptmx_status_t;


typedef void (*msg_call_back_t)(const char*, int);
struct pts_creat_t
{
	int master_ptmx;
	char slave_ptmx_name[50];
	int ptmx_status;
	pthread_t pts_loop_handler_d;
	msg_call_back_t call;
	int buff_size;
};

struct pts_creat_t* pts_creat();
int pts_destroy(struct pts_creat_t **p);
int ptmx_open(struct pts_creat_t *self);
void *pts_loop_handler(void *par);
void ptmx_exit(struct pts_creat_t *self);

#ifdef __cplusplus
}
#include <string>
class ptmx
{
private:
	struct pts_creat_t ptm;
public:
	ptmx()
	{
		memset(&ptm, 0, sizeof(struct pts_creat_t));
		ptm.buff_size = 1024 * 4;
	}
	int open()
	{
		return ptmx_open(&ptm);
	}
	void set_buff_size(int size)
	{
		ptm.buff_size = size;
	}
	void set_msg_call_back(msg_call_back_t msg)
	{
		ptm.call = msg;
	}
	std::string get_slave_ptmx_name()
	{
		return std::string(ptm.slave_ptmx_name);
	}
	void exit()
	{
		if(ptm.ptmx_status)
			ptmx_exit(&ptm);
	}
	~ptmx()
	{
		exit();
	}
};
#endif // __cplusplus

#endif //__PTS_CREAT_H__