#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "ZedBoard.h"

using namespace std;

/**
 * Write a 4-byte value at the specified general-purpose I/O location.
 *
 * @param pBase     Base address returned by 'mmap'.
 * @parem offset    Offset where device is mapped.
 * @param value     Value to be written.
 */
void ZedBoard::RegisterWrite(int offset, int value)
{
	* (int *) (pBase + offset) = value;
}

/**
 * Read a 4-byte value from the specified general-purpose I/O location.
 *
 * @param pBase     Base address returned by 'mmap'.
 * @param offset    Offset where device is mapped.
 * @return      Value read.
 */
int ZedBoard::RegisterRead(int offset)
{
	return * (int *) (pBase + offset);
}


/**
 * Initialize general-purpose I/O
 *  - Opens access to physical memory /dev/mem
 *  - Maps memory at offset 'gpio_address' into virtual address space
 *
 * @param  fd   File descriptor passed by reference, where the result
 *      of function 'open' will be stored.
 * @return  Address to virtual memory which is mapped to physical,
 *              or MAP_FAILED on error.
 */
ZedBoard::ZedBoard() {
	fd = open( "/dev/mem", O_RDWR);
	pBase = (char *) mmap(
			NULL,
			gpio_size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			gpio_address);

	if (pBase == MAP_FAILED) {
		throw -1;
	}
}

/**
 * Close general-purpose I/O.
 *
 * @param pBase Virtual address where I/O was mapped.
 * @param fd    File descriptor previously returned by 'open'.
 */
ZedBoard::~ZedBoard()
{
	munmap(pBase, gpio_size);
	close(fd);
}

int ZedBoard::PushButtonGet() {
	for(int i = 0; i < 5; i++) {
		int state = RegisterRead(gpio_pbtnl_offset + i*sizeof(int));
		if (state > 0) {
			return i+1;
		}
	}
	return 0;
}


