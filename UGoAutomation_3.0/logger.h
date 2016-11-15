#ifndef __LOGGER_H
#define __LOGGER_H
    
    #include "global.h"
    
    #define MAX_LOG_SIZE 100
    
    // Macro definition for printing information to the logs
    #define LOG_PRINT(level, ...) log_print(__FILE__, __LINE__, level, __VA_ARGS__ )
    
    enum LOGGER_LEVEL {
        // use to log a terrible occurence.
        // A log to assert will drop the system in while loop
        LOGGER_ASSERT = 1,
        
        // use for printing general debugging information
        // Example: entered into function [function_name]
        LOGGER_DEBUG = 2,
        
        // use in try/catch blocks to see what went wrong
        LOGGER_ERROR = 4, 
        
        // use for informing anyone reading the log about events that took place.
        // Example: Successfully retrieved records from database
        LOGGER_INFO = 8,
        
        // use for logging near stack-trace information
        LOGGER_VERBOSE = 16,
        
        // use when information seems wrong, but not really fatal.
        // Example: in the default of a switch statement that is not expected to enter
        LOGGER_WARNING = 32 
    };
    
    void logger_init();
    void log_print(char* filename, int line, enum LOGGER_LEVEL level, char *fmt,...);    
    void set_log_level(char level);
    
#endif

