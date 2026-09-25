#include "logger.hpp"

#include <chrono>

int Logger::counter = 0;
std::ofstream Logger::out("log.txt");
bool Logger::on = false;
std::string Logger::indent_buffer = "";

void Logger::log(const std::string& msg) {
    if(on) {
        using namespace std::chrono;

        // Get current time point
        auto now = system_clock::now();
        auto now_time_t = system_clock::to_time_t(now);
        auto now_us = duration_cast<microseconds>(now.time_since_epoch()) % 1000000;

        // Format time as HH:MM:SS.mmm
        std::tm now_tm;
        #ifdef _WIN32
            localtime_s(&now_tm, &now_time_t);
        #else
            localtime_r(&now_time_t, &now_tm);
        #endif

        out << indent_buffer << "[" << counter << "] "
            << std::put_time(&now_tm, "%H:%M:%S")
            << '.' << std::setfill('0') << std::setw(6) << now_us.count()
            << " - " << msg << std::endl;

        counter++;
    }
}

void Logger::turn_on() {
    on = true;
}

void Logger::turn_off() {
    on = false;
}

void Logger::indent() {
    indent_buffer += '\t';
}

void Logger::dedent() {
    if(!indent_buffer.empty()) indent_buffer.pop_back();
}