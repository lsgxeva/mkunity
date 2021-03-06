/*
 * linux/fs/hfs/dir.c
 *
 * Copyright (C) 1995-1997  Paul H. Hargrove
 * This file may be distributed under the terms of the GNU Public License.
 *
 * This file contains directory-related functions independent of which
 * scheme is being used to represent forks.
 *
 * Based on the minix file system code, (C) 1991, 1992 by Linus Torvalds
 *
 * "XXX" in a comment is a note to myself to consider changing something.
 *
 * In function preconditions the term "valid" applied to a pointer to
 * a structure means that the pointer is non-NULL and the structure it
 * points to has all fields initialized to consistent values.
 */

#include "hfs.h"
#include <linux/hfs_fs_sb.h>
#include <linux/hfs_fs_i.h>
#include <linux/hfs_fs.h>

/*================ File-local functions ================*/

/*
 * build_key()
 *
 * Build a key for a file by the given name in the given directory.
 * If the name matches one of the reserved names returns 1 otherwise 0.
 */
static int build_key(struct hfs_cat_key *key, struct inode *dir,
		     const char *name, int len)
{
	struct hfs_name cname;
	const struct hfs_name *reserved;

	/* mangle the name */
	hfs_nameout(dir, &cname, name, len);

	/* check against reserved names */
	reserved = HFS_SB(dir->i_sb)->s_reserved1;
	while (reserved->Len) {
		if (hfs_streq(reserved, &cname)) {
			return 1;
		}
		++reserved;
	}

	/* check against the names reserved only in the root directory */
	if (HFS_I(dir)->entry->cnid == htonl(HFS_ROOT_CNID)) {
		reserved = HFS_SB(dir->i_sb)->s_reserved2;
		while (reserved->Len) {
			if (hfs_streq(reserved, &cname)) {
				return 1;
			}
			++reserved;
		}
	}

	/* build the key */
	hfs_cat_build_key(HFS_I(dir)->entry->cnid, &cname, key);

	return 0;
}

/*
 * update_dirs_plus()
 *
 * Update the fields 'i_size', 'i_nlink', 'i_ctime', 'i_mtime' and
 * 'i_version' of the inodes associated with a directory that has
 * had a file ('is_dir'==0) or directory ('is_dir'!=0) added to it.
 */
static inline void update_dirs_plus(struct hfs_cat_entry *dir, int is_dir)
{
	int i;

	for (i = 0; i < 4; ++i) {
		struct inode *tmp = dir->sys_entry[i];
		if (tmp) {
			if (S_ISDIR(tmp->i_mode)) {
				if (is_dir &&
				    (i == HFS_ITYPE_TO_INT(HFS_ITYPE_NORM))) {
					/* In "normal" directory only */
					++(tmp->i_nlink);
				}
				tmp->i_size += HFS_I(tmp)->dir_size;
				tmp->i_version = ++event;
			}
			tmp->i_ctime = tmp->i_mtime = CURRENT_TIME;
		}
	}
}

/*
 * update_dirs_plus()
 *
 * Update the fields 'i_size', 'i_nlink', 'i_ctime', 'i_mtime' and
 * 'i_version' of the inodes associated with a directory that has
 * had a file ('is_dir'==0) or directory ('is_dir'!=0) removed.
 */
static inline void update_dirs_minus(struct hfs_cat_entry *dir, int is_dir)
{
	int i;

	for (i = 0; i < 4; ++i) {
		struct inode *tmp = dir->sys_entry[i];
		if (tmp) {
			if (S_ISDIR(tmp->i_mode)) {
				if (is_dir &&
				    (i == HFS_ITYPE_TO_INT(HFS_ITYPE_NORM))) {
					/* In "normal" directory only */
					--(tmp->i_nlink);
				}
				tmp->i_size -= HFS_I(tmp)->dir_size;
				tmp->i_version = ++event;
			}
			tmp->i_ctime = tmp->i_mtime = CURRENT_TIME;
		}
	}
}

/*
 * mark_inodes_deleted()
 *
 * Update inodes associated with a deleted entry to reflect its deletion.
 */
static inline void mark_inodes_deleted(struct hfs_cat_entry *entry, int is_dir)
{
	int i;

	for (i = 0; i < 4; ++i) {
		struct inode *tmp = entry->sys_entry[i];
		if (tmp) {
			if (is_dir && S_ISDIR(tmp->i_mode)) {
				tmp->i_size = 0;
			}
			tmp->i_nlink = 0;
			tmp->i_ctime = CURRENT_TIME;
		}
	}
}

/*================ Global functions ================*/

/*
 * hfs_dir_read()
 *
 * This is the read() entry in the file_operations structure for HFS
 * directories.	 It simply returns an error code, since reading is not
 * supported.
 */
hfs_rwret_t hfs_dir_read(struct inode * inode, struct file * filp,
			 char *buf, hfs_rwarg_t count)
{
	return -EISDIR;
}

/*
 * hfs_create()
 *
 * This is the create() entry in the inode_operations structure for
 * regular HFS directories.  The purpose is to create a new file in
 * a directory and return a corresponding inode, given the inode for
 * the directory and the name (and its length) of the new file.
 */
int hfs_create(struct inode * dir, const char * name, int len, int mode,
	       struct inode ** result)
{
	struct hfs_cat_entry *entry = HFS_I(dir)->entry;
	struct hfs_cat_entry *new;
	struct hfs_cat_key key;
	int error;

	*result = NULL;

	/* build the key, checking against reserved names */
	if (build_key(&key, dir, name, len)) {
		error = -EEXIST;
	} else {
		/* try to create the file */
		error = hfs_cat_create(entry, &key, 
				       (mode & S_IWUSR) ? 0 : HFS_FIL_LOCK,
				       HFS_SB(dir->i_sb)->s_type,
				       HFS_SB(dir->i_sb)->s_creator, &new);
	}

	if (!error) {
		update_dirs_plus(entry, 0);

		/* create an inode for the new file */
		*result = __hfs_iget(new, HFS_I(dir)->file_type, 0);
		if (!(*result)) {
			/* XXX correct error? */
			error = -EIO;
		}
	}

	iput(dir);
	return error;
}

/*
 * hfs_mkdir()
 *
 * This is the mkdir() entry in the inode_operations structure for
 * regular HFS directories.  The purpose is to create a new directory
 * in a directory, given the inode for the parent directory and the
 * name (and its length) of the new directory.
 */
int hfs_mkdir(struct inode * parent, const char * name, int len, int mode)
{
	struct hfs_cat_entry *entry = HFS_I(parent)->entry;
	struct hfs_cat_key key;
	int error;

	/* build the key, checking against reserved names */
	if (build_key(&key, parent, name, len)) {
		error = -EEXIST;
	} else {
		/* try to create the directory */
		error = hfs_cat_mkdir(entry, &key, NULL);
	}

	if (!error) {
		update_dirs_plus(entry, 1);
	}

	iput(parent);
	return error;
}

/*
 * hfs_mknod()
 *
 * This is the mknod() entry in the inode_operations structure for
 * regular HFS directories.  The purpose is to create a new entry
 * in a directory, given the inode for the parent directory and the
 * name (and its length) and the mode of the new entry (and the device
 * number if the entry is to be a device special file).
 *
 * HFS only supports regular files and directories and Linux disallows
 * using mknod() to create directories.  Thus we just check the arguments
 * and call hfs_create().
 */
int hfs_mknod(struct inode *dir, const char *name, int len, int mode, int rdev)
{
	int error;
	struct inode *inode;

	if (!dir) {
		error = -ENOENT;
	} else if (S_ISREG(mode)) {
		error = hfs_create(dir, name, len, mode, &inode);
		if (!error) {
			iput(inode);
		}
	} else {
		error = -EPERM;
	}
	return error;
}

/*
 * hfs_unlink()
 *
 * This is the unlink() entry in the inode_operations structure for
 * regular HFS directories.  The purpose is to delete an existing
 * file, given the inode for the parent directory and the name
 * (and its length) of the existing file.
 */
int hfs_unlink(struct inode * dir, const char * name, int len)
{
	struct hfs_cat_entry *entry = HFS_I(dir)->entry;
	struct hfs_cat_entry *victim = NULL;
	struct hfs_cat_key key;
	int error;

	if (build_key(&key, dir, name, len)) {
		error = -EPERM;
	} else if (!(victim = hfs_cat_get(entry->mdb, &key))) {
		error = -ENOENT;
	} else if (victim->type != HFS_CDR_FIL) {
		error = -EPERM;
	} else if (!(error = hfs_cat_delete(entry, victim, 1))) {
		mark_inodes_deleted(victim, 0);
		update_dirs_minus(entry, 0);
	}

	hfs_cat_put(victim);	/* Note that hfs_cat_put(NULL) is safe. */
	iput(dir);
	return error;
}

/*
 * hfs_rmdir()
 *
 * This is the rmdir() entry in the inode_operations structure for
 * regular HFS directories.  The purpose is to delete an existing
 * directory, given the inode for the parent directory and the name
 * (and its length) of the existing directory.
 */
int hfs_rmdir(struct inode * parent, const char * name, int len)
{
	struct hfs_cat_entry *entry = HFS_I(parent)->entry;
	struct hfs_cat_entry *victim = NULL;
	struct hfs_cat_key key;
	int error;

	if (build_key(&key, parent, name, len)) {
		error = -EPERM;
	} else if (!(victim = hfs_cat_get(entry->mdb, &key))) {
		error = -ENOENT;
	} else if ((entry == victim) ||
		   (entry->sys_entry[0] && entry->sys_entry[0]->i_mount) ||
		   (entry->sys_entry[1] && entry->sys_entry[1]->i_mount) ||
		   (entry->sys_entry[2] && entry->sys_entry[2]->i_mount) ||
		   (entry->sys_entry[3] && entry->sys_entry[3]->i_mount)) {
		error = -EPERM;
	} else if (victim->type != HFS_CDR_DIR) {
		error = -ENOTDIR;
	} else if (!(error = hfs_cat_delete(entry, victim, 1))) {
		mark_inodes_deleted(victim, 1);
		update_dirs_minus(entry, 1);
	}
	 
	hfs_cat_put(victim);	/* Note that hfs_cat_put(NULL) is safe. */
	iput(parent);
	return error;
}

/*
 * hfs_rename()
 *
 * This is the rename() entry in the inode_operations structure for
 * regular HFS directories.  The purpose is to rename an existing
 * file or directory, given the inode for the current directory and
 * the name (and its length) of the existing file/directory and the
 * inode for the new directory and the name (and its length) of the
 * new file/directory.
 */
int hfs_rename(struct inode *old_dir, const char * old_name, int old_len,
	       struct inode *new_dir, const char * new_name, int new_len,
	       int must_be_dir)
{
	struct hfs_cat_entry *old_parent = HFS_I(old_dir)->entry;
	struct hfs_cat_entry *new_parent = HFS_I(new_dir)->entry;
	struct hfs_cat_entry *victim = NULL;
	struct hfs_cat_entry *deleted;
	struct hfs_cat_key key;
	int error;

	if (build_key(&key, old_dir, old_name, old_len) ||
	    (HFS_ITYPE(old_dir->i_ino) != HFS_ITYPE(new_dir->i_ino))) {
		error = -EPERM;
	} else if (!(victim = hfs_cat_get(old_parent->mdb, &key))) {
		error = -ENOENT;
	} else if (must_be_dir && (victim->type != HFS_CDR_DIR)) {
		error = -ENOENT;
	} else if (build_key(&key, new_dir, new_name, new_len)) {
		error = -EPERM;
	} else if (!(error = hfs_cat_move(old_parent, new_parent,
					  victim, &key, &deleted))) {
		int is_dir = (victim->type == HFS_CDR_DIR);

		update_dirs_minus(old_parent, is_dir);
		if (deleted) {
			mark_inodes_deleted(deleted, is_dir);
			hfs_cat_put(deleted);
		} else {
			update_dirs_plus(new_parent, is_dir);
		}
	}
	 
	hfs_cat_put(victim);	/* Note that hfs_cat_put(NULL) is safe. */
	iput(old_dir);
	iput(new_dir);
	return error;
}
