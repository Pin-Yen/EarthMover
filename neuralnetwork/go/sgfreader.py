import board as b

import json
import os
import random

def output_board(board, last_move, next_move, black_turn, file, counter):
    file.write(str(counter) + '\n')

    for row in board:
        for data in row:
            file.write(data)
        file.write('\n')

    file.write(str(last_move[0]) + ',' + str(last_move[1]) + ',')
    file.write(str(next_move[0]) + ',' + str(next_move[1]) + ',')
    if black_turn:
        file.write('b')
    else:
        file.write('w')
    file.write('\n')

# Rotate and mirror
# There are 8 kinds of results, (4 rotate * 2 mirror),
# key is used to decide result.
def rotateMirror(board, last_move, next_move):
    key = random.randrange(8)
    for m in range(2):
        for r in range(4):
            if key == m * 4 + r:
                return
            rotate(board, last_move, next_move)
        mirror(board, last_move, next_move)

def rotate(board, last_move, next_move):
    temp = [[0] * 19 for _ in range(19)]
    # Rotate 90 degrees clockwise (row -> col, col -> 18 - row).
    for row in range(19):
        for col in range(19):
            temp[col][18 - row] = board[row][col]

    for row in range(19):
        board[row] = temp[row]

    temp = next_move[0]
    next_move[0] = next_move[1]
    next_move[1] = 18 - temp

    if last_move[0] != -1:
        temp = last_move[0]
        last_move[0] = last_move[1]
        last_move[1] = 18 - temp

def mirror(board, last_move, next_move):
    temp = [[0] * 19 for _ in range(19)]
    # Mirror (row -> col, col -> row).
    for row in range(19):
        for col in range(19):
            temp[col][row] = board[row][col];

    for row in range(19):
        board[row] = temp[row]

    temp = next_move[0]
    next_move[0] = next_move[1]
    next_move[1] = temp

    if last_move[0] != -1:
        temp = last_move[0]
        last_move[0] = last_move[1]
        last_move[1] = temp

def next(key):
    if key[0] == 'T':
        return None, None
    if len(key) <= 3:
        return None, None
    black_turn = True if key[0] == 'B' else False
    row = ord(key[2]) - 97
    col = ord(key[3]) - 97
    if not (0 < row < 18 and 0 < col < 18):
        return None, None
    return [row, col], black_turn


# Check this game's player rank.
# Both player;s rank should be 5d+.
def checkRank(data):
    whiteIndex = data.find('WR[')
    blackIndex = data.find('BR[')
    if whiteIndex == -1 or  blackIndex == -1:
        return False
    whiteRank = int(data[whiteIndex + 3 : whiteIndex + 4])
    blackRank = int(data[blackIndex + 3 : blackIndex + 4])
    if whiteRank < 5 or blackRank < 5:
        return False
    return True

def getHandicap(data):
    index = data.find('HA[')
    if index == -1:
        return 0
    while data[index] != '[':
        index += 1
    return int(data[index + 1 : index + 2])

def getAddBlack(data, num):
    index = data.find('AB[')
    add = []
    for i in range(num):
        while data[index] != '[':
            index += 1
        row = ord(data[index + 1 : index + 2]) - 97
        col = ord(data[index + 2 : index + 3]) - 97
        add.append([row, col])
        index += 2
    return add

def main():
    # This constant value is the probability of
    # select current board to be one of the training data,
    # it's used to prevent over fitting.
    SELECT_RATE = .8  # 8% board will be used

    # Counter
    inputCounter = 0
    outputCounter = 0

    fileCounter = 1

    outputFile = open('data1.txt', 'w')

    board = b.Board()

    # Read sgf data.
    for root, dirs, files in os.walk('sgf/'):
        for name in files:
            file = open(os.path.join(root, name), 'r')
            data = file.read()

            data = data.split(';')
            inf = data[1]

            if not checkRank(inf):
                continue

            board.clear()
            play_no_counter = 0

            handicap = getHandicap(inf)
            if handicap > 0:
                addBlack = getAddBlack(inf, handicap)
                for row, col in addBlack:
                    board.put(row, col, board.BLACK)
                board.black_turn = False

            data = data[2 : len(data)]
            last_move = [-1, -1]

            for i in range(len(data)):
                next_move, black_turn = next(data[i])
                if next_move == None:
                    break

                inputCounter += 1

                if random.random() < SELECT_RATE:
                    outputCounter += 1

                    # Copy current board.
                    copy_board = [row[:] for row in board.board]
                    copy_last_move = [last_move[0], last_move[1]]
                    copy_next_move = [next_move[0], next_move[1]]

                    rotateMirror(copy_board, copy_last_move, copy_next_move)

                    # Output input output data to 'outputFile'
                    output_board(copy_board, copy_last_move, copy_next_move,
                                 black_turn, outputFile, outputCounter)

                    if outputCounter % 128000 == 0:
                        outputFile.close()
                        fileCounter += 1
                        outputFile = open('data' +
                                          str(fileCounter) + '.txt', 'w')

                board.play(next_move[0], next_move[1])
                last_move[0] = next_move[0]
                last_move[1] = next_move[1]


                # Print input/output info
                if inputCounter % 10000 == 0:
                    print('Input', str(inputCounter),
                          'Output', str(outputCounter), end = '\r')

    print('Input', str(inputCounter),
          'Output', str(outputCounter))
    outputFile.close()

if __name__ == '__main__':
    main()
