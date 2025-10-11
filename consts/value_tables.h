
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