/*
 * You don't have to change anything in this file!
 * Anyways, feel free to play around with parameters for testing purposes.
 */

#ifndef A2_DEFINITIONS_H
#define A2_DEFINITIONS_H
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include "resources.h"
#include "vector.h"

#define UPPER_LIMIT_WAITERS 1000
#define UPPER_LIMIT_GUESTS  2000
#define LOWER_LIMIT            0

#define NUM_WAITERS     "num_waiters"
#define NUM_GUESTS      "num_guests"

#define NUM_TABLES 50

#define MAX_SATION 100
#define START_SATION 0

#define NR_ARGUMENTS 3
#define ERROR -1

// ------------------------------------------------------------------------
void checkAllowedRange(ssize_t value, ssize_t upper_limit, char* name)
{
  if (value < LOWER_LIMIT || value > upper_limit) {
    fprintf(stderr, "%s allowed range: [%d,%zd].\n", name, LOWER_LIMIT, upper_limit);
    exit(ERROR);
  }
}

// ------------------------------------------------------------------------
void checkParametersValidity(ssize_t num_waiters, ssize_t num_guests) 
{
  checkAllowedRange(num_waiters, UPPER_LIMIT_WAITERS, NUM_WAITERS);
  checkAllowedRange(num_guests, UPPER_LIMIT_GUESTS, NUM_GUESTS);
}

// ------------------------------------------------------------------------
void handleParameters(int argc, char* argv[], ssize_t* num_waiters, ssize_t* num_guests)
{
  if (argc != NR_ARGUMENTS) 
  {
    fprintf(stderr, "Usage: %s <num_waiters> <num_guests>\n", argv[0]);
    exit(ERROR);
  }

  // check the validity of parameters
  *num_waiters = atoi(argv[1]);
  *num_guests = atoi(argv[2]);

  checkParametersValidity(*num_waiters, *num_guests);
}

// ------------------------------------------------------------------------
void driveToWork(Waiter* waiters, ssize_t num_waiters, void* start_routine)
{
  for (ssize_t i = 0; i < num_waiters; i++) 
  {
    waiters[i].id = i;
    assert(!pthread_create(&waiters[i].waiter, NULL, start_routine, (void*)&waiters[i]));
  }
}

// ------------------------------------------------------------------------
void chooseARestaurant(Guest* guests, ssize_t num_guests, void* start_routine)
{
  for (ssize_t i = 0; i < num_guests; i++) 
  {
    guests[i].id = i;
    guests[i].sation = START_SATION;
    assert(!pthread_create(&guests[i].guest, NULL, start_routine, (void*)&guests[i]));
  }
}

// ------------------------------------------------------------------------
void freeResources(Waiter* waiters, Guest* guests, vector* free_tables, vector* tables_to_serve)
{
  free(waiters);
  free(guests);
  
  vector_iterator it = vector_begin(free_tables);
  while (it != vector_end(free_tables)) 
  {
    free(*it);
    vector_erase(free_tables, it);
  }
  vector_destroy(free_tables);

  it = vector_begin(tables_to_serve);
  while (it != vector_end(tables_to_serve))
  {
    free(*it);
    vector_erase(tables_to_serve, it);
  }
  vector_destroy(tables_to_serve);
}

// ------------------------------------------------------------------------
void chooseMainCourse() 
{
  ssize_t microsec_to_sleep = 1000 + rand() % (20 * 1000);
  nanosleep((const struct timespec[]){{0, 1000L * microsec_to_sleep}}, NULL);
}

// ------------------------------------------------------------------------
ssize_t getSation()  
{
  return rand() % MAX_SATION;
}

// ------------------------------------------------------------------------
void eatMeal(Guest* guest) 
{
  ssize_t microsec_to_sleep = 1000 + rand() % (23 * 1000);
  nanosleep((const struct timespec[]){{0, 1000L * microsec_to_sleep}}, NULL);
  guest->sation += getSation();
}

// ------------------------------------------------------------------------
size_t isBreakNeeded() 
{
  return rand() % 4 == 0;
}

// ------------------------------------------------------------------------
void takeBreak() 
{
  ssize_t microsec_to_sleep = 1000 + rand() % (17 * 1000);
  nanosleep((const struct timespec[]){{0, 1000L * microsec_to_sleep}}, NULL);
}

// ------------------------------------------------------------------------
void takeOrder()
{
  ssize_t microsec_to_sleep = 1000 + rand() % (15 * 1000);
  nanosleep((const struct timespec[]){{0, 1000L * microsec_to_sleep}}, NULL);  
}

// ------------------------------------------------------------------------
void clean()
{
  ssize_t microsec_to_sleep = 1000 + rand() % (13 * 1000);
  nanosleep((const struct timespec[]){{0, 1000L * microsec_to_sleep}}, NULL);  
}

// ------------------------------------------------------------------------

#endif //A2_DEFINITIONS_H
