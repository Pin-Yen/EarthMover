import json
import os
import random

def outputData(board, blackTurn, output, file, counter):
    # Input feature: self chess, opp chess, empty point.
    input = [[[0] * 19 for _ in range(19)] for _ in range(3)]

    for row in range(19):
        for col in range(19):
            if board[row][col] == 0:  # Empty point.
                input[2][row][col] = 1
            elif board[row][col] == -1:  # Black chess.
                if blackTurn:
                    input[0][row][col] = 1
                else:
                    input[1][row][col] = 1
            else:  # White chess.
                if blackTurn:
                    input[1][row][col] = 1
                else:
                    input[0][row][col] = 1

    file.write(str(counter) + '\n')

    for feature in range(3):
        for row in range(19):
            for col in range(19):
                file.write(str(input[feature][row][col]))
                file.write(' ')
            file.write('\n')
        file.write('\n')

    for row in range(19):
        for col in range(19):
            file.write(str(output[row][col]))
            file.write(' ')
        file.write('\n')
    file.write('\n')

# Rotate and mirror
# There are 8 kinds of results, (4 rotate * 2 mirror),
# key is used to decide result.
def rotateMirror(board, key):
    for m in range(2):
        for r in range(4):
            if key == m * 4 + r:
                return
            rotate(board)
        mirror(board)

def rotate(board):
    temp = [[0] * 19 for _ in range(19)]
    # Rotate 90 degrees clockwise (row -> col, col -> 18 - row).
    for row in range(19):
        for col in range(19):
            temp[col][18 - row] = board[row][col]

    for row in range(19):
        board[row] = temp[row]

def mirror(board):
    temp = [[0] * 19 for _ in range(19)]
    # Mirror (row -> col, col -> row).
    for row in range(19):
        for col in range(19):
            temp[col][row] = board[row][col];

    for row in range(19):
        board[row] = temp[row]

def removeIfDead(board, row, col, selColor):
    group = []
    if noLiberty(board, row, col, selColor, group):
        for coor in group:
            board[coor[0]][coor[1]] = 0

def noLiberty(board, row, col, selColor, group):
    group.append([row, col])

    # Calculate opponent's liberty around the playing point
    d = [[0, 1], [1, 0], [0, -1], [-1, 0]];
    for i in range(4):
        checkRow = row + d[i][0]
        checkCol = col + d[i][1]
        if checkRow < 0 or checkRow > 18 or checkCol < 0 or checkCol > 18:
            continue
        # Calculate liberty, and remove chess if dead.
        if board[checkRow][checkCol] == 0:
            return False
        elif board[checkRow][checkCol] == selColor:
            if [checkRow, checkCol] in group:
                continue
            if not noLiberty(board, checkRow, checkCol, selColor, group):
                return False

    return True


def next(key):
    if key[0] == 'T':
        return []
    if len(key) <= 3:
        return []
    blackTurn = True if key[0] == 'B' else False
    row = ord(key[2]) - 97
    col = ord(key[3]) - 97
    if row < 0 or row > 18 or col < 0 or col > 18:
        return []
    return [row, col, blackTurn]


# Put a chess according to key, key is sgf type,
# and check if any opponent chess die and remove them
def play(board, key):
    blackTurn = True if key[0] == 'B' else False
    row = ord(key[2]) - 97
    col = ord(key[3]) - 97

    selColor = -1 if blackTurn else 1
    oppColor = 1 if blackTurn else -1
    # Put chess.
    board[row][col] = selColor

    # Calculate opponent's liberty around the playing point
    d = [[0, 1], [1, 0], [0, -1], [-1, 0]];
    for i in range(4):
        checkRow = row + d[i][0]
        checkCol = col + d[i][1]
        if checkRow < 0 or checkRow > 18 or checkCol < 0 or checkCol > 18:
            continue
        # Calculate liberty, and remove chess if dead.
        if board[checkRow][checkCol] == oppColor:
            removeIfDead(board, checkRow, checkCol, oppColor)


# Check this game's player rank.
# Both player;s rank should be 5d+.
def checkRank(data):
    whiteIndex = data.find('WR')
    blackIndex = data.find('BR')
    if whiteIndex == -1 or  blackIndex == -1:
        return False
    whiteRank = int(data[whiteIndex + 3 : whiteIndex + 4])
    blackRank = int(data[blackIndex + 3 : blackIndex + 4])
    if whiteRank < 5 or blackRank < 5:
        return False
    return True

def getHandicap(data):
    index = data.find('HA')
    if index == -1:
        return 0
    while data[index] != '[':
        index += 1
    return int(data[index + 1 : index + 2])

def getAddBlack(data, num):
    index = data.find('AB')
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
    SELECT_RATE = .02  # 2% board will be used

    # Counter
    inputCounter = 0
    outputCounter = 0

    outputFile = open('cnndata.txt', 'w')

    # Read sgf data.
    for root, dirs, files in os.walk('sgf/'):
        for name in files:
            file = open(os.path.join(root, name), 'r')
            data = file.read()

            data = data.split(';')
            inf = data[1]

            if not checkRank(inf):
                continue

            board = [[0] * 19 for _ in range(19)]

            #print(inf)
            handicap = getHandicap(inf)
            #print(handicap)
            if handicap > 0:
                addBlack = getAddBlack(inf, handicap)
                #print(addBlack)
                for i in range(handicap):
                    board[addBlack[i][0]][addBlack[i][1]] = -1


            data = data[2 : len(data)]
            #print(data)

            for i in range(len(data)):
                nextMove = next(data[i])
                if nextMove == []:
                    break

                inputCounter += 1

                if random.random() < SELECT_RATE:
                    outputCounter += 1

                    # Copy current board.
                    copyBoard = [row[:] for row in board]
                    output = [[0] * 19 for _ in range(19)]
                    output[nextMove[0]][nextMove[1]] = 1

                    # Random mirror rotate.
                    randomMirrorRotateSeed = random.randrange(8)
                    rotateMirror(copyBoard, randomMirrorRotateSeed)
                    rotateMirror(output, randomMirrorRotateSeed)

                    # Output input output data to 'outputFile'
                    outputData(copyBoard, nextMove[2], output,
                               outputFile, outputCounter)

                play(board, data[i])

            # Print input/output info
            print('Input', str(inputCounter),
                  'Output', str(outputCounter), end = '\r')

    outputFile.close()

if __name__ == '__main__':
    main()
