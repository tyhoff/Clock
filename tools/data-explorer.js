var roomId;
var questionNumber;
var answer;
var fb;
var fireOnBool = true;

var fireLoad = function() {
  $('#loadRoom').click(function() {
    $('#roomData').html('');
    $('#requestFields').prop('disabled', false);

    roomId = $('#room').val();

    // this is to prevent having multiple fireOn callbacks
    if(fireOnBool) {
      fb = new Firebase('https://kirby.firebaseio.com/rooms/' + roomId);
      fireOnBool = false;
    } else {
      fb.off(); // this removes all callbacks
      fb = new Firebase('https://kirby.firebaseio.com/rooms/' + roomId);
    }

    fireOn();

  });
}

var fireOn = function() {
  fb.on('child_added', function(snapshot){
    var obj = snapshot.val();
    console.log(obj);
    $('#roomData').prepend(wellGenerator(snapshot.name(), obj.answer, obj.question_number));
  });
}

var fieldDisabler = function() {
  $('#room').change(function() {
    $('#requestFields').prop('disabled', true);
  });
}

var firePush = function() {
  $('#form').keydown(function(e) {
    if(e.keyCode == 13) {
      $('#submitRequest').click();
    }
  });

  $('#submitRequest').click(function() {
    questionNumber = parseInt($('#questionNumber').val());
    answer = parseAnswer($('#answer').val());

    fb.push({
      'question_number': questionNumber,
      'answer': answer
    });
  });
}

var wellGenerator = function(id, ans, qnum) {
  return "<div class='well well-sm'><span>" + id + "<br>Question Number: " + qnum + " | Answer: " + ans + "</span></div>";
}

var parseAnswer = function(a) {
  var value = 0;
  if(a != '0') {
    value = a.charCodeAt(0);
  }
  return value;
}

$(function() {
  fireLoad();
  fieldDisabler();
  firePush();
});

// var roomId = 'KIRBY';
//
// var fb = new Firebase('https://kirby.firebaseio.com/rooms/' + roomId);
//
// var first = true; // so we don't get one unless it's new
//
// console.log("blah");
//
// // fb.endAt().limit(1).on('child_added', function(snapshot) {
// fb.limit(1).on('child_added', function(snapshot) {
//   console.log("ON");
//   if(!first) {
//     console.log("ON-if");
//     var data = snapshot.val();
//     console.log(data);
//     // msg = gen_message(data.question_number, data.answer);
//     // Pebble.sendAppMessage( msg, send_msg_success, send_msg_fail );
//   } else {
//     first = false;
//   }
// });
