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
#include "AdafruitIO_Feed.h"
#include "AdafruitIO.h"

AdafruitIO_Feed::AdafruitIO_Feed(AdafruitIO *io, const char *n):AdafruitIO_MQTT()
{
  _io = io;
  name = n;
  _sub = 0;
  _pub = 0;
  _dataCallback = 0;

  _init();
}

AdafruitIO_Feed::~AdafruitIO_Feed()
{
  if(_sub)
    delete _sub;

  if(_pub)
    delete _pub;

  if(data)
    delete data;

  if(_topic)
    free(_topic);

  if(_feed_url)
    free(_feed_url);

  if(_create_url)
    free(_create_url);
}

void AdafruitIO_Feed::onMessage(AdafruitIODataCallbackType cb)
{
  _dataCallback = cb;
}

bool AdafruitIO_Feed::save(char *value, double lat, double lon, double ele)
{
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(bool value, double lat, double lon, double ele)
{
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(String value, double lat, double lon, double ele)
{
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(int value, double lat, double lon, double ele)
{
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(unsigned int value, double lat, double lon, double ele)
{
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(long value, double lat, double lon, double ele)
{
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(unsigned long value, double lat, double lon, double ele)
{
  data->setValue(value, lat, lon, ele);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(float value, double lat, double lon, double ele, int precision)
{
  data->setValue(value, lat, lon, ele, precision);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::save(double value, double lat, double lon, double ele, int precision)
{
  data->setValue(value, lat, lon, ele, precision);
  return _pub->publish(data->toCSV());
}

bool AdafruitIO_Feed::exists()
{

#ifdef AIO_ERROR
  AIO_ERR_PRINTLN("AdafruitIO_Feed->exists()")
  AIO_ERR_PRINT("_feed_url: ")
  AIO_ERR_PRINTLN(_feed_url)
#endif

  _io->_http->beginRequest();
  _io->_http->get(_feed_url);
  //_io->_http->sendHeader("X-AIO-Key", _io->_key);
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

bool AdafruitIO_Feed::create()
{
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
  AIO_ERR_PRINTLN("AdafruitIO_Feed->create()")
  AIO_ERR_PRINT("_create_url: ")
  AIO_ERR_PRINTLN(_create_url)
  AIO_ERR_PRINT("POST body: ")
  AIO_ERR_PRINTLN(body)
#endif

  _io->_http->beginRequest();
  _io->_http->post(_create_url);

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

void AdafruitIO_Feed::setLocation(double lat, double lon, double ele)
{
  data->setLocation(lat, lon, ele);
}

void AdafruitIO_Feed::subCallback(char *val, uint16_t len)
{
  data->setCSV(val);

  // call callback with data
  if(_dataCallback)
    _dataCallback(data);
}

void AdafruitIO_Feed::_init()
{
  // Update url creation in order to send the key as parameter of GET request because sending it as an HTTP header seems not work in 
  // version 2.6.0
  
  // dynamically allocate memory for mqtt topic and REST URLs
  // _topic = (char *) malloc(sizeof(char) * (strlen(_io->_username) + strlen(name) + 8)); // 8 extra chars for /f/, /csv & null termination
  // _feed_url = (char *) malloc(sizeof(char) * (strlen(_io->_username) + strlen(name) + 16)); // 16 extra for api path & null term
  // _create_url = (char *) malloc(sizeof(char) * (strlen(_io->_username) + 15)); // 15 extra for api path & null term
  
  // New implementation:
  
  // 8 extra chars for /f/, /csv & null termination
  _topic = (char *) malloc(sizeof(char) * (strlen(_io->_username) + strlen(name) + 8));
  
  // 27 extra for:
  // "/api/v2/"     => lenght: 8
  // "/feeds/"      => lenght: 7
  // "?x-aio-key="  => lenght: 11
  // null term      => lenght: 1
  _feed_url = (char *) malloc(sizeof(char) * (strlen(_io->_username) + strlen(name) + strlen(_io->_key) + 27));
  
  // 26 extra for:
  // "/api/v2/"     => lenght: 8
  // "/feeds"       => lenght: 6
  // "?x-aio-key="  => lenght: 11
  // null term      => lenght: 1
  _create_url = (char *) malloc(sizeof(char) * (strlen(_io->_username) + strlen(_io->_key) + 26));

  // init feed data
  data = new AdafruitIO_Data(this);

  if(_topic && _create_url && _feed_url) {

    // build topic string
    strcpy(_topic, _io->_username);
    strcat(_topic, "/f/");
    strcat(_topic, name);
    strcat(_topic, "/csv");

// Ensure you have already started Serial communication
// before enabling serial debug otherwise on some Arduino boards
// you can have issues
#ifdef SKETCH_DEBUG
    AIO_ERR_PRINTLN("AdafruitIO_Feed->_init()")
    AIO_ERR_PRINT("_topic: ")
    AIO_ERR_PRINTLN(_topic)
#endif

    // build feed url string
    strcpy(_feed_url, "/api/v2/");
    strcat(_feed_url, _io->_username);
    strcat(_feed_url, "/feeds/");
    strcat(_feed_url, name);
    // send the key as parameter of GET request
    strcat(_feed_url, "?x-aio-key=");
    strcat(_feed_url, _io->_key);

    // build create url string
    strcpy(_create_url, "/api/v2/");
    strcat(_create_url, _io->_username);
    strcat(_create_url, "/feeds");
    // send the key as parameter of GET request
    strcat(_create_url, "?x-aio-key=");
    strcat(_create_url, _io->_key);

    // setup subscription
    _sub = new Adafruit_MQTT_Subscribe(_io->_mqtt, _topic);
    _pub = new Adafruit_MQTT_Publish(_io->_mqtt, _topic);
    _io->_mqtt->subscribe(_sub);

    _sub->setCallback(this, &AdafruitIO_MQTT::subCallback);

  } else {

// Add debug info  
#ifdef SKETCH_DEBUG
    AIO_ERR_PRINT("malloc failed!")
#endif

    // malloc failed
    _topic = 0;
    _create_url = 0;
    _feed_url = 0;
    _sub = 0;
    _pub = 0;
    data = 0;

  }

}
