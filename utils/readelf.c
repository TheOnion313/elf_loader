#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#include "../consts/value_tables.h"
#include "../consts/types.h"

int hexdump_string(unsigned char *str, size_t str_size) {
    for (size_t j = 0; j < str_size;) {
        size_t row_size = (str_size - j < 16) ? str_size - j : 16;
        for (size_t i = 0; i < row_size; i++) {
            printf("%02x ", str[j + i]);

        }
        printf("[");
        for (size_t i = 0; i < row_size; i++) {
            char c = str[j + i];
            if (isprint(c)) {
                printf("%c", c);
            } else {
                printf(".");
            }
        }
        j += row_size;
        printf("]\n");
        }
}

int main(int argc, char *argv[]) {
    int ret = 0;
    if (argc != 2) {
        printf("Usage: readelf elf-file\n");

        return 1;
    }
    const char *elf_file_path = argv[1];

    const int elf_file_fd = open(elf_file_path, O_RDONLY);

    unsigned char e_ident[EI_NIDENT];
    size_t bytes_read = read(elf_file_fd, e_ident, EI_NIDENT);
    if (bytes_read != EI_NIDENT) {
        printf("Failed to read ELF header\n");
        ret = 1;
        goto clean;
    }

    printf("ELF Header:\n");
    printf("  Magic:   ");
    hexdump_string(e_ident, EI_NIDENT);
    printf("\n");

    const EIdent *ident = (EIdent*)e_ident;
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

    char *os_abi;
    if (ident->os_abi >= ELFOSABI_DEFINED_VALUES) {
        os_abi = "Architecture-specific value";
    } else {
        os_abi = os_abi_index[ident->os_abi];
    }
    printf("  OS/ABI:                            %s\n", os_abi);
    printf("  ABI Version:                       %i\n", ident->abi_version);

    void *elf_header;
    size_t rest_of_header;
    if (ident->class == ELF_64BIT) {
        elf_header = (ElfHeader_64*) malloc(sizeof(ElfHeader_64));
        rest_of_header = sizeof(ElfHeader_64) - EI_NIDENT;
    } else {
        elf_header = (ElfHeader_32*) malloc(sizeof(ElfHeader_32));
        rest_of_header = sizeof(ElfHeader_32) - EI_NIDENT;
    }
    if (!elf_header) {
        printf("Failed to allocate memory for ELF header");
        ret = 1;
        goto clean;
    }

    bytes_read = read(elf_file_fd, elf_header + EI_NIDENT, rest_of_header);
    if (bytes_read != rest_of_header) {
        printf("Failed to read ELF header, read %ld bytes\n", bytes_read);
        ret = 1;
        goto clean;
    }
    memcpy(elf_header, e_ident, EI_NIDENT);
    // printf("ELF Header:\n");
    // hexdump_string(elf_header + EI_NIDENT, rest_of_header);
    
    HalfWord type;
    if (ident->class == ELF_64BIT) {
        type = ((ElfHeader_64*)elf_header)->e_type;
    } else {
        type = ((ElfHeader_32*)elf_header)->e_type;        
    }
    char *type_val;
    switch(type) {
        case ET_NONE:
            type_val = "(NONE) No file type";
            break;
        case ET_REL:
            type_val = "(REL) Relocatable file";
            break;
        case ET_EXEC:
            type_val = "(EXEC) Non PIE Executable file";
            break;
        case ET_DYN:
            type_val = "(DYN) Shared object file or PIE Executable";
            break;
        case ET_CORE:
            type_val = "(CORE) Core file";
            break;
        default:
            type_val = "Undefined file type";
            break;
    }
    printf("  Type:                              %s\n", type_val);
    char *machine_val = "Invalid";
    HalfWord machine;
    if (ident->class == ELF_64BIT) {
        machine = ((ElfHeader_64*)elf_header)->e_machine;
    } else {
        machine = ((ElfHeader_32*)elf_header)->e_machine;        
    }
    if (machine < ELFMACHINE_DEFINED_VALUES) {
        machine_val = machine_index[machine];
    }
    printf("  Machine:                           %s\n", machine_val);

    clean:
    if (elf_file_fd) {
        close(elf_file_fd);
    }
    if (elf_header) {
        free(elf_header);
    }

    return ret;
}