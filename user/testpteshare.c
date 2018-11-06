#include <inc/x86.h>
#include <inc/lib.h>

#define VA	((char *) 0xA0000000)
const char *msg = "hello, world\n";
const char *msg2 = "goodbye, world\n";

void childofspawn(void);

void
umain(int argc, char **argv)
{
	int r;

	if (argc != 0)
		childofspawn();

	if ((r = sys_page_alloc(0, VA, PTE_P|PTE_W|PTE_U|PTE_SHARE)) < 0)		//VA指向的物理页是共享的
		panic("sys_page_alloc: %e", r);

	// check fork
	if ((r = fork()) < 0)				//在fork()时，子进程和父进程共享标识PTE_SHARE的页
		panic("fork: %e", r);
	if (r == 0) {						//子进程
		strcpy(VA, msg);				//向子进程的VA虚拟地址处拷贝msg
		exit();
	}
	wait(r);				//等子进程执行完毕
	cprintf("fork handles PTE_SHARE %s\n", strcmp(VA, msg) == 0 ? "right" : "wrong");	//父进程检查VA虚拟地址处是否也能感受到子进程写入的msg字符串

	// check spawn
	if ((r = spawnl("/testpteshare", "testpteshare", "arg", 0)) < 0)		
		panic("spawn: %e", r);
	wait(r);
	cprintf("spawn handles PTE_SHARE %s\n", strcmp(VA, msg2) == 0 ? "right" : "wrong");

	breakpoint();
}

void
childofspawn(void)
{
	strcpy(VA, msg2);
	exit();
}
