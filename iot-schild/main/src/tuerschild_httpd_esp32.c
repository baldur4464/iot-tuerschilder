#include "tuerschild.h"

#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_http_server.h"



static const char* tag = "tuerschild_httpd_esp32";

#define CONF_RECEIVED_BIT (1<<20)

typedef struct post_context
{
    char* content;
    int put;
} post_context_t;

EventGroupHandle_t httpd_events = NULL;




static httpd_handle_t server = NULL;
static httpd_config_t server_conf = HTTPD_DEFAULT_CONFIG();

static esp_err_t get_handler(httpd_req_t* request);
static esp_err_t post_handler(httpd_req_t* request);

int parse_url_encoded(char* str);

static char main_page[] = 
        "<html><body>"
            "<form action =\"conf\" method = \"post\">"
            
                "<label for=\"fSSID\">SSID:</label><br>"
                "<input type=\"text\" id=\"fSSID\" name=\"SSID\"><br>"
                
                "<label for=\"lpass\">pass:</label><br>"
                "<input type=\"password\" id=\"lpass\" name=\"lpass\"><br>"
                
                "<label for=\"ltopic\">topic:</label><br>"
                "<input type=\"text\" id=\"ltopic\" name=\"ltopic\"><br>"
                
                "<label for=\"lport\">port:</label><br>"
                "<input type=\"text\" id=\"lport\" name=\"lport\" inputmode=\"numeric\"><br>"
                
                "<label for=\"lbroker\">broker:</label><br>"
                "<input type=\"text\" id=\"lbroker\" name=\"lbroker\"><br>"

                "<button type=\"submit\">Einstellungen Speichern</button>"
                
            "</form>"
            "</html></body>";


static const httpd_uri_t conf_get = {
    .uri       = "/conf",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = main_page
};

static httpd_uri_t conf_post = {
    .uri       = "/conf",
    .method    = HTTP_POST,
    .handler   = post_handler
};

static esp_err_t get_handler(httpd_req_t* request)
{   
    TUERSCHILD_LOGI(tag, "server got \"GET\" request");
    httpd_resp_send(request, request->user_ctx, HTTPD_RESP_USE_STRLEN);
    
    
    
    return ESP_OK;
}

