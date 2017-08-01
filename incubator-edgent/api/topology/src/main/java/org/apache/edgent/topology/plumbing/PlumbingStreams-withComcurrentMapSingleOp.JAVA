/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
package org.apache.edgent.topology.plumbing;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

import org.apache.edgent.function.Function;
import org.apache.edgent.oplet.plumbing.Barrier;
import org.apache.edgent.oplet.plumbing.Isolate;
import org.apache.edgent.oplet.plumbing.PressureReliever;
import org.apache.edgent.oplet.plumbing.UnorderedIsolate;
import org.apache.edgent.topology.TStream;
import org.apache.edgent.topology.TopologyProvider;

/**
 * Plumbing utilities for {@link TStream}.
 * Methods that manipulate the flow of tuples in a streaming topology,
 * but are not part of the logic of the application.
 */
public class PlumbingStreams {
  
    /**
     * Insert a blocking delay between tuples.
     * Returned stream is the input stream delayed by {@code delay}.
     * <p>
     * Delays less than 1msec are translated to a 0 delay.
     * <p>
     * This function always adds the {@code delay} amount after receiving
     * a tuple before forwarding it.  
     * <p>
     * Downstream tuple processing delays will affect
     * the overall delay of a subsequent tuple.
     * <p>
     * e.g., the input stream contains two tuples t1 and t2 and
     * the delay is 100ms.  The forwarding of t1 is delayed by 100ms.
     * Then if a downstream processing delay of 80ms occurs, this function
     * receives t2 80ms after it forwarded t1 and it will delay another
     * 100ms before forwarding t2.  Hence the overall delay between forwarding
     * t1 and t2 is 180ms.
     * See {@link #blockingThrottle(long, TimeUnit) blockingThrottle}.
     * 
     * @param stream Stream t
     * @param delay Amount of time to delay a tuple.
     * @param unit Time unit for {@code delay}.
     * 
     * @return Stream that will be delayed.
     */
    public static <T> TStream<T> blockingDelay(TStream<T> stream, long delay, TimeUnit unit) {
        return stream.map(t -> {try {
            Thread.sleep(unit.toMillis(delay));
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            throw new RuntimeException(e);
        } return t;}) ;
    }
    
    /**
     * Maintain a constant blocking delay between tuples.
     * The returned stream is the input stream throttled by {@code delay}.
     * <p>
     * Delays less than 1msec are translated to a 0 delay.
     * <p>
     * Sample use:
     * <pre>{@code
     * TStream<String> stream = topology.strings("a", "b, "c");
     * // Create a stream with tuples throttled to 1 second intervals.
     * TStream<String> throttledStream = blockingThrottle(stream, 1, TimeUnit.SECOND);
     * // print out the throttled tuples as they arrive
     * throttledStream.peek(t -> System.out.println(new Date() + " - " + t));
     * }</pre>
     * <p>
     * The function adjusts for downstream processing delays.
     * The first tuple is not delayed.  If {@code delay} has already
     * elapsed since the prior tuple was forwarded, the tuple 
     * is forwarded immediately.
     * Otherwise, forwarding the tuple is delayed to achieve
     * a {@code delay} amount since forwarding the prior tuple.
     * <p>
     * e.g., the input stream contains two tuples t1 and t2 and
     * the delay is 100ms.  The forwarding of t1 is delayed by 100ms.
     * Then if a downstream processing delay of 80ms occurs, this function
     * receives t2 80ms after it forwarded t1 and it will only delay another
     * 20ms (100ms - 80ms) before forwarding t2.  
     * Hence the overall delay between forwarding t1 and t2 remains 100ms.
     * 
     * @param <T> tuple type
     * @param stream the stream to throttle
     * @param delay Amount of time to delay a tuple.
     * @param unit Time unit for {@code delay}.
     * @return the throttled stream
     */
    public static <T> TStream<T> blockingThrottle(TStream<T> stream, long delay, TimeUnit unit) {
        return stream.map( blockingThrottle(delay, unit) );
    }

    private static <T> Function<T,T> blockingThrottle(long delay, TimeUnit unit) {
        long[] nextTupleTime = { 0 };
        return t -> {
            long now = System.currentTimeMillis();
            if (nextTupleTime[0] != 0) {
                if (now < nextTupleTime[0]) {
                    try {
                        Thread.sleep(nextTupleTime[0] - now);
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                        throw new RuntimeException(e);
                    }
                    now = System.currentTimeMillis();
                }
            }
            nextTupleTime[0] = now + unit.toMillis(delay);
            return t;
        };
    }
    
