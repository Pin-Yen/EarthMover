import json
import random

def outputData():
    # TODO

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

def main():
    # This constant value is the probability of
    # select current board to be one of the training data,
    # it's used to prevent over fitting.
    SELECT_RATE = 0.05  # 5% board will be used

    # Counter
    inputCounter = 0
    outputCounter = 0

    '''
    # Read json data.
    file = open('jsondata.json', 'r')
    jsonData = json.loads(file.read())
    file.close()
    '''

    # TODO

    outputFile = open('cnndata.txt', 'w')

    outputFile.close()

if __name__ == "__main__":
    main()
