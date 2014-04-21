var getParameterByName = function(name) {
  name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
  var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
    results = regex.exec(location.search);
  return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

$(function() {
  var config = localStorage.get_item('config')
  var roomId = config["room-id"];

  if( roomId ) {
    $('#room-id').val(roomId);
  }

  $('#submit').click(function(){
    roomId = $('#room-id').val();
    config["room-id"] = roomId;

    config["vib-toggle"] = $('#vib-toggle').is(':checked');

    localStorage.setItem('config', config);

    window.location.href = "pebblejs://close#" +
                            encodeURIComponent(JSON.stringify(config));
  });
});
