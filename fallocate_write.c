#define _GNU_SOURCE
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/falloc.h>

int main() {
	printf("reading from /dev/urandom\n");
	int in = open("/dev/urandom", O_RDONLY);
	if(in < 0) {
		perror("open failed");
		exit(1);
	}
	char name[] = "./blob.XXXXXX";
	int out = mkstemp(name);
	if(out < 0) {
		perror("mkstemp failed");
		exit(1);
	}
	printf("writing to %s\n", name);
	/* gocryptfs writes 128kiB + encryption overhead blocks */
	char buf[132096];
	printf("writing blocks of %ld bytes each\n", sizeof(buf));
	/* we start at offset 18 due to the gocryptfs header */
	long off = 18;
	for(int i = 1; ; i++) {
		int n = read(in, buf, sizeof(buf));
		if(n == 0){
			printf("read failed: end of file\n");
			exit(1);
		}
		if(n < 0){
			perror("read failed");
			exit(1);
		}
		if(n != sizeof(buf)) {
			printf("short read, retrying\n");
			continue;
		}
		n = fallocate(out, FALLOC_FL_KEEP_SIZE, off, sizeof(buf));
		if(n != 0) {
			perror("fallocate failed");
			exit(1);
		}
		n = pwrite(out, buf, sizeof(buf), off);
		if(n < 0) {
			perror("pwrite failed");
			exit(1);
		}
		if(n != sizeof(buf)) {
			printf("short write\n");
			exit(1);
		}
		off += sizeof(buf);
		if(i%100 == 0) {
			printf("wrote %d blocks, total %ld MiB\n",
				i, i*sizeof(buf)/1024/1024);
		}
	}
}
