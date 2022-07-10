/*********************************************************
 * structure to hold configuration, not platform dependent
 *  set functions       set new value, stringsd are duplicated, previous strings are freed, must not use on configuration with value strings in static memory
 *  init_empty_conf     call this to initialise all values as empty,  without deallocating old strings, i.e. after creating an uninitialized object
 *  create_empty_conf   allocates empty and null-initialized object, before freeing the object, use set_empty_conf to free the strings of the properties 
 *  set_empty_conf      sets all properties to null, frees them if necessary, don't call, if properties are static
 ********************************************************/

#include "tuerschild.h"
#include <string.h>
tuerschild_config_t* create_empty_conf()
{
    tuerschild_config_t* conf = malloc(sizeof(tuerschild_config_t));
    init_empty_conf(conf);

    return conf;
}

void init_empty_conf(tuerschild_config_t* conf)
{
    conf->broker = NULL;
    conf->password = NULL;
    conf->port = 0;
    conf->ssid = NULL;
    conf->topic  = NULL;
    conf->ap_ssid = NULL;
    conf->ap_password = NULL;
    conf->ntp_server = NULL;
}
void conf_set_ntp(tuerschild_config_t* conf, const char* ntp)
{
    if(conf->ntp_server) {
        free(conf->ntp_server);
    }
    conf->ntp_server = strdup(ntp);
}
void conf_set_ssid(tuerschild_config_t* conf, const char* ssid)
{
    if(conf->ssid) {
        free(conf->ssid);
    }
    conf->ssid = strdup(ssid);
    
}

void conf_set_pass(tuerschild_config_t* conf, const char* password)
{
    if(conf->password) {
        free(conf->password);
    }
    conf->password = strdup(password);
    
}

void conf_set_broker(tuerschild_config_t* conf, const char* broker)
{
    if(conf->broker) {
        free(conf->broker);
    }
    conf->broker = strdup(broker);
    
}

void conf_set_port(tuerschild_config_t* conf, const uint16_t port)
{
    
    conf->port = port;
    
}

void conf_set_topic(tuerschild_config_t* conf, const char* topic)
{
    if(conf->topic) {
        free(conf->topic);
    }
    conf->topic = strdup(topic);
    
}

void conf_set_ap_ssid(tuerschild_config_t* conf, const char* ap_ssid)
{
    if(conf->ap_ssid) {
        free(conf->ap_ssid);
    }
    conf->ap_ssid = strdup(ap_ssid);
    
}

void conf_set_ap_pass(tuerschild_config_t* conf, const char* ap_pass)
{
    if(conf->ap_password) {
        free(conf->ap_password);
    }
    conf->ap_password = strdup(ap_pass);
    
}

void conf_set_ap_chan(tuerschild_config_t* conf, const uint8_t chan)
{
    conf->ap_channel = chan;
    
}

void conf_set(tuerschild_config_t* conf, const char* ssid, const char* password, const char* broker, uint16_t port, const char* topic, const char* ap_ssid, const char* ap_pass, uint8_t ap_chan, const char* ntp)
{
    conf_set_ssid(conf, ssid);
    conf_set_pass(conf, password);
    conf_set_broker(conf, broker);
    conf_set_port(conf, port);
    conf_set_topic(conf, topic);
    conf_set_ap_ssid(conf, ap_ssid);
    conf_set_ap_pass(conf, ap_pass);
    conf_set_ap_chan(conf, ap_chan);
    conf_set_ntp(conf, ntp);
}

void set_empty_conf(tuerschild_config_t *conf)
{
	if(conf->ssid) {
		free(conf->ssid);
	}
	if(conf->password) {
		free(conf->password);
	}
	if(conf->broker) {
		free(conf->broker);
	}
	if(conf->topic) {
		free(conf->topic);
	}
    if(conf->ap_ssid) {
        free(conf->ap_ssid);
    }
    if(conf->ap_password) {
        free(conf->ap_password);
    }
    if(conf->ntp_server) {
        free(conf->ntp_server);
    }
    init_empty_conf(conf);
}
