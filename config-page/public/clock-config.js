var getParameterByName = function(name) {
  name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
  var regex = new RegExp( "[\\?&]" + name + "=([^&#]*)"),
                          results = regex.exec(location.search);
  return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
};

function simpleObjInspect(oObj, key, tabLvl) {
    key = key || "";
    tabLvl = tabLvl || 1;
    var tabs = "";
    for(var i = 1; i < tabLvl; i++){
        tabs += "\t";
    }
    var keyTypeStr = " (" + typeof key + ")";
    if (tabLvl == 1) {
        keyTypeStr = "(self)";
    }
    var s = tabs + key + keyTypeStr + " : ";
    if (typeof oObj == "object" && oObj !== null) {
        s += typeof oObj + "\n";
        for (var k in oObj) {
            if (oObj.hasOwnProperty(k)) {
                s += simpleObjInspect(oObj[k], k, tabLvl + 1);
            }
        }
    } else {
        s += "" + oObj + " (" + typeof oObj + ") \n";
    }
    return s;
}

$(function() {
  var item = window.localStorage.getItem('config');
  var roomId;
  var config;

  if ( item === null || item.length === 0 ){
    config = {};
  } else {
    config = JSON.parse(item);
    console.log("CCCCCCCCONFIG" + simpleObjInspect(item));

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