    /**
     * Insert a blocking delay before forwarding the first tuple and
     * no delay for subsequent tuples.
     * <p>
     * Delays less than 1msec are translated to a 0 delay.
     * <p>
     * Sample use:
     * <pre>{@code
     * TStream<String> stream = topology.strings("a", "b, "c");
     * // create a stream where the first tuple is delayed by 5 seconds. 
     * TStream<String> oneShotDelayedStream =
     *      stream.map( blockingOneShotDelay(5, TimeUnit.SECONDS) );
     * }</pre>
     * 
     * @param <T> tuple type
     * @param stream input stream
     * @param delay Amount of time to delay a tuple.
     * @param unit Time unit for {@code delay}.
     * @return the delayed stream
     */
    public static <T> TStream<T> blockingOneShotDelay(TStream<T> stream, long delay, TimeUnit unit) {
        return stream.map( blockingOneShotDelay(delay, unit) );
    }

    private static <T> Function<T,T> blockingOneShotDelay(long delay, TimeUnit unit) {
        long[] initialDelay = { unit.toMillis(delay) };
        return t -> {
            if (initialDelay[0] != -1) {
                try {
                    Thread.sleep(initialDelay[0]);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    throw new RuntimeException(e);
                }
                initialDelay[0] = -1;
            }
            return t;
            };
    }
    
    /**
     * Relieve pressure on upstream processing by discarding tuples.
     * This method ensures that upstream processing is not
     * constrained by any delay in downstream processing,
     * for example by a connector not being able to connect
     * to its external system.
     * <P>
     * Any downstream processing of the returned stream is isolated
     * from {@code stream} so that any slow down does not affect {@code stream}.
     * When the downstream processing cannot keep up with rate of
     * {@code stream} tuples will be dropped from returned stream.
     * <BR>
     * Up to {@code count} of the most recent tuples per key from {@code stream}
     * are maintained when downstream processing is slow, any older tuples
     * that have not been submitted to the returned stream will be discarded.
     * <BR>
     * Tuple order is maintained within a partition but is not guaranteed to
     * be maintained across partitions.
     * </P>
     * 
     * @param stream Stream to be isolated from downstream processing.
     * @param keyFunction Function defining the key of each tuple.
     * @param count Maximum number of tuples to maintain when downstream processing is backing up.
     * @return Stream that is isolated from and thus relieves pressure on {@code stream}.
     * 
     * @param <T> Tuple type.
     * @param <K> Key type.
     * @see #isolate(TStream, int) isolate
     */
    public static <T,K> TStream<T> pressureReliever(TStream<T> stream, Function<T,K> keyFunction, int count) {
        return stream.pipe(new PressureReliever<>(count, keyFunction));
    }
    
    /**
     * Isolate upstream processing from downstream processing.
     * <BR>
     * Implementations may throw {@code OutOfMemoryExceptions} 
     * if the processing against returned stream cannot keep up
     * with the arrival rate of tuples on {@code stream}.
     *  
     * @param stream Stream to be isolated from downstream processing.
     * @param ordered {@code true} to maintain arrival order on the returned stream,
     * {@code false} to not guaranteed arrival order.
     * @return Stream that is isolated from {@code stream}.
     */
    public static <T> TStream<T> isolate(TStream<T> stream, boolean ordered) {
        return stream.pipe(
                ordered ? new Isolate<T>() : new UnorderedIsolate<T>());
    }
    
    /**
     * Isolate upstream processing from downstream processing.
     * <P>
     * If the processing against the returned stream cannot keep up
     * with the arrival rate of tuples on {@code stream}, upstream
     * processing will block until there is space in the queue between
     * the streams.
     * </P><P>
     * Processing of tuples occurs in the order they were received.
     * </P>
     *  
     * @param stream Stream to be isolated from downstream processing.
     * @param queueCapacity size of the queue between {@code stream} and
     *        the returned stream.
     * @return Stream that is isolated from {@code stream}.
     * @see #pressureReliever(TStream, Function, int) pressureReliever
     */
    public static <T> TStream<T> isolate(TStream<T> stream, int queueCapacity) {
      return stream.pipe(new Isolate<T>(queueCapacity));
    }
    
