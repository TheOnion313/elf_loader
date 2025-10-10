#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>

#define EI_NIDENT 16

#define ELF_MAGIC 0x7f454c46 # 0x7f + "ELF"

#define ELF_32BIT 1
#define ELF_64BIT 2

#define LSB 1
#define MSB 2

#define ELFOSABI_NONE 0
#define ELFOSABI_HPUX 1
#define ELFOSABI_NETBSD 2
#define ELFOSABI_LINUX 3
#define ELFOSABI_SOLARIS 6
#define ELFOSABI_AIX 7
#define ELFOSABI_IRIX 8
#define ELFOSABI_FREEBSD 9
#define ELFOSABI_TRU64 10
#define ELFOSABI_MODESTO 11
#define ELFOSABI_OPENBSD 12
#define ELFOSABI_OPENVMS 13
#define ELFOSABI_NSK 14

#define ELFOSABI_DEFINED_VALUES 15

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

int hexdump_string(char *str, size_t str_size) {
    for (size_t i = 0; i < str_size; i++) {
        printf("%02x%s", str[i], i == str_size - 1 ? "" : " ");
    }
    printf(" [");
    for (size_t i = 0; i < str_size; i++) {
        char c = str[i];
        if (isprint(c)) {
            printf("%c", c);
        } else {
            printf(".");
        }
    }
    printf("]");

}

int main(int argc, char *argv[]) {
    int ret = 0;
    if (argc != 2) {
        printf("Usage: readelf elf-file\n");

        return 1;
    }
    char *elf_file_path = argv[1];

    int elf_file_fd = open(elf_file_path, O_RDONLY);

    unsigned char e_ident[EI_NIDENT];
    size_t bytes_read = read(elf_file_fd, e_ident, EI_NIDENT);
    if (bytes_read != EI_NIDENT) {
        printf("Failed to read ELF header\n");
        ret = 1;
        goto clean;
    }

    printf("ELf Header:\n");
    printf("  Magic:   ");
    hexdump_string(e_ident, EI_NIDENT);
    printf("\n");

    EIdent *ident = (EIdent*)e_ident;
    char *class;
    switch(ident->class) {
        case ELF_64BIT:
            class = "ELF64";
            break;
        case ELF_32BIT:
            class = "ELF32";
            break;
        default:
            printf("\nError: Invalid ELF Class\n");
            ret = 1;
            goto clean;
    }
    printf("  Class:                             %s\n", class);

    char *data;
    switch(ident->data_order) {
        case LSB:
            data = "LSB";
            break;
        case MSB:
            data = "MSB";
            break;
        default:
            printf("\nError: Invalid Data format\n");
            ret = 1;
            goto clean;
    }
    printf("  Data:                              %s\n", data);
    printf("  Version:                           %i\n", ident->version);


    char *os_abi_index[ELFOSABI_DEFINED_VALUES];

    os_abi_index[ELFOSABI_NONE] = "No extensions or unspecified";
    os_abi_index[ELFOSABI_HPUX] = "Hewlett-Packard HP-UX";
    os_abi_index[ELFOSABI_NETBSD] = "NetBSD";
    os_abi_index[ELFOSABI_LINUX] = "Linux";
    os_abi_index[ELFOSABI_SOLARIS] = "Sun Solaris";
    os_abi_index[ELFOSABI_AIX] = "AIX";
    os_abi_index[ELFOSABI_IRIX] = "IRIX";
    os_abi_index[ELFOSABI_FREEBSD] = "FreeBSD";
    os_abi_index[ELFOSABI_TRU64] = "Compaq TRU64 UNIX";
    os_abi_index[ELFOSABI_MODESTO] = "Novell Modesto";
    os_abi_index[ELFOSABI_OPENBSD] = "Open BSD";
    os_abi_index[ELFOSABI_OPENVMS] = "Open VMS";
    os_abi_index[ELFOSABI_NSK] = "Hewlett-Packard Non-Stop Kernel";
    char *os_abi;
    if (ident->os_abi >= ELFOSABI_DEFINED_VALUES) {
        os_abi = "Architecture-specific value";
    } else {
        os_abi = os_abi_index[ident->os_abi];
    }
    printf("  OS/ABI:                            %s\n", os_abi);
    printf("  ABI Version:                       %i\n", ident->abi_version);

    clean:
    close(elf_file_fd);

    return ret;
}