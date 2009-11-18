#include "procinfo.h"

#ifdef __linux__
int procinfo(const unsigned int pid, struct procinfostruct *pinfo)
{
  char ppath[32];
  sprintf(ppath,"/proc/%u/stat",pid);
  char cbuf=0;
  long int rp, n, cut, cst;
  long unsigned int ut, st, vs;
  float jtms=1000./sysconf(_SC_CLK_TCK);

  FILE* procf=fopen(ppath,"r");

  if(!procf) {
    perror(ppath);
    return -1;
  }

  while(!feof(procf)){cbuf=fgetc(procf); if(cbuf==')') break;}

  if(cbuf==EOF) {
    fprintf(stderr,"Error: procinfo: invalid stat file format: %s\n",ppath);
    return -1;
  }

  fscanf(procf," %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %ld %ld %*d %ld %*d %*d %*u %lu %ld",&ut,&st,&cut,&cst,&n,&vs,&rp);

  if(feof(procf) || ferror(procf)) {
    fprintf(stderr,"Error: procinfo: invalid stat file format: %s\n",ppath);
    return -1;
  }

  fclose(procf);

  pinfo->utime=(ut+cut)*jtms;
  pinfo->stime=(st+cst)*jtms;
  pinfo->nice=n;
  pinfo->rsize=rp*sysconf(_SC_PAGESIZE);
  pinfo->vsize=vs;

  return 0;
}

long long int sysfreemem(){

  FILE* procf=fopen("/proc/meminfo","r");
  int free, cached, inactive;

  if(!procf) {
    perror("/proc/meminfo");
    return -1;
  }
  fscanf(procf," %*s %*i %*s %*s %i %*s %*s %*i %*s %*s %i %*s %*s %*i %*s %*s %*i %*s %*s %i",&free,&cached,&inactive);
  fclose(procf);

  return ((long long int)free+cached+inactive)*1024;
}
#endif

#ifdef __FreeBSD__
struct pi_mem_struct pims;

int procinfo(const unsigned int pid, struct procinfostruct *pinfo)
{
  kvm_t *kd;
  struct kinfo_proc *kinfo;
  int cnt;
  kd=kvm_open(NULL, "/dev/null", NULL, O_RDONLY, "kvm");
  kinfo=kvm_getprocs(kd, KERN_PROC_PID, pid, &cnt);

  if(cnt!=1) {
    fprintf(stderr,"Error: procinfo: Cannot retrieve information for process %u\n",pid);
    return -1;
  }
  pinfo->utime=(kinfo->ki_rusage.ru_utime.tv_sec+kinfo->ki_childutime.tv_sec)*1000+((kinfo->ki_rusage.ru_utime.tv_usec+kinfo->ki_childutime.tv_usec)+500)/1000;
  pinfo->stime=(kinfo->ki_rusage.ru_stime.tv_sec+kinfo->ki_childstime.tv_sec)*1000+((kinfo->ki_rusage.ru_stime.tv_usec+kinfo->ki_childstime.tv_usec)+500)/1000;
  pinfo->nice=kinfo->ki_nice;
  pinfo->rsize=kinfo->ki_rssize*sysconf(_SC_PAGESIZE);
  pinfo->vsize=kinfo->ki_size;
  kvm_close(kd);
  return 0;
}

long long int sysfreemem()
{
  long long int ret=0;
  size_t len=4;
  int buf;

  if(!pims.pagesize) {
    pims.pagesize=sysconf(_SC_PAGESIZE);
    
    if(sysctlnametomib("vm.stats.vm.v_inactive_count",pims.inactive_mibs,&len)<0) {
      perror("sysctlnametomib");
      return 1;
    }
 
    if(sysctlnametomib("vm.stats.vm.v_cache_count",pims.cached_mibs,&len)<0) {
      perror("sysctlnametomib");
      return 1;
    }
 
    if(sysctlnametomib("vm.stats.vm.v_free_count",pims.free_mibs,&len)<0) {
      perror("sysctlnametomib");
      return 1;
    }
  }

  sysctl(pims.inactive_mibs,4,&buf,&len,NULL,0);
  ret+=buf;
  sysctl(pims.cached_mibs,4,&buf,&len,NULL,0);
  ret+=buf;
  sysctl(pims.free_mibs,4,&buf,&len,NULL,0);
  ret+=buf;
  return ret*pims.pagesize;
}
#endif
