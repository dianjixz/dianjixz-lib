#include "pts_creat.h"
#include <string.h>
#if defined(__linux__) || defined(__GLIBC__) || defined(__GNU__)
#define _GNU_SOURCE /* GNU glibc grantpt() prototypes */
#endif

struct pts_creat_t* pts_creat()
{
	struct pts_creat_t* p = (struct pts_creat_t*)malloc(sizeof(struct pts_creat_t));
	memset(p, 0, sizeof(struct pts_creat_t));
}

int pts_destroy(struct pts_creat_t **p)
{
	if((p == NULL) || (*p == NULL)) return -1;
	if((*p)->ptmx_status == PTMX_RUN)
	{
		ptmx_exit(*p);
	}
	free(*p);
	*p = NULL;
	return 0;
}


int ptmx_open(struct pts_creat_t *self)
{
	char slavename[50];
	self->master_ptmx = open("/dev/ptmx", O_RDWR | O_NOCTTY);
	if(self->master_ptmx <= 0)
	{
		return self->master_ptmx;
	}
	grantpt(self->master_ptmx);
	unlockpt(self->master_ptmx);
	ptsname_r(self->master_ptmx, slavename, 50);
	strcpy(self->slave_ptmx_name, slavename);
	self->ptmx_status = PTMX_RUN;
	pthread_create(&self->pts_loop_handler_d, NULL, pts_loop_handler, self);
	return self->master_ptmx;
}


void *pts_loop_handler(void *par)
{
	struct pts_creat_t* pts = (struct pts_creat_t*)par;

	char *buff = (char*)malloc(pts->buff_size);
	do
	{
		int size = read(pts->master_ptmx, buff, pts->buff_size);
		if (size > 0)
		{
			if (pts->call)
			{
				pts->call(buff, size);
			}
		}
		else
		{
			usleep(100000);
		}
	} while (pts->ptmx_status);
	free(buff);
	return NULL;
}

void ptmx_exit(struct pts_creat_t *self)
{
	self->ptmx_status = PTMX_STOP;
	char tmp_buff[128];
	sprintf(tmp_buff, "echo \"exit\" >> %s", self->slave_ptmx_name);
	system(tmp_buff);
	pthread_join(self->pts_loop_handler_d, NULL);
	close(self->master_ptmx);
}

#if defined(__linux__) || defined(__GLIBC__) || defined(__GNU__)
#undef name _GNU_SOURCE /* GNU glibc grantpt() prototypes */
#endif
