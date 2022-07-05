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

static EventGroupHandle_t httpd_events = NULL;





static httpd_handle_t server = NULL;
static httpd_config_t server_conf = HTTPD_DEFAULT_CONFIG();

static esp_err_t get_handler(httpd_req_t* request);
static esp_err_t post_handler(httpd_req_t* request);

static int parse_url_encoded(char* str);
static char* get_string_in_post(char* post, const char* key, int* len);


static const char page_before_ssid[] = 
        "<html><body>"
            "<form action =\"conf\" method = \"post\">"
            
                "<label for=\"fSSID\">SSID:</label><br>"
                "<input type=\"text\" id=\"fSSID\" name=\"SSID\" value=\"";
static const char page_before_topic[] ="\"><br>"
                
                "<label for=\"lpass\">pass:</label><br>"
                "<input type=\"password\" id=\"lpass\" name=\"lpass\"><br>"

                "<label for=\"lpass_conf\">confirm pass:</label><br>"
                "<input type=\"password\" id=\"lpass_confirm\" name=\"lpass_confirm\"><br>"
                
                "<label for=\"ltopic\">topic:</label><br>"
                "<input type=\"text\" id=\"ltopic\" name=\"ltopic\" value = \"";
static const char page_before_port[] = "\"><br>"
                
                "<label for=\"lport\">port:</label><br>"
                "<input type=\"text\" id=\"lport\" name=\"lport\" inputmode=\"numeric\" value=\"";
static const char page_before_broker[] = "\"><br>"
                
                "<label for=\"lbroker\">broker:</label><br>"
                "<input type=\"text\" id=\"lbroker\" name=\"lbroker\" value=\"";
static const char page_before_ap_ssid[] = "\"><br>"
                
                "<label for=\"lap_ssid\">ap_ssid:</label><br>"
                "<input type=\"text\" id=\"lap_ssid\" name=\"lap_ssid\" value=\"";

static const char page_before_ap_chan[] = "\"><br>"

                "<label for=\"lap_pass\">ap_pass:</label><br>"
                "<input type=\"password\" id=\"lap_pass\" name=\"lap_pass\"><br>"

                "<label for=\"lap_pass_conf\">confirm ap_pass:</label><br>"
                "<input type=\"password\" id=\"lap_pass_confirm\" name=\"lap_pass_confirm\"><br>"
                
                "<label for=\"lap_chan\">ap_chan:</label><br>"
                "<input type=\"text\" id=\"lap_chan\" name=\"lap_chan\" inputmode=\"numeric\"value=\"";
static const char page_before_ntp[] = "\"><br>"
                
                "<label for=\"lntp\">ntp:</label><br>"
                "<input type=\"text\" id=\"lntp\" name=\"lntp\" value=\"";
static const char page_end[] = "\"><br>"

                "<button type=\"submit\">Einstellungen Speichern</button>"
                
            "</form>"
            "</html></body>";


static httpd_uri_t conf_get = {
    .uri       = "/conf",
    .method    = HTTP_GET,
    .handler   = get_handler,
    
};

