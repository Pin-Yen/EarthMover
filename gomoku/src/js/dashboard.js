
// google charts
google.charts.load('current', {'packages':['gauge']});
var gaugeManager= new GaugeManager();
google.charts.setOnLoadCallback(update);




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

    // // RAM usage
    // $('#rss').html('RAM usage: ' + (Math.round(resBody.rss / (1024*1024))).toString() + ' MB');
    // Update time.
    var date = new Date(resBody.timestamp *1000);
    $('#update-time').html(dateTimeString(date));


    // rss ,vsz, thread
    var rss = Math.round(resBody.rss / (1024*1024));
    var vsz = Math.round(resBody.vsz/ (1024*1024));

    gaugeManager.updateRssGauge(rss);
    gaugeManager.updateVszGauge(vsz);
    gaugeManager.updateThreadGauge(resBody.num_threads);


    // AI info.
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

  setTimeout(update, 15000);

}
function GaugeManager() {
  var rssGauge, vszGauge, threadGauge;

  function updateRssGauge(rss) {
    rssGauge = rssGauge || new google.visualization.Gauge(document.getElementById('rss-gauge'));
    var data = google.visualization.arrayToDataTable([
      ['Label', 'Value'],
      ['RSS (MB)', rss],
    ]);
    var options = {
      width: 600, height: 180,
      redFrom: 275, redTo: 300,
      yellowFrom:200, yellowTo: 275,
      minorTicks: 5,
      max : 300
    };
    rssGauge.draw(data, options);
  }

  function updateVszGauge(vsz) {
    vszGauge = vszGauge || new google.visualization.Gauge(document.getElementById('vsz-gauge'));
    var data = google.visualization.arrayToDataTable([
      ['Label', 'Value'],
      ['VSZ (MB)', vsz],
    ]);
    var options = {
      width: 600, height: 180,
      redFrom: 900, redTo: 1000,
      yellowFrom:650, yellowTo: 900,
      minorTicks: 5,
      max : 1000
    };
    vszGauge.draw(data, options);
  }

  function updateThreadGauge(thread) {
    threadGauge = threadGauge || new google.visualization.Gauge(document.getElementById('thread-gauge'));
    var data = google.visualization.arrayToDataTable([
      ['Label', 'Value'],
      ['Thread', thread],
    ]);
    var options = {
      width: 600, height: 180,
      redFrom: 10, redTo: 12,
      yellowFrom:5, yellowTo: 10,
      minorTicks: 5,
      max : 12
    };
    threadGauge.draw(data, options);
  }

  return {
    updateRssGauge : updateRssGauge,
    updateVszGauge : updateVszGauge,
    updateThreadGauge : updateThreadGauge
  }
}




