#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <linux/list.h>
#include <linux/sched.h>

struct pq_task {
    struct list_head list;     // List head for the priority queue
    struct task_struct *task;  // Pointer to the actual task
};

// Priority queue structure
struct priority_queue {
    struct list_head wait_queue;  // List of waiting tasks
};

// Function prototypes for priority queue operations
void init_priority_queue(struct priority_queue *pq);
void add_to_priority_queue(struct priority_queue *pq, struct task_struct *task, int priority);
struct task_struct *get_highest_priority_task(struct priority_queue *pq);
void remove_from_priority_queue(struct priority_queue *pq, struct task_struct *task);

#endif /* PRIORITY_QUEUE_H */