    /**
     * Perform analytics concurrently.
     * <P>
     * This is a convenience function that calls
     * {@link #concurrent(TStream, List, Function)} after
     * creating {@code pipeline} and {@code combiner} functions
     * from the supplied {@code mappers} and {@code combiner} arguments.
     * </P><P>
     * That is, it is logically, if not exactly, the same as:
     * <pre>{@code
     * List<Function<TStream<T>,TStream<U>>> pipelines = new ArrayList<>();
     * for (Function<T,U> mapper : mappers)
     *   pipelines.add(s -> s.map(mapper));
     * concurrent(stream, pipelines, s -> s.map(combiner));
     * }</pre>
     * </P>
     * 
     * @param <T> Tuple type on input stream.
     * @param <U> Tuple type generated by mappers.
     * @param <R> Tuple type of the result.
     * 
     * @param stream input stream
     * @param mappers functions to be run concurrently.  Each mapper MUST
     *                 return a non-null result.
     *                 A runtime error will be generated if a null result
     *                 is returned.
     * @param combiner function to create a result tuple from the list of
     *                 results from {@code mappers}.
     *                 The input list order is 1:1 with the {@code mappers} list.
     *                 I.e., list entry [0] is the result from mappers[0],
     *                 list entry [1] is the result from mappers[1], etc.
     * @return result stream
     */
    public static <T,U,R> TStream<R> concurrentMap(TStream<T> stream, List<Function<T,U>> mappers, Function<List<U>,R> combiner) {
      Objects.requireNonNull(stream, "stream");
      Objects.requireNonNull(mappers, "mappers");
      Objects.requireNonNull(combiner, "combiner");
      
      List<Function<TStream<T>,TStream<U>>> pipelines = new ArrayList<>();
      for (Function<T,U> mapper : mappers) {
        pipelines.add(s -> s.map(mapper));
      }
      
      return concurrent(stream, pipelines, s -> s.map(combiner));
    }
    
    // Q: is there any value to this implementation approach?  Or just dispose of it?
    @SuppressWarnings("unused")
    private static <T,U,R> TStream<R> concurrentMapSingleOp(TStream<T> stream, List<Function<T,U>> mappers, Function<List<U>,R> combiner) {
      Objects.requireNonNull(stream, "stream");
      Objects.requireNonNull(mappers, "mappers");
      Objects.requireNonNull(combiner, "combiner");
      
      // INITIAL IMPL TO GET STARTED - validate interface and test
      // explore an impl with no new oplets
      //
      // This is the most lightweight impl possible wrt no intermediate streams
      // i.e., all of the processing is handled within a single injected map()
      // 
      // TODO: want to create ExecutorService using provider's ThreadFactory service.
      //       Can't get RuntimeServicesSupplier from a stream.
      //
      // Note, we impose this "non-null mapper result" requirement so as
      // to enable alternative implementations that might be burdened if
      // null results were allowed.
      // The implementation below could easily handle null results w/o
      // losing synchronization, with the combiner needing to deal with
      // a null result in the list it's given.
      
      AtomicReference<ExecutorService> executorRef = new AtomicReference<>();
      
      return stream.map(tuple -> {
        if (executorRef.get() == null) {
          executorRef.compareAndSet(null, Executors.newFixedThreadPool(Math.min(mappers.size(), 20)));
        }
        ExecutorService executor = executorRef.get();
        List<U> results = new ArrayList<>(Collections.nCopies(mappers.size(), null));
        List<Future<?>> futures = new ArrayList<>(mappers.size());

        // Submit a task for each mapper invocation
        int ch = 0;
        for (Function<T,U> mapper : mappers) {
          final int resultIndx = ch++;
          Future<?> future = executor.submit(() -> {
            U result = mapper.apply(tuple);
            if (result == null)
              throw new IllegalStateException("mapper index "+resultIndx+" returned null");
            results.set(resultIndx, result); 
          });
          futures.add(future);
        }
        // Await completion of all
        for (Future<?> future : futures) {
          try {
            future.get();
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            throw new RuntimeException("mapper interrupted", e);
          } catch (Exception e) {
            throw new RuntimeException("mapper threw", e);
          }
        }
        // Run the combiner
        R result = combiner.apply(results);
        return result;
      });
      
    }

