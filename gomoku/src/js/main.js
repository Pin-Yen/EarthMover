var timer = { 'black': null, 'white': null };

var player = { 'black': 'human', 'white': 'computer'};

var game = {black : "N/A", white : "N/A", rule : "N/A", startTime : -1,
            earthmover : {level : -1, version : 0}};
var gameID;

var board = new Board();

var dialog = new Dialog();

var sessionManager = new SessionManager(); 

function notifyWinner(winnerColor) {
  // TODO: display game status
  setTimeout(function() { alert(winnerColor + " wins !"); }, 0);
  board.enable = false;
  board.gameStarted = false;
  $('.ctrl-replay input').prop('disabled', false);
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);

  // write winner to database
  firebase.database().ref(gameID).child('result').set(winnerColor);
}

function post(params, path) {
  
  return new Promise(function(resolve, reject) {
    
    var req = new XMLHttpRequest();
    req.open('POST', path);

    req.onreadystatechange = function() {

      if (req.readyState != 4) {
        // Response not ready yet.
        return;

      } else if(req.status == 200) {
        // OK, resolve promise with JSON response
        resolve(JSON.parse(req.responseText));

      } else if (req.status == 204){
        resolve();

      } else {
        // Failed, reject.
        reject(req.responseText);
      }
    }

    // add session id to payload.
    params = params || {};
    params.sessionId = sessionManager.getSessionID();
    
    req.send(JSON.stringify(params));

  });

}

// Handles the next round of game.
function checkNextPlayer() {
  if (humanTurn()) {
    board.draw(board.mousePos);
    board.enable = true;
    $('.ctrl-game input').prop('disabled', false);
    $('.ctrl-analyze input').prop('disabled', false);
    if (board.playNo == 0 || (board.playNo == 1 && player['black'] == 'computer'))
      $('#ctrl-undo').prop('disabled', true);
  
  } else {
    post(null, 'think').then(function (response) {
       putAiPoint(response.row, response.col);
        if (checkWinner(response))
          return;
        checkNextPlayer();
    }).catch(function onError(error) {
      alert('think failed');
    });
  }

  timer[board.whoTurn()].start();
}

// Returns true if someone wins.
function checkWinner(response) {
  if (response.winner != -1) {
    notifyWinner(response.winner == 0 ? 'Black' : 'White');
    return true;
  }
  return false;
}

// Puts AI's move onto the chessboard.
function putAiPoint(row, col) {
  if (row == -1) {
    board.pass();
    alert('computer pass'); //computer pass
  } else {
    board.play([col, row]); // play at ai's respond point
  }

  if (col == board.mousePos[0] && row == board.mousePos[1])
    board.mousePos = [-1, -1];

}

function humanTurn() {
  return player[board.whoTurn()] == 'human';
}

function btnPlayNumberClick() {
  board.changePlayNumber();

  $('#play-number-check').toggle();
}

function btnCoordinateClick() {
  board.changeCoordinate();

  $('#coordinate-check').toggle();
}

function btnWatchGameClick() {
  var gameIdPrompt = prompt("Please paste the game ID here");

  if (gameIdPrompt != null) {
    gameID = gameIdPrompt.trim();

    // clear the board
    board.init();

    // check if game id exists. If existing, populate player data.
    var gameRef = firebase.database().ref(gameID);
    gameRef.once('value').then(function(snapshot) {

      if (snapshot.val() == null) {
        alert("Sorry, the game ID you entered is invalid");
        return;
      }

      // deside player's place
      if (snapshot.child('black').val() == 'human') {
        // black == human, white == human -> sel: black, human, opp: white, human
        // black == human, white == ai -> sel: black, human, opp: white, ai
        dialog.initPlayer('sel', 'black', true);
        dialog.initPlayer('opp', 'white', (snapshot.child('white').val() == 'human'));
      } else {
        if (snapshot.child('white').val() == 'human') {
        // black == ai, white == human -> sel: white, human, opp: black, ai
        dialog.initPlayer('sel', 'white', true);
        dialog.initPlayer('opp', 'black', false);
        } else {
          // black == ai, white == ai -> sel: black, ai, opp: white, ai
          dialog.initPlayer('sel', 'black', false);
          dialog.initPlayer('opp', 'white', false);
        }
      }
    });


    gameRef.child('record').on('child_added', function(data) {
      console.log('key=' + data.key +",row=" + data.val().r + ",col=" + data.val().c);
      board.put([data.val().r, data.val().c], data.key);
    });
  }
}

function changeDisplayNo(changeAmount) {
  board.changeDisplayNo(changeAmount);
}

function undo() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  var times = (player[board.whoTurn(board.playNo + 1)] === 'human' ? 1 : 2);
  board.undo(times);

  post({ times: times }, 'undo').then(function (response) {
    checkNextPlayer();
  }).catch(function onError(error) {
    alert('undo failed');
  });
}

function pass() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  board.pass();
  post(null, 'pass').then(function (response) {
    checkNextPlayer();
  }).catch(function onError(error) {
    alert('pass failed');
  });
}

function resign() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  timer[board.whoTurn()].stop();
  post(null, 'resign').then(function (response) {
    notifyWinner(board.whoTurn(board.playNo + 1));

  }).catch(function onError(error){
    alert("resign failed");
  });
}

function hint() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  board.enable = false;

  post(null, 'think').then(function (response) {
    putAiPoint(response.row, response.col);
    if (checkWinner(response))
      return;
    checkNextPlayer();
  
  }).catch(function onError(error) {
    alert("think failed");
  });
}

var analyzing = false

function analyzeClick() {
  analyzing = !analyzing;

  $('#analyze-check').toggle();

  $('.player-information').toggle();

  $('#tree-visualize').toggle();

  if (analyzing) {
    $('.board').addClass('aside');
    D3.requestTree();
  } else {
    $('.board').removeClass('aside');
    D3.removeTree();
  }
}

function SessionManager() {
  
  var sessionID;

  function getSessionID() {
    return sessionID || generateSessionID();
  }

  function generateSessionID() {
    
    const charset = "abcdefghijklmnopqrstuvwxyz0123456789";
    
    sessionID = "";
    for (let i = 0; i < 10; ++i) 
      sessionID += charset.charAt(Math.floor(Math.random() * charset.length));

    return sessionID;
  }

  return {getSessionID: getSessionID};
}

function refresh() {
  D3.requestTree();
}

// keep alive when game is in progress
// lastResponse : whether the previous keepAlive request success or not
function keepAlive(prevResponse) {
  
  if(board.gameStarted) {
    post(null, 'keepAlive').then(function onSuccess() {
      setTimeout(keepAlive, 15000, true);
    }).catch(function onError() {
      if (prevResponse) {
        alert('Sorry, there are some issues with your game.\n'+
          '1. Check your internet connection.\n' + 
          '2. Your game may be terminated by the server due to long ' +
          'period of inactivity.')
      }
      setTimeout(keepAlive, 15000, false);

    });

  }
}

setTimeout(keepAlive, 15000, true);

// Quit game when user leaves.
window.addEventListener('unload', function onUnload() {
  post(null, '/quit');
});

// Confirm leave.
// window.onbeforeunload = function (e) {
//   e.returnValue = 'test';
//   return 'Are you sure you want to quit this game ?'
// }
window.addEventListener('beforeunload', function onBeforeUnload(e) {
  e.returnValue = 'Are you sure you want to quit this game ?';
  return 'Are you sure you want to quit this game ?'
});