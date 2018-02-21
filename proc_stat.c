#include "proc_stat.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
 
int get_proc_stat(struct proc_stat_t *proc_stat)  {
	
	// Generate filename string.
	char path_name[30];
	sprintf(path_name, "/proc/%d/stat", getpid());


	// Open file.
	FILE* file = fopen(path_name, "r");
	if (file == NULL) {
		return -1;
	}

	// parse file
	//                                       10                                      20                                       30                                    40                                     50
	fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d"
		,&(proc_stat->pid)
		,proc_stat->comm
		,&(proc_stat->state)
		,&(proc_stat->ppid)
		,&(proc_stat->pgrp)
		,&(proc_stat->session)
		,&(proc_stat->tty_nr)
		,&(proc_stat->tpgid)
		,&(proc_stat->flags)
		,&(proc_stat->minflt)
		,&(proc_stat->cminflt)
		,&(proc_stat->majflt)
		,&(proc_stat->cmajflt)
		,&(proc_stat->utime)
		,&(proc_stat->stime)
		,&(proc_stat->cutime)
		,&(proc_stat->cstime)
		,&(proc_stat->priority)
		,&(proc_stat->nice)
		,&(proc_stat->num_threads)
		,&(proc_stat->itrealvalue)
		,&(proc_stat->starttime)
		,&(proc_stat->vsize)
		,&(proc_stat->rss)
		,&(proc_stat->rsslim)
		,&(proc_stat->startcode)
		,&(proc_stat->endcode)
		,&(proc_stat->startstack)
		,&(proc_stat->kstkesp)
		,&(proc_stat->kstkeip)
		,&(proc_stat->signal)
		,&(proc_stat->blocked)
		,&(proc_stat->sigignore)
		,&(proc_stat->sigcatch)
		,&(proc_stat->wchan)
		,&(proc_stat->nswap)
		,&(proc_stat->cnswap)
		,&(proc_stat->exit_signal)
		,&(proc_stat->processor)
		,&(proc_stat->rt_priority)
		,&(proc_stat->policy)
		,&(proc_stat->delayacct_blkio_ticks)
		,&(proc_stat->guest_time)
		,&(proc_stat->cguest_time)
		,&(proc_stat->start_data)
		,&(proc_stat->end_data)
		,&(proc_stat->start_brk)
		,&(proc_stat->arg_start)
		,&(proc_stat->arg_end)
		,&(proc_stat->env_start)
		,&(proc_stat->env_end)
		,&(proc_stat->exit_code));

	if (fclose(file) != 0) {
		return -1;
	}
	
	return 0;
}