    /**
     * Perform analytics concurrently.
     * <P>
     * Process input tuples one at at time, invoking the specified
     * analytics ({@code pipelines}) concurrently, combine the results,
     * and then process the next input tuple in the same manner.
     * </P><P>
     * Logically, instead of doing this:
     * <pre>{@code
     * sensorReadings<T> -> A1pipeline -> A2pipeline -> A3pipeline -> results<R>
     * }</pre>
     * create a graph that's logically like this:
     * <pre>{@code
     * - 
     *                      |->  A1pipeline  ->|
     * sensorReadings<T> -> |->  A2pipeline  ->| -> result<R>
     *                      |->  A3pipeline  ->|
     * }</pre>
     * </P><P>
     * The typical use case for this is when an application has a collection
     * of independent analytics to perform on each tuple and the analytics
     * are sufficiently long running such that performing them concurrently
     * is desired.
     * </P><P>
     * Note, this is in contrast to "parallel" stream processing,
     * which in Java8 Streams and other contexts means processing multiple
     * tuples in parallel, each on a replicated processing pipeline.
     * </P><P>
     * Threadsafety - one of the following must be true:
     * <ul>
     * <li>the tuples from {@code stream} are threadsafe</li>
     * <li>the {@code pipelines} do not modify the input tuples</li>
     * <li>the {@code pipelines} provide their own synchronization controls
     *     to protect concurrent modifications of the input tuples</li>
     * </ul>
     * </P><P>
     * Logically, a thread is allocated for each of the {@code pipelines}.
     * The actual degree of concurrency may be {@link TopologyProvider} dependent.
     * </P>
     * 
     * @param <T> Tuple type on input stream.
     * @param <U> Tuple type generated by pipelines.
     * @param <R> Tuple type of the result.
     * 
     * @param stream input stream
     * @param pipelines a list of functions to add a pipeline to the topology.
     *                 Each {@code pipeline.apply()} is called with {@code stream}
     *                 as the input, yielding the pipeline's result stream.
     *                 For each input tuple, a pipeline MUST create exactly one output tuple.
     *                 Tuple flow into the pipelines will cease if that requirement
     *                 is not met.
     * @param combiner a function that creates a result stream from a stream
     *                 whose tuples are the list of each pipeline's result.
     *                 The input tuple list's order is 1:1 with the {@code pipelines} list.
     *                 I.e., list entry [0] is the result from pipelines[0],
     *                 list entry [1] is the result from pipelines[1], etc.
     * @return result stream
     */
    public static <T,U,R> TStream<R> concurrent(TStream<T> stream, List<Function<TStream<T>,TStream<U>>> pipelines, Function<TStream<List<U>>,TStream<R>> combiner) {
      Objects.requireNonNull(stream, "stream");
      Objects.requireNonNull(pipelines, "pipelines");
      Objects.requireNonNull(combiner, "combiner");
      
      // Summary of what's below:
      //           |-> isolate(1) -> p1 -> |
      // stream -> |-> isolate(1) -> p2 -> |-> barrier(10) -> combiner 
      //           |-> isolate(1) -> p3 -> |
      //                . . .

      int barrierQueueCapacity = 10;  // don't preclude pipelines from getting ahead some.
      
      // Add concurrent (isolated) fanouts
      List<TStream<T>> fanouts = new ArrayList<>(pipelines.size());
      for (int i = 0; i < pipelines.size(); i++)
        fanouts.add(isolate(stream, 1).tag("concurrent.isolated-ch"+i));
      
      // Add pipelines
      List<TStream<U>> results = new ArrayList<>(pipelines.size());
      int ch = 0;
      for (Function<TStream<T>,TStream<U>> pipeline : pipelines) {
        results.add(pipeline.apply(fanouts.get(ch)).tag("concurrent-ch"+ch));
        ch++;
      }
      
      // Add the barrier
      TStream<List<U>> barrier = barrier(results, barrierQueueCapacity).tag("concurrent.barrier");
      
      // Add the combiner
      return combiner.apply(barrier);
    }

    /**
     * A tuple synchronization barrier.
     * <P>
     * Same as {@code barrier(others, 1)}
     * </P>
     * @see #barrier(List, int)
     */
    public static <T> TStream<List<T>> barrier(List<TStream<T>> streams) {
      return barrier(streams, 1);
    }

    /**
     * A tuple synchronization barrier.
     * <P>
     * A barrier has n input streams with tuple type {@code T}
     * and one output stream with tuple type {@code List<T>}.
     * Once the barrier receives one tuple on each of its input streams,
     * it generates an output tuple containing one tuple from each input stream.
     * It then waits until it has received another tuple from each input stream.
     * </P><P>
     * Input stream 0's tuple is in the output tuple's list[0],
     * stream 1's tuple in list[1], and so on.
     * </P><P>
     * The barrier's output stream is isolated from the input streams.
     * </P><P>
     * The barrier has a queue of size {@code queueCapacity} for each
     * input stream.  When a tuple for an input stream is received it is
     * added to its queue.  The stream will block if the queue is full.
     * </P>
     *
     * @param <T> Type of the tuple.
     * 
     * @param streams the list of input streams
     * @param queueCapacity the size of each input stream's queue
     * @return the output stream
     * @see Barrier
     */
    public static <T> TStream<List<T>> barrier(List<TStream<T>> streams, int queueCapacity) {
      List<TStream<T>> others = new ArrayList<>(streams);
      TStream<T> s1 = others.remove(0);
      return s1.fanin(new Barrier<T>(queueCapacity), others);
    }
 
}
