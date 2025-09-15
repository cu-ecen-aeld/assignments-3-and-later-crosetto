#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
  struct thread_data* tp = (struct thread_data*) thread_param;
  usleep(1000*tp->wait_to_obtain);
  pthread_mutex_lock(tp->mutex);
  usleep(1000*tp->wait_to_release);
  pthread_mutex_unlock(tp->mutex);
  tp->thread_complete_success=true;
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
  return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{

  struct thread_data* td;
  td = malloc(sizeof(struct thread_data));
  td->wait_to_release = wait_to_release_ms;
  td->wait_to_obtain = wait_to_obtain_ms;
  td->mutex = mutex;
  int err = pthread_create(thread, NULL, &threadfunc, td);
  
  /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    return err==0;
}

