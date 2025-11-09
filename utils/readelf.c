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

#define GET_HEADER_VALUE_ADDR(elf_header, value) (is_64 ? &((ElfHeader_64 *)elf_header)->value : &((ElfHeader_32 *)elf_header)->value)
#define GET_HEADER_VALUE(elf_header, value) (is_64 ? ((ElfHeader_64 *)elf_header)->value : ((ElfHeader_32 *)elf_header)->value)

#define GET_PROGRAM_HEADER_VALUE_ADDR(elf_program_header, value) is_64 ? &((ProgramHeader_64 *)elf_program_header)->value : &((ProgramHeader_32 *)elf_program_header)->value
#define GET_PROGRAM_HEADER_VALUE(elf_program_header, value) (is_64 ? ((ProgramHeader_64 *)elf_program_header)->value : ((ProgramHeader_32 *)elf_program_header)->value)

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

int load_elf_header(const int elf_file_fd, void **header_ptr) {
    unsigned char e_ident[EI_NIDENT];
    size_t bytes_read = read(elf_file_fd, e_ident, EI_NIDENT);
    if (bytes_read != EI_NIDENT) {
        printf("Failed to read ELF header\n");
        return 1;
    }
    const EIdent *ident = (EIdent*)e_ident;

    if (ident->class == ELF_64BIT) {
        ElfHeader_64 *elf_header = malloc(sizeof(ElfHeader_64));
        const size_t rest_of_header = sizeof(ElfHeader_64) - EI_NIDENT;
        if (!elf_header) {
            printf("Failed to allocate memory for ELF header");
            return 1;
        }

        bytes_read = read(elf_file_fd, ((unsigned char *)elf_header) + EI_NIDENT, rest_of_header);
        if (bytes_read != rest_of_header) {
            printf("Failed to read ELF header, read %ld bytes\n", bytes_read);
            return 1;
        }
        memcpy(elf_header, e_ident, EI_NIDENT);
        *header_ptr = elf_header;
    } else {
        ElfHeader_32 *elf_header = malloc(sizeof(ElfHeader_64));
        const size_t rest_of_header = sizeof(ElfHeader_64) - EI_NIDENT;
        if (!elf_header) {
            printf("Failed to allocate memory for ELF header");
            return 1;
        }

        bytes_read = read(elf_file_fd, ((unsigned char *)elf_header) + EI_NIDENT, rest_of_header);
        if (bytes_read != rest_of_header) {
            printf("Failed to read ELF header, read %ld bytes\n", bytes_read);
            return 1;
        }
        memcpy(elf_header, e_ident, EI_NIDENT);
        *header_ptr = elf_header;
    }

    return 0;
}

int load_program_header(const int elf_file_fd, off_t header_offset, int is_64, void **header_ptr) {
    int ret = 0;
    void *program_header = NULL;
    off_t off = lseek(elf_file_fd, header_offset, SEEK_SET);
    if (off == -1) {
        printf("Failed to seek offset %jd for program header\n", header_offset);
        return 1;
    }

    size_t header_size = is_64 ? sizeof(ProgramHeader_64) : sizeof(ProgramHeader_32);
    program_header = malloc(header_size);
    if (!program_header) {
        printf("Failed to allocate memory for program header at offset %jd with size %zu\n", header_offset, header_size);
        return 1;
    }

    size_t bytes_read = read(elf_file_fd, program_header, header_size);
    if (bytes_read != header_size) {
        printf("Failed to read %zu bytes from file at offset %jd, read %zu bytes\n", header_size, header_offset, bytes_read);
        return 1;
    }

    *header_ptr = program_header;
    return ret;
}

int is_64bit(void *elf_header) {
    unsigned char elf_class = ((EIdent*)&(((ElfHeader_64 *)elf_header)->e_ident))->class;
    switch(elf_class) {
        case ELF_64BIT:
            return 1;
        case ELF_32BIT:
            return 0;
        default:
            return -1;
    }
}

unsigned char get_data_order(void *elf_header) {
    return ((EIdent*)&(((ElfHeader_64 *)elf_header)->e_ident))->data_order;
}

unsigned char get_version(void *elf_header) {
    return ((EIdent*)&(((ElfHeader_64 *)elf_header)->e_ident))->version;
}

unsigned char get_os_abi(void *elf_header) {
    return ((EIdent*)&(((ElfHeader_64 *)elf_header)->e_ident))->os_abi;
}

unsigned char get_abi_version(void *elf_header) {
    return ((EIdent*)&(((ElfHeader_64 *)elf_header)->e_ident))->abi_version;
}

void print_offset(void *offset, char is_64) {
    if (is_64) {
        printf("%lx", *(Offset_64 *)offset);
    } else {
        printf("%x", *(Offset_32 *)offset);
    }
}

void print_ph_offset(void *offset, char is_64) {
    if (is_64) {
        printf("%016lx", *(Offset_64 *)offset);
    } else {
        printf("%016x", *(Offset_32 *)offset);
    }
}

void print_address(void *address, char is_64) {
    if (is_64) {
        printf("%ld", *(Address_64 *)address);
    } else {
        printf("%d", *(Address_32 *)address);
    }
}


int print_elf_header(const int elf_file_fd) {
    int ret = 0;

    void *elf_header = NULL;
    if (load_elf_header(elf_file_fd, &elf_header)) {
        ret = 1;
        goto clean;
    }
    printf("ELF Header:\n");
    printf("  Magic:   ");
    hexdump_string((char *)&(((ElfHeader_64 *)elf_header)->e_ident), EI_NIDENT);
    printf("\n");

    int is_64 = is_64bit(elf_header);
    if (is_64 == -1) {
        printf("\nError: Invalid ELF Class\n");
        ret = 1;
        goto clean;
    }
    printf("  Class:                             %s\n", is_64 ? "ELF64" : "ELF32");
    char *data;
    switch(get_data_order(elf_header)) {
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
    printf("  Version:                           %i\n", get_version(elf_header));

    char *os_abi;
    if (get_os_abi(elf_header) >= ELFOSABI_DEFINED_VALUES) {
        os_abi = "Architecture-specific value";
    } else {
        os_abi = os_abi_index[get_os_abi(elf_header)];
    }
    printf("  OS/ABI:                            %s\n", os_abi);
    printf("  ABI Version:                       %i\n", get_abi_version(elf_header));

    HalfWord type = GET_HEADER_VALUE(elf_header, e_type);
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
    HalfWord machine = GET_HEADER_VALUE(elf_header, e_machine);
    if (machine < ELFMACHINE_DEFINED_VALUES) {
        machine_val = machine_index[machine];
    }
    printf("  Machine:                           %s\n", machine_val);
    printf("  Version:                           0x%x\n", GET_HEADER_VALUE(elf_header, e_version));
    printf("  Entry point address:               0x");
    print_offset((void *)GET_HEADER_VALUE_ADDR(elf_header, e_entry), is_64);
    printf("\n");
    printf("  Start of program headers:          ");
    print_address(GET_HEADER_VALUE_ADDR(elf_header, e_phoff), is_64);
    printf(" (bytes into file)\n");
    printf("  Start of section headers:          ");
    print_address(GET_HEADER_VALUE_ADDR(elf_header, e_shoff), is_64);
    printf(" (bytes into file)\n");
    printf("  Flags:                             0x%x\n", GET_HEADER_VALUE(elf_header, e_flags));
    printf("  Size of this header:               %d (bytes)\n", GET_HEADER_VALUE(elf_header, e_ehsize));
    printf("  Size of program headers:           %d (bytes)\n", GET_HEADER_VALUE(elf_header, e_phentsize));
    printf("  Number of program headers:         %d\n", GET_HEADER_VALUE(elf_header, e_phnum));
    printf("  Size of section headers:           %d (bytes)\n", GET_HEADER_VALUE(elf_header, e_shentsize));
    printf("  Number of section headers:         %d\n", GET_HEADER_VALUE(elf_header, e_shnum));
    printf("  Section header string table index: %d\n", GET_HEADER_VALUE(elf_header, e_shstrndx));

    clean:
    if (elf_header) {
        free(elf_header);
    }

    return ret;
}

int print_elf_program_header(void *program_header, int is_64) {
    Word type = GET_PROGRAM_HEADER_VALUE(program_header, p_type);
    switch(type) {
        case PT_NULL:
            printf("  NULL           ");
            break;
        case PT_LOAD:
            printf("  LOAD           ");
            break;
        case PT_DYNAMIC:
            printf("  DYNAMIC        ");
            break;
        case PT_INTERP:
            printf("  INTERP         ");
            break;
        case PT_NOTE:
            printf("  NOTE           ");
            break;
        case PT_SHLIB:
            printf("  SHLIB          ");
            break;
        case PT_PHDR:
            printf("  PHDR           ");
            break;
        case PT_TLS:
            printf("  TLS            ");
            break;
        default:
            if (type <= PT_HIOS && type >= PT_LOOS) {
                printf("  OS spec type   ");
            } else if (type <= PT_HIPROC && type >= PT_LOPROC) {
                printf("  Proc spec type ");
            } else {
                printf("\nInvalid type: %u\n", type);
                return 1;
            }
    }
    printf("0x");
    print_ph_offset(GET_PROGRAM_HEADER_VALUE_ADDR(program_header, p_offset), is_64);
    printf(" 0x");
    print_ph_offset(GET_PROGRAM_HEADER_VALUE_ADDR(program_header, p_vaddr), is_64);
    printf(" 0x");
    print_ph_offset(GET_PROGRAM_HEADER_VALUE_ADDR(program_header, p_paddr), is_64);
    printf("\n                 0x");
    print_ph_offset(GET_PROGRAM_HEADER_VALUE_ADDR(program_header, p_filesz), is_64);
    printf(" 0x");
    print_ph_offset(GET_PROGRAM_HEADER_VALUE_ADDR(program_header, p_memsz), is_64);
    Word flags = GET_PROGRAM_HEADER_VALUE(program_header, p_flags);
    printf("  %c%c%c    0x%x\n", (flags & PF_R) == PF_R ? 'R' : ' ', (flags & PF_W) == PF_W ? 'W' : ' ', (flags & PF_X) == PF_X ? 'E' : ' ', GET_PROGRAM_HEADER_VALUE(program_header, p_align));
}

int print_elf_program_headers(const int elf_file_fd) {
    int ret = 0;
    void *elf_header = NULL, *program_header = NULL;
    char *interpreter_path = NULL;

    if (load_elf_header(elf_file_fd, &elf_header)) {
        ret = 1;
        goto clean;
    }
    int is_64 = is_64bit(elf_header);
    if (is_64 == -1) {
        printf("\nError: Invalid ELF Class\n");
        ret = 1;
        goto clean;
    }

    HalfWord type = GET_HEADER_VALUE(elf_header, e_type);
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
    printf("Elf file type is %s\nEntry point 0x", type_val);
    print_offset((void *)GET_HEADER_VALUE_ADDR(elf_header, e_entry), is_64);
    printf("\nThere are %d program headers, starting at offset ", GET_HEADER_VALUE(elf_header, e_phnum)); 
    print_address(GET_HEADER_VALUE_ADDR(elf_header, e_phoff), is_64);
    printf("\n\nProgram Headers:\n  Type           Offset             VirtAddr           PhysAddr\n                 FileSiz            MemSiz            Flags  Align\n");

    size_t offset = 0;
    for (size_t i = 0; i < GET_HEADER_VALUE(elf_header, e_phnum); i++) {
        int suc = load_program_header(elf_file_fd, ((void *)GET_HEADER_VALUE(elf_header, e_phoff)) + offset, is_64, &program_header);
        if (suc == 1) {
            printf("Failed to load program header\n");
            ret = 1;
            goto clean;
        }
        print_elf_program_header(program_header, is_64);
        if (GET_PROGRAM_HEADER_VALUE(program_header, p_type) == PT_INTERP) {

            off_t off = lseek(elf_file_fd, GET_PROGRAM_HEADER_VALUE(program_header, p_offset), SEEK_SET);
            if (off == -1) {
                printf("Failed to seek offset %jd for interpreter\n", GET_PROGRAM_HEADER_VALUE(program_header, p_offset));
                ret = 1;
                goto clean;
            }
            size_t interp_len = GET_PROGRAM_HEADER_VALUE(program_header, p_filesz);
            interpreter_path = malloc(interp_len);
            if (!interpreter_path) {
                printf("Failed to allocate memory for interpeter path of size %zu\n", interp_len);
                ret = 1;
                goto clean;
            }
            size_t suc = read(elf_file_fd, interpreter_path, interp_len);
            if (suc != interp_len) {
                printf("Failed to read interpreter path from file.\n");
                ret = 1;
                goto clean;
            }
            printf("      [Requesting program interpreter: %s]\n", interpreter_path);
            free(interpreter_path);
            interpreter_path = NULL;
        }
        free(program_header);
        program_header = NULL;
        offset += GET_HEADER_VALUE(elf_header, e_phentsize);
    }
    clean:
        if (elf_header) {
            free(elf_header);
        }
        if (program_header) {
            free(program_header);
        }
        if (interpreter_path) {
            free(interpreter_path);
        }

    return ret;
}

int print_elf_section_headers(const int elf_file_fd) {
    int ret = 0;
    void *elf_header = NULL, *program_header = NULL;
    if (load_elf_header(elf_file_fd, &elf_header)) {
        ret = 1;
        goto clean;
    }
    int is_64 = is_64bit(elf_header);
    if (is_64 == -1) {
        printf("\nError: Invalid ELF Class\n");
        ret = 1;
        goto clean;
    }
    int sec_headers_num = GET_HEADER_VALUE(elf_header, e_shnum);
    printf("There are %d section headers, starting at offset 0x", sec_headers_num);
    print_offset((void *)GET_HEADER_VALUE_ADDR(elf_header, e_shoff), is_64);
    printf(":\n\nSection Headers:\n");
    printf("  [Nr] Name              Type             Address           Offset\n       Size              EntSize          Flags  Link  Info  Align\n");

    clean:
        if (elf_header) {
            free(elf_header);
        }

    return ret;
}

int main(int argc, char *argv[]) {
    int elf_file_fd, ret = 0;
    if (argc != 3) {
        printf("Usage: readelf <mode> elf-file\n");
        ret = 1;
        goto clean;
    }
    const char *elf_file_path = argv[2];
    const char *mode = argv[1];
    if (mode[0] != '-') {
        printf("Unknown mode: %s\n", mode);
        ret = 1;
        goto clean;
    }
    elf_file_fd = open(elf_file_path, O_RDONLY);
    switch(mode[1]) {
        case 'h':
            return print_elf_header(elf_file_fd);
        case 'l':
            return print_elf_program_headers(elf_file_fd);
        case 'S':
            return print_elf_section_headers(elf_file_fd);
        default:
            printf("Unknown mode: %s\n", mode);
    }
    clean:
        if (elf_file_fd) {
            close(elf_file_fd);
        }
        return ret;
}