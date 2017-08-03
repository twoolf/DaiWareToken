import os
import logging
import math
import traceback
import time as clocktime
import urllib2
from datetime import timedelta

import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data

def setFlags():
    # Flags for defining the cluster
    tf.app.flags.DEFINE_integer("num_workers",  os.getenv('num_workers',   1), "Num Workers. The ONLY one that must get set in k8s")
    tf.app.flags.DEFINE_string("ps_hosts",      os.getenv('ps_hosts',     ""), "Comma-separated list of hostname:port pairs")
    tf.app.flags.DEFINE_string("worker_hosts",  os.getenv('worker_hosts', ""), "Comma-separated list of hostname:port pairs")
    tf.app.flags.DEFINE_string("job_name",      os.getenv('job_name',   "worker"),    "One of 'ps', 'worker'")
    tf.app.flags.DEFINE_integer("task_index",   os.getenv('task_index',   0),    "Index of task within the job")

    # Model / Data Flags
    tf.app.flags.DEFINE_string("data_dir",      os.getenv('data_dir',   "MNIST_data/"), "Directory for storing mnist data")
    tf.app.flags.DEFINE_integer("hidden_units", os.getenv('hidden_units', 100),  "Number of units in the hidden layer of the NN")
    tf.app.flags.DEFINE_integer("batch_size",   os.getenv('batch_size',   100), "Training batch size")


    return tf.app.flags.FLAGS

FLAGS = setFlags()
IMAGE_PIXELS = 28

class ElapsedFormatter():
    """ Used to provide time-relative-to-launch timestamps """
    def __init__(self):
        self.start_time = clocktime.time()

    def format(self, record):
        elapsed_seconds = record.created - self.start_time
        #using timedelta here for convenient default formatting
        elapsed = timedelta(seconds = elapsed_seconds)
        return "{} <{}:{}> {}".format(elapsed, record.module, record.lineno, record.getMessage())

def detectAWS():
    """
    Detects where this host is running inside AWS. THe URL below will only open from
    within an AWS EC2 instance
    """
    try:
      _ = urllib2.urlopen('http://instance-data.ec2.internal')
      return True
    except urllib2.URLError:
      return False

def main(_):

    # -----------------------------------------------------------
    #   CONFIGURE CLUSTER 
    # -----------------------------------------------------------
    if detectAWS():
        # k8s statefulsets use deterministic hostnames with index added to statefulset name
        hostname   = os.environ['HOSTNAME']
        logging.info("Detected AWS. Using hostname: %s", hostname)
        namesplice = hostname.split('-')
        index      = int(namesplice[-1])
        nodegroup  = namesplice[0] if len(namesplice) == 2 else '-'.join(namesplice[:-1])
        if index == 0:
            job_name, task_index = 'ps', 0
        else:
            job_name, task_index = 'worker', index-1

        tf_hostname   = nodegroup + '-%d.tensorflow-svc:2222'
        ps_hosts      = [tf_hostname%0]
        worker_hosts  = [tf_hostname%(i+1) for i in range(FLAGS.num_workers)]

        logfile = 'lmc.log'

    else:
        # For non-k8s deployments, the following must be passed in via flags
        logging.info("Running outside of AWS")
        job_name     = FLAGS.job_name
        task_index   = FLAGS.task_index
        ps_hosts     = FLAGS.ps_hosts.split(",")
        worker_hosts = FLAGS.worker_hosts.split(",")

    num_workers   = len(worker_hosts)
    isSupervisor  = (job_name=='worker' and task_index==0)
    logging.info('  # PS  :  %d   %s', len(ps_hosts), str(ps_hosts))
    logging.info('  # WK  :  %d   %s', len(worker_hosts), str(worker_hosts))
    logging.info('  My role  :  %s', job_name)
    logging.info('  index #  :  %s', task_index)
    logging.info("I am " + ('' if (job_name=='ps') else 'NOT ') + "the Param Server")
    logging.info("I am " + ('' if isSupervisor else 'NOT ') + "the master-chief")

    # Create a cluster from the parameter server and worker hosts.
    cluster = tf.train.ClusterSpec({"ps": ps_hosts, "worker": worker_hosts})

    # Create and start a server for the local task.
    server = tf.train.Server(cluster, job_name=job_name, task_index=task_index)

    #------------------------------------------------------------
    #   BUILD GRAPH
    #------------------------------------------------------------
    # These are the model params -- they will be located on the Param Server
    with tf.device('/job:ps/task:0'):
        # Variables of the hidden layer
        hid_w = tf.Variable(
            tf.truncated_normal([IMAGE_PIXELS * IMAGE_PIXELS, FLAGS.hidden_units],
                                stddev=1.0 / IMAGE_PIXELS), name="hid_w")
        hid_b = tf.Variable(tf.zeros([FLAGS.hidden_units]), name="hid_b")

        # Variables of the softmax layer
        sm_w = tf.Variable(
            tf.truncated_normal([FLAGS.hidden_units, 10],
                                stddev=1.0 / math.sqrt(FLAGS.hidden_units)),
            name="sm_w")
        sm_b = tf.Variable(tf.zeros([10]), name="sm_b")


    # This is the model graph -- each worker runs a replicated copy of this
    with tf.device(tf.train.replica_device_setter(
        worker_device="/job:worker/task:%d" % FLAGS.task_index,
        cluster=cluster)):

        x  = tf.placeholder(tf.float32, [None, IMAGE_PIXELS * IMAGE_PIXELS])
        y_ = tf.placeholder(tf.float32, [None, 10])

        hid_lin = tf.nn.xw_plus_b(x, hid_w, hid_b)
        hid     = tf.nn.relu(hid_lin)

        y    = tf.nn.softmax(tf.nn.xw_plus_b(hid, sm_w, sm_b))
        loss = -tf.reduce_sum(y_ * tf.log(tf.clip_by_value(y, 1e-10, 1.0)))

        global_step = tf.Variable(0)
        train_op = tf.train.AdagradOptimizer(0.01).minimize(loss, global_step=global_step)

    init_op = tf.global_variables_initializer()

    #------------------------------------------------------------
    #   PS (master) EXECUTION
    #------------------------------------------------------------
    if FLAGS.job_name == "ps":
        # Param Servers don't do much. They just execute data I/O requests and join a thread
        logging.info('Parameter server is ready.')
        server.join()
        return


    #------------------------------------------------------------
    #   WORKER EXECUTION
    #------------------------------------------------------------
    # Create a "supervisor", which oversees the training process.
    logging.debug('Creating the supervisor')
    if isSupervisor:
        logging.debug("I am the chief!")
    sv = tf.train.Supervisor(is_chief=isSupervisor,
                             logdir="/tmp/train_logs",
                             init_op=init_op,
                             global_step=global_step,
                             save_model_secs=600)

    mnist = input_data.read_data_sets(FLAGS.data_dir, one_hot=True)

    # The supervisor takes care of session initialization, restoring from
    # a checkpoint, and closing when done or an error occurs.
    with sv.prepare_or_wait_for_session(server.target, start_standard_services=True) as sess:
        # Loop until the supervisor shuts down or 1000000 steps have completed.
        step = 0
        while not sv.should_stop() and step < 1000:
            # Run a training step asynchronously.
            # See `tf.train.SyncReplicasOptimizer` for additional details on how to
            # perform *synchronous* training.

            batch_xs, batch_ys = mnist.train.next_batch(FLAGS.batch_size)
            train_feed = {x: batch_xs, y_: batch_ys}

            _, step = sess.run([train_op, global_step], feed_dict=train_feed)
            if step % 100 == 0: 
                print ("Done step %d" % step)

    if detectAWS():
        logging.info('  Entering a DO LOOP until someone kills me....')
        while True:
            logging.info('    Please kill me... $$SUCCESS$$')
            clocktime.sleep(60)

    # Ask for all the services to stop.
    sv.stop()

if __name__ == "__main__":

    handler = logging.StreamHandler()
    handler.setFormatter(ElapsedFormatter())
    logging.getLogger().addHandler(handler)
    logging.getLogger().setLevel(logging.DEBUG)
    logging.info("Logging initialized")

    try:
        tf.app.run()
    except Exception as err:
        logging.error("MNIST Program has TERMINATED PREMATURELY:")
        traceback.print_exc()
        logging.error("%s has FAILED.\n$$FAILED$$\n", os.environ['HOSTNAME'])
        if detectAWS():
            while True:
                logging.warn("  Running in k8s. Please kill me.")
                clocktime.sleep(300)