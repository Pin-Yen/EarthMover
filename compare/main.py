import compare as compare
import player as player
import const as const

import argparse as argparse
import subprocess as subprocess
import time as time

def main():
  # parse args
  args = parseArgs()

  # execute AIs
  subprocess.Popen(['../' + args['program1'], str(args['port1'])], bufsize=0, stdout = subprocess.PIPE)
  subprocess.Popen(['../' + args['program2'], str(args['port2'])], bufsize=0, stdout = subprocess.PIPE)

  time.sleep(3)
  # create player
  rule = const.RULE_FREESTYLE if args['rule'] == 'freestyle' else const.RULE_RENJU_BASIC
  player1 = player.Player(args['port1'], rule, args['program1_level'])
  player2 = player.Player(args['port2'], rule, args['program2_level'])

  # compare
  comp = compare.Compare(player1, player2, args['rounds'])
  comp.run()

  # print result
  printResult(args, player1, player2)

def parseArgs():
  """
  parse arguments from the command line,
  returns a dictionary.
  """
  parser = argparse.ArgumentParser()

  parser.add_argument('program1', help='The filename of the first AI to be compared')
  parser.add_argument('program1_level', type=int, help='The level of program 1, 0 is the weakest and 2 is the strongest')
  parser.add_argument('port1', type=int, help='Port of the first AI')
  parser.add_argument('program2', help='The filename of the second AI to be compared')
  parser.add_argument('program2_level', type=int, help='The level of program 2, 0 is the weakest and 2 is the strongest')  
  parser.add_argument('port2', type=int, help='Port of the first AI')
  parser.add_argument('--rounds', required=True, type=int, help='Specifies the number of matches to play')
  parser.add_argument('--rule', default='renju_basic', choices=['freestyle', 'renju_basic'],\
   help='Specifies the rule, should be \'renju_basic\' or \'freestyle\'. Defaults to renju_basic')

  return vars(parser.parse_args())

def printResult(args, player1, player2):
  rounds = args['rounds']

  # print general info
  print('\ntotal matches:', rounds)
  print('rule:', args['rule'])
  
  print('---------------------------------------')

  # print header
  print('{:10}{:>6}{:>6}{:>6}{:>6}'.format("Name","level", "WinR", "LoseR", "TieR"))

  for player in [player1, player2]:
    print('{name:10}{level:6}{winrate:6.1f}{loserate:6.1f}{tierate:6.1f}'\
      .format(name=args['program1'], level=player.level, winrate=player.winCount/rounds * 100\
        , loserate=player.loseCount/rounds * 100, tierate=player.tieCount/rounds * 100))

if __name__ == '__main__':
  main()