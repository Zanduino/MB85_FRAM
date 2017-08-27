/*******************************************************************************************************************
** MB85_FRAM class method definitions. See the header file for program details and version information            **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
*******************************************************************************************************************/
#include "MB85_FRAM.h"                                                        // Include the header definition    //
                                                                              //----------------------------------//
MB85_FRAM_Class::MB85_FRAM_Class()  {}                                        // Empty & unused class constructor //
MB85_FRAM_Class::~MB85_FRAM_Class() {}                                        // Empty & unused class destructor  //
/*******************************************************************************************************************
** Method begin starts communications with the device. There are 2 possible memory sizes, 8KB and 32KB which look **
** identical and have the same I2C address ranges as well as instruction sets.                                    **
*******************************************************************************************************************/
bool MB85_FRAM_Class::begin() {                                               // Find I2C device                  //
  uint8_t devInfo[3] = { 0, 0, 0 };                                           // store device information         //
  Wire.begin();                                                               // Start I2C as master device       //
  for(uint8_t i=MB85_MIN_ADDRESS;i<MB85_MIN_ADDRESS+8;i++) {                  // loop all possible addresses      //
    Wire.beginTransmission(i);                                                // Check current address for device //
    if (Wire.endTransmission()==0) {                                          // If no error we have a device     //
      Wire.beginTransmission(MB85_SLAVE_ID_ADDRESS>>1);                       // Use code to get back the mfgr    //
      Wire.write(i<<1);                                                       // and device id.                   //
      _TransmissionStatus = Wire.endTransmission(false);                      // Keep control of I2C bus          //
      Wire.requestFrom(MB85_SLAVE_ID_ADDRESS>>1,3);                           // Request 3 data bytes             //
      devInfo[0]=Wire.read();devInfo[1]=Wire.read();devInfo[2]=Wire.read();   // Get the 3 data bytes             //
      if (((devInfo[0]<<4)+(devInfo[1]>>4))==0xA &&                           // If the manufacturer and the      //
          (((devInfo[1] & 0x0F) << 8) + devInfo[2]) == 0x510) {               // product ID match the success     //
        /***********************************************************************************************************
        ** There are several different chips which match this ID, we need to determine if it is an 8Kb or a 32Kb  **
        ** memory. The device has no direct means of returning the size. But we can take advantage of the fact    **
        ** the memory overlows automagically back to address 0 on reads/writes. So we store the current contents  **
        ** of addr(0), addr(32767) and addr(32768). If the values for 0 and 32768 are different then we know that **
        ** it cannot be the 8KB device. Otherwise we write a new value to addr(32768) and then read addr(0); if   **
        ** addr(0) has changed then we know we had an overflow backt to the beginning and that it is an 8KB FRAM, **
        ** then we reset the data to what it was before the test started                                          **
        ***********************************************************************************************************/
        Wire.beginTransmission(i);                                            // Start transmission               //
        Wire.write((uint8_t)0);Wire.write((uint8_t)0);                        // Start at address 0               //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        Wire.requestFrom(i,(uint8_t)1);                                       // Request 1 byte of data           //
        devInfo[0] = Wire.read();                                             // Load to array                    //
        Wire.beginTransmission(i);                                            // Start transmission               //
        Wire.write(0x1F);Wire.write(0xFF);                                    // Position to address 8191         //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        Wire.requestFrom(i,(uint8_t)2);                                       // Request two bytes of data        //
        devInfo[1] = Wire.read();                                             // Get two more bytes               //
        devInfo[2] = Wire.read();                                             //                                  //
        Wire.beginTransmission(i);                                            // Start transmission               //
        Wire.write((uint8_t)0);Wire.write((uint8_t)0);                        // Start at address 0               //
        Wire.write(~devInfo[0]);                                              // write the ~ (not) value          //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        Wire.beginTransmission(i);                                            // Start transmission               //
        Wire.write(0x1F);Wire.write(0xFF);                                    // Position to address 8191         //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        Wire.requestFrom(i,(uint8_t)2);                                       // Request 2 bytes of data          //
        Wire.read();                                                          // ignore first byte                //
        if (Wire.read()==devInfo[2]) {                                        // If the values match, then we have//
          _I2C[i-MB85_MIN_ADDRESS] = 32;                                      // no overflow                      //
        } else {                                                              //                                  //
          _I2C[i-MB85_MIN_ADDRESS] = 8;                                       //                                  //
        } // of if-then-else we have a wraparound                             //                                  //
        Wire.beginTransmission(i);                                            // Start transmission               //
        Wire.write((uint8_t)0);Wire.write((uint8_t)0);                        // Start at address 0               //
        Wire.write(devInfo[0]);                                               // Restore original data            //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
      } // of if-then we have found a MB85xxxx FRAM                           //                                  //
    } // of if-then we have found a device                                    //                                  //
  } // of for-next each I2C address loop                                      //                                  //
  return false;                                                               // return failure if we get here    //
} // of method begin()                                                        //                                  //
