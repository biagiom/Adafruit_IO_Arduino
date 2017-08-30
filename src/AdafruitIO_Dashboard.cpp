//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Copyright (c) 2015-2016 Adafruit Industries
// Authors: Tony DiCola, Todd Treece
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.
//
#include "AdafruitIO_Dashboard.h"
#include "AdafruitIO.h"

AdafruitIO_Dashboard::AdafruitIO_Dashboard(AdafruitIO *io, const char *n)
{
  _io = io;
  name = n;
}

AdafruitIO_Dashboard::~AdafruitIO_Dashboard(){}

bool AdafruitIO_Dashboard::exists()
{
  String url = "/api/v2/";
  url += _io->_username;
  url += "/dashboards/";
  url += name;

// Add debug info
#ifdef AIO_ERROR
  AIO_ERR_PRINTLN("AdafruitIO_Dashboard->exists()")
  AIO_ERR_PRINT("GET url: ")
  AIO_ERR_PRINTLN(url)
#endif

  _io->_http->beginRequest();
  _io->_http->get(url.c_str());
  _io->_http->sendHeader("X-AIO-Key", _io->_key);
  _io->_http->endRequest();

  int status = _io->_http->responseStatusCode();
  String resp = _io->_http->responseBody(); // needs to be read even if not used

// Add debug info 
#ifdef AIO_ERROR
  AIO_ERR_PRINT("HTTP GET status code: ")
  AIO_ERR_PRINTLN(status)
  AIO_ERR_PRINT("HTTP GET response: ")
  AIO_ERR_PRINTLN(resp)
#endif

  return status == 200;
}

bool AdafruitIO_Dashboard::create()
{
  String url = "/api/v2/";
  url += _io->_username;
  url += "/dashboards";

// Add debug info
#ifdef AIO_ERROR
  AIO_ERR_PRINTLN("AdafruitIO_Dashboard->create()")
  AIO_ERR_PRINT("POST url: ")
  AIO_ERR_PRINTLN(url)
#endif

  // Send data with JSON format
  // String body = "{";
  // body += "\"name\":";
  // body += "\"";
  // body += name;
  // body += "\"";
  // body += "}";
  String body = "name=";
  body += name;

// Add debug info
#ifdef AIO_ERROR
  AIO_ERR_PRINT("POST body: ")
  AIO_ERR_PRINTLN(body)
#endif

  _io->_http->beginRequest();
  _io->_http->post(url.c_str());

  // use "application/json" type instead of "application/x-www-form-urlencoded"
  _io->_http->sendHeader("Content-Type", "application/x-www-form-urlencoded");
  //_io->_http->sendHeader("Content-Type", "application/json");
  _io->_http->sendHeader("Content-Length", body.length());
  //_io->_http->sendHeader("X-AIO-Key", _io->_key);

  // the following call to endRequest
  // should be replaced by beginBody once the
  // Arduino HTTP Client Library is updated
  _io->_http->beginBody();
  //_io->_http->endRequest();

  _io->_http->print(body);
  _io->_http->endRequest();

  int status = _io->_http->responseStatusCode();
  String resp = _io->_http->responseBody(); // needs to be read even if not used

// Add debug info  
#ifdef AIO_ERROR
  AIO_ERR_PRINT("HTTP POST status code: ")
  AIO_ERR_PRINTLN(status)
  AIO_ERR_PRINT("HTTP POST response: ")
  AIO_ERR_PRINTLN(resp)
#endif

  return status == 201;
}

const char* AdafruitIO_Dashboard::user() {
  return _io->_username;
}

AdafruitIO* AdafruitIO_Dashboard::io() {
  return _io;
}

ToggleBlock* AdafruitIO_Dashboard::addToggleBlock(AdafruitIO_Feed *feed)
{
  return new ToggleBlock(this, feed);
}

MomentaryBlock* AdafruitIO_Dashboard::addMomentaryBlock(AdafruitIO_Feed *feed)
{
  return new MomentaryBlock(this, feed);
}

SliderBlock* AdafruitIO_Dashboard::addSliderBlock(AdafruitIO_Feed *feed)
{
  return new SliderBlock(this, feed);
}

GaugeBlock* AdafruitIO_Dashboard::addGaugeBlock(AdafruitIO_Feed *feed)
{
  return new GaugeBlock(this, feed);
}

TextBlock* AdafruitIO_Dashboard::addTextBlock(AdafruitIO_Feed *feed)
{
  return new TextBlock(this, feed);
}

ChartBlock* AdafruitIO_Dashboard::addChartBlock(AdafruitIO_Feed *feed)
{
  return new ChartBlock(this, feed);
}

ColorBlock* AdafruitIO_Dashboard::addColorBlock(AdafruitIO_Feed *feed)
{
  return new ColorBlock(this, feed);
}

MapBlock* AdafruitIO_Dashboard::addMapBlock(AdafruitIO_Feed *feed)
{
  return new MapBlock(this, feed);
}

StreamBlock* AdafruitIO_Dashboard::addStreamBlock(AdafruitIO_Feed *feed)
{
  return new StreamBlock(this, feed);
}

ImageBlock* AdafruitIO_Dashboard::addImageBlock(AdafruitIO_Feed *feed)
{
  return new ImageBlock(this, feed);
}
