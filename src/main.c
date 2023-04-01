#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

int get_cores()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

void *spinner(void *message)
{
    const char *animation[] = {"-", "\\", "|", "/"};
    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            printf("\r%s [%s]", message, animation[i]);
            fflush(stdout);
            usleep(100000);
        }
    }
}

int singleCoreTest()
{
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    int x = 0;
    for (int i = 0; i < 1000000000; i++)
    {
        x += i;
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    return (int)(cpu_time_used * 1000); // Convert to milliseconds
}

void *dualCoreTest(void *thread_id)
{
    int id = *((int *)thread_id);
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    int x = 0;
    for (int i = id * 500000000; i < (id + 1) * 500000000; i++)
    {
        x += i;
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    pthread_exit(NULL);
}

int main()
{
    /* Get the number of threads available and leave two cores for system and the spinner */
    int num_threads = get_cores() - 2;
    pthread_t spinner_thread;
    pthread_create(&spinner_thread, NULL, spinner, (void *)"Benchmarking single-core performance ");
    int singleCoreTime = singleCoreTest();
    pthread_cancel(spinner_thread);
    printf("\x1b[2K");
    printf("\rSingle-core test time: %d milliseconds\n", singleCoreTime);

    pthread_t threads[num_threads];
    int thread_args[num_threads];
    int dualCoreTime;
    clock_t start, end;
    double cpu_time_used;

    printf("Starting dual-core test with %d threads\n", num_threads);
    start = clock();
    pthread_create(&threads[0], NULL, spinner, (void *)"Benchmarking dual-core performance ");
    for (int i = 0; i < num_threads; i++)
    {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, dualCoreTest, (void *)&thread_args[i]);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    end = clock();
    pthread_cancel(spinner_thread);
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\x1b[2K");
    printf("\x1b[1A");
    printf("\x1b[2K");
    printf("\rDual-core test time: %f seconds\n", cpu_time_used);
    return 0;
}