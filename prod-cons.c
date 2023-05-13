/*
 *	File	: pc.c
 *
 *	Title	: Demo Producer/Consumer.
 *
 *	Short	: A solution to the producer consumer problem using
 *		pthreads.
 *
 *	Long 	:
 *
 *	Author	: Andrae Muys
 *
 *	Date	: 18 September 1997
 *
 *	Revised	:
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>       // include the library that contains the gettimeofday() function
#include <signal.h>         // To handle the signal

#define QUEUESIZE 20
#define LOOP 10000
/*Define the number of producers an consumers
*/
#define PRODUCER 10
#define CONSUMER 16

void *producer (void *args);
void *consumer (void *args);

// Struct to measure the time
struct timeval current_time[2*QUEUESIZE];
// Variables to save times
int times[100000];
int count = 0;

// The new struct for FIFO
typedef struct {
  void *(*work)(void *);
  void *arg;
}workFunction;

typedef struct {
  workFunction buffer[QUEUESIZE];           // An array with type workFunction

  int buf[QUEUESIZE];                       // Buf: array, size: QUEUESIZE
  long head, tail;                          // head and tail: Indexes for the array
  int full, empty;                          // full, empty: Use as booleans: TRUE, FALSE
  pthread_mutex_t *mut;                     // A mutex variable
  pthread_cond_t *notFull, *notEmpty;       // Two condition variables, notFull, notEmpty
} queue;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, workFunction in);      // The queue will have workFunction types
void queueDel (queue *q, workFunction *out);    // *out workFunction type

// work function: Argument a pointer
void *work(void *work_array){
  float *work_return;
  work_return = (float *)malloc(10*sizeof(float));
  int i = 0;
  int * work_int = (int *)(work_array);
  for(i = 0; i < 10; i++){
    work_return[i] = (float)sin(work_int[i]);
  }
  printf("\n");
  return (NULL);
}

/*A handler to close the file when we kill the program
*/
void handler(int sig){
  FILE *fp = fopen("time_measurement_q20_con16.txt","wb");
  int x = 0;
  for(x = 0; x < count; x++){
    char str[32];
    sprintf(str, "%d", times[x]);               // Convert the time into string to save it on the file
    fputs(str, fp);                             // Place the string(time) into the file
    fputc('\n',fp);
  }
  fclose(fp);                                   // Close the file
}


int main ()
{
  queue *fifo;                              // Create a queue fifo
  // Create an array of producers and consumers
  pthread_t pro[PRODUCER], con[CONSUMER];                       // Create two pthreads pro and con

  // Handle the ctrl+c
  signal (SIGINT, handler);

  fifo = queueInit ();                      // Run the function to initialize the fifo
  if (fifo ==  NULL) {                      // If the initialization did not return an array print an error message
    fprintf (stderr, "main: Queue Init failed.\n");
    exit (1);
  }
  /*Create multiple producers and consumers. Also join for multiple producers and consumers
  */
  int i = 0;
  for(i = 0; i < PRODUCER; i++)
    pthread_create (&pro[i], NULL, producer, fifo);          // Create the pthread pro, with start function the producer and argument fifo

  int j = 0;
  for(j = 0; j < CONSUMER; j++)
    pthread_create (&con[j], NULL, consumer, fifo);          // Create the pthread con, with start function the consumer and argument fifo

  int k = 0;
  for(k = 0; k < PRODUCER; k++)
    pthread_join (pro[k], NULL);                             // Consumer, wait the producer thread

  int l = 0;
  for(l = 0; l < CONSUMER; l++)
    pthread_join (con[l], NULL);                             // Consumer, wait the producer thread

  queueDelete (fifo);                                   // Delete the fifo queue struct pointer
  return 0;
}

