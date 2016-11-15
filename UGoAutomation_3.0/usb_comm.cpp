/***************************************************
* USB_COMMUNICATION                   <usb_comm.cpp>
* Authored By: Steven McKeown
*
* The USB communication file deals with serial
* sending and receiving data to external systems.
*
* SM070716
***************************************************/
#include "usb_comm.h"
#include <HardwareSerial.h>

/* Buffer to store read bytes from a frame */
char hmi_in_buffer[255];

/* Keep track of how many bytes for a message read already */
char current_bytes_read = 0;

/* Standard CRC16 tables */
static const unsigned short crc_table[256] = {
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/* START FUNCTION DESCRIPTION *********************
crcsum                               <usb_comm.cpp>
SYNTAX: unsigned short crcsum(const unsigned char*, unsigned long, unsigned short );
DESCRIPTION:
Calculates the crc checksum.
PARAMETER1: The message to calculate the CRC for
PARAMETER2: The length of the message
PARAMETER2: The CRC_INIT
RETURN VALUE:  the calculated checksum
SM070716
END DESCRIPTION ***********************************/
unsigned short crcsum(const unsigned char* message, unsigned long length, unsigned short crc) {
  unsigned long i;

  for(i = 0; i < length; i++) {
    CRC(crc, message[i]);
  }
  return crc;
}

/* START FUNCTION DESCRIPTION *********************
crcverify                            <usb_comm.cpp>
SYNTAX: void crcverify(unsigned char*, unsigned long);
DESCRIPTION:
Checks a meesage to see if the CRC passes
PARAMETER1: The message to check
PARAMETER2: The lenght of the message
RETURN VALUE:  true if passed
SM070716
END DESCRIPTION ***********************************/
int crcverify(const unsigned char* message, unsigned long length) {
  /*
   * Returns true if the last two bytes in a message is the crc of the
   * preceding bytes.
   */
  unsigned short expected;

  expected = crcsum(message, length - 2, CRC_INIT);
  return (expected & 0xff) == message[length - 2] &&
    ((expected >> 8) & 0xff) == message[length - 1];
}

/* START FUNCTION DESCRIPTION *********************
crcappend                            <usb_comm.cpp>
SYNTAX: void crcappend(unsigned char*, unsigned long);
DESCRIPTION:
Appends a message with the CRC16
PARAMETER1: The message to insert the crc into
PARAMETER2: The lenght of the message
RETURN VALUE:  null
SM070716
END DESCRIPTION ***********************************/
void crcappend(unsigned char* message, unsigned long length) {
  unsigned long crc;

  crc = crcsum(message, length, CRC_INIT);
  message[length] = (unsigned char)(crc & 0xff);
  message[length+1] = (unsigned char)((crc >> 8) & 0xff);
}

/* START FUNCTION DESCRIPTION *********************
usb_communication_process            <usb_comm.cpp>
SYNTAX: int usb_communication_process( void );
DESCRIPTION:
Process any incoming messages.
RETURN VALUE: 0 
SM070716
END DESCRIPTION ***********************************/
int usb_communication_process() {
  // read the incoming serial data  
  while (Serial.available() && current_bytes_read < sizeof(hmi_in_buffer)) {
    hmi_in_buffer[current_bytes_read++] = Serial.read();
    if (current_bytes_read > 1) {
      // check for end of frame message, if we have and end of frame leave the
      // next message in the buffer.
      if (hmi_in_buffer[current_bytes_read - 1] == 0x55 && hmi_in_buffer[current_bytes_read - 2] == 0x7F) {
        break;
      }
    }
  }
  
  // TODO: maybe track the state of the current read so that
  // we do not need to continue checking SOF on incremental
  // reads!
  if (current_bytes_read > 0) {
    // check and make sure we are starting a message
    if (hmi_in_buffer[0] ==(START_OF_MESSAGE & 0xFF)) {
      if (current_bytes_read > 1) {
        if (hmi_in_buffer[1] ==  START_OF_MESSAGE >> 8) {
          if (current_bytes_read > 3) {
            // read in the length of the message
            int len = (hmi_in_buffer[2] & 0xFF) + (hmi_in_buffer[3] << 8);
            if (current_bytes_read >= len) {
              // TODO: Calculate CRC16 to make sure that we do not have corruption
#ifdef CRC16
              if (crcverify(((unsigned char*)&hmi_in_buffer), len - 2)) {
#else
              if (true) {
#endif          
                int message_id = (hmi_in_buffer[7] << 8) + (hmi_in_buffer[6] & 0xFF);
                usb_communication_parse_message(message_id, hmi_in_buffer);
                current_bytes_read = 0;
              } else {
              // CRC FAILED!!! should send error message
              }
            } 
          }
        } else {
          // garbage on the serial line. Reset
          current_bytes_read = 0;
        }
      }
    } else {
        // garbage on the serial line. Reset
        current_bytes_read = 0;
    }
  }
  return 0;
}

/* START FUNCTION DESCRIPTION *********************
usb_commuication_create_default_message      <usb_comm.cpp>
SYNTAX: void usb_commuication_create_default_message(short, hmi_message_t*);
DESCRIPTION:
Perpares an empty message for sending through the USB port.
PARAMETER1: The message type to send
PARAMETER2: A pointer to the message to prepare
RETURN VALUE:  null
SM070716
END DESCRIPTION ***********************************/
void usb_commuication_create_default_message(short MessageID, hmi_message_t* message) {
  memset(&(*message), 0, sizeof(hmi_message_t));
  message->start_of_frame = 0x557E;
  message->len = sizeof(hmi_message_t);
  message->src = 0;
  message->dest = 0;
  message->message_id = MessageID | 0x0A00;
}

/* START FUNCTION DESCRIPTION *********************
usb_communication_send_message       <usb_comm.cpp>
SYNTAX: void usb_communication_send_message(hmi_message_t, unsigned int);
DESCRIPTION:
Sends a message through the USB serial port.
PARAMETER1: The message to send
PARAMETER2: The length of the message
RETURN VALUE:  null
SM070716
END DESCRIPTION ***********************************/
void usb_communication_send_message(hmi_message_t msgToSend, unsigned int len) {
  // CALCULATE CRC16!
  // due the endianess, lets explicity set bytes to send
  int i = 0;
  unsigned char hmi_out_buffer[255];
  hmi_out_buffer[i++] = (START_OF_MESSAGE & 0xFF);
  hmi_out_buffer[i++] = START_OF_MESSAGE >> 8;
  hmi_out_buffer[i++] = ((12 + len) & 0xFF);
  hmi_out_buffer[i++] = (12 + len) >> 8;
  hmi_out_buffer[i++] = 0; // src
  hmi_out_buffer[i++] = 0; // dest
  hmi_out_buffer[i++] = msgToSend.message_id & 0xFF;
  hmi_out_buffer[i++] = msgToSend.message_id >> 8;

  for (int j = 0; j < len; j++) {
    hmi_out_buffer[i++] = msgToSend.payload[j];
  }
  
  crcappend(((unsigned char*)&hmi_out_buffer), i);

  i = i + 2; // offset for CRC
  
  // END OF FRAME
  hmi_out_buffer[i++] = 0x7F; // EOF
  hmi_out_buffer[i++] = 0x55; // EOF

  Serial.write(hmi_out_buffer, i);
  Serial.write("\n");
}

void usb_communication_parse_message(short message_id, char* buffer){
  hmi_message_t replyMessage;
     
  switch (message_id) {
    case MSG_AUTO_CYCLE:
      mediator_send_message(MEDIATOR_AUTO_CYCLE_START, (char*)"");
      usb_communication_send_message(replyMessage, sizeof(replyMessage.auto_cycle));
    break;
  
    case MSG_SANITIZE_BLENDER:
      mediator_send_message(MEDIATOR_CLEAN_CYCLE_START, (char*)"");
      break;

    case MSG_INITIALIZE:
      mediator_send_message(MEDIATOR_INITIALIZE, (char*)"");
      break;
    case MSG_MACHINE_STOP:
      mediator_send_message(MEDIATOR_STOP_REQUEST, (char*)"");
      break;
    case MSG_TOGGLE_ACTUATOR_STATE:
      digitalWrite(buffer[8], !digitalRead(buffer[8]));
      break;

    case MSG_REBLEND:
      mediator_send_message(MEDIATOR_REBLEND, (char*)"");
      break;

    case MSG_JOG_TOP:
      mediator_send_message(MEDIATOR_JOG_TOP,(char*)"");
      break;

    case MSG_JOG_BOTTOM:
      mediator_send_message(MEDIATOR_JOG_BOTTOM, (char*)"");
      break;

    case MSG_MOVE_UP:
          mediator_send_message(MEDIATOR_MOVE_UP, (char*)"");
    break;
    
    case MSG_MOVE_DOWN:
          mediator_send_message(MEDIATOR_MOVE_DOWN, (char*)"");
    break;
    case MSG_DISABLE_KEYPAD:
          mediator_send_message(MEDIATOR_DISABLE_KEYPAD, (char*)"");
    break;
    default:
      // NOT IMPLEMENTED YET!
    break;
  }
  
}

void c_send_message(hmi_message_t msg, unsigned int size) {
  usb_communication_send_message(msg, size);
}

void send_status(char* message){
    hmi_message_t msg;
    msg.message_id = MSG_STATUS;
    memset(&msg.status_message.message, 0, sizeof(msg.status_message.message));
    memcpy(&msg.status_message.message, message, strlen(message));
    usb_communication_send_message(msg, strlen(message));
}
