
extern char *os_abi_index[];
extern char *machine_index[];
#define ELFOSABI_DEFINED_VALUES 15
#define ELFMACHINE_DEFINED_VALUES 101

#define LINE_BREAK 16
#define EI_NIDENT 16
#define ELF_MAGIC 0x7f454c46 # 0x7f + "ELF"

#define ELF_32BIT 1
#define ELF_64BIT 2

#define LSB 1
#define MSB 2

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7
#define PT_LOOS 0x60000000
#define PT_HIOS 0x6fffffff
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

#define PF_R 0x4
#define PF_W 0x2
#define PF_X 0x1
