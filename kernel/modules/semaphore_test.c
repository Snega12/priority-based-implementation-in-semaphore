#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/random.h>  // for random number generation
#include <linux/mutex.h>
#include <linux/slab.h>

#define NUM_THREADS 5

struct thread_data {
    int id;
    int priority;
    pid_t pid;
    struct task_struct *task;
};

static struct semaphore sema;
static struct mutex lock;
static struct thread_data *threads[NUM_THREADS];

// Function to find the thread with the highest priority
static struct thread_data *get_highest_priority_thread(void)
{
    struct thread_data *highest = NULL;
    int i;

    mutex_lock(&lock);  // Lock access to the thread data
    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i] && (!highest || threads[i]->priority > highest->priority)) {
            highest = threads[i];
        }
    }
    mutex_unlock(&lock);
    return highest;
}

static int thread_function(void *data)
{
    struct thread_data *tdata = (struct thread_data *)data;
    tdata->pid = current->pid;  // Assign current process/thread ID

    printk(KERN_INFO "Thread %d (priority %d, pid %d) waiting for semaphore\n",
           tdata->id, tdata->priority, tdata->pid);

    while (true) {
        // Check if this thread has the highest priority to acquire the semaphore
        if (get_highest_priority_thread() == tdata) {
            if (down_interruptible(&sema)) {
                return -ERESTARTSYS;
            }

            printk(KERN_INFO "Thread %d (priority %d, pid %d) acquired the semaphore\n",
                   tdata->id, tdata->priority, tdata->pid);
            ssleep(1);  // Simulate some work
            printk(KERN_INFO "Thread %d (priority %d, pid %d) released the semaphore\n",
                   tdata->id, tdata->priority, tdata->pid);

            up(&sema);
            break;
        } else {
            msleep(50);  // Yield and allow other threads to run
        }
    }
   
    return 0;
}

static int __init semaphore_test_init(void)
{
    int i;
    unsigned int rand_priority;

    // Initialize the semaphore to allow only 1 thread at a time
    sema_init(&sema, 1);
    mutex_init(&lock);  // Initialize the mutex

    // Create and assign threads with random priority
    for (i = 0; i < NUM_THREADS; i++) {
        threads[i] = kmalloc(sizeof(struct thread_data), GFP_KERNEL);
        threads[i]->id = i;

        // Assign random priority between 1 and 100
        get_random_bytes(&rand_priority, sizeof(rand_priority));
        threads[i]->priority = (rand_priority % 100) + 1;  // Ensure priority is between 1 and 100

        // Log the priority assigned to the thread
        printk(KERN_INFO "Thread %d has priority %d\n", threads[i]->id, threads[i]->priority);

        // Create the threads
        threads[i]->task = kthread_run(thread_function, threads[i], "thread-%d", threads[i]->id);
        if (IS_ERR(threads[i]->task)) {
            printk(KERN_ERR "Failed to create thread %d\n", threads[i]->id);
            return PTR_ERR(threads[i]->task);
        }
    }

    return 0;
}

static void __exit semaphore_test_exit(void)
{
    int i;

    // Clean up and free allocated memory
    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i]) {
            kthread_stop(threads[i]->task);
            kfree(threads[i]);
        }
    }

    printk(KERN_INFO "Semaphore test module exiting\n");
}

module_init(semaphore_test_init);
module_exit(semaphore_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Semaphore test module with dynamic priority-based access without sorting");

	

