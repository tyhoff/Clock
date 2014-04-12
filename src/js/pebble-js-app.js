var last_request_forwarded = null;
var last_request_accepted = null;

function gen_test_message(){
  return { "question_number": 69 };
}

function send_msg_success( e ){
  ;
}

function send_msg_fail( e ){ console.log(e.error.message);
}

Pebble.addEventListener("ready", function(e) {
  console.log(e.ready);
  console.log("Hello world! - Sent from your javascript application.");
  // call firebase listening here
  test_msg = gen_test_message();
  console.log(test_msg);
  Pebble.sendAppMessage( test_msg, send_msg_success, send_msg_fail );
});

/*
function handle_new_firebase_message( var msg ){
  if ( msg.room_id === my_room_id ){
    if ( msg.)

  }
}
*/

Pebble.addEventListener("appmessage", function(e) {
  var msg = e.payload;
  console.log( JSON.stringify(e.payload) );
  if ( msg.question_number ){
  } else if ( msg.answer ){
  } else {
    console.log("unknown message");
    console.log(msg);
  }
});
