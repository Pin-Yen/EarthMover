import tensorflow as tf
import numpy as np
import random

class Network(object):
    def __init__(self, depth=10, width=48):
        self.sess = tf.Session()
        # Network's depth and width.
        self.depth = depth
        self.width = width
        self.miniBatch = 0

    # Set up network structure.
    def setUp(self):
        # Input: 19x19x3, output: 361.
        self.x = tf.placeholder(tf.float32, [None, 19, 19, 3])
        self.y = tf.placeholder(tf.float32, [None, 361])

        # Each layers is composed of conv and bn layer
        # First layer.
        with tf.name_scope('Layer1'):
            conv1 = tf.layers.conv2d(
                self.x, self.width, 3, padding='same',
                activation=tf.nn.relu, name='conv1')
            bn1 = tf.layers.batch_normalization(conv1, name='bn1')

        # Hidden layer.
        currentLayer = bn1
        for i in range(2, self.depth + 1):
            with tf.name_scope('Layer' + str(i)):
                conv = tf.layers.conv2d(currentLayer, self.width, 3,
                    padding = 'same', activation = tf.nn.relu, name='conv' + str(i))
                bn = tf.layers.batch_normalization(conv, name='bn' + str(i))
            currentLayer = bn

        # Output layer.
        with tf.name_scope('Layer' + str(self.depth + 1)):
            conv1x1 = tf.layers.conv2d(currentLayer, 1, 1,
                padding = 'same', activation = tf.nn.relu, name='conv1x1')
            output = tf.nn.softmax(tf.reshape(conv1x1, [-1, 361]))

        # Loss function.
        with tf.name_scope('Loss'):
            self.loss = tf.losses.softmax_cross_entropy(
                onehot_labels = self.y, logits = output)
            tf.summary.scalar('loss', self.loss)

        # Optimizer.
        self.opt = tf.train.AdamOptimizer(1e-4).minimize(self.loss)

        # Accuracy.
        with tf.name_scope('Accuracy'):
            correct_prediction = tf.equal(tf.argmax(output, 1),
                                          tf.argmax(self.y, 1))
            self.accuracy = tf.reduce_mean(
                tf.cast(correct_prediction, tf.float32))
            tf.summary.scalar('accuracy', self.accuracy)

        # Summary.
        self.merged = tf.summary.merge_all()
        self.trainWriter = tf.summary.FileWriter("logs/train", self.sess.graph)
        self.testWriter = tf.summary.FileWriter("logs/test", self.sess.graph)


    # Initialize variable.
    def initVar(self):
        self.sess.run(tf.global_variables_initializer())


    # Train network.
    def train(self, trainData, testData=None, batchSize=128):
        for i in range(trainData.dataSize // batchSize):
            self.miniBatch += 1
            print('mini batch ' + str(self.miniBatch), end='\r')
            # Get in/out data.
            x, y = trainData.getBatch(i, batchSize)
            self.sess.run([self.opt], {self.x: x, self.y: y})

            # Upgrade accuracy to summary.
            if i % 10 == 0:
                summ = self.sess.run(self.merged,
                        {self.x: x, self.y: y})

                self.trainWriter.add_summary(summ, self.miniBatch)
                if testData != None:
                    testX, testY = testData.getBatch(0, 256)
                    testSumm = self.sess.run(self.merged,
                        {self.x: testX, self.y: testY})

                    self.testWriter.add_summary(testSumm, self.miniBatch)


    # Save/Load network.
    def save(self, path='net/nn'):
        tf.train.Saver().save(self.sess, path)

    def load(self, path='net/nn'):
        tf.train.Saver().restore(self.sess, path)


class Data(object):
    def __init__(self):
        self.input = []
        self.output = []
        self.dataSize = 0

    def loadData(self, path):
        dataSize = 0
        with open(path, 'r') as f:
            while f.readline().rstrip() != '':
                x = []
                y = []
                dataSize += 1
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
                self.input.append(x)
                self.output.append(y)

                # Print inf
                if dataSize % 10000 == 0:
                    print('Reading the', dataSize, 'th data.', sep=' ', end='\r')

        self.dataSize = dataSize
        print('Finish reading', dataSize, 'data.', sep=' ')

    # Get data by batch.
    def getBatch(self, index, batchSize):
        x = self.input[index : index + batchSize]
        y = self.output[index : index + batchSize]
        return x, y


    # Clear all data.
    def clearData(self):
        self.input = []
        self.output = []
        self.dataSize = 0

    def shuffleData(self):
        data = list(zip(self.input, self.output))
        random.shuffle(data)
        self.input, self.output = zip(*data)


def main():
    testData = Data()
    testData.loadData('data_2863_01.txt')
    data = Data()
    data.loadData('data_135724_025.txt')

    nn = Network()
    nn.setUp()
    nn.initVar()
    for i in range(10):
        nn.train(data, testData)
        nn.save()
        data.shuffleData()



if __name__ == '__main__':
    main()