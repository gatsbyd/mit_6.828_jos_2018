// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	if (!((err & FEC_WR) && (uvpt[PGNUM(addr)] & PTE_COW))) { //只有因为写操作写时拷贝的地址这中情况，才可以抢救。否则一律panic
		panic("pgfault():not cow");
	}


	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	addr = ROUNDDOWN(addr, PGSIZE);
	if ((r = sys_page_map(0, addr, 0, PFTEMP, PTE_U|PTE_P)) < 0)		//将当前进程PFTEMP也映射到当前进程addr指向的物理页
		panic("sys_page_map: %e", r);
	if ((r = sys_page_alloc(0, addr, PTE_P|PTE_U|PTE_W)) < 0)	//令当前进程addr指向新分配的物理页
		panic("sys_page_alloc: %e", r);
	memmove(addr, PFTEMP, PGSIZE);								//将PFTEMP指向的物理页拷贝到addr指向的物理页
	if ((r = sys_page_unmap(0, PFTEMP)) < 0)					//解除当前进程PFTEMP映射
		panic("sys_page_unmap: %e", r);
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	void *addr = (void*) (pn * PGSIZE);
	if (uvpt[pn] & PTE_SHARE) {
		sys_page_map(0, addr, envid, addr, PTE_SYSCALL);		//对于表示为PTE_SHARE的页，拷贝映射关系，并且两个进程都有读写权限
	} else if ((uvpt[pn] & PTE_W) || (uvpt[pn] & PTE_COW)) { //对于UTOP以下的可写的或者写时拷贝的页，拷贝映射关系的同时，需要同时标记当前进程和子进程的页表项为PTE_COW
		if ((r = sys_page_map(0, addr, envid, addr, PTE_COW|PTE_U|PTE_P)) < 0)
			panic("sys_page_map：%e", r);
		if ((r = sys_page_map(0, addr, 0, addr, PTE_COW|PTE_U|PTE_P)) < 0)
			panic("sys_page_map：%e", r);
	} else {
		sys_page_map(0, addr, envid, addr, PTE_U|PTE_P);	//对于只读的页，只需要拷贝映射关系即可
	}
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	extern void _pgfault_upcall(void);
	set_pgfault_handler(pgfault);	//设置缺页处理函数
	envid_t envid = sys_exofork();	//系统调用，只是简单创建一个Env结构，复制当前用户环境寄存器状态，UTOP以下的页目录还没有建立
	if (envid == 0) {				//子进程将走这个逻辑
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	if (envid < 0) {
		panic("sys_exofork: %e", envid);
	}

	uint32_t addr;
	for (addr = 0; addr < USTACKTOP; addr += PGSIZE) {
		if ((uvpd[PDX(addr)] & PTE_P) && (uvpt[PGNUM(addr)] & PTE_P) //为什么uvpt[pagenumber]能访问到第pagenumber项页表条目：https://pdos.csail.mit.edu/6.828/2018/labs/lab4/uvpt.html
			&& (uvpt[PGNUM(addr)] & PTE_U)) {
			duppage(envid, PGNUM(addr));	//拷贝当前进程映射关系到子进程
		}
	}
	int r;
	if ((r = sys_page_alloc(envid, (void *)(UXSTACKTOP-PGSIZE), PTE_P | PTE_W | PTE_U)) < 0)	//为子进程分配异常栈
		panic("sys_page_alloc: %e", r);
	sys_env_set_pgfault_upcall(envid, _pgfault_upcall);		//为子进程设置_pgfault_upcall

	if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)	//设置子进程为ENV_RUNNABLE状态
		panic("sys_env_set_status: %e", r);
	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
