#include "tuerschild.h"

volatile int topic_in_len = 0;
volatile int data_in_len = 0;

volatile char topic_in_buf[TOPIC_IN_MAX_LEN];
volatile char data_in_buf[DATA_IN_MAX_LEN];
