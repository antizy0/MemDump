/*
====================================================
Name		: MemDump.c
Author		: DKing
Version		: 1.0
=====================================================
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

#ifndef PTRACE_ATTACH
#define PTRACE_ATTACH 16
#define PTRACE_DETACH 17
#endif 

int main(int argc, char **argv){
	int result = 0;
	int pid = -1;
	unsigned long start_addr=0,end_addr=0;
	unsigned long length = 0;
	char pathBuff[256] = {'\0'};
	int status = -1;
	int fd = -1;

	void *saveAddr = NULL;
	FILE *svfp = NULL;

	if(argc!=5){
		printf("usage: MemDump pid start_addr end_addr filename\n");
		result = -1;
		goto mReturn;
	}

	pid = atoi(argv[1]);
	sscanf(argv[2], "%lx", &start_addr);
	sscanf(argv[3], "%lx", &end_addr);
	length = end_addr - start_addr;

	printf("MemDump length : %ld \n", length);

	/* 
	if(ptrace(PTRACE_ATTACH, pid)==-1){
		printf("pid=%d ptrace attach failed, error:%s\n", pid, strerror(errno));
		result = -1;
		goto mReturn;
	}

	status = waitpid(pid, NULL, WUNTRACED);
	if(status==-1){
		printf("waitpid pid=%d failed, error:%s\n", pid, strerror(errno));
		ptrace(PTRACE_DETACH, pid);
		result = -1;
		goto mReturn;
	}
	*/	
	
	memset(pathBuff, '\0', sizeof(pathBuff));
	sprintf(pathBuff, "/proc/%d/mem", pid);
	fd = open(pathBuff, O_RDONLY);
	if(fd==-1){
		printf("open file=%s failed, error:%s\n", pathBuff, strerror(errno));
		result = -1;
		//ptrace(PTRACE_DETACH, pid);
		goto mReturn;
	}

	if(lseek(fd, start_addr, 0)==-1){
		printf("lseek failed , error: %s\n", strerror(errno));
		//ptrace(PTRACE_DETACH, pid);
		close(fd);
		result = -1;
	}
	else{
		saveAddr = malloc(length);
		if(saveAddr){
			if(length > read(fd, saveAddr, length)){
				printf("read failed,error: %s\n", strerror(errno));
				result = -1;
			}else{
				svfp = fopen(argv[4], "wb+");
				if(svfp){
					fwrite(saveAddr, 1, length, svfp);
					fclose(svfp);
					result = 0;
				}else{
					printf("fopen %s failed ,error: %s\n", argv[4], strerror(errno));
					result = -1;
				}
			}
			free(saveAddr);
		}else{
			printf("malloc failed, error:%s\n", strerror(errno));
			result = -1;
		}
		//ptrace(PTRACE_DETACH, pid);
		close(fd);
	}

mReturn:
	return result;
}
