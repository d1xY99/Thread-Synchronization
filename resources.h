
#ifndef A2_RESOURCES_H
#define A2_RESOURCES_H
#include "pthread.h"
#include "stdint.h"
#include "stdlib.h"
#include "vector.h"
#include <stdbool.h>
#include <assert.h>

/* STUDENT TODO:
 *  make sure shared resources are locked correctly
 *  add any needed synchronization primitives here
 */

typedef struct {
  pthread_t guest;
  ssize_t id;
  ssize_t sation;
} Guest;

typedef struct{
  pthread_t waiter;
  ssize_t id;
} Waiter;

typedef struct {
  Guest* guest;
  ssize_t id;
  bool table_set;
  bool meal_set;
  pthread_mutex_t mtx;
  sem_t meal_sem;
} Table;

#endif //A2_RESOURCES_H