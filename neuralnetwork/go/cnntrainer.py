import cnn
import board as b
import random

class Data(object):
    def __init__(self):
        self.data = []
        self.data_size = 0

    # Read data from 'path'.
    # If 'max_data > 0, will only read up to 'max_data'.
    def load_data(self, path, max_data=0):
        print('Reading data from ' + path)
        data_size = 0
        board = b.Board()
        with open(path, 'r') as f:
            while f.readline().rstrip() != '':
                data_size += 1

                # Read data to board.
                for row in range(19):
                    line = f.readline().rstrip()
                    col = 0
                    for data in line:
                        board.board[row][col] = data
                        col += 1

                # Read information.
                # (last_row, lost_col, next_row, next_col, who_turn)
                inf = f.readline().split(',')
                #board.last_move = [int(inf[0]), int(inf[1])]
                board.black_turn = (inf[4] == 'b')

                # Get input data.
                x = board.nn_input_data()
                # Create output data.
                y = [0] * 361
                y[int(inf[2]) * 19 + int(inf[3])] = 1

                # Append data.
                self.data.append((x, y))

                # Print inf
                if data_size % 10000 == 0:
                    print('Reading the ' + str(data_size) +
                          ' th data.', end='\r')

                if max_data > 0:
                    if data_size == max_data:
                        break

        self.data_size = data_size
        print('Finish reading ' + str(data_size) + ' data.')

    # Get data by batch.
    def get_data(self, index, batch_size):
        x, y = zip(*(self.data[index : index + batch_size]))
        return x, y

    # Get a batch of random data.
    def get_random_data(self, batch_size):
        data = random.sample(self.data, batch_size)
        x, y = zip(*data)
        return x, y

    # Clear all data.
    def clear_data(self):
        self.data = []
        self.data_size = 0

    # Shuffle data.
    def shuffle(self):
        random.shuffle(self.data)


# Train multiple data, 'train_data_path' should be a list of data path
def train_multi_input(network, train_data_path, test_data_path, epoch):
    epoch_counter = 0
    train_data = Data()
    test_data = Data()
    test_data.load_data(test_data_path)
    while epoch_counter < epoch:
        for data_path in train_data_path:
            train_data.clear_data()
            train_data.load_data(data_path)
            train_data.shuffle()
            network.train(train_data, test_data, 128)
            epoch_counter += 1
            print('epoch ' + str(epoch_counter))
            network.save('net/nn_epoch' + str(epoch_counter))
            if epoch_counter == epoch:
                return

def train_single_input(network, train_data_path, test_data_path, epoch):
    train_data = Data()
    train_data.load_data(train_data_path)
    test_data = Data()
    test_data.load_data(test_data_path)
    for i in range(epoch):
        train_data.shuffle()
        network.train(train_data, test_data, 128)
        print('epoch ' + str(i + 1))
        network.save('net/nn_epoch' + str(i + 1))

def main():
    network = cnn.Network()
    network.set_up()
    network.init_var()

    data_list = []
    for i in range(1, 7):
        data_list.append('data' + str(i) + '.txt')
    train_multi_input(network, data_list, 'testdata.txt', 32)


if __name__ == '__main__':
    main()