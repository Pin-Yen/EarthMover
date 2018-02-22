function post() {
  
  return new Promise(function(resolve, reject) {
    
    var req = new XMLHttpRequest();
    req.open('POST', '/usage');

    req.onreadystatechange = function() {

      if (req.readyState == 4 && req.status == 200) {
        resolve(JSON.parse(req.responseText));
      } 
    }

    req.send();

  });

}
function dateTimeString(date) {
  return date.toTimeString() + ', ' + date.toDateString();
}
function update() {
  post().then(function onSuccess(resBody) {

    // RAM usage
    $('#rss').html('RAM usage: ' + (Math.round(resBody.rss / (1024*1024))).toString() + ' MB');
    // Update time.
    var date = new Date(resBody.timestamp *1000);
    $('#update-time').html(dateTimeString(date));

    var tableRow = $('.ai-row');
    tableRow.each(function populateEachRow(index) {
      var ai = resBody['ai'][index];
      if (! ai)
        return false;
      $(this).children('.index').html(ai.id);
      var date = new Date(ai.lastAlive*1000);
      $(this).children(".timestamp").html(dateTimeString(date));
    });
  });

  setTimeout(update, 20000);

}

update();

