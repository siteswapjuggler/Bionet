//  Bionet.cpp
//  Created by Sylvain Garnavault on 19/11/2017.

#include "Bionet.h"

/******************************
 * class constructor          *
 ******************************/

_bionet::_bionet(HardwareSerial * serial) {
    _serial = serial;
}


/******************************
 * bionet begin setup         *
 ******************************/


void _bionet::begin() {
    _serial->begin(19200);
    _previousTimer = 0;
}


/*******************************
 * bionet framerate management *
 *******************************/

bool _bionet::update() {
    unsigned long timer = millis();
    if (timer - _previousTimer > BIONET_FRAMERATE) {
        _previousTimer = timer;
        return true;
    }
    return false;
}


/******************************
 * read a byte type variable  *
 ******************************/

bool _bionet::readByteVar(unsigned char VarNumber, unsigned char * ByteRead) {
    
    int incomingByte;
    int ReceiveData;
    int ReceiveCheckSum;
    unsigned char CheckSum;
    boolean ReadError=false;
    
    _serial->write(VarNumber & QUESTION_MASK);
    delay(BIONET_INTERFRAME);
    
    // receive the byte
    incomingByte = _serial->read();      // the Var Number of question
    if (incomingByte != (int)VarNumber) ReadError=true;
    
    incomingByte = _serial->read();      // the Var Number of answer
    if (incomingByte != (int)VarNumber) ReadError=true;
    
    ReceiveData     = _serial->read();    // data
    ReceiveCheckSum = _serial->read();    // checksum
    
    CheckSum =(unsigned char)(~(VarNumber+ReceiveData));
    
    if (ReceiveCheckSum != CheckSum) {
        // checksum error
        ReadError=true;
        *ByteRead=0;
    }
    
    else {
        // return the Byte value
        *ByteRead=(unsigned char)ReceiveData;
    }
    
    flushSerialBuffer();
    return (ReadError);
}


/******************************
 * read a uint type variable  *
 ******************************/

bool _bionet::readUIntVar(unsigned char VarNumber, unsigned int * UIntRead) {
    int incomingByte;
    int ReceiveData;
    int ReceiveDataHigh;
    int ReceiveCheckSum;
    unsigned char CheckSum;
    boolean ReadError=false;
    
    _serial->write(VarNumber & QUESTION_MASK);
    delay(BIONET_INTERFRAME);
    
    // receive the byte
    incomingByte = _serial->read(); // the VarNumber of question
    if (incomingByte != VarNumber) ReadError=true;
    
    incomingByte = _serial->read(); // the VarNumber of answer
    if (incomingByte != VarNumber) ReadError=true;
    
    ReceiveData     = _serial->read();  // data low
    ReceiveDataHigh = _serial->read();  // data high
    ReceiveCheckSum = _serial->read();  // checksum
    
    CheckSum =(unsigned char)(~(VarNumber + ReceiveData + ReceiveDataHigh));
    if (ReceiveCheckSum != CheckSum) {
        ReadError=true;
        *UIntRead=0;
    }
    else {
        *UIntRead=(unsigned int)(ReceiveDataHigh << 8 | ReceiveData);
    }

    flushSerialBuffer();
    return (ReadError);
}


/******************************
 * write a type byte variable *
 ******************************/

bool _bionet::writeByteVar(unsigned char VarNumber, unsigned char Byte) {
    int           incomingByte;
    unsigned char CheckSum;
    bool          ReadError=false;
    
    VarNumber = VarNumber | WRITE_BIT;
    
    CheckSum =  VarNumber;
    CheckSum += Byte;
    CheckSum =~ CheckSum;
    
    _serial->write(VarNumber);
    _serial->write(Byte);
    _serial->write(CheckSum);
    delay(BIONET_INTERFRAME);
    
    incomingByte=_serial->read(); // the VarNumber of question
    if (incomingByte != (int)VarNumber) ReadError=true;
    
    incomingByte=_serial->read(); // the Byte of question
    if (incomingByte != (int)Byte) ReadError=true;
    
    incomingByte=_serial->read(); // the Checksum of question
    if (incomingByte != (int)CheckSum) ReadError=true;
    
    incomingByte=_serial->read(); // the Write OK of answer
    if (incomingByte != WRITE_OK) ReadError=true;
    
    flushSerialBuffer();
    return (ReadError);
}


/******************************
 * write a type Uint variable *
 ******************************/

bool _bionet::writeUIntVar(unsigned char VarNumber, unsigned int UInt) {
    int           incomingByte;
    unsigned char CheckSum;
    unsigned char Byte;
    unsigned char ByteHigh;
    bool          ReadError=false;
    
    VarNumber =  VarNumber | WRITE_BIT;
    Byte      =  UInt & 255;
    ByteHigh  =  UInt >> 8;
    
    CheckSum  =  VarNumber;
    CheckSum  += Byte;
    CheckSum  += ByteHigh;
    CheckSum   = ~CheckSum;
    
    _serial->write(VarNumber);
    _serial->write(Byte);
    _serial->write(CheckSum);
    delay(BIONET_INTERFRAME);
    
    incomingByte = _serial->read(); // the VarNumber of question
    if (incomingByte != (int)VarNumber) ReadError=true;
    
    incomingByte = _serial->read(); // the Byte of question
    if (incomingByte != (int)Byte) ReadError=true;

    incomingByte = _serial->read(); // the Byte of question
    if (incomingByte != (int)ByteHigh) ReadError=true;

    incomingByte = _serial->read(); // the Checksum of question
    if (incomingByte != (int)CheckSum) ReadError=true;
    
    incomingByte = _serial->read(); // the Write OK of answer
    if (incomingByte != WRITE_OK) ReadError=true;
    
    flushSerialBuffer();
    return (ReadError);
}


/****************************************
 * send Network State command           *
 ****************************************/

void _bionet::sendRun() {
	sendState(RUN);
}

void _bionet::sendInit() {
	sendState(INIT);
}

void _bionet::sendError() {
	sendState(ERROR);
}

void _bionet::sendStop() {
	sendState(STOP);
}

/* DC00_Architecture_IE7
 * cf. 9.3.1 and 9.3.2 
 */

void _bionet::sendState(uint8_t state) {
    flushSerialBuffer();
    _serial->write(0x80);
    _serial->write(state);
    _serial->write(0x00);
    _serial->write(~(0x80+state));
    delay(BIONET_INTERFRAME);
    flushSerialBuffer();
}


/******************************
 * Flush Serial Buffer        * 
 ******************************/

void _bionet::flushSerialBuffer(void) {
    int incomingByte;  
    do {
        incomingByte = _serial->read();
    }
    while (incomingByte != -1);
}
