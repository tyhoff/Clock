var getParameterByName = function(name) {
  name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
  var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
    results = regex.exec(location.search);
  return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

$(function() {
  var roomId = getParameterByName('room-id');

  if(roomId && roomId !== "") {
    $('#room-id').val(roomId);
  }

  $('#submit-room').click(function(){
    roomId = $('#room-id').val();
    window.location.href = "pebblejs://close#" + roomId;
  });
});
