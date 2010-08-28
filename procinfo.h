#ifndef _PROCINFO_
#define _PROCINFO_

#include <stdio.h>
#include <unistd.h>

#ifdef __linux__
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#else
#ifdef __FreeBSD__
#include <fcntl.h>
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>

struct pi_mem_struct {
  int inactive_mibs[4];
  int cached_mibs[4];
  int free_mibs[4];
  long int pagesize;
};

extern struct pi_mem_struct pims;
#else
#error libprocinfo has not been implemented on this platform
#endif
#endif

struct procinfostruct {
  unsigned long long int utime; //process user time in msecs
  unsigned long long int stime; //process system time in msecs
  int nice;                     //nice level
  unsigned long long int rsize; //process resident memory size
  unsigned long long int vsize; //process virtual memory size
};

#ifdef __cplusplus
extern "C" {
  int procinfo(const unsigned int pid, struct procinfostruct *pinfo);
  long long int sysfreemem();
}
#endif

int procinfo(const unsigned int pid, struct procinfostruct *pinfo);
long long int sysfreemem();

#endif
