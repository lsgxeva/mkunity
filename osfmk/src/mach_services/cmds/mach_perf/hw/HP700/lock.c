/*
 * Copyright 1991-1998 by Open Software Foundation, Inc. 
 *              All Rights Reserved 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 *  
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT, 
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 */

/*
 * MkLinux
 */

#include <mach_perf.h>

struct slock {
	int		lock_data[4];	
};

/*
 * we'll always assign a block of 16 bytes to the spinlock and then assume
 * that the real lock is the one aligned on a 16 byte boundary
 */
#define	_align_spin_lock(s) \
  ((int *)(((int) (s) + sizeof(struct slock) - 1) & ~(sizeof(struct slock) - 1)))

#define simple_lock_init(s) (*_align_spin_lock(s) = -1)

#define simple_lock(s) spin_try_lock(s)

lock_loop()
{
	register unsigned i = loops;
	struct slock l;

	simple_lock_init(&l);
	start_time();
	while(i--) {
		simple_lock(&l);
		simple_unlock(&l);
	}
	stop_time();
}