static httpd_uri_t conf_post = {
    .uri       = "/conf",
    .method    = HTTP_POST,
    .handler   = post_handler
    
};
char buf[100];
static esp_err_t get_handler(httpd_req_t* request)
{   
    
    tuerschild_config_t* old_conf = request->user_ctx;
    TUERSCHILD_LOGI(tag, "server got \"GET\" request");
    //httpd_resp_send(request, request->user_ctx, HTTPD_RESP_USE_STRLEN);
    httpd_resp_sendstr_chunk(request, page_before_ssid);
    if(old_conf->ssid && strlen(old_conf->ssid)) {
        httpd_resp_sendstr_chunk(request, old_conf->ssid);
    }

    httpd_resp_sendstr_chunk(request, page_before_topic);
    if(old_conf->topic && strlen(old_conf->topic)) {
        httpd_resp_sendstr_chunk(request, old_conf->topic);
    }

    httpd_resp_sendstr_chunk(request, page_before_port);
    if(old_conf->port) {
        snprintf(buf, 99, "%d", old_conf->port);
        httpd_resp_sendstr_chunk(request, buf);
    }

    httpd_resp_sendstr_chunk(request, page_before_broker);
    if(old_conf->broker && strlen(old_conf->broker)) {
        httpd_resp_sendstr_chunk(request, old_conf->broker);
    }

    httpd_resp_sendstr_chunk(request, page_before_ap_ssid);
    if(old_conf->ap_ssid && strlen(old_conf->ap_ssid)) {
        httpd_resp_sendstr_chunk(request, old_conf->ap_ssid);
    }

    httpd_resp_sendstr_chunk(request, page_before_ap_chan);
    if(old_conf->ap_channel) {
        snprintf(buf, 99, "%d", old_conf->ap_channel);
        httpd_resp_sendstr_chunk(request, buf);
    }
    httpd_resp_sendstr_chunk(request, page_before_ntp);
    if(old_conf->ntp_server && strlen(old_conf->ntp_server)) {
        httpd_resp_sendstr_chunk(request, old_conf->ntp_server);
    }

    httpd_resp_sendstr_chunk(request, page_end);
    
    httpd_resp_send_chunk(request, NULL, 0);
    
    
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
        
        
        int ssid_len;
        char* ssid = get_string_in_post(ctx->content, "SSID=", &ssid_len);
        
        int pass_len;
        char* pass = get_string_in_post(ctx->content, "pass=", &pass_len);

        int pass_conf_len;
        char* pass_conf = get_string_in_post(ctx->content, "pass_confirm=", &pass_conf_len);
        
        int broker_len;
        char* broker = get_string_in_post(ctx->content, "broker=", &broker_len);
        
        int topic_len;
        char* topic = get_string_in_post(ctx->content, "topic=", &topic_len);
            

        int port_len;
        char* port_str = get_string_in_post(ctx->content, "port=", &port_len);

        int ap_ssid_len;
        char* ap_ssid = get_string_in_post(ctx->content, "ap_ssid=", &ap_ssid_len);

        int ap_pass_len;
        char* ap_pass = get_string_in_post(ctx->content, "ap_pass=", &ap_pass_len);

        int ap_pass_conf_len;
        char* ap_pass_conf = get_string_in_post(ctx->content, "ap_pass_confirm=", &ap_pass_conf_len);


        int ap_chan_len;
        char* ap_chan_str = get_string_in_post(ctx->content, "ap_chan=", &ap_chan_len);

        int ntp_len;
        char* ntp = get_string_in_post(ctx->content, "ntp=", &ntp_len);

        ssid[ssid_len] = 0;
        pass[pass_len] = 0;
        pass_conf[pass_conf_len] = 0;
        topic[topic_len] = 0;
        broker[broker_len] = 0;
        port_str[port_len] = 0;
        ap_ssid[ap_ssid_len] = 0;
        ap_pass[ap_pass_len] = 0;
        ap_pass_conf[ap_pass_conf_len] = 0;
        ap_chan_str[ap_chan_len] = 0;
        ntp[ntp_len] = 0;
        
        parse_url_encoded(ssid);
        parse_url_encoded(pass);
        parse_url_encoded(pass_conf);
        parse_url_encoded(topic);
        parse_url_encoded(broker);
        parse_url_encoded(ap_ssid);
        parse_url_encoded(ap_pass);
        parse_url_encoded(ap_pass_conf);
        parse_url_encoded(ntp);
        
        
        //TUERSCHILD_LOGW(tag, "conf: %*s %*s %*s %*s ", ssid_len, ssid, pass_len, pass, broker_len, broker, topic_len, topic);
        //TUERSCHILD_LOGW(tag, "conf: %s %s %s %s %s", ssid, pass, broker, topic, port_str);
        //TUERSCHILD_LOGW(tag, "field lengths: %d, %d, %d, %d, %d", ssid_len, pass_len, broker_len, topic_len, port_len);

        if( (strcmp(pass_conf, pass) == 0) && (strcmp(ap_pass_conf, ap_pass)  == 0) ) {
            TUERSCHILD_LOGW(tag, "saving new config");
            
            tuerschild_config_t* conf = request->user_ctx;
            //tuerschild_config_t new_conf = {.broker = broker, .password = pass, .port = atoi(port), .ssid = ssid, .topic = topic};
            //*conf = new_conf;
            
            if(pass_len && pass_conf_len && ap_pass_len && ap_pass_conf_len && ntp_len){
              conf_set(conf, ssid, pass, broker, atoi(port_str), topic, ap_ssid, ap_pass, atoi(ap_chan_str), ntp);  
            } else {
                conf_set_ssid(conf, ssid);
                conf_set_broker(conf, broker);
                conf_set_port(conf, atoi(port_str));
                conf_set_topic(conf, topic);
                conf_set_ap_ssid(conf, ap_ssid);
                conf_set_ap_chan(conf, atoi(ap_chan_str));
                conf_set_ntp(conf, ntp);
                if(pass_len && pass_conf) {
                    conf_set_pass(conf, pass);
                }
                if(ap_pass_len && ap_pass_conf_len) {
                    conf_set_ap_pass(conf, ap_pass);
                }
            }
            httpd_resp_send(request, "saved config", HTTPD_RESP_USE_STRLEN);
            xEventGroupSetBits(httpd_events, CONF_RECEIVED_BIT);
        } else {
            httpd_resp_send(request, "passwords differ", HTTPD_RESP_USE_STRLEN);
        }

        

        free(ctx->content);
        free(ctx);
        request->sess_ctx = NULL;
    }
    
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

    conf_get.user_ctx = conf;

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
        
    }
    *put = 0;
    return put - str;
}

char* get_string_in_post(char* post, const char* key_str, int* len)
{
    
    char* val_str = strstr(post, key_str) + strlen(key_str);
    char* end = strchr(val_str, '&');
    if(!end) {
        *len = strlen(val_str);
    } else 
    *len = end - val_str;

    return val_str;
}