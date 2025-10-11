typedef uint16_t HalfWord;
typedef uint32_t Word;

typedef uint32_t Address_32;
typedef uint32_t Offset_32;

typedef uint64_t Address_64;
typedef uint64_t Offset_64;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    HalfWord e_type;
    HalfWord e_machine;
    Word e_version;
    Address_64 e_entry;
    Offset_64  e_phoff;
    Offset_64 e_shoff;
    Word e_flags;
    HalfWord e_ehsize;
    HalfWord e_phentsize;
    HalfWord e_phnum;
    HalfWord e_shentsize;
    HalfWord e_shnum;
    HalfWord e_shstrndx;
} ElfHeader_64;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    HalfWord e_type;
    HalfWord e_machine;
    Word e_version;
    Address_32 e_entry;
    Offset_32  e_phoff;
    Offset_32 e_shoff;
    Word e_flags;
    HalfWord e_ehsize;
    HalfWord e_phentsize;
    HalfWord e_phnum;
    HalfWord e_shentsize;
    HalfWord e_shnum;
    HalfWord e_shstrndx;
} ElfHeader_32;

typedef struct {
    unsigned char magic[4];
    unsigned char class;
    unsigned char data_order;
    unsigned char version;
    unsigned char os_abi;
    unsigned char abi_version;
    unsigned char padding[7];
} EIdent;
