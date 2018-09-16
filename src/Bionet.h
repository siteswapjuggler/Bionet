//  Bionet.h
//  Created by Sylvain Garnavault on 19/11/2017.

#ifndef Bionet_h
#define Bionet_h

#define BIONET_INTERFRAME 5
#define BIONET_FRAMERATE  50

#define WRITE_OK      0xA5
#define WRITE_NOK	  0x33
#define WRITE_BIT     0b10000000
#define QUESTION_MASK 0b01111111

#define INIT  0x00
#define RUN   0x01
#define STOP  0x02
#define ERROR 0x03

#include <Arduino.h>


class _bionet {
    private :
        HardwareSerial * _serial;
        unsigned long _previousTimer;
        void flushSerialBuffer();
		void sendState(uint8_t);
    
    public :
        _bionet(HardwareSerial*);
        void begin();
        bool update();
        void sendRun();
		void sendInit();
		void sendStop();
		void sendError();
        bool readByteVar(unsigned char, unsigned char*);
        bool readUIntVar(unsigned char, unsigned int*);
        bool writeByteVar(unsigned char, unsigned char);
        bool writeUIntVar(unsigned char, unsigned int);
};

typedef _bionet Bionet;

#endif /* Bionet_h */
