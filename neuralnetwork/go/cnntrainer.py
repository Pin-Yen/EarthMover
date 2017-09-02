import tensorflow as tf
import numpy as np
import random

class Network(object):
    def __init__(self, depth=10, width=48):
        self.sess = tf.Session()
        # Network's depth and width.
        self.depth = depth
        self.width = width
        self.global_batch = 0

    # Set up network structure.
    def set_up(self):
        # Input: 19x19x3, output: 361.
        self.x = tf.placeholder(tf.float32, [None, 19, 19, 3])
        self.y = tf.placeholder(tf.float32, [None, 361])

        # Each layers excpet last layer is composed of conv and bn layer
        # First layer.
        with tf.name_scope('Layer1'):
            conv1 = tf.layers.conv2d(
                self.x, self.width, 3, padding='same',
                activation=tf.nn.relu, name='conv1')
            bn1 = tf.layers.batch_normalization(conv1, name='bn1')

        # Hidden layer.
        current_layer = bn1
        for i in range(2, self.depth + 1):
            with tf.name_scope('Layer' + str(i)):
                conv = tf.layers.conv2d(current_layer, self.width, 3,
                                        padding='same', activation=tf.nn.relu,
                                        name='conv' + str(i))
                bn = tf.layers.batch_normalization(conv, name='bn' + str(i))
            current_layer = bn

        # Last layer.
        with tf.name_scope('Layer' + str(self.depth + 1)):
            conv1x1 = tf.layers.conv2d(current_layer, 1, 1,
                padding='same', activation=tf.nn.relu, name='conv1x1')
            logits = tf.reshape(conv1x1, [-1, 361])

        # Softmax output.
        with tf.name_scope('Output'):
            self.output = tf.nn.softmax(logits, name='output')

        # Loss function.
        with tf.name_scope('Loss'):
            self.loss = tf.reduce_mean(
                tf.nn.softmax_cross_entropy_with_logits(logits=logits,
                                                        labels=self.y))
        tf.summary.scalar('loss', self.loss)

        # Optimizer.
        #self.opt = tf.train.RMSPropOptimizer(1e-3).minimize(self.loss)
        self.opt = tf.train.AdamOptimizer(1e-4).minimize(self.loss)

        # Accuracy.
        with tf.name_scope('Accuracy'):
            correct = tf.equal(tf.argmax(logits, 1), tf.argmax(self.y, 1))
            self.accuracy = tf.reduce_mean(tf.cast(correct, tf.float32))
        tf.summary.scalar('accuracy', self.accuracy)

        # Summary.
        self.merged = tf.summary.merge_all()
        self.train_writer = tf.summary.FileWriter(
            "logs/train", self.sess.graph)
        self.test_writer = tf.summary.FileWriter(
            "logs/test", self.sess.graph)

    # Initialize variable.
    def init_var(self):
        self.sess.run(tf.global_variables_initializer())

    # Train network.
    def train(self, train_data, test_data, batch_size):
        for i in range(train_data.data_size // batch_size):
            print('\rbatch ' + str(self.global_batch), end='')
            # Get in/out data.
            x, y = train_data.get_data(i * batch_size, batch_size)
            # Train network.
            self.sess.run([self.opt], {self.x: x, self.y: y})

            # Upgrade accuracy to summary.
            if self.global_batch % 10 == 0:
                # Accuracy of train data.
                summ = self.sess.run(
                    self.merged, {self.x: x, self.y: y})
                self.train_writer.add_summary(summ, self.global_batch)
                # Accuracy of test data.
                x, y = test_data.get_random_data(batch_size)
                testSumm = self.sess.run(
                    self.merged, {self.x: x, self.y: y})
                self.test_writer.add_summary(testSumm, self.global_batch)

            # Increase global batch counter
            self.global_batch += 1

    # Save/Load network.
    def save(self, path='net/nn'):
        tf.train.Saver().save(self.sess, path)

    def load(self, path='net/nn'):
        tf.train.Saver().restore(self.sess, path)


class Data(object):
    def __init__(self):
        self.data = []
        self.data_size = 0

    def load_data(self, path, max_data=0):
        print('Reading data from ' + path)
        data_size = 0
        with open(path, 'r') as f:
            while f.readline().rstrip() != '':
                x = []
                y = []
                data_size += 1
                features = []

                # Get input data
                for i in range(3):
                    # Three feautre map.
                    dx=[]
                    for l in range(19):
                        line = f.readline().rstrip()
                        dx.append([int(data) for data in line.split()])
                    # Append 19x19 input to feature map.
                    features.append(dx)
                    f.readline()
                # Reshape data([3x19x19]->[19x19x3])
                for r in range(19):
                    row = []
                    for c in range(19):
                        row.append([features[0][r][c], features[1][r][c], features[2][r][c]])
                    x.append(row)

                # Get output data
                for l in range(19):
                    line = f.readline().rstrip()
                    for data in line.split():
                        y.append(int(data))
                f.readline()

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
    network = Network()
    network.set_up()
    network.init_var()

    train_single_input(network, 'data_135724_025.txt', 'data_2863_01.txt', 20)


if __name__ == '__main__':
    main()