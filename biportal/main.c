#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>

int main (int argc, const char * argv[]) {
    if(geteuid()) return fprintf(stderr, "unprivileged\n"), printf("%d",EPERM), 1;
    if(argc==1) {
	struct stat st;
	if(stat(*argv, &st)) return fprintf(stderr, "failed to self-stat\n"), printf("%d",errno), 2;
	if(st.st_uid!=0 && chown(*argv, 0, 0)) return fprintf(stderr, "failed to self-chown\n"), printf("%d",errno), 3;
	if((!(st.st_mode&S_ISUID)) && chmod(*argv, S_ISUID|0555)) return fprintf(stderr, "failed to self-chmod\n"), printf("%d",errno), 4;
	return printf("0"),0;
    }
    if(argc==2 && !strcmp(argv[1],"-k")) {
	pid_t p = fork();
	if(p<0) return fprintf(stderr, "failed to fork\n"), printf("%d",errno), 5;
	if(!p) {
	    if(setuid(0)) return fprintf(stderr,"failed to setuid\n"), printf("%d",errno),8;
	    execl("/bin/launchctl","/bin/launchctl", "remove", "com.apple.tftpd", NULL);
	    exit(errno);
	}
	int r, wp;
	while((wp=waitpid(p,&r,0))<0 && errno==EINTR);
	if(p!=wp) return fprintf(stderr,"failed to wait for launchctl"), 6;
	if(!WIFEXITED(r)) return fprintf(stderr,"launchctl failed"), 7;
	return fprintf(stderr,"launchctl terminated with %d",WEXITSTATUS(r)), r;
    }
    if(argc!=4) return fprintf(stderr, "Usage: %s s h p\n",*argv), 10;
    struct sockaddr_in sin; memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(argv[2]);
    sin.sin_port = htons(strtol(argv[3],NULL,0));
    int rc;
    printf("%d",
	   rc=(bind( (int)strtol(argv[1],NULL,0), (struct sockaddr*)&sin,sizeof(sin) )<0)
	   ? errno : 0);
    return rc;
}


