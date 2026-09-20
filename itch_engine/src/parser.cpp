#include "parser.h"
#include "order_book.h"
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <endian.h> // Provides blazing-fast Linux native be32toh / be64toh macros

// Global allocation-free order book instance
LimitOrderBook order_book;

bool ItchParser::initialize(const std::string& filepath) {
    file_descriptor = open(filepath.c_str(), O_RDONLY);
    if (file_descriptor == -1) {
        std::cerr << "[-] Failed to open source data file." << std::endl;
        return false;
    }

    struct stat file_properties;
    if (fstat(file_descriptor, &file_properties) == -1) {
        close_file();
        return false;
    }
    file_size = file_properties.st_size;

    file_memory = static_cast<uint8_t*>(mmap(
        nullptr, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0
    ));

    if (file_memory == MAP_FAILED) {
        std::cerr << "[-] Kernel memory mapping failed." << std::endl;
        file_memory = nullptr;
        close_file();
        return false;
    }

    madvise(file_memory, file_size, MADV_SEQUENTIAL);
    std::cout << "[+] Memory mapped " << file_size << " bytes successfully." << std::endl;
    return true;
}

void ItchParser::parse_stream() {
    if (!file_memory) return;

    size_t offset = 0;
    
    while (offset < file_size) {
        // 1. Decode Big-Endian message length using explicit bit manipulations
        uint16_t message_length = (file_memory[offset] << 8) | file_memory[offset + 1];
        offset += 2;

        char message_type = static_cast<char>(file_memory[offset]);

        // 2. Isolate and parse an 'Add Order' packet (Type 'A')
        if (message_type == 'A') {
            // Map the raw byte frame pointers directly onto our packed memory layout struct
            const OrderAddMessage* raw_msg = reinterpret_cast<const OrderAddMessage*>(&file_memory[offset]);

            // Create a local, clean representation holding decoded values
            DecodedOrder order;
            order.timestamp   = be64toh(raw_msg->timestamp); // 64-bit Big-Endian to Host
            order.order_id    = be64toh(raw_msg->order_reference_number);
            order.shares      = be32toh(raw_msg->shares);    // 32-bit Big-Endian to Host
            order.price       = be32toh(raw_msg->price);
            order.side        = raw_msg->buy_sell_indicator;

            // Route our clean data straight into our allocation-free order book matrix
            order_book.handle_add_order(order);
        }

        offset += message_length;
    }
}

void ItchParser::close_file() {
    if (file_memory) { munmap(file_memory, file_size); file_memory = nullptr; }
    if (file_descriptor != -1) { close(file_descriptor); file_descriptor = -1; }
}

ItchParser::~ItchParser() { close_file(); }