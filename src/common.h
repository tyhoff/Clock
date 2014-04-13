#ifndef COMMON_H
#define COMMON_H
#include <pebble.h>

// global functions
void fill_request_init( void );
void app_msg_init( void );
void receive_answer_init( void );
void all_answers_init( void );
void trigger_answer_request_notifier(void);
void trigger_answer_pending_notifier(void);
void send_msg( int32_t question_number_value, int32_t answer_value);

char *itoa(int num);

void accept_request_init();
void send_request_init();

#endif
