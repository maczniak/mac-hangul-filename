#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "h.h"

/*
useful references:
https://en.wikipedia.org/wiki/Zip_(file_format)#Design
https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT
*/

int zip(char *filename) {
	// make a backup file
	pid_t child_pid = fork();
	if (child_pid == 0) {
		char **argv = malloc(sizeof(char *) * (4 + 1));
		if (argv == NULL) return -1;
		argv[0] = "cp";
		argv[1] = "-a";
		argv[2] = filename;
		argv[3] = malloc(strlen(filename) + strlen(POSTFIX) + 1);
		if (argv[3] == NULL) return -1;
		strcpy(argv[3], filename);
		strcpy(argv[3] + strlen(filename), POSTFIX);
		argv[4] = NULL;
		execve("/bin/cp", argv, NULL);
		return -1;
	} else {
		int ret;
		wait(&ret);
	}
	
	struct stat st;
	if (stat(filename, &st) != 0) return -1;
	int fd = open(filename, O_RDWR);
	char *mem = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED) return -1;
	
	// end of central directory record
	char *eocd = memmem(mem + st.st_size - 500, 500, "\x50\x4b\x05\x06", 4);
	if (eocd == NULL) return -1;
	int entries = *((short *)(eocd + 10));
	long offset = *((int *)(eocd + 16));
	// central directory file header
	char *cdfh = mem + offset;
	for (int i = 0; i < entries; i++) {
		// TODO: more sanity check
		char buf[BUFSIZE];
		int filename_size = *((short *)(cdfh + 28));
		int extra_comment_size = *((short *)(cdfh + 30)) + *((short *)(cdfh + 32));
		long lfh_offset = *((int *)(cdfh + 42));
		memcpy(buf, cdfh + 46, filename_size);
		buf[filename_size] = '\0';
		char *combined = combine(buf);
		int combined_size = strlen(combined);
		bzero(buf + combined_size, filename_size - combined_size);
		memcpy(cdfh + 46, combined, filename_size);
		memcpy(mem + lfh_offset + 30, combined, filename_size);
		cdfh += 46 + filename_size + extra_comment_size;
	}
	return 0;
}
