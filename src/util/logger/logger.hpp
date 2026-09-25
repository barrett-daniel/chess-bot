#pragma once

#include <fstream>

#ifdef NDEBUG
    #define LOGGER_ON do {} while(0)
    #define LOGGER_OFF do {} while(0)
    #define LOG(expr) do {} while(0)
    #define LOG_INDENT do {} while(0)
    #define LOG_DEDENT do {} while(0)
    #define LOG_TRACE do {} while(0)
#else
    #define LOGGER_ON Logger::turn_on()
    #define LOGGER_OFF Logger::turn_off()
    #define LOG(expr) Logger::log(expr)
    #define LOG_INDENT Logger::indent()
    #define LOG_DEDENT Logger::dedent()
    #define LOG_TRACE Logger::LogScope log_scope(std::string(__PRETTY_FUNCTION__) + " called")
#endif

class Logger {
    public:
        static void log(const std::string& msg);
        static void init();
        static void turn_off();
        static void turn_on();
        static void indent();
        static void dedent();

    class LogScope {
        public:
            LogScope(const std::string& msg) {
                Logger::log(msg);
                Logger::indent();
            }

            ~LogScope() {
                Logger::dedent();
            }
    };

    private:
        static int counter;
        static std::ofstream out;
        static bool on;
        static std::string indent_buffer;
};