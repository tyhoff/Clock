# Pebble Cheat API and internal documentation

## Message structure:
  These fields are present in every message
      uint8_t uuid
      uint8_t room_id
      uint8_t sender_id
      uint8_t recipient_id

  ANSWER_REQUEST
  
      uint8_t question_number
      
  ANSWER_REQUEST_ACCEPTED
  
      uint8_t request_id
      
  ANSWER_BROADCAST
  
      uint8_t question_number
      char    answer


## Windows

### home screen - `window`
Just has a clock, will have an indicator of some kind when a request comes in
  
Possible Transitions:
- `send_request` - appears: when the user triggers the request mechanism
- `fill_request` - appears: when the user accepts a request

  Messages:
    can send `ANSWER_REQUEST_ACCEPTED`

### send request window - `send_request`
purpose: user wants to request an answer

Possible Transitions
  
- `window`
exit status from this window could be:
- success - the request was sent
- failure - user aborted the request before it was sent

  Messages:
    Will send `ANSWER_REQUEST` on success

### answer an incoming request - `fill_request`
purpose: user is answering classmate's query

Possible Transitions
- `window`
exit status from this window could be:
- success - the request was sent
- failure - user aborted the request before it was sent

  Messages:
    will send `ANSWER_BROADCAST` on success
