
#include "json.hpp"
#include <filesystem>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <istream>
#include <ctime>
#include <cstdio>

#define QUOTE_PATH "./quotes.json"

#define ANSI_RED_BOLD "\u001b[1;31m"
#define ANSI_NORMAL   "\u001b[0m"

#define FATAL_ERROR_STR "QUOTES :: " ANSI_RED_BOLD "FATAL ERROR" ANSI_NORMAL

using nlohmann::json;


int main() {
    // Get current time for logger file name
    struct tm dt;
    time_t timestamp;
    time(&timestamp);
    dt = *localtime(&timestamp);

    std::string log_file_name = ""; 
    log_file_name += 
        std::to_string(dt.tm_year) + "_" + 
        std::to_string(dt.tm_mday) + "_" + 
        std::to_string(dt.tm_mon ) + "_" +
        std::to_string(dt.tm_hour) + "_" +
        std::to_string(dt.tm_min ) + "_" +
        std::to_string(dt.tm_sec ) + ".log";

    std::ofstream O_LOG(log_file_name);
    if (!O_LOG.is_open()) {
        std::cerr << "Failed to open log file: " << log_file_name << std::endl;
        return -1;
    }

    json j;

    std::ifstream I_JSON_FILE;
    I_JSON_FILE.open(QUOTE_PATH);

    if (!I_JSON_FILE.is_open()) {
        std::string ERR_MSG = std::string(FATAL_ERROR_STR) + " :: Quotes I_JSON_FILE FAILED to open.";
        throw std::runtime_error(ERR_MSG);
    }

    I_JSON_FILE.seekg(0, I_JSON_FILE.end);
    std::streamsize I_JSON_FILE_size = I_JSON_FILE.tellg();
    I_JSON_FILE.seekg(0, I_JSON_FILE.beg);
    if (I_JSON_FILE_size <= 0) {
        std::string ERR_MSG = std::string(FATAL_ERROR_STR) + " :: Quotes I_JSON_FILE EMPTY or INVALID.";
        throw std::runtime_error(ERR_MSG);
    }

    std::srand(std::time(nullptr));
    std::streamoff rand_offset = std::rand() % I_JSON_FILE_size;
    I_JSON_FILE.seekg(rand_offset);

    std::string current_line;
    std::getline(I_JSON_FILE, current_line);

    std::string json_string;
    char c;
    while (I_JSON_FILE.get(c)) {
        if (c == '{') {
            json_string += c;
            break;
        }
    }

    int brace_count = 1;
    while (brace_count > 0 && I_JSON_FILE.get(c)) {
        json_string += c;
        if      (c == '{') brace_count++;
        else if (c == '}') brace_count--;
    }

    I_JSON_FILE.close();

    j = json::parse(json_string);

    std::string tx = j["quoteText"];
    std::string auth = j["quoteAuthor"];
    std::cout << '"' << tx << '"' << "  - " << auth << std::endl;

    O_LOG.close();
    // If nothing was logged, delete the log file
    if (std::filesystem::file_size(std::filesystem::path(log_file_name)) <= 0) {
        std::filesystem::remove(std::filesystem::path(log_file_name));
    }

    return 0;
}