static esp_err_t post_handler(httpd_req_t* request)
{   
    post_context_t* ctx;
    int ret;

    TUERSCHILD_LOGI(tag, "server got \"POST\" request");

    if(!request->sess_ctx) {
        TUERSCHILD_LOGI(tag, "neue post session");
        ctx = malloc(sizeof(post_context_t));
        request->sess_ctx = ctx;
        ctx->content = malloc(request->content_len + 1 );
        ctx->put = 0;
    } else {
        ctx = request->sess_ctx;
    }
    ret = httpd_req_recv(request, ctx->content + ctx->put, request->content_len);
    if(ret <= 0) {
        free(ctx->content);
        free(ctx);
        request->sess_ctx = NULL;
        return ESP_FAIL;
    }
    ctx->put += ret;
    if(ret == request->content_len) {
        #warning reponse muell
        #warning printing config
        TUERSCHILD_LOGI(tag, "post session vorbei");
        ctx->content[ctx->put++] = 0;
        TUERSCHILD_LOGI(tag, "%.*s", ctx->put, ctx->content);
        
        char* end;
        char* ssid = strstr(ctx->content, "SSID=") + strlen("SSID=");
        end = strchr(ssid, '&');
        if(!end) {
            end = ctx->content + ctx->put-1;
        }
        int ssid_len = end - ssid;
        
        char* pass = strstr(ctx->content, "pass=") + strlen("pass=");
        end = strchr(pass, '&');
        if(!end) {
            end = ctx->content + ctx->put-1;
        }
        int pass_len = end - pass;
        
        char* broker = strstr(ctx->content, "broker=")+ strlen("broker=");
        end = strchr(broker, '&');
        if(!end) {
            end = ctx->content + ctx->put-1;
        }
        int broker_len = end - broker;

        char* topic = strstr(ctx->content, "topic=")+ strlen("topic=");
        end = strchr(topic, '&');
        if(!end) {
            end = ctx->content + ctx->put-1;
        }
        int topic_len = end - topic;

        char* port = strstr(ctx->content, "port=")+ strlen("port=");
        end = strchr(port, '&');
        if(!end) {
            end = ctx->content + ctx->put-1;
        }
        int port_len = end - port;

        ssid[ssid_len] = 0;
        pass[pass_len] = 0;
        topic[topic_len] = 0;
        broker[broker_len] = 0;
        port[port_len] = 0;
        
        parse_url_encoded(ssid);
        parse_url_encoded(pass);
        parse_url_encoded(topic);
        parse_url_encoded(broker);
        
        
        //TUERSCHILD_LOGW(tag, "conf: %*s %*s %*s %*s ", ssid_len, ssid, pass_len, pass, broker_len, broker, topic_len, topic);
        TUERSCHILD_LOGW(tag, "conf: %s %s %s %s %s", ssid, pass, broker, topic, port);
        //TUERSCHILD_LOGW(tag, "field lengths: %d, %d, %d, %d, %d", ssid_len, pass_len, broker_len, topic_len, port_len);

        if(ssid && pass && topic&& broker) {
            TUERSCHILD_LOGW(tag, "saving new config");
            tuerschild_config_t* conf = request->user_ctx;
            tuerschild_config_t new_conf = {.hostname = broker, .password = pass, .port = atoi(port), .ssid = ssid, .topic = topic};
            *conf = new_conf;
        }

        httpd_resp_send(request, "saved config", HTTPD_RESP_USE_STRLEN);
        #warning TODO FREE connection context
        //free(ctx->content);
        //free(ctx);
        request->sess_ctx = NULL;
    }
    xEventGroupSetBits(httpd_events, CONF_RECEIVED_BIT);
    return ESP_OK;
}

int start_recv_config(tuerschild_config_t *conf)
{
    esp_err_t error = ESP_OK;
    server_conf.server_port = 80;
    server_conf.lru_purge_enable = 1;
    error = httpd_start(&server, &server_conf);
    if(error != ESP_OK) {
        TUERSCHILD_LOGE(tag, "failed to start httpd");
        return 0;
    }
    TUERSCHILD_LOGI(tag, "server has port: %d", server_conf.server_port);
    error = httpd_register_uri_handler(server, &conf_get);
    if(error != ESP_OK) {
        TUERSCHILD_LOGE(tag, "failed to register get with httpd");
        return 0;
    }

    
    httpd_events = xEventGroupCreate();
    if(!httpd_events) {
        TUERSCHILD_LOGE(tag, "failed to create event group");
    
        return 0;
    }

    conf_post.user_ctx = conf;
    
    error = httpd_register_uri_handler(server, &conf_post);
    if(error != ESP_OK) {
        TUERSCHILD_LOGE(tag, "failed register post with httpd");
    
        vEventGroupDelete(httpd_events);
        return 0;
    }

    TUERSCHILD_LOGI(tag, "httpd is up");
    return 1;
}

int stop_recv_config()
{
    httpd_stop(server);
    
    vEventGroupDelete(httpd_events);
    return 1;
}

int done_recv_conf()
{
    return !!(CONF_RECEIVED_BIT &xEventGroupGetBits(httpd_events));
}

int parse_url_encoded(char* str)
{
    char* put = str;
    char* get  =str;
    char buf[3];
    buf[2] = 0;
    while(*(get)) {
        switch(*get) {
        case '%' :
           //*(put++) = ((tolower(*(get+1))-'0') << 4) | (tolower(*(get+2))-'0');
            buf[0] = *(++get);
            buf[1] = *(++get);
            
            get++;
            *(put++) = strtol(buf, NULL, 16);
            
            break;
        case '+':
            *(put++) = ' ';
            get++;
            break;
        default:
            *(put++) = *(get++);
        }
        putchar(*(put-1));
    }
    *put = 0;
    return put - str;
}