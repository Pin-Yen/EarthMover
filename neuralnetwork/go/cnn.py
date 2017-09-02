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

        # Each layer excpet last layer is composed of conv and bn layer
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

    def predict(self, input_data):
        prob = self.sess.run(self.output, feed_dict={self.x: input_data})
        return prob.reshape([19, 19])