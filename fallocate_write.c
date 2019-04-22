#define _GNU_SOURCE
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/falloc.h>
#include <sys/time.h>

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
	while(1) {
		const int blocks_per_loop = 1000;
		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		for(int i = 1; i <= blocks_per_loop; i++) {
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
		}
		gettimeofday(&t2, NULL);
		float deltat = ( t2.tv_sec - t1.tv_sec ) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
		printf("total %6ld MiB, %6.2f MiB/s\n", off/1024/1024,
			blocks_per_loop*sizeof(buf)/deltat/1024/1024);
	}
}
