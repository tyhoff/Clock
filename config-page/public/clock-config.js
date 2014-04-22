var getParameterByName = function(name) {
  name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
  var regex = new RegExp( "[\\?&]" + name + "=([^&#]*)"),
                          results = regex.exec(location.search);
  return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
};

$(function() {
  var item = window.localStorage.getItem('config');
  var roomId;
  var config;

  if ( item === null || item.length === 0 ){
    config = {};
  } else {
    config = JSON.parse(item);
    if ( config.hasOwnProperty("room-id") ){
      roomId = config['room-id'];
      $('#room-id').val(roomId);
    }
  }

  $('#submit').click(function(){
    roomId = $('#room-id').val();
    config['room-id'] = roomId;

    config['vib-toggle'] = $('#vib-toggle').is(':checked');

    window.localStorage.setItem('config', config);

    var js_conf = JSON.stringify(config);
    console.log( js_conf );
    window.location.href = "pebblejs://close#" + encodeURIComponent( js_conf );
  });
});
