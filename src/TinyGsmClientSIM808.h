/**
 * @file     TinyGsmClientSIM808.h
 * @author   Volodymyr Shymanskyy
 * @license  LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date     Nov 2016
 */

#ifndef TinyGsmClientSIM808_h
#define TinyGsmClientSIM808_h
//#pragma message("TinyGSM:  TinyGsmClientSIM808")

#include <TinyGsmClientSIM800.h>


class TinyGsmSim808: public TinyGsmSim800
{

public:

  TinyGsmSim808(Stream& stream)
    : TinyGsmSim800(stream)
  {}

  /*
   * GPS location functions
   */

  // enable GPS
  bool enableGPS() {
    // uint16_t state;

    sendAT(GF("+CGNSPWR=1"));
    if (waitResponse() != 1) {
      return false;
    }

    return true;
  }

  bool disableGPS() {
    // uint16_t state;

    sendAT(GF("+CGNSPWR=0"));
    if (waitResponse() != 1) {
      return false;
    }

    return true;
  }

  // get the RAW GPS output
  // works only with ans SIM808 V2
  MyString<> getGPSraw() {
    sendAT(GF("+CGNSINF"));
    if (waitResponse(GF(GSM_NL "+CGNSINF:")) != 1) {
      return "";
    }
    MyString<> res = MyString<>::fromStreamUntil(stream,'\n');
    waitResponse();
    res.trim();
    return res;
  }

  // get GPS informations
  // works only with ans SIM808 V2
  bool getGPS(float *lat, float *lon, float *speed=0, int *alt=0, int *vsat=0, int *usat=0) {
    //String buffer = "";
    // char chr_buffer[12];
    bool fix = false;

    sendAT(GF("+CGNSINF"));
    if (waitResponse(GF(GSM_NL "+CGNSINF:")) != 1) {
      return false;
    }

    MyString<>::fromStreamUntil(stream, ','); // mode
    if ( MyString<>::fromStreamUntil(stream, ',').toInt() == 1 ) fix = true;
    MyString<>::fromStreamUntil(stream, ','); //utctime
    *lat =  MyString<>::fromStreamUntil(stream, ',').toFloat(); //lat
    *lon =  MyString<>::fromStreamUntil(stream, ',').toFloat(); //lon
    if (alt != NULL) *alt =  MyString<>::fromStreamUntil(stream, ',').toFloat(); //lon
    if (speed != NULL) *speed = MyString<>::fromStreamUntil(stream, ',').toFloat(); //speed
	MyString<>::fromStreamUntil(stream, ',');
	MyString<>::fromStreamUntil(stream, ',');
	MyString<>::fromStreamUntil(stream, ',');
	MyString<>::fromStreamUntil(stream, ',');
	MyString<>::fromStreamUntil(stream, ',');
	MyString<>::fromStreamUntil(stream, ',');
	MyString<>::fromStreamUntil(stream, ',');
    if (vsat != NULL) *vsat = MyString<>::fromStreamUntil(stream, ',').toInt(); //viewed satelites
    if (usat != NULL) *usat = MyString<>::fromStreamUntil(stream, ',').toInt(); //used satelites
    MyString<> res = MyString<>::fromStreamUntil(stream, '\n');

    waitResponse();

    return fix;
  }

  // get GPS time
  // works only with SIM808 V2
  bool getGPSTime(int *year, int *month, int *day, int *hour, int *minute, int *second) {
    bool fix = false;
    char chr_buffer[12];
    sendAT(GF("+CGNSINF"));
    if (waitResponse(GF(GSM_NL "+CGNSINF:")) != 1) {
      return false;
    }

    for (int i = 0; i < 3; i++) {
      String buffer = stream.readStringUntil(',');
      buffer.toCharArray(chr_buffer, sizeof(chr_buffer));
      switch (i) {
        case 0:
          //mode
          break;
        case 1:
          //fixstatus
          if ( buffer.toInt() == 1 ) {
            fix = buffer.toInt();
          }
          break;
        case 2:
          *year = buffer.substring(0,4).toInt();
          *month = buffer.substring(4,6).toInt();
          *day = buffer.substring(6,8).toInt();
          *hour = buffer.substring(8,10).toInt();
          *minute = buffer.substring(10,12).toInt();
          *second = buffer.substring(12,14).toInt();
          break;

        default:
          // if nothing else matches, do the default
          // default is optional
          break;
      }
    }
    MyString<> res = MyString<>::fromStreamUntil(stream, '\n');
    waitResponse();

    if (fix) {
      return true;
    } else {
      return false;
    }
  }

};

#endif
