fallocate_write: fallocate_write.c Makefile
	gcc -Wall -Wextra -o fallocate_write fallocate_write.c

.PHONY: clean
clean:
	rm -f fallocate_write blob.*