void *producer (void *q)
{
  queue *fifo;
  int i;
  /*An array of random numbers to calculate the sins
  */
  int *p = (int *)malloc(sizeof(int)*10);
  int k = 0;
  for(k = 0; k < 10; k++)
    p[k] = k;

  /* A workFunction type which the producer will insert inside the queue
  */
  workFunction in_queue;

  fifo = (queue *)q;

  for (i = 0; i < LOOP; i++) {                      // Loop to run the code multiple times
    pthread_mutex_lock (fifo->mut);                 // Mutex lock
    while (fifo->full) {                            // While fifo is full
      printf ("producer: queue FULL.\n");           // Print fifo is full
      pthread_cond_wait (fifo->notFull, fifo->mut); // producer thread wait until the fifo is notFull, then unlock the mutex
    }
    //arg in_queue into the queue
    in_queue.arg = p;
    in_queue.work = work;
    queueAdd (fifo, in_queue);

    pthread_mutex_unlock (fifo->mut);               // Unlock the mutex
    pthread_cond_signal (fifo->notEmpty);           // The consumer thread waits for the notEmpty signal
    //usleep (100000);
  }
  return (NULL);
}

void *consumer (void *q)
{
  queue *fifo;
  int i, d;

  /*A workFunction type
  */
  workFunction out;

  fifo = (queue *)q;
  //for (i = 0; i < LOOP; i++) {
  while(1){                                             // Consumer will always run
    pthread_mutex_lock (fifo->mut);                     // Lock the mutex
    while (fifo->empty) {                               // While fifo is empty
      printf ("consumer: queue EMPTY.\n");              // Print the queue is empty
      pthread_cond_wait (fifo->notEmpty, fifo->mut);    // Mutex will remain lock until the fifo is empty. If FIFO not empty unlock the mutex
    }

    queueDel (fifo, &out);                              // The out workFunction will take the final workFunction queue element
    out.work(out.arg);                                  // Here the consumer calls the work function
    free(out.arg);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notFull);                // The producer thread waits for the notFull signal
    printf ("consumer: recieved %d.\n", d);
    //usleep(200000);
  }
  return (NULL);
}

// Queue init: Take a queue, malloc an array for the queue and return the pointer that shows on the first index of the array
queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));                 // Malloc a size of queue
  if (q == NULL) return (NULL);                         // If q = NULL, cannot create the queue

// Initialize the parameters of the queue
  q->empty = 1;                                         // q empty at the begin -> TRUE
  q->full = 0;                                          // q full at the begin -> FALSE
  q->head = 0;                                          // q head and tail at the first index of the array (0)
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));       // Allocate space for the mutex
  pthread_mutex_init (q->mut, NULL);                                    // Initialize the mutex
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));     // Allocate space for the condition variable notFull
  pthread_cond_init (q->notFull, NULL);                                 // Initialize notFull condition variable
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));    // Allocate space for the condition variable notEmpty
  pthread_cond_init (q->notEmpty, NULL);                                // Initialize notEmpty condition variable

  return (q);
}

// Function to delete the queue
void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);               // Destroy the mutex
  free (q->mut);                                // Free the mutex's space
  pthread_cond_destroy (q->notFull);            // Destroy  the condition variable notFull
  free (q->notFull);                            // Free the notFull's space
  pthread_cond_destroy (q->notEmpty);           // Destroy the condition variable notEmpty
  free (q->notEmpty);                           // Free the notEmpty's space
  free (q);                                     // Free the q array space
}

// Add queue
// The queue has types workFunction (not int), so in the queue will be added types of workFunction
void queueAdd (queue *q, workFunction in)
{
// Now the workFunction types will be added to the buffer which is an array of workFunction types
  q->buffer[q->tail] = in;             // Go to the tail index element of the buffer and place the in
  gettimeofday(&current_time[q->tail], NULL);       // Here take the current time and place it at the  q->tail index. Time that the consumer places an element to the queue

  q->tail++;                        // Increase the tail by one
  if (q->tail == QUEUESIZE)         // If the tail went at the final element of the queue, place the tail at the begin
    q->tail = 0;
  if (q->tail == q->head)           // If the tail is equal with head, then the queue is full
    q->full = 1;
  q->empty = 0;

  return;
}

// queueDel will delete types of workFunction
void queueDel (queue *q, workFunction *out)
{
  *out = q->buffer[q->head];                // here buffer
  gettimeofday(&current_time[2*(q->head)], NULL);       // Here take the current time and place it at the  2*q->head index. Time that the consumer takes the an element from the queue

  // Write on the file
  times[count] = (1000000*current_time[2*(q->head)].tv_sec - 1000000*current_time[q->head].tv_sec + current_time[2*(q->head)].tv_usec - current_time[q->head].tv_usec);
  count++;

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}
