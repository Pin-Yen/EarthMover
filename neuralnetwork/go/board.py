class Board(object):
    def __init__(self):
        self.DIMEN = 19
        self.EMPTY = '-'
        self.BLACK = 'X'
        self.WHITE = 'O'
        self.clear()

    # Clear board.
    def clear(self):
        self.black_turn = True
        self.last_move = [-1, -1]
        self.board = [[self.EMPTY] * self.DIMEN for _ in range(self.DIMEN)]

    def _out_of_bound(self, r, c):
        return not (0 <= r <= self.DIMEN - 1 and 0 <= c <= self.DIMEN - 1)

    def _remove_if_dead(self, row, col, sel_color):
        group = []
        if self._no_liberty(row, col, sel_color, group):
            for coor in group:
                self.board[coor[0]][coor[1]] = self.EMPTY

    def _no_liberty(self, row, col, sel_color, group):
        group.append([row, col])

        # Check the surrounding points.
        d = [[0, 1], [1, 0], [0, -1], [-1, 0]]
        for i in range(4):
            check_r = row + d[i][0]
            check_c = col + d[i][1]
            if self._out_of_bound(check_r, check_c):
                continue
            # Exist empty point around, return false.
            if self.board[check_r][check_c] == self.EMPTY:
                return False
            elif self.board[check_r][check_c] == sel_color:
                if [check_r, check_c] in group:
                    continue
                if not self._no_liberty(check_r, check_c, sel_color, group):
                    return False
        # Every neighbor point is neither empty nor self group that lived.
        return True

    def play(self, row, col):
        sel_color = self.BLACK if self.black_turn else self.WHITE
        opp_color = self.WHITE if self.black_turn else self.BLACK
        # Put chess.
        self.board[row][col] = sel_color
        self.last_move = [row, col]
        self.black_turn = not self.black_turn

        # Calculate opponent's liberty around the playing point
        d = [[0, 1], [1, 0], [0, -1], [-1, 0]];
        for i in range(4):
            check_r = row + d[i][0]
            check_c = col + d[i][1]
            if self._out_of_bound(check_r, check_c):
                continue
            # Calculate liberty, and remove chess if dead.
            if self.board[check_r][check_c] == opp_color:
                self._remove_if_dead(check_r, check_c, opp_color)

    def put(self, row, col, color):
        self.board[row][col] = color

    def print(self):
        def _start_point(row, col):
            return ((row == 4 or row == 10 or row == 16) and
                    (col == 4 or col == 10 or col == 16))

        board = ''
        for row in range(self.DIMEN + 2):
            for col in range(self.DIMEN + 2):
                if row == 0 or row == self.DIMEN + 1:
                    # If at the first or the last row,
                    # print the coordinate with letter.
                    if col == 0 or col == self.DIMEN + 1:
                        board += '    '
                    else:
                        board += '   ' + chr(64 + col)
                elif col == 0 or col == self.DIMEN + 1:
                    # If at the first or the last column,
                    # print the coordinate with number.
                    if col == 0:
                        board += '{:>4}'.format(row)
                    else:
                        board += '    ' + str(row)
                else:
                    if col == 1:
                        board += '   '
                    elif row == 1 or row == self.DIMEN:
                        board += '═══'
                    elif _start_point(row, col):
                        board += '──╼'
                    elif _start_point(row, col - 1):
                        board += '╾──'
                    else:
                        board +=  '───'

                    if (self.board[row - 1][col - 1] == self.EMPTY):
                        if row == 1:
                            if col == 1:
                                board += '╔'
                            elif col == self.DIMEN:
                                board += '╗'
                            else:
                                board += '╤'
                        elif row == self.DIMEN:
                            if col == 1:
                                board += '╚'
                            elif col == self.DIMEN:
                                board += '╝'
                            else:
                                board += '╧'
                        else:
                            if col == 1:
                                board += '╟'
                            elif col == self.DIMEN:
                                board += '╢'
                            else:
                                board += '╋' if _start_point(row, col) else '┼'
                    else:
                        board += self.board[row - 1][col - 1]


                    # If at the last column, print \n.
                if col == self.DIMEN + 1:
                    board += '\n    ';

                    # If not at the first or last row, print │ between two row.
                    if 0 < row < self.DIMEN:
                        for i in range(19):
                            if i == 0 or i == self.DIMEN - 1:
                                board += '   ║'
                            else:
                                board += '   │'
                    board += '\n'

        print(board)

    def get_input(self):
        while True:
            print('enter the coordinate of next move (A1 ~ S19)')
            input_val = input()
            input_len = len(input_val)

            valid_input = False
            if input_len == 2 or input_len == 3:
                valid_input = True

                # Get column.
                col = ord(input_val[0])
                if ord('A') <= col <= ord('S'):
                    col -= ord('A')
                elif ord('a') <= col <= ord('s'):
                    col -= ord('a')
                else:
                    valid_input = False

                # get row
                if not input_val[1:3].isdigit():
                    valid_input = False

                row = int(input_val[1:3])

                if 1 <= row <= self.DIMEN:
                    row -= 1
                else:
                    valid_input = False

            if valid_input:
                return row, col

            print('Invalid input')

    def _calc_group_liberty(self, row, col, sel_color,
                            group, empty_points):
        group.append([row, col])
        liberty = 0

        # Check the surrounding points.
        d = [[0, 1], [1, 0], [0, -1], [-1, 0]]
        for i in range(4):
            check_r = row + d[i][0]
            check_c = col + d[i][1]
            if self._out_of_bound(check_r, check_c):
                continue
            # Exist empty point around.
            if self.board[check_r][check_c] == self.EMPTY:
                # This point not in empty pont array, ++liberty.
                if [check_r, check_c] not in empty_points:
                    empty_points.append([check_r, check_c])
                    liberty += 1
            # Exist self stone around.
            elif self.board[check_r][check_c] == sel_color:
                # This point not check yet, recursive check it.
                if [check_r, check_c] not in group:
                    liberty += self._calc_group_liberty(
                        check_r, check_c, sel_color, group, empty_points)
        return liberty

    def _calc_liberty(self):
        # Cpoy board.
        liberty_board = [row[:] for row in self.board]
        for row in range(self.DIMEN):
            for col in range(self.DIMEN):
                # Calculate liberty if this point not check yet.
                # If this ponit checked before, this point's data
                # should change into numbers of liberty, not BLACK/WHITE.
                if (liberty_board[row][col] == self.BLACK or
                    liberty_board[row][col] == self.WHITE):
                    group = []
                    empty_points = []
                    liberty = self._calc_group_liberty(
                        row, col, liberty_board[row][col], group, empty_points)
                    # Change data from BLACK/WHITE to numbers of liberty.
                    for coor in group:
                        liberty_board[coor[0]][coor[1]] = liberty
        return liberty_board


    def nn_input_data(self):
        data = []
        sel_color = self.BLACK if self.black_turn else self.WHITE
        liberty_board = self._calc_liberty()
        '''
        Input:
            self stone
            opp stone
            empty point
            the stone that have 1 liberty
            ...                 2
            ...                 3
            ...                 4
        '''
        for row in range(self.DIMEN):
            row_data = []
            for col in range(self.DIMEN):
                append_data = [0] * 7
                if self.board[row][col] == self.EMPTY:
                    append_data[2] = 1
                elif self.board[row][col] == sel_color:
                    append_data[0] = 1
                else:
                    append_data[1] = 1

                liberty = liberty_board[row][col]
                if liberty != '-':
                    liberty = min(liberty, 4)
                    append_data[2 + liberty] = 1

                row_data.append(append_data)
            data.append(row_data)
        return data