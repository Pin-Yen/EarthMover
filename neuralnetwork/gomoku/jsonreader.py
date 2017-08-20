import json
import random

def outputData(board, blackTurn, output, file, counter):
    # Input feature: self chess, opp chess, empty point.
    input = [[[0] * 15 for _ in range(15)] for _ in range(3)]

    for row in range(15):
        for col in range(15):
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
        for row in range(15):
            for col in range(15):
                file.write(str(input[feature][row][col]))
            file.write('\n')
        file.write('\n')

    for row in range(15):
        for col in range(15):
            file.write(str(output[row][col]))
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
    temp = [[0] * 15 for _ in range(15)]
    # Rotate 90 degrees clockwise (row -> col, col -> 14 - row).
    for row in range(15):
        for col in range(15):
            temp[col][14 - row] = board[row][col]

    for row in range(15):
        board[row] = temp[row]

def mirror(board):
    temp = [[0] * 15 for _ in range(15)]
    # Mirror (row -> col, col -> row).
    for row in range(15):
        for col in range(15):
            temp[col][row] = board[row][col];

    for row in range(15):
        board[row] = temp[row]

def main():
    # This constant value is the probability of
    # select current board to be one of the training data,
    # it's used to prevent over fitting.
    SELECT_RATE = 0.05  # 5% board will be used

    # Counter
    inputCounter = 0
    outputCounter = 0

    # Read json data.
    file = open('jsondata.json', 'r')
    jsonData = json.loads(file.read())
    file.close()

    outputFile = open('cnndata.txt', 'w')

    for game in jsonData:
        # Check this game exist record.
        if 'record' not in jsonData[game]:
            continue

        board = [[0] * 15 for _ in range(15)]
        count = 0
        for move in jsonData[game]['record']:
            if move == None:
                continue

            inputCounter += 1

            row = move['r']
            col = move['c']

            if random.random() < SELECT_RATE:
                outputCounter += 1

                # Copy current board.
                copyBoard = [row[:] for row in board]
                # Current board's output is next move.
                output = [[0] * 15 for _ in range(15)]
                output[row][col] = 1

                # Random mirror rotate.
                randomMirrorRotateSeed = random.randrange(8)
                rotateMirror(copyBoard, randomMirrorRotateSeed)
                rotateMirror(output, randomMirrorRotateSeed)

                # Output input output data to 'outputFile'
                blackTurn = (count & 1 == 0)
                outputData(copyBoard, blackTurn, output,
                           outputFile, outputCounter)

            # Update board, black: -1, white: 1.
            board[row][col] = 1 if count & 1 == 1 else -1
            count += 1

            # Print input/output info
            print('Input', str(inputCounter),
                  'Output', str(outputCounter), end = '\r')

    outputFile.close()

if __name__ == "__main__":
    main()