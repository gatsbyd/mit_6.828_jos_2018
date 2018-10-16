#ifndef JOS_INC_ELF_H
#define JOS_INC_ELF_H

#define ELF_MAGIC 0x464C457FU	/* "\x7FELF" in little endian */

struct Elf {
	uint32_t e_magic;	// must equal ELF_MAGIC
	uint8_t e_elf[12];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;	//program header offset
	uint32_t e_shoff;	//section header offset
	uint32_t e_flags;
	uint16_t e_ehsize;	//this header size
	uint16_t e_phentsize;	//program header size
	uint16_t e_phnum;		//number of program headers
	uint16_t e_shentsize;	//section header size
	uint16_t e_shnum;		//number of section headers
	uint16_t e_shstrndx;	//section header string table idnex
};

struct Proghdr {		//program header结构，《程序员自我修养》p165
	uint32_t p_type;	//类型，LOAD类型的常量为1
	uint32_t p_offset;	//该Segment在文件中的偏移
	uint32_t p_va;		//该Segment第一个字节在虚拟地址空间的起始位置
	uint32_t p_pa;		//该Segment的物理装载地址
	uint32_t p_filesz;	//在ELF文件中的大小
	uint32_t p_memsz;	//在虚拟地址空间占用的大小，可能大于p_filez，比如BSS
	uint32_t p_flags;	//权限属性
	uint32_t p_align;	//对齐属性
};

struct Secthdr {			//段表的每一项结构
	uint32_t sh_name;		//段名
	uint32_t sh_type;		//段的类型
	uint32_t sh_flags;		//段的标志
	uint32_t sh_addr;		//虚拟地址
	uint32_t sh_offset;		//该段在文件中的偏移
	uint32_t sh_size;		//段的长度
	uint32_t sh_link;		//段链接信息
	uint32_t sh_info;		//段链接信息
	uint32_t sh_addralign;	//短地址对齐
	uint32_t sh_entsize;	//Section Entry Size
};

// Values for Proghdr::p_type
#define ELF_PROG_LOAD		1

// Flag bits for Proghdr::p_flags
#define ELF_PROG_FLAG_EXEC	1
#define ELF_PROG_FLAG_WRITE	2
#define ELF_PROG_FLAG_READ	4

// Values for Secthdr::sh_type	//段类型，对应Secthdr的sh_type字段
#define ELF_SHT_NULL		0	//无效段
#define ELF_SHT_PROGBITS	1	//代码段或者代码段
#define ELF_SHT_SYMTAB		2	//符号表
#define ELF_SHT_STRTAB		3	//字符串表

// Values for Secthdr::sh_name
#define ELF_SHN_UNDEF		0

#endif /* !JOS_INC_ELF_H */
