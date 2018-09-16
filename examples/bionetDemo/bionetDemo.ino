/*
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Bionet.h>
Bionet bionet = Bionet(&Serial);

//****************************************************************************
// SETUP PROGRAM
//****************************************************************************

void setup() {
  bionet.begin();
}

//***************************************************************************
// MAIN LOOP
//****************************************************************************

unsigned char Byte;
unsigned int  UInt;

void loop() {  
 if (bionet.update()) {
    bionet.sendRun();
    bionet.writeByteVar(1, Byte);
    bionet.readUIntVar(2, &UInt);
  }
}


