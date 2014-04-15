#include "common.h"
static char *translate_error(AppMessageResult result);

extern int32_t question_number;
extern int32_t answer;
extern uint8_t all_answers[120];
extern uint8_t all_answers_length;

enum {
  QUESTION_NUMBER   = 0x0,
  ANSWER            = 0x1,
  ALL_ANSWERS       = 0x2
};

static void out_sent_handler( DictionaryIterator *sent, 
                            void *context ) {
  // outgoing message was delivered
  APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage - Message sent" );
}

static void out_failed_handler( DictionaryIterator * failed,
                                AppMessageResult reason,
                                void * context ) {
  // outgoing message failed
  APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage - Outbox failure - %s", translate_error(reason) );
  
  // retry on failure
  app_message_outbox_send();
}

static void in_received_handler( DictionaryIterator * received, 
                                 void * context ) {
  // incoming message received
  APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage - Message received" );

  // check if it is a list of all answers
  Tuple * all_answers_tuple = dict_find( received, ALL_ANSWERS );
  if (all_answers_tuple != NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "List of answers array received. len: %d\n", all_answers_tuple->length );
    memcpy(all_answers, all_answers_tuple->value->data, all_answers_tuple->length);
    all_answers_length = all_answers_tuple->length;
    return;
  }

  //check if it is a question
  // TODO - Log this a little more.

  Tuple * question_number_tuple = dict_find( received, QUESTION_NUMBER );
  Tuple * answer_tuple          = dict_find( received, ANSWER );
  int32_t new_question = 0;
  int32_t new_answer = 0;
  if ( question_number_tuple != NULL ){
    new_question = question_number_tuple->value->int32;  
  }
  if ( answer_tuple != NULL ){
    new_answer = answer_tuple->value->int32;  
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "new_answer has value %ld", new_answer );
  APP_LOG(APP_LOG_LEVEL_DEBUG, "new_question has value %ld", new_question );

  if ( new_question > 0 && new_answer > 0 ){
    // this is a ANSWER_BROADCAST
    question_number = ( question_number_tuple->value->int32 );
    answer = (answer_tuple->value->int32);
    receive_answer_init();
  }
  if ( new_question > 0 && new_answer <= 0 ){
    if ( question_number == new_question ){
      // this is a QUESTION REQUEST RESPONSE
      // should display some feedback to the user saying your classmate
      // is working on your question
      question_number = new_question;
      trigger_answer_pending_notifier();
    } else {
      // QUESTION REQUEST
      // should trigger request_notifier
      question_number = new_question;
      trigger_answer_request_notifier();
    }
  }
}

static void in_dropped_handler( AppMessageResult reason, void * context ) {
  // incoming message dropped
  APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage - Dropped incoming message\n" );
}

void send_msg( int32_t question_number_value, int32_t answer_value){
  DictionaryIterator * outbox_iter;
  if ( app_message_outbox_begin(&outbox_iter) != APP_MSG_OK ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage - Opening outbox failed\n" );
    return;
  }
  if ( outbox_iter == NULL ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage - Outbox iterator is NULL\n" );
    return;
  }
  if ( question_number_value != -1 ){
    dict_write_int32( outbox_iter,
                      QUESTION_NUMBER,
                      question_number_value );
  }
  if ( answer_value != -1 ){
    dict_write_int32( outbox_iter,
                      ANSWER,
                      answer_value );
  }
  if ( dict_write_end( outbox_iter ) == 0 ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage - the parameters for writing were invalid" );
  }
  app_message_outbox_send();
}


void app_msg_init(){
  /* register message handlers  */
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  /* message handling init */
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
}

static char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}
