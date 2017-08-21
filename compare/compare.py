import const as const
import player as player

class Compare(object):
  def __init__(self, player1, player2, rounds):
    self.player1 = player1
    self.player2 = player2
    self.rounds = rounds

  def run(self):

    # play self.rounds matches
    for round in range(self.rounds):
      if round % 2 == 0:
        self.match(self.player1, self.player2)

      else:
        self.match(self.player1, self.player2)

  def match(self, black, white):
    """
    Plays a match.
    """

    previousPlayRow, previousPlayCol = (None, None)
    totalMoves = 0

    opponentPassed = False # records if the opponent passed on its previous move.

    while True:
      if totalMoves % 2 == 1:
        # black's turn
        black.play(previousPlayRow, previousPlayCol)
        previousPlayRow, previousPlayCol, whoWin = black.think()
      else:
        # white's turn 
        white.play(previousPlayRow, previousPlayCol)
        previousPlayRow, previousPlayCol, whoWin = white.think()  

      totalMoves += 1

      # Check if both players passed.
      if previousPlayRow == -1 and previousPlayCol == -1:
        if opponentPassed:
          whoWin = const.RESULT_TIE
        else:
          opponentPassed = True
      else:
        opponentPassed = False

      # If Game ends, record the result
      if whoWin != const.RESULT_NONE:
        if whoWin == const.RESULT_BLACK_WIN:
          black.incrementWin()
          white.incrementLose()
        elif whoWin == const.RESULT_WHITE_WIN:
          white.incrementWin()
          black.incrementLose()
        elif whoWin == RESULT_TIE:
          black.incrementTie()
          white.incrementTie()

        return
