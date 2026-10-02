
/*
    Goal: To simulate CPU scheduling polices. A program to implement a simulatore with First Come first serve, Round Robin, and Shortest Job First.
    The simulator selects a task to run from the ready queue based on the scheduling algorithm. 
    Because the project intends to SIMULATE a cpu scheduler, it does not require any actual process ceeation or execution. 
    When a task is scheduled, the simulator will simply print out what task is selected to run at a time.

    Scheduling Algoritims:
    1) First Come First Serve
    2) Round Robin
    3) Shortest Job First

    Task Info:
    1. It will be read from an inpuit file: pid arrival_time burst_time  
        All of the feilds are integer type where:
            pid is a unique numeric process ID
            arrival_time is the time when the task arrives in the unit of milliseconds
            burst_time is the CPU time requested by a task 
    
    Comment-line Usage and Examples 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 20

// Structure of the task type
typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int start_time;
    int end_time;
    int waiting_time;
    int is_completed;
} Task;


// Function Prototypes 
void simulate_fcfs(Task tasks[], int num_tasks);
void simulate_sjf(Task tasks[], int num_tasks);
void simulate_round_robin(Task tasks[], int num_tasks, int time_quantum);
void print_stats_table(Task tasks[], int num_tasks);
void reset_tasks(Task source[], Task dest[], int num_tasks);


int main(int argc, char *argv[]) {


    // Makes sure the command line is correctly used 
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input_file [FCFS|RR|SJF] [time_quantum]\n", argv[0]);
        return 1;
    }


    // Sets the given command line inpuits
    char *filename = argv[1];
    char *algo = argv[2];
    int time_quantum = 0;

    // Incase it is a Round Robin!
    if (strcmp(algo, "RR") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Round Robin algorithm requires a time_quantum parameter.\n");
            return 1;
        }
        time_quantum = atoi(argv[3]);
    }

    // Opens the file
    FILE *file = fopen(filename, "r");

    // If file fails to load
    if (!file) {
        perror("Error opening file!");
        return 1;
    }

    // If it fails to read the number of process
    int num_tasks;
    if (fscanf(file, "%d", &num_tasks) != 1) {
        fprintf(stderr, "There is something wrong with the number of processes\n");
        fclose(file);
        return 1;
    }

    // Removes the input line extra info
    char header_buf[100];
    fgets(header_buf, sizeof(header_buf), file); 
    fgets(header_buf, sizeof(header_buf), file); 


    Task tasks[MAX_TASKS];

    // Goes through the number of tasks
    // Gives an error incase there is a failed parsing of the task
    for (int i = 0; i < num_tasks; i++) {
        if (fscanf(file, "%d %d %d", &tasks[i].pid, &tasks[i].arrival_time, &tasks[i].burst_time) != 3) {
            fprintf(stderr, "Error parsing task data at row %d.\n", i + 1);
            fclose(file);
            return 1;
        }
        
        // Creates the tracking values
        tasks[i].remaining_time = tasks[i].burst_time;
        tasks[i].is_completed = 0;
        tasks[i].start_time = -1;
        tasks[i].end_time = -1;
        tasks[i].waiting_time = 0;
    }

    // Closes the file
    fclose(file);

    // Checks the command line
    // Depending on the line, a different function is outputted
    // But gives a warning if not one of them
    if (strcmp(algo, "FCFS") == 0) {
        simulate_fcfs(tasks, num_tasks);
    } else if (strcmp(algo, "SJF") == 0) {
        simulate_sjf(tasks, num_tasks);
    } else if (strcmp(algo, "RR") == 0) {
        simulate_round_robin(tasks, num_tasks, time_quantum);
    } else {
        fprintf(stderr, "Error: Unknown algorithm '%s'. Choose FCFS, RR, or SJF.\n", algo);
        return 1;
    }

    return 0;

}


// Prints the final table
void print_stats_table(Task tasks[], int num_tasks) {

    // The labels for the table 
    printf("PID \t Arrival Time \t Start Time \t End Time \t Running Time \t Waiting Time\n");
    double total_waiting_time = 0;

    // Goes through the number of tasks and prints out the info
    for (int i = 0; i < num_tasks; i++) {
        printf("%d \t %d \t\t %d \t\t %d \t\t %d \t\t %d\n", 
               tasks[i].pid, tasks[i].arrival_time, tasks[i].start_time, 
               tasks[i].end_time, tasks[i].burst_time, tasks[i].waiting_time);
        total_waiting_time += tasks[i].waiting_time;
    }

    // Prints out averafe waiting time
    printf("Average Waiting Time: %.2f\n", total_waiting_time / num_tasks);
}


// Simulates First come first serve by taking the tasks from the inputed file, and how many tasks there are
void simulate_fcfs(Task tasks[], int num_tasks) {
    int current_time = 0;
    int completed_count = 0;

    printf("FCFS:\n");

    // Loops through the tasks, doing all of them
    while (completed_count < num_tasks) {

        int choice_idx = -1;
        int earliest_arrival = 1e9; // could use int_max but this uses one less library

        // Finds the earliest uncompleteted task by going through the num_tasks
        for (int i = 0; i < num_tasks; i++) {
            // if the task isnt completed, and the arrival time is less than or equal to the current time
            if (!tasks[i].is_completed && tasks[i].arrival_time <= current_time) {
                // look if the arrival time is less than the earliest arrival
                // and make it the arrival time, and the choice_idx that task
                if (tasks[i].arrival_time < earliest_arrival) {
                    earliest_arrival = tasks[i].arrival_time;
                    choice_idx = i;
                }
            }
        }

        // If there's no uncompleted task, idle the CPU
        if (choice_idx == -1) {
            current_time++; 
            continue;
        }

        // Uses arrow operators to access, and change the start_time, end_time, and waiting_time
        // arrow operator is the choice_idx
        Task *t = &tasks[choice_idx];
        t->start_time = current_time; // makes the start time the current time for next process
        t->end_time = current_time + t->burst_time; // the end time will be the current time AND burst time
        t->waiting_time = t->start_time - t->arrival_time; // the wait time is the start time minus the arrival time
        t->is_completed = 1;
        
        // This task is now completed!
        current_time = t->end_time;
        completed_count++;
    }

    print_stats_table(tasks, num_tasks);
}

// Simulates SJF by taking the tasks, and how many tasks are there
void simulate_sjf(Task tasks[], int num_tasks) {
    int current_time = 0;
    int completed_count = 0;
    Task ordered_output[MAX_TASKS]; 
    int output_idx = 0;

    printf("SJF:\n");

    // Goes through the tasks
    while (completed_count < num_tasks) {
        int choice_idx = -1;
        int shortest_burst = 1e9;

        // Picks the task with the shortest burst time among the current ready processes
        // by going through the tasks, and changing if it is shorter
        for (int i = 0; i < num_tasks; i++) {
            if (!tasks[i].is_completed && tasks[i].arrival_time <= current_time) {
                if (tasks[i].burst_time < shortest_burst) {
                    shortest_burst = tasks[i].burst_time;
                    choice_idx = i;
                }
            }
        }

        // If there's no uncompleted task, idle the CPU
        if (choice_idx == -1) {
            current_time++; // CPU Idle slot
            continue;
        }

        // Uses arrow operators to access, and change the start_time, end_time, and waiting_time
        // arrow operator is the choice_idx
        Task *t = &tasks[choice_idx];
        t->start_time = current_time; // makes the start time the current time for next process
        t->end_time = current_time + t->burst_time; // the end time will be the current time AND burst time
        t->waiting_time = t->start_time - t->arrival_time; // the wait time is the start time minus the arrival time
        t->is_completed = 1;
        
        // This task is now completed!
        current_time = t->end_time;
        current_time = t->end_time;
        ordered_output[output_idx++] = *t;
        completed_count++;
    }

    print_stats_table(ordered_output, num_tasks);
}

// simulates round robin with obtaining the task list, the number of tasks, and the time quantum
void simulate_round_robin(Task tasks[], int num_tasks, int time_quantum) {
    int current_time = 0;
    int completed_count = 0;

    // prepares the queue ints
    int ready_queue[MAX_TASKS * 10]; 
    int head = 0, tail = 0;
    int in_queue[MAX_TASKS] = {0};

    // Prints out the table header needed for RR
    printf("RR (Time quantum = %d):\n", time_quantum);
    printf("PID \t Start Time \t End Time \t Running Time\n");

    // Goes through the number tasks 
    // if the arrival time is less than or equal to the current time, push it into the queue
    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].arrival_time <= current_time) {
            ready_queue[tail++] = i;
            in_queue[i] = 1;
        }
    }

    // goes through the number of tasks
    while (completed_count < num_tasks) {
        // if the head is the tail
        if (head == tail) {
            // add to the current time
            current_time++;
            // then go through the number tasks
            for (int i = 0; i < num_tasks; i++) {
                // and if the arival time is less, or equal to the current time, and it isnt completed or in queue
                // add it to queue
                if (tasks[i].arrival_time <= current_time && !in_queue[i] && !tasks[i].is_completed) {
                    ready_queue[tail++] = i;
                    in_queue[i] = 1;
                }
            }
            continue;
        }

        // dequeues 
        int idx = ready_queue[head++];
        Task *t = &tasks[idx];

        // gets how many milliseconds the process will take to execute
        int execution_slice = (t->remaining_time > time_quantum) ? time_quantum : t->remaining_time;
        
        // gets the capture time and execution trace
        int slice_start = current_time;
        int slice_end = current_time + execution_slice;

        printf("%d \t %d \t\t %d \t\t %d\n", t->pid, slice_start, slice_end, execution_slice);

        // goes through the clock
        for (int step = 0; step < execution_slice; step++) {
            current_time++;
            // checks for any interleaved arrivals by checking if current time is the arrival time while not being in queue or completed
            for (int i = 0; i < num_tasks; i++) {
                if (tasks[i].arrival_time == current_time && !in_queue[i] && !tasks[i].is_completed) {
                    ready_queue[tail++] = i;
                    in_queue[i] = 1;
                }
            }
        }

        // Uses arrow operators to access, and change the start_time, end_time, and waiting_time
        // arrow operator is the choice_idx
        t->remaining_time -= execution_slice;

        // If the remaining time isnt zero then re-enqueue 
        if (t->remaining_time == 0) {
            t->end_time = current_time;
            t->is_completed = 1;
            t->waiting_time = t->end_time - t->arrival_time - t->burst_time;
            completed_count++;
        } else {
            ready_queue[tail++] = idx;
        }
    }

    // prints the final round robin here
    printf("\nPID \t Arrival Time \t Running Time \t End Time \t Waiting Time\n");
    double total_waiting_time = 0;
    for (int i = 0; i < num_tasks; i++) {
        printf("%d \t %d \t\t %d \t\t %d \t\t %d\n", 
               tasks[i].pid, tasks[i].arrival_time, tasks[i].burst_time, tasks[i].end_time, tasks[i].waiting_time);
        total_waiting_time += tasks[i].waiting_time;
    }
    printf("Average Waiting Time: %.2f\n", total_waiting_time / num_tasks);
}