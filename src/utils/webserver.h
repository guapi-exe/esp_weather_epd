#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

extern DNSServer dnsServer; 
extern AsyncWebServer server;


void initWebServer();

#endif