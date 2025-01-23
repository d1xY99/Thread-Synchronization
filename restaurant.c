#include "definitions.h"

vector free_tables;
vector tables_to_serve;

bool restaurant_opened = false;
/*
 * STUDENT TODO BEGIN:
 *  locking-related global variables may be declared here
 */

/* Restaurant opened condition */
pthread_cond_t  opened_cv;
pthread_mutex_t opened_cv_mtx;

sem_t free_tables_sem;
sem_t tables_to_serve_sem;

pthread_mutex_t free_tables_mtx;
pthread_mutex_t tables_to_serve_mtx;


// ------------------------------------------------------------------------
/* STUDENT TODO:
 *  make sure : 
    ~ shared resources are locked correctly
    ~ the table, for which the meal was prepared gets notified
 */
void prepareMeal(Table* table)
{
  table->meal_set = true;

  printf("Waiter finished preparing table %zd's order!\n", table->id);
  
  assert(!sem_post(&table->meal_sem)); // NOTIFYING ABOUT MEAL
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 *  make sure : 
    ~ shared resources are locked correctly
 */
void cleanTable(Table* table)
{
  clean();
  
  table->table_set = true;

  printf("Waiter finished cleaning table %zd!\n", table->id);
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 *  make sure : 
    ~ shared resources are locked correctly
    ~ everbody is notified about the opening
 */
void openRestaurant()
{
  restaurant_opened = true;

  printf("KITCHEN OPENED, DAY STARTING!\n");

  assert(!pthread_mutex_lock(&opened_cv_mtx));
  assert(!pthread_cond_broadcast(&opened_cv));
  assert(!pthread_mutex_unlock(&opened_cv_mtx));
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 * make sure :
    ~ all shared resources are locked correctly
 */
vector_iterator findFreeTable()
{
  vector_iterator it = vector_begin(&free_tables);
  vector_iterator it_end = vector_end(&free_tables);
  vector_iterator table_it = NULL;

  for (; it != it_end; ++it) 
  {
   if (((Table*)*it)->table_set) 
    {
      table_it = it;
      break;
    }
  }
  
  return table_it;
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 * make sure :
    ~ all shared resources are locked correctly
 */
vector_iterator findTableToClean()
{
  vector_iterator it = vector_begin(&tables_to_serve);
  vector_iterator it_end = vector_end(&tables_to_serve);
  vector_iterator table_it = NULL;

  for (; it != it_end; ++it) 
  {
    if (!(((Table*)*it)->table_set) && !(((Table*)*it)->meal_set)) 
    {
      table_it = it;
      break;
    }
  }
  return table_it;
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 * make sure :
    ~ all shared resources are locked correctly
 */
vector_iterator findTableWithOrder()
{
  vector_iterator it = vector_begin(&tables_to_serve);
  vector_iterator it_end = vector_end(&tables_to_serve);
  vector_iterator table_it = NULL;

  for (; it != it_end; ++it) 
  {
    if ((((Table*)*it)->table_set) && !(((Table*)*it)->meal_set)) 
    {
      table_it = it;
      break;
    }
  }
  return table_it;
}

/* STUDENT TODO:
 *  make sure : 
    ~ shared resources are locked correctly
    ~ waiter does not serve a table until a guest arrives 
    ~ waiters don't work when there is nothing to do
    ~ waiters prioritze preparing orders over cleaning tables
 */
void workAtRestaurant(Waiter* waiter)
{
  assert(!pthread_mutex_lock(&opened_cv_mtx));
  assert(!pthread_cond_wait(&opened_cv, &opened_cv_mtx));
  assert(!pthread_mutex_unlock(&opened_cv_mtx));

  printf("Waiter %zd arrives to work\n", waiter->id);

  while(1)
  {
    printf("Waiter %zd waits for the next work order\n", waiter->id);

    if(isBreakNeeded())
    {
      printf("Waiter %zd is taking a break!\n", waiter->id);
      takeBreak(); 
      continue;
    }

    assert(!sem_wait(&tables_to_serve_sem));

    assert(!pthread_mutex_lock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE LOCK

    vector_iterator table_it = findTableWithOrder();
    Table* table = NULL;

    if(table_it != NULL)
    {
      table = *table_it;

      vector_erase(&tables_to_serve, table_it);

      assert(!pthread_mutex_unlock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE UNLOCK BRANCH 1

      takeOrder();

      if(!table->table_set)
      {
        printf("Waiter %zd prepared a meal for an empty table\n", waiter->id);
      }
      else if(table->meal_set)
      {
        printf("Waiter %zd prepared a meal, eventhough it was already served\n", waiter->id);
      }

      prepareMeal(table);

    }
    else 
    { 
      table_it = findTableToClean();

      if(table_it == NULL)
      {
        assert(!pthread_mutex_unlock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE UNLOCK BRANCH 2
        printf("Waiter %zd has nothing to do!\n", waiter->id);
        takeBreak();
        continue;
      }

      table = *table_it;

      vector_erase(&tables_to_serve, table_it);

      assert(!pthread_mutex_unlock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE UNLOCK BRANCH 3

      if(table->table_set)
      {
        printf("Waiter %zd wanted to clean an already clean table\n", waiter->id); 
      }

      cleanTable(table);

      assert(!pthread_mutex_lock(&free_tables_mtx)); // <------- FREE TABLES LOCK
      
      vector_push_back(&free_tables, table);

      assert(!pthread_mutex_unlock(&free_tables_mtx)); // <------- FREE TABLES UNLOCK

      assert(!sem_post(&free_tables_sem));
    }
    
  }
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 *  make sure : 
    ~ shared resources are locked correctly
    ~ guests wait for the restaurant to open (do not ask for a free table)
    ~ guests do not leave before they have eaten 
 */
void eatAtRestaurant(Guest* guest)
{
  assert(!pthread_mutex_lock(&opened_cv_mtx));
  assert(!pthread_cond_wait(&opened_cv, &opened_cv_mtx));
  assert(!pthread_mutex_unlock(&opened_cv_mtx));

  assert(!sem_wait(&free_tables_sem)); // <----------- WAITING FOR FREE TABLE

  printf("Guest %zd asks for a free table\n", guest->id);

  assert(!pthread_mutex_lock(&free_tables_mtx)); // <------- FREE TABLES LOCK

  vector_iterator table_it = findFreeTable();
  Table* table = *table_it;

  if(table->guest != NULL)
  {
    printf("Guest %zd is confused, someone is already sitting in their place!\n", guest->id);
  } 
  else if(!table->table_set)
  {
    printf("Guest %zd is confused, the table they got isn't even set\n", guest->id);
  }

  table->guest = guest;

  vector_erase(&free_tables, table_it);

  assert(!pthread_mutex_unlock(&free_tables_mtx)); // <------- FREE TABLES UNLOCK

  chooseMainCourse();

  assert(!pthread_mutex_lock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE LOCK

  vector_push_back(&tables_to_serve, table);

  assert(!pthread_mutex_unlock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE UNLOCK

  assert(!sem_post(&tables_to_serve_sem));

  assert(!sem_wait(&table->meal_sem));   // <----------- WAITING FOR MEAL

  if(!table->meal_set) 
  {
    printf("Waiter said meal is ready, but there is no meal on guest %zd's table!\n", guest->id);
  }

  eatMeal(guest);

  assert(!pthread_mutex_lock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE LOCK

  table->table_set = false;
  table->meal_set = false;
  table->guest = NULL;
  vector_push_back(&tables_to_serve, table);

  assert(!pthread_mutex_unlock(&tables_to_serve_mtx)); // <------- TABLES_TO_SERVE UNLOCK

  assert(!sem_post(&tables_to_serve_sem));

  printf("Guest %zd is sated and can go home\n", guest->id);
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 *  make sure : 
    ~ synchronization primitive(s) initialized correctly
    ~ shared resources are locked correctly
 */
void setTheTables(vector* free_tables, vector* tables_to_serve, ssize_t num_tables)
{
  vector_init(free_tables);
  vector_init(tables_to_serve);

  pthread_cond_init(&opened_cv, NULL);
  pthread_mutex_init(&opened_cv_mtx, NULL);

  sem_init(&free_tables_sem, 0, 0);
  sem_init(&tables_to_serve_sem, 0, num_tables);

  pthread_mutex_init(&free_tables_mtx, NULL);
  pthread_mutex_init(&tables_to_serve_mtx, NULL);

  for (ssize_t i = 0; i < num_tables; i++) 
  {
    Table* table = malloc(sizeof(Table));

    if(!table)
    {
      free(table);
      fprintf(stderr, "Could not allocate memory!\n");
      exit(ERROR);
    }
    
    table->guest = NULL;
    table->id = i;
    table->table_set = false;
    table->meal_set = false;

    pthread_mutex_init(&table->mtx, NULL);
    sem_init(&table->meal_sem, 0, 0);

    vector_push_back(tables_to_serve, table);   
  }
}

// ------------------------------------------------------------------------
/* STUDENT TODO:
 *  make sure : 
    ~ synchronization primitive(s) initialized correctly
    ~ shared resources are locked correctly
    ~ all waiters end their work, once all guests are served
 */
int main(int argc, char* argv[])
{
  srand(time(NULL));

  ssize_t num_waiters;
  ssize_t num_guests;

  handleParameters(argc, argv, &num_waiters, &num_guests);
  
  Waiter* waiters = malloc(num_waiters * sizeof(Waiter));
  Guest* guests = malloc(num_guests * sizeof(Guest));
  
  if (!waiters || !guests) 
  {
    free(waiters);
    free(guests);
    fprintf(stderr, "Could not allocate memory!\n");
    exit(ERROR);
  }
 
  setTheTables(&free_tables, &tables_to_serve, NUM_TABLES);
  driveToWork(waiters, num_waiters, (void*(*)(void*))workAtRestaurant);
  chooseARestaurant(guests, num_guests, (void*(*)(void*))eatAtRestaurant); 
  
  pthread_t manager;
  assert(!pthread_create(&manager, NULL, (void*(*)(void*))&openRestaurant, NULL));

  for (ssize_t i = 0; i < num_guests; i++) 
  {
    pthread_join(guests[i].guest, NULL);
  }

  for (ssize_t i = 0; i < num_waiters; i++) 
  {
    pthread_cancel(waiters[i].waiter);
    pthread_join(waiters[i].waiter, NULL);
  }

  pthread_join(manager, NULL);

  printf("KITCHEN CLOSED, DAY FINISHED!\n");

  /* Mutex cleanup */
  for (vector_iterator it = vector_begin(&free_tables);
       it != vector_end(&free_tables);
       ++it) 
  {
    pthread_mutex_destroy(&((Table*)*it)->mtx);
    sem_destroy(&((Table*)*it)->meal_sem);
  }

  for (vector_iterator it = vector_begin(&tables_to_serve);
       it != vector_end(&tables_to_serve);
       ++it) 
  {
    pthread_mutex_destroy(&((Table*)*it)->mtx);
    sem_destroy(&((Table*)*it)->meal_sem);
  }

  pthread_mutex_destroy(&free_tables_mtx);
  pthread_mutex_destroy(&tables_to_serve_mtx);

  sem_destroy(&free_tables_sem);
  sem_destroy(&tables_to_serve_sem);

  freeResources(waiters, guests, &free_tables, &tables_to_serve);

  return 0;
}