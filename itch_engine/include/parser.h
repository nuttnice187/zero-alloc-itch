#pragma once
#include <string>
#include <cstdint>

class ItchParser {
private:
    int file_descriptor = -1;
    uint8_t* file_memory = nullptr;
    size_t file_size = 0;

public:
    ItchParser() = default;
    ~ItchParser();

    // Initialize memory mapping for ultra-fast, zero-copy reading
    bool initialize(const std::string& filepath);
    
    // Core execution loop to stream through bytes sequentially
    void parse_stream();
    
    void close_file();
};
