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
    if ( config.hasOwnProperty("vib-status") ){
      vibStatus = config['vib-status'];
      $('#vib-toggle').prop('checked', vibStatus );
    }
  }

  $('#submit').click(function(){
    if ( typeof config === 'string' ){
      config = {};
    }
    roomId = $('#room-id').val();
    config['room-id'] = roomId;
    config['vib-status'] = $('#vib-toggle').is(':checked');

    var jsconfig = JSON.stringify(config);
    window.localStorage.setItem('config', jsconfig);
    window.location.href = "pebblejs://close#" + encodeURIComponent( jsconfig );
  });
});
