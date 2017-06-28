var Dialog = function() {
  this.dialog = $('#dialog-new-game');

  $('#dl-ok').click(this.ok.bind(this));
  $('#dl-cancel').click(this.toggle.bind(this));
  $('#new-game').click(this.toggle.bind(this));
};

Dialog.prototype.ok = function() {
  // get the black/white player's radio buttons' result
  var black = $('input[name="black"]:checked').val(),
      white = $('input[name="white"]:checked').val();

  if ($('#dl-name-black').val().trim() == '')
    $('#dl-name-black').val('you');
  if ($('#dl-name-white').val().trim() == '')
    $('#dl-name-white').val('you');

  player = {'black': black, 'white': white};

  var rule = parseInt($('input[name="rule"]:checked').val(), 10), level = parseInt($('#dl-select').val(), 10);

  post({rule: rule, level: level}, 'start');

  // deside player's place
  if (black == 'human') {
    // black == human, white == human -> sel: black, human, opp: white, human
    // black == human, white == ai -> sel: black, human, opp: white, ai
    this.initPlayer('sel', 'black', true);
    this.initPlayer('opp', 'white', (white == 'human'));
  } else {
    if (white == 'human') {
      // black == ai, white == human -> sel: white, human, opp: black, ai
      this.initPlayer('sel', 'white', true);
      this.initPlayer('opp', 'black', false);
    } else {
      // black == ai, white == ai -> sel: black, ai, opp: white, ai
      this.initPlayer('sel', 'black', false);
      this.initPlayer('opp', 'white', false);
    }
  }

  // initialize board
  board.init();

  // initialize timer
  if (timer.black != null) timer.black.stop();
  if (timer.white != null) timer.white.stop();
  timer = {black: new Timer($('.black .pi-timer')), white: new Timer($('.white .pi-timer'))};

  $('.ctrl-replay input').prop('disabled', true);
  $('.ctrl-game input').prop('disabled', false);
  $('.ctrl-analyze input').prop('disabled', false);

  this.toggle();

  // populate 'game' variable
  game.rule = rule;
  game.earthmover.level = level;
  game.black = black;
  game.white = white;
  game.startTime = firebase.database.ServerValue.TIMESTAMP;

  var databaseRef = firebase.database().ref().push();
  gameID = databaseRef.key;
  databaseRef.set(game);
};

// initialize player
// NOTE: player should be 'sel' or 'opp', color should be 'black' or 'white'
// human is bool
Dialog.prototype.initPlayer = function(player, color, human) {
  var oppColor = (color == 'black' ? 'white' : 'black');

  // set color
  $('.player-information.' + player).removeClass(oppColor);
  $('.player-information.' + player).addClass(color);

  // set chess
  $('#pi-chess-' + player).attr('src', 'gomoku/src/png/chess_' + color + '.png');
  $('#pi-icon-' + player).attr('src', 'gomoku/src/png/' + (human ? 'human' : 'icon') + '.png');

  // set name
  $('#pi-name-' + player).html((human ? $('#dl-name-' + color).val() : 'EarthMover'));
}

Dialog.prototype.toggle = function() {
  this.dialog.toggle();
};