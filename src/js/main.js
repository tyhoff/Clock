function ajax(url, method, data, callback) {
  var req = new XMLHttpRequest();
  req.open(method, url, true);
  req.onload = function(e) {
    if (req.readyState === 4 && req.status === 200) {
      if (req.status === 200) {
        callback(JSON.parse(req.responseText));
      } else {
        console.log("HTTP Error: " + JSON.stringify(req));
      }
    }
  };
  req.send(data);
}

function getReq(url, callback) {
  return ajax(url, 'GET', null, callback);
}

var bytearray;
var last_sent_time = 0;
var config;

function gen_message(question_number, answer){
  return { "question_number": question_number, "answer": answer };
}

function send_msg_success( e ){
  console.log("SUCCESS - " + e);
}

function send_msg_fail( e ){
  console.log(e.error.message);
}

function parseFireBaseAnswerData(data) {
  var table = [];
  var bytearray = [];

  for (var key in data) {
    var response = data[key];

    if (table[response.question_number] === undefined) {
      table[response.question_number] = [];
    }
    if (response.answer !== 0) {
      table[response.question_number].push(response.answer);
    }
  }

  for (var key in data) {
    var response = data[key];
    if (table[response.question_number] !== undefined &&
        table[response.question_number].length === 0) {
      delete(table[response.question_number]);
    }
  }

  var compact_table = [];

  for (var i = 0; i < table.length; i++) {
    var list = table[i];
    if (list === undefined) {
      continue;
    }

    console.log(" list(length "+list.length+") at index "+ i + " contains ");

    for (var j = 0; j < list.length; j++) {
      if (compact_table[i] === undefined){
        compact_table[i] = new Array(4);
        for (var k = 0; k < 4; k++){
          compact_table[i][k] = 0;
        }
      }
      var normalized_index = list[j] - 65;
      compact_table[i][normalized_index] += 1;
    }
  }

  for (var i = 0; i < compact_table.length; i++){
    if (compact_table[i] === undefined ){
      continue;
    }
    // check if any of the options A(0), B(1).. have data
    hasAnswers = false;
    for (var j = 0; j < 4; j++){
      if (compact_table[i][j] > 0 ){
        hasAnswers = true;
        break;
      }
    }
    if (hasAnswers){
      // only if this number has answers will we push it onto the array
      bytearray.push(i);
      for (var j = 0; j < 4; j++){
        bytearray.push(compact_table[i][j]);
      }
    }
  }

  Pebble.sendAppMessage({"2":bytearray});
}

var getAndSetRoomId = function(callback) {
  console.log("Ready begin");
  var config_str = window.localStorage.getItem('config');
  if(config_str) {
    config = JSON.parse( config_str );
    var roomId = config["room-id"];
    console.log(JSON.stringify(config));
    console.log(typeof config);
    callback(roomId);
  } else {
    Pebble.showSimpleNotificationOnPebble("Woah there!",
                                          "Set the room inside of Clock on your phone.");
  }
}

var fireOn = function(roomId) {
  Firebase.INTERNAL.forceWebSockets();
  console.log("FireOn RoomId - " + roomId);

  var fb = new Firebase('https://kirby.firebaseio.com/rooms/' + roomId);

  var first = true; // so we don't get one unless it's new

  fb.endAt().limit(1).on('child_added', function(snapshot) {
    if(!first) {
      var data = snapshot.val();
      var current_time = Date.now();
      var time_diff = current_time - last_sent_time;
      if ( time_diff > 100 ){
        var msg = gen_message(data.question_number, data.answer);
        Pebble.sendAppMessage( msg, send_msg_success, send_msg_fail );
      }
    } else {
      first = false;
    }
  });
  getReq('https://kirby.firebaseio.com/rooms/' + roomId + '/.json', parseFireBaseAnswerData);

  fb.on('value', function (snapshot) {
    var data = snapshot.val();
    parseFireBaseAnswerData(data);
  });
}

Pebble.addEventListener("ready", function(e) {
  // localStorage.removeItem('config');
  getAndSetRoomId(fireOn);
});

Pebble.addEventListener("appmessage", function(e) {
  var roomId = config['room-id'];
  console.log("AppMessage - " + roomId);
  var fb = new Firebase('https://kirby.firebaseio.com/rooms/' + roomId );
  var msg = e.payload;

  var new_msg = {'question_number': 0, 'answer': 0};

  if ( typeof msg.question_number !== undefined ){
    new_msg.question_number = msg.question_number;
  }
  if ( msg.answer && typeof msg.answer !== undefined ){
    new_msg.answer = msg.answer;
  }
  last_sent_time = Date.now();
  fb.push( new_msg );
});

Pebble.addEventListener("showConfiguration", function (e) {
  var url = "https://www.cs.purdue.edu/homes/sopell/clock-config.html";
  Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function (e) {
  if (!e.response){
    return;
  }
  var json = decodeURIComponent( e.response );
  config = JSON.parse( json );

  console.log("room id is " + config["room-id"] );
  console.log("vibration status is " + config["vib-toggle"] );

  window.localStorage.setItem( 'config', JSON.stringify( config ) );
  getAndSetRoomId(fireOn);
});
