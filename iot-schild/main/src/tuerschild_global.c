/****************************
 * global buffers for mqtt reception, not volatile, because event handlers are not called from interrupts
 ***************************/


#include "tuerschild.h"



int topic_in_len = 0;   //lenght of topic received
int data_in_len = 0;    //length of raw message received

//one extra for zero termination
char topic_in_buf[TOPIC_IN_MAX_LEN+1];    //topic on which we received the message, typically exactyl the same as the topic in configuration
char data_in_buf[DATA_IN_MAX_LEN+1];      //raw message we received over mqtt
