import tensorflow as tf
import numpy as np

class Network(object):
    def __init__(self, depth=10, width=48):
        self.sess = tf.Session()
        # Network's depth and width
        self.depth = depth
        self.width = width


    def setUp(self):
        # Input: 19x19x3, output: 361
        self.x = tf.placeholder(tf.float32, [None, 19, 19, 3])
        self.y = tf.placeholder(tf.float32, [None, 361])

        # Each layers is composed of conv and bn layer
        # First layer
        with tf.name_scope('Layer1'):
            conv1 = tf.layers.conv2d(
                self.x, self.width, 3, padding='same',
                activation=tf.nn.relu, name='conv')
            bn1 = tf.layers.batch_normalization(conv1, name='bn')

        # Hidden layer
        currentLayer = bn1
        for i in xrange(2, self.depth + 1):
            with tf.name_scope('Layer' + str(i)):
                conv = tf.layers.conv2d(currentLayer, self.width, 3,
                    padding = 'same', activation = tf.nn.relu, name='conv')
                bn = tf.layers.batch_normalization(conv, name='bn')
            currentLayer = bn

        # Output layer
        with tf.name_scope('Output layer'):
            conv1x1 = tf.layers.conv2d(currentLayer, 1, 1,
                padding = 'same', activation = tf.nn.relu, name='conv1x1')
            output = tf.nn.softmax(tf.reshape(conv1x1, [-1, 361]))

        # Loss function
        with tf.name_scope('Loss'):
            self.loss = tf.losses.softmax_cross_entropy(
                onehot_labels = self.y, logits = output)
            tf.summary.scalar('loss', self.loss)

        # Optimizer
        self.opt = tf.train.AdamOptimizer(1e-4).minimize(self.loss)

        # Accuracy
        with tf.name_scope('Accuracy'):
            correct_prediction = tf.equal(tf.argmax(output, 1),
                                          tf.argmax(self.y, 1))
            self.accuracy = tf.reduce_mean(
                tf.cast(correct_prediction, tf.float32))
            tf.summary.scalar('accuracy', self.accuracy)

        self.merged = tf.summary.merge_all()
        self.writer = tf.summary.FileWriter("logs/", self.sess.graph)

    def initVar(self):
        self.sess.run(tf.global_variables_initializer())


    def train(self, data, batchSize=128):
        for i in range(data.dataSize // batchSize):
            batch_x, batch_y = data.getBatch(i, batchSize)
            self.sess.run([self.opt],
                {self.x: batch_x, self.y: batch_y})
            if i % 10 == 0:
                summ = self.sess.run(self.merged,
                        {self.x: batch_x, self.y: batch_y})

                self.writer.add_summary(summ, i)



    def save(self, path='net/nn'):
        tf.train.Saver().save(self.sess, path)

    def load(self, path='net/nn'):
        tf.train.Saver().restore(self.sess, path)


class Data(object):
    def __init__(self):
        pass

    def loadData(self, path):
        self.input = []
        self.output = []
        dataSize = 0
        with open(path, 'r') as f:
            while True:
                x = []
                y = []
                index = f.readline().rstrip()
                if index == '':
                    break
                dataSize += 1
                filters = []
                for i in range(3):
                    dx=[]
                    for l in range(19):
                        line = f.readline().rstrip()
                        dx.append([int(data) for data in line.split()])
                    filters.append(dx)
                    f.readline()
                for r in range(19):
                    row = []
                    for c in range(19):
                        row.append([filters[0][r][c], filters[1][r][c], filters[2][r][c]])
                    x.append(row)

                for l in range(19):
                    line = f.readline().rstrip()
                    for data in line.split():
                        y.append(int(data))
                f.readline()
                self.input.append(x)
                self.output.append(y)
                if dataSize % 10000 == 0:
                    print(dataSize)

        self.dataSize = dataSize

    def getBatch(self, index, batchSize):
        x = self.input[index : index + batchSize]
        y = self.output[index : index + batchSize]
        return x, y


def main():
    d = Data()
    d.loadData('data1.txt')

    nn = Network()
    nn.setUp()
    nn.initVar()
    for i in range(10):
        nn.train(d)
        nn.save()


if __name__ == '__main__':
    main()