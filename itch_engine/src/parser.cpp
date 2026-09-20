#include "parser.h"
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

bool ItchParser::initialize(const std::string& filepath) {
    // Open using raw Linux system calls for unbuffered speed
    file_descriptor = open(filepath.c_str(), O_RDONLY);
    if (file_descriptor == -1) {
        std::cerr << "[-] Failed to open source data file." << std::endl;
        return false;
    }

    // Retrieve file size to allocate virtual addresses accurately
    struct stat file_properties;
    if (fstat(file_descriptor, &file_properties) == -1) {
        close_file();
        return false;
    }
    file_size = file_properties.st_size;

    // Execute memory map: Map file directly into execution memory space
    file_memory = static_cast<uint8_t*>(mmap(
        nullptr, 
        file_size, 
        PROT_READ, 
        MAP_SHARED, 
        file_descriptor, 
        0
    ));

    if (file_memory == MAP_FAILED) {
        std::cerr << "[-] Kernel memory mapping failed." << std::endl;
        file_memory = nullptr;
        close_file();
        return false;
    }

    // Advise the kernel that we plan to read this sequentially to trigger proactive caching
    madvise(file_memory, file_size, MADV_SEQUENTIAL);
    
    std::cout << "[+] Memory mapped " << file_size << " bytes successfully." << std::endl;
    return true;
}

void ItchParser::parse_stream() {
    if (!file_memory) return;

    size_t offset = 0;
    
    // Linear scan through data mimicking low-latency processing loops
    while (offset < file_size) {
        // NASDAQ ITCH protocol prefixes every packet with a 2-byte Big-Endian length indicator
        uint16_t message_length = (file_memory[offset] << 8) | file_memory[offset + 1];
        offset += 2;

        // Isolate the specific message type byte frame
        char message_type = static_cast<char>(file_memory[offset]);

        // Process message blocks here using your zero-allocation pools...
        // For example: if (message_type == 'A') handle_add_order(&file_memory[offset]);

        offset += message_length;
    }
}

void ItchParser::close_file() {
    if (file_memory) {
        munmap(file_memory, file_size);
        file_memory = nullptr;
    }
    if (file_descriptor != -1) {
        close(file_descriptor);
        file_descriptor = -1;
    }
}

ItchParser::~ItchParser() {
    close_file();
}
