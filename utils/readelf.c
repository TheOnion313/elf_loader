#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>

#define EI_NIDENT 16

typedef uint16_t HalfWord;
typedef uint32_t Word;

typedef uint32_t Address_32;
typedef uint32_t Offset_32;

typedef uint64_t Address_64;
typedef uint64_t Offset_64;

typedef struct {
    unsigned char e_ident;
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
    unsigned char e_ident;
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
        goto clean;
        return 1;
    } else {
        printf("ELf Header:\n");
        printf("  Magic:   ");
        hexdump_string(e_ident, EI_NIDENT);
        printf("\n");
    }

    clean:
    close(elf_file_fd);

    return 0;
}