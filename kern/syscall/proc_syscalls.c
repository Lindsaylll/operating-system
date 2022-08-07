#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/wait.h>
#include <lib.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <thread.h>
#include <addrspace.h>
#include <copyinout.h>
#include <mips/trapframe.h>
#include <limits.h>

#include <clock.h>
#include <opt-A2.h>
#include <opt-A3.h>
#include <vfs.h>
#include <kern/fcntl.h>
#include <test.h>

/* this implementation of sys__exit does not do anything with the exit code */
/* this needs to be fixed to get exit() and waitpid() working properly */


void sys__exit(int exitcode)
{

  struct addrspace *as;
  struct proc *p = curproc;
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */

  //seems not need belowing  ????

  // #if OPT_A2
  //   curproc->p_exitcode = exitcode; //  ???
  //   // curproc->p_exitstatus = 0;      // exit for now ???
  // #else
  //   (void)exitcode;
  // #endif //OPT_A2 exit

  //   DEBUG(DB_SYSCALL, "Syscall: _exit(%d)\n", exitcode);

  //   KASSERT(curproc->p_addrspace != NULL);
  //   as_deactivate();

  /*
   * clear p_addrspace before calling as_destroy. Otherwise if
   * as_destroy sleeps (which is quite possible) when we
   * come back we'll be calling as_activate on a
   * half-destroyed address space. This tends to be
   * messily fatal.
   */
  as = curproc_setas(NULL);
  as_destroy(as);
#if OPT_A2
  // int num = 
  for (unsigned int i = 0; i < array_num(p->p_children);)
  {
    struct proc *temp_child;
    temp_child = (struct proc *)(array_get(p->p_children, i)); // shallow copy???
    array_remove(p->p_children, i);
    spinlock_acquire(&temp_child->p_lock);
    if (temp_child->p_exitstatus == 1) // if exited
    {
      spinlock_release(&temp_child->p_lock);
      proc_destroy(temp_child);
    }
    else // still can run
    {
      temp_child->p_parent = NULL;
      spinlock_release(&temp_child->p_lock);
    }
  }
#else
#endif //OPT_A2 exit

  /* detach this thread from its process */
  /* note: curproc cannot be used after this call */
  proc_remthread(curthread);

  /* if this is the last user process in the system, proc_destroy()
     will wake up the kernel menu thread */
#if OPT_A2
  spinlock_acquire(&p->p_lock);
  if (p->p_parent == NULL) // not running ???
  {
    spinlock_release(&p->p_lock);
    proc_destroy(p);
  }
  else
  {
    // mark as exit
    p->p_exitstatus = 1;
    p->p_exitcode = exitcode;
    spinlock_release(&p->p_lock);
  }

#else
  proc_destroy(p);
  (void)exitcode;
#endif //OPT_A2 exit

  thread_exit();
  /* thread_exit() does not return, so we should never get here */
  panic("return from thread_exit in sys_exit\n");
}











/* stub handler for getpid() system call                */
#if OPT_A2
int sys_getpid(pid_t *retval)
{
  /* for now, this is just a stub that always returns a PID of 1 */
  /* you need to fix this to make it work properly */
  struct proc *p = curproc;
  pid_t pid = p->p_pid;
  *retval = pid;
  return (0);
}
#else
/* stub handler for waitpid() system call                */
int sys_getpid(pid_t *retval)
{
  *retval = 1;
  return (0);
}
#endif // OPT_A2

int sys_waitpid(pid_t pid,
                userptr_t status,
                int options,
                pid_t *retval)
{
  int exitstatus;
  int result;

  /* this is just a stub implementation that always reports an
     exit status of 0, regardless of the actual exit status of
     the specified process.   
     In fact, this will return 0 even if the specified process
     is still running, and even if it never existed in the first place.

     Fix this!
  */

#if OPT_A2
  int num = array_num(curproc->p_children);
  bool found = false;
  struct proc *temp_child;
  for (int i = 0; i < num; i++)
  {
    struct proc *child_proc = array_get(curproc->p_children, i);
    pid_t child_pid = child_proc->p_pid;
    if (child_pid == pid)
    {
      temp_child = child_proc;
      array_remove(curproc->p_children, i);
      found = true;
      break;
    }
  }
  if (!found)
  {
    *retval = -1;
    return (ESRCH);
  }
  spinlock_acquire(&temp_child->p_lock);
  while (!temp_child->p_exitstatus)
  {
    spinlock_release(&temp_child->p_lock);
    clocksleep(1);
    spinlock_acquire(&temp_child->p_lock);
  }
  spinlock_release(&temp_child->p_lock);

  int tmp_child_exitcode = temp_child->p_exitcode;
  exitstatus = tmp_child_exitcode;
  proc_destroy(temp_child);
  exitstatus = _MKWAIT_EXIT(tmp_child_exitcode);

#else
#endif //OPT_A2 waitpid

  if (options != 0)
  {
    return (EINVAL);
  }
  /* for now, just pretend the exitstatus is 0 */

#if OPT_A2
#else
  exitstatus = 0;
#endif //OPT_A2 waitpid
  result = copyout((void *)&exitstatus, status, sizeof(int));
  if (result)
  {
    return (result);
  }
  *retval = pid;
  return (0);
}

#if OPT_A2
int sys_fork(pid_t *retval, struct trapframe *tf)
{
  struct proc *p = curproc;
  KASSERT(p != NULL);

  struct proc *childproc = proc_create_runprogram("child");
  // KASSERT(childproc != NULL);
  if (childproc == NULL)
  {
    return (ENOMEM);
  }
  // OPT_A2 exit
  childproc->p_parent = p; // deep copy?
  array_add(p->p_children, childproc, NULL);
  // int num = array_num(p->p_children);
  // printf("%d", num);

  as_copy(curproc_getas(), &(childproc->p_addrspace));
  struct trapframe *trapframe_for_child = (struct trapframe *)kmalloc(sizeof(struct trapframe));
  KASSERT(trapframe_for_child != NULL);

  *trapframe_for_child = *tf; //shallow cp
  // unfinished wait finishing enter_forked_process
  *retval = childproc->p_pid;
  thread_fork("childproc thread", childproc, (void *)enter_forked_process, trapframe_for_child, 0);
  clocksleep(1);

  return (0); //return err?
}
#else
#endif //OPT_A2

#if OPT_A3

char **args_alloc(int argc)
{
  char **space = kmalloc(argc * sizeof(char *));
  for (int i = 0; i < argc; ++i)
  {
    space[i] = kmalloc(ARG_MAX);
  }
  return space;
}

void args_free(char **arr, int argc)
{
  for (int i = 0; i < argc; ++i)
  {
    kfree(arr[i]);
  }
  kfree(arr);
}

char **argcopy_in(char **arr, int argc, char **argv, char *progname)
{
  // int result = copyinstr(progname, arr[0], 32, NULL);
  // if(result){
  //   return result;
  // }
  (void)progname;
  for (int i = 0; i < argc; ++i)
  {
    copyinstr((const_userptr_t)argv[i], arr[i], (size_t)ARG_MAX, NULL);
    // if(result){
    //   return result;
    // }
  }
  return arr;
}

// vaddr_t  argcopy_out(vaddr_t * stackptr, char * to_be_copy){

// 	// int dec = ( sizeof(to_be_copy) % 4 != 0 + sizeof(to_be_copy) / 4) *4;
// 	int dec = ROUNDUP( strlen(to_be_copy) + 1, 4);
// 	// sizeof(to_be_copy)
// 	// int dec = ROUNDUP( sizeof(to_be_copy), 4);
// 	*stackptr -= dec;
// 	int result = copyoutstr(to_be_copy, (userptr_t) *stackptr, dec, NULL);
// 	if(result) return result;
// 	return *stackptr;

// }

int sys_exec(char *progname, char **argv)
{

  // (void) nargs;
  struct addrspace *as;
  struct vnode *v;
  vaddr_t entrypoint, stackptr;
  int result;

  // for latter to destory old as

  /* Open the file. */
  // char progname[128];
  // strcpy(progname, args[0]);

  result = vfs_open(progname, O_RDONLY, 0, &v);
  if (result)
  {
    // args_free(new_argv, nargs);
    return result;
  }

  /* find nargs */
  int nargs = 0;
  char **tmp_argv = argv;
  while (tmp_argv[nargs] != NULL)
  {
    nargs++;
    // ++tmp_argv;
  }

  char **new_argv = args_alloc(nargs);
  new_argv = argcopy_in(new_argv, nargs, tmp_argv, progname);

  /* We should be a new process. */
  // KASSERT(curproc_getas() == NULL);

  /* Create a new address space. */
  as = as_create();
  if (as == NULL)
  {
    vfs_close(v);
    // args_free(new_argv, nargs);
    return ENOMEM;
  }

  /* Switch to it and activate it. */
  struct addrspace *old_as = curproc_setas(as);
  as_activate();
  /* destory the old address space. */
  as_destroy(old_as);

  /* Load the executable. */
  result = load_elf(v, &entrypoint);
  if (result)
  {
    /* p_addrspace will go away when curproc is destroyed */
    vfs_close(v);
    // args_free(new_argv, nargs);
    return result;
  }

  /* Done with the file now. */
  vfs_close(v);

  /* Define the user stack in the address space */
  result = as_define_stack(as, &stackptr);
  if (result)
  {
    /* p_addrspace will go away when curproc is destroyed */
    // args_free(new_argv, nargs);
    return result;
  }
  vaddr_t argv_user[nargs + 1];
  // do we need this line
  argv_user[nargs] = (vaddr_t)NULL;
  // stackptr = ROUNDUP(stackptr, 4);
  for (int i = nargs - 1; i > -1; --i)
  {
    stackptr = ROUNDUP(argcopy_out(&stackptr, new_argv[i]), 8);
    argv_user[i] = stackptr;
    // result = copyoutstr(args[i], (userptr_t) stackptr, strlen(args[i]) + 1, NULL	);
  }
  // copyout((void *) NULL, (userptr_t) stackptr, (size_t) 4);
  argv_user[nargs] = 0;
  for (int i = nargs; i > -1; --i)
  {
    stackptr -= sizeof(argv_user[i]);
    result = copyout(&argv_user[i], (userptr_t)stackptr, sizeof(argv_user[i]));
    if (result)
      return result;
  }

  /* free all array*/
  args_free(new_argv, nargs);

  //  stack top to bottom
  //  heap bottom to top
  /* Warp to user mode. */
  enter_new_process(nargs /*argc*/, (userptr_t)stackptr /*userspace addr of argv*/,
                    stackptr, entrypoint);

  /* enter_new_process does not return. */
  panic("enter_new_process returned\n");
  return EINVAL;
}
#else
#endif //OPT_A3 execv
