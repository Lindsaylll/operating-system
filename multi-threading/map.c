/* map.c
 * ----------------------------------------------------------
 *  CS350
 *  Assignment 2
 *  Question 1
 *
 *  Purpose:  Gain experience with threads and basic
 *  synchronization.
 *
 *  YOU MAY ADD WHATEVER YOU LIKE TO THIS FILE.
 *  YOU CANNOT CHANGE THE SIGNATURE OF MultithreadedWordCount.
 * ----------------------------------------------------------
 */
#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

/* --------------------------------------------------------------------
 * MultithreadedWordCount
 * --------------------------------------------------------------------
 * Takes a Library of articles containing words and a word.
 * Returns the total number of times that the word appears in the
 * Library.
 *
 * For example, "There the thesis sits on the theatre bench.", contains
 * 2 occurences of the word "the".
 * --------------------------------------------------------------------
 */

// this SingleThreadedWordCount is copied form the given file main.c
struct ARGS
{
  struct Library **lib;
  char *word;
  int start;
  int offset;
  // int local_wc;
};

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
size_t wordCount = 0;

void *SingleThreadedWC(void *args)
{
  struct ARGS *arg;
  arg = (struct ARGS *)args;
  char *word = arg->word;

  int end = arg->start + arg->offset;
  struct Article *art = malloc(sizeof(struct Article));
  for (int i = arg->start; i < end; i++)
  {
    printf("in signle thread count; start is : %d\n", i);
    // struct Library *lib = ;
    
    art = (*arg->lib)->articles[i];
    for (unsigned int j = 0; j < art->numWords; j++)
    {
      // Get the length of the function.
      size_t len = strnlen(art->words[j], MAXWORDSIZE);
      if (!strncmp(art->words[j], word, len))
      {
        pthread_mutex_lock(&mutex1);
        wordCount++;
        pthread_mutex_unlock(&mutex1);
      }
    }
  }

  // return wordCount;
  // printf("in signle thread count\n");
  return 0;
}

size_t MultithreadedWordCount(struct Library *lib, char *word)
{
  printf("Parallelizing with %d threads...\n", NUMTHREADS);
  pthread_t *thread_id;
  // pthread_t thread_id[numthreads];
  struct ARGS **all_args;
  printf("about to malloc\n");
  all_args = malloc(NUMTHREADS * sizeof(struct ARGS *));
  printf("mallocated struct ARGS *\n");
  thread_id = malloc(NUMTHREADS * sizeof(pthread_t));
  printf("mallocated struct pthread_t\n");
  int offset = ceil(lib->numArticles / NUMTHREADS);
  int st = 0;
  for (int i = 0; i < NUMTHREADS - 1; ++i)
  {
    printf("dealing arg %d, with start = %d\n", i, st);
    all_args[i] = malloc(sizeof(struct ARGS));
    all_args[i]->lib = &lib;
    all_args[i]->word = word;
    all_args[i]->offset = offset;
    all_args[i]->start = st;
    st += offset;
  }
  printf("dealing arg %d, with start = %d\n", NUMTHREADS, st);
  all_args[NUMTHREADS-1] = malloc(sizeof(struct ARGS));
  all_args[NUMTHREADS-1]->lib = &lib;
  all_args[NUMTHREADS-1]->word = word;
  all_args[NUMTHREADS-1]->offset = lib->numArticles - st;
  all_args[NUMTHREADS-1]->start = st;
  // printf("finish malloc all_args[NUMTHREADS]\n");
  // printf("all_args[NUMTHREADS]->offset is: %d\n", all_args[2]->offset);
  for(int i = 0; i < NUMTHREADS; ++i){
printf("all_args[%d]->start is: %d\nx", i, all_args[i]->start);
  }
  

  for (int i = 0; i < NUMTHREADS; ++i)
  {

    int rt = pthread_create(&thread_id[i], NULL, SingleThreadedWC, (void *)all_args[i]);
    if (rt)
    {
      printf("Thread creation failed: #%d\n", i);
    }
    // printf("rt is: %d\n", rt);
  }
  printf("about to join\n");
  for (int i = 0; i < NUMTHREADS; ++i)
  {
    pthread_join(thread_id[i], NULL);
  }

  return wordCount;
}
