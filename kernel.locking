#include <linux/priority_queue.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
struct priority_queue *create_priority_queue(void) {
    struct priority_queue *queue = kmalloc(sizeof(struct priority_queue), GFP_KERNEL);
    if (queue) {
        INIT_LIST_HEAD(&queue->head); // Initialize the list head
    }
    return queue;
}

void add_to_priority_queue(struct priority_queue *queue, struct task_struct *task, int priority) {
    struct prio_node *new_node = kmalloc(sizeof(struct prio_node), GFP_KERNEL);
    if (!new_node) return; // Handle memory allocation failure

    new_node->task = task;
    new_node->priority = priority;

    struct prio_node *cur_node = NULL; // Rename current to cur_node
    struct list_head *pos;

    // Insert in the correct position based on priority
    list_for_each(pos, &queue->head) {
        cur_node = list_entry(pos, struct prio_node, list);
        if (cur_node->priority < priority) {
            break; // Found the right spot
        }
    }

    // Insert before the found position
    list_add(&new_node->list, pos->prev);
}
struct task_struct *get_highest_priority_task(struct priority_queue *queue) {
    if (!list_empty(&queue->head)) {
        struct prio_node *highest = list_first_entry(&queue->head, struct prio_node, list);
        return highest->task; // Return the highest priority task
    }
    return NULL; // Queue is empty
}

void remove_from_priority_queue(struct priority_queue *queue, struct task_struct *task) {
    struct prio_node *cur_node = NULL; // Rename current to cur_node
    struct list_head *pos, *n;

    list_for_each_safe(pos, n, &queue->head) {
        cur_node = list_entry(pos, struct prio_node, list);
        if (cur_node->task == task) {
            list_del(pos); // Remove from the list
            kfree(cur_node); // Free the node
            return;
        }
    }
}

void free_priority_queue(struct priority_queue *queue) {
    struct prio_node *cur_node = NULL; // Rename current to cur_node
    struct list_head *pos, *n;

    list_for_each_safe(pos, n, &queue->head) {
        cur_node = list_entry(pos, struct prio_node, list);
        list_del(pos); // Remove from the list
        kfree(cur_node); // Free the node
    }

    kfree(queue); // Finally free the queue structure
}


MODULE_LICENSE("GPL");


//EXPORT_SYMBOL(create_priority_queue);

//EXPORT_SYMBOL(add_to_priority_queue);

//EXPORT_SYMBOL(remove_from_priority_queue);
//EXPORT_SYMBOL(free_priority_queue);

