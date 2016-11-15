/***************************************************
  Logger                                  <logger.c>
  Authored By: Steven McKeown

  This file controls all the logger for the X-series panel.

  Usage example:
  LOG_PRINT(LOGGER_INFO, "Received data from USB, byte count: %d", byte_count);

  Current supported formatting charaters are %s
  for string and %d for numbers

  SM033116
***************************************************/
#include "logger.h"
#include "usb_comm.h"

// log level
char log_level;

/* START FUNCTION DESCRIPTION *********************
  logger_init                              <logger.c>

  SYNTAX: void logger_init( void );

  DESCRIPTION:
  Initializes the logger.

  RETURN VALUE:  null

  SM033116
  END DESCRIPTION ***********************************/
void logger_init()
{
  log_level = 0;//LOGGER_ASSERT | LOGGER_ERROR | LOGGER_VERBOSE;// | LOGGER_DEBUG;
}

/* START FUNCTION DESCRIPTION *********************
  log_print                                <logger.c>

  SYNTAX: void log_print( char* filename, int line, enum LOGGER_LEVEL level, char *fmt,... );

  DESCRIPTION:
  Stores items that meet thel log_level into the log history and send the item
  to the USBUART

  PARAMETER1: The file that called the function
  PARAMETER2: The line number the call originated from
  PARAMETER3: The type of log
  PARAMETER4: Formatted string
  PARAMETER5: VA list of items to format

  RETURN VALUE:  null

  SM033116
  END DESCRIPTION ***********************************/
void log_print(char* filename, int line, enum LOGGER_LEVEL level, char *fmt, ...)
{
  // only log elements that are in the include list
  if (log_level & level)
  {
    va_list list;
    char *p;
    char *r;
    int e;
    int i = 0;
    char log_builder[200];

    memset(log_builder, 0, 200);

    // print out where we came from
    i = sprintf((char*)&log_builder[0], " [%s][line: %d] ", filename, line);

    // print out severity, maybe change this to string in future
    log_builder[i++] = level;

    // start parsing the log message
    va_start( list, fmt );

    for ( p = fmt ; *p ; ++p )
    {
      if ( *p != '%' )//If simple string
      {
        log_builder[i++] = *p;
      }
      else
      {
        switch ( *++p )
        {
          /* string */
          case 's':
            {
              r = va_arg( list, char * );
              i += sprintf((char*)&log_builder[i], "%s", r);
              continue;
            }

          /* integer */
          case 'd':
            {
              e = va_arg( list, int );

              i += sprintf((char*)&log_builder[i], "%d", e);
              continue;
            }

          default:
            log_builder[i++] = *p;
        }
      }
    }
    
    va_end( list );
    
#ifdef USB_COMMUNICATION
    hmi_message_t msg;
    msg.message_id = MSG_LOG;
    msg.log_message.level = level;
    memset(&msg.log_message.message, 0, sizeof(msg.log_message.message));
    memcpy(&msg.log_message.message, &log_builder, i);
    c_send_message(msg, i + 1);
#endif

    // if an assert fails, keep in infinite loop
    while (level == LOGGER_ASSERT) {}
  }
}

/* START FUNCTION DESCRIPTION *********************
  set_log_level                            <logger.c>

  SYNTAX: void set_log_level( uint8 level );

  DESCRIPTION:
  Sets the level of logging information to display through the UART

  PARAMETER1: The mask value of the new log level

  RETURN VALUE:  null

  SM033116
  END DESCRIPTION ***********************************/
void set_log_level(char level)
{
  log_level = level;
}
