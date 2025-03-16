#include <stdio.h>
#include <stdlib.h>

// define the necesssary macros
#define MEM_SIZE 256    // initializes memory size to 256KB
#define FRAME_SIZE 4    // initializes frame size to 4KB
#define TOTAL_FRAMES (MEM_SIZE / FRAME_SIZE)  // dynamically initialize no. of frames

// structure for process information
typedef struct {
    int id;                 // unique identifier for process
    int arrival_time;       // arrival time of process in units
    int size;               // size of the process in KB
    int frames_allocated;   // no. of frames allocated to the process
    int allocated_frames[TOTAL_FRAMES]; // array to store allocated frame indices
} Process;


// function to dynamically read processes from a file
int read_processes(Process **processes) {
    char filename[] = "processes_file.txt";

    // open the processes file in read mode
    FILE *fptr = fopen(filename, "r");

    // check whether file opens successfully or not
    if(fptr == NULL) {
        printf("ERROR: Unable to open file: %s\n", filename);
        return -1;      // return func with error
    }

    int count = 0;      // total no. of processes
    int capacity = 10;  // process holding capacity in memory 

    // dynamically allocate memory for the processes using malloc
    *processes = malloc(capacity * sizeof(Process));

    // check whether memory allocation was successfull or not
    if(*processes == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fptr);   // close the file before returning to main
        return -1;      // return func with error
    }

    // dynamically read the file contents (process info)
    while (fscanf(fptr, "%d %d", &(*processes)[count].arrival_time, &(*processes)[count].size) != EOF)     // runs till end of file is not reached
    {
        (*processes)[count].id = count + 1;     // sets the process id
        (*processes)[count].frames_allocated = ((*processes)[count].size + FRAME_SIZE - 1) / FRAME_SIZE;    // assigns the no. of frames to the process
        count++;

        // expand allocated memory if needed
        if(count >= capacity) {
            capacity *= 2;      // doubles capacity
            *processes = realloc(*processes, capacity * sizeof(Process));   // reallocates memory for the processes using realloc
            
            // check whether memory reallocation was successfull or not
            if(*processes == NULL) {
                printf("ERROR: Memory reallocation failed!\n");
                fclose(fptr);   // close the file before returning to main
                return -1;      // return func with error  
            }
        }
    }

    fclose(fptr);
    return count;
}


// function to allocate frames
void allocate_frames_to_processes(Process *processes, int process_count, int *free_frames) {
    int allocated = 0;
    for(int i = 0; i < process_count; i++) {
        // check whether all frames are occupied or not
        if((allocated + processes[i].frames_allocated) <= TOTAL_FRAMES) {
            for(int j = 0; j < processes[i].frames_allocated; j++) {
                // increments the allocated frames array of the process by 1
                processes[i].allocated_frames[j] = allocated++;
            }
        } else {
            // sets incoming processes frame count to 0
            processes[i].frames_allocated = 0;
        }
    }
    *free_frames = TOTAL_FRAMES - allocated;
}


void display_process_stats(Process *processes, int process_count) {
    // initialize total, max and min sizes
    int total_size = 0;
    int max_size = processes[0].size;
    int min_size = processes[0].size;

    for(int i = 0; i < process_count; i++) {
        total_size += processes[i].size;
        if(processes[i].size > max_size) {  // calculates the max size
            max_size = processes[i].size;
        }
        if(processes[i].size < min_size) {  // calculates the min size
            min_size = processes[i].size;
        }
    }
    // calculate the average size of processes
    float avg_size = (float) total_size / process_count;

    // display the process stats
    printf("------------------------------------------------------------\n");
    printf("                    Process Size Summary\n");
    printf("------------------------------------------------------------\n");
    printf("Total Processes: %d\n", process_count);
    printf("Max Process Size: %d KB\n", max_size);
    printf("Min Process Size: %d KB\n", min_size);
    printf("Averge Process Size: %.2f KB\n", avg_size);
}


int main() {
    // display basic program info to the user
    printf("============================================================\n");
    printf("                    MEMORY MODULE OF OS\n");
    printf("============================================================\n");
    printf("Total Main Memory: %d KB\n", MEM_SIZE);
    printf("Memory Management Technique Used: Paging (Frame Size: %d KB)\n", FRAME_SIZE);
    printf("------------------------------------------------------------\n");
    printf("                       Memory Status\n");
    printf("------------------------------------------------------------\n");
    printf("Total frames: %d\n", TOTAL_FRAMES);

    // create an instance of struct and initialize it to null
    Process *processes = NULL;
    int process_count = read_processes(&processes);
    if(process_count == -1) {
        return 1;   // safely exit the program if error occurs
    }

    int free_frames;
    allocate_frames_to_processes(processes, process_count, &free_frames);

    printf("Occupied Frames: %d\n", (TOTAL_FRAMES - free_frames));
    printf("Free Frames: %d\n", free_frames);

    printf("------------------------------------------------------------\n");
    printf("  Process ID   Arrival Time   Size (KB)   Frames Allocated\n");  
    printf("------------------------------------------------------------\n");
    for(int i = 0; i < process_count; i++) {
        printf("      P%-2d           %-2d          %-2d        %s", processes[i].id, processes[i].arrival_time, processes[i].size, "");
    
        // prints "-" if the process is not allocated any frame
        if(processes[i].frames_allocated == 0) { printf(" -\n"); }
        else {
            for(int j = 0; j < processes[i].frames_allocated; j++) {
                printf(" F%d", processes[i].allocated_frames[j]);
            }
            printf("\n");   // skips to next line
        }
    }

    display_process_stats(processes, process_count);
    free(processes);    // free allocated memory to prevent memory leaks
    return 0;
}