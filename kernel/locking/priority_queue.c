
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/priority_queue.h>
#include <linux/slab.h>
#include <linux/export.h>

// Initialize the priority queue
void init_priority_queue(struct priority_queue *pq) {
    INIT_LIST_HEAD(&pq->wait_queue);
}

// Add a task to the priority queue based on its priority
void add_to_priority_queue(struct priority_queue *pq, struct task_struct *task, int priority) {
    struct pq_task *new_task = kmalloc(sizeof(struct pq_task), GFP_KERNEL);
    new_task->task = task;
    INIT_LIST_HEAD(&new_task->list);

    struct list_head *pos;
    list_for_each(pos, &pq->wait_queue) {
        struct pq_task *existing_task = list_entry(pos, struct pq_task, list);
        if (priority < existing_task->task->prio) {
            list_add(&new_task->list, pos);
            return;
        }
    }
    list_add_tail(&new_task->list, &pq->wait_queue); // If no higher priority, add at the end
}
//EXPORT_SYMBOL(add_to_priority_queue);
// Get the highest priority task from the queue
struct task_struct *get_highest_priority_task(struct priority_queue *pq) {
    struct list_head *pos;
    struct pq_task *highest_priority_task = NULL;

    list_for_each(pos, &pq->wait_queue) {
        struct pq_task *task = list_entry(pos, struct pq_task, list);
        if (!highest_priority_task || task->task->prio < highest_priority_task->task->prio) {
            highest_priority_task = task;
        }
    }

    return highest_priority_task ? highest_priority_task->task : NULL;
}
//EXPORT_SYMBOL(get_highest_priority_task);
// Remove a task from the priority queue
void remove_from_priority_queue(struct priority_queue *pq, struct task_struct *task) {
    struct list_head *pos, *tmp;

    list_for_each_safe(pos, tmp, &pq->wait_queue) {
        struct pq_task *pq_task = list_entry(pos, struct pq_task, list);
        if (pq_task->task == task) {
            list_del(&pq_task->list);
            kfree(pq_task);  // Free the memory allocated for this task
            return;
        }
    }
}
	

//EXPORT_SYMBOL(remove_from_priority_queue);

MODULE_LICENSE("GPL");
