#include "assignment.h"


// to ensure that consumer threads can only enter compute,
//  if the capacity assertion does not fail
void consume_enter(struct resource *resource)
{
    printf("entered consume_enter");

    pthread_mutex_lock(&resource->mutex);
    // int max1 =
    while ((resource->num_consumers + 1) > (resource->num_producers * resource->ratio))
    {
        printf("in consume_enter waiting (unadded); cust:%ld, prod: %ld ", resource->num_consumers, resource->num_producers);
        pthread_cond_wait(&resource->cond, &resource->mutex);
        // pthread_mutex_lock(&resource->mutex);
    }
    resource->num_consumers += 1;
    // pthread_mutex_unlock(&resource->mutex);
}

// to ensure that we account for
//  the consumer threads in the resource
void consume_exit(struct resource *resource)
{
    printf("entered consume_exit");
    // pthread_mutex_lock(&resource->mutex);
    resource->num_consumers -= 1;
    printf("in consume_exit SIGNAL (minused cons); cust:%ld, prod: %ld ", resource->num_consumers, resource->num_producers);

    pthread_cond_signal(&resource->cond);

    pthread_mutex_unlock(&resource->mutex);
}

// to ensure that new producer threads are accounted for in the resource
void produce_enter(struct resource *resource)
{
    printf("entered produce_enter");
    pthread_mutex_lock(&resource->mutex);
    resource->num_producers += 1;
    printf("in produce_enter SIGNAL(added pro); cust:%ld, prod: %ld ", resource->num_consumers, resource->num_producers);
    pthread_cond_signal(&resource->cond);
    // pthread_mutex_unlock(&resource->mutex);
}

// to ensure that producer threads can only exit
// if the capacity assertion does not fail
void produce_exit(struct resource *resource)
{
    printf("entered produce_exit");
    // pthread_mutex_lock(&resource->mutex);
    // int max1 =
    while (resource->num_consumers > ((resource->num_producers - 1) * resource->ratio))
    {
        printf("in produce_exit waiting (unminused); cust:%ld, prod: %ld ", resource->num_consumers, resource->num_producers);
        pthread_cond_wait(&resource->cond, &resource->mutex);
        // pthread_mutex_lock(&resource->mutex);
    }
    resource->num_producers -= 1;
    pthread_mutex_unlock(&resource->mutex);
}
