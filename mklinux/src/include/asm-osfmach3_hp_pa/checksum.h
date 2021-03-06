/*
 * Copyright (c) 1991-1998 Open Software Foundation, Inc. 
 *  
 * 
 */
/*
 * MkLinux
 */

#ifndef _ASM_OSFMACH3_HPPA_CHECKSUM_H
#define _ASM_OSFMACH3_HPPA_CHECKSUM_H


/*
 *	This is a version of ip_compute_csum() optimized for IP headers,
 *	which always checksum on 4 octet boundaries.
 */
extern unsigned short ip_fast_csum(unsigned char * iph, unsigned int ihl);

/*
 * computes the checksum of the TCP/UDP pseudo-header
 * returns a 16-bit checksum, already complemented
 */
extern unsigned short int csum_tcpudp_magic(unsigned long saddr,
					   unsigned long daddr,
					   unsigned short len,
					   unsigned short proto,
					   unsigned int sum);

/*
 * computes the checksum of a memory block at buff, length len,
 * and adds in "sum" (32-bit)
 *
 * returns a 32-bit number suitable for feeding into itself
 * or csum_tcpudp_magic
 *
 * this function must be called with even lengths, except
 * for the last fragment, which may be odd
 *
 * it's best to have buff aligned on a 32-bit boundary
 */
extern unsigned int csum_partial(const unsigned char * buff, int len, unsigned int sum);

/*
 * the same as csum_partial, but copies from src while it
 * checksums
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */
unsigned int csum_partial_copy( const char *src, char *dst, int len, int sum);

/*
 * the same as csum_partial_copy, but copies from user space.
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */

unsigned int csum_partial_copy_fromuser(const char *src, char *dst, int len, int sum);

/*
 * this routine is used for miscellaneous IP-like checksums, mainly
 * in icmp.c
 */

extern unsigned short ip_compute_csum(unsigned char * buff, int len);

static inline short csum_fold(unsigned int sum)
{
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

#endif /* _ASM_OSFMACH3_HPPA_CHECKSUM_H */
