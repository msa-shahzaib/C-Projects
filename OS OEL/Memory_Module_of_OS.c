#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_MEMORY_MB 64
#define FRAME_SIZE_MB 1
#define TOTAL_FRAMES (TOTAL_MEMORY_MB / FRAME_SIZE_MB)
#define MAX_PAGES_PER_PROCESS 5
#define MAX_PROCESSES 100
#define MAX_LINE_LENGTH 100

typedef struct {
    int processId;
    int arrivalTime;
    int sizeMB;
    int pagesNeeded;
    int pagesAllocated;
    int pendingPages; // Track pending pages for oversized processes
} Process;

typedef struct {
    int occupied;
    int processId;
    int arrivalTime;
} Frame;

// Global memory (array of frames)
Frame memory[TOTAL_FRAMES];

// Queue to maintain FIFO order for frames
int frameQueue[TOTAL_FRAMES];
int front = 0, rear = 0;

void initializeMemory() {
    for (int i = 0; i < TOTAL_FRAMES; i++) {
        memory[i].occupied = 0;
        memory[i].processId = -1;
        memory[i].arrivalTime = -1;
    }
}

void enqueue(int frameNumber) {
    frameQueue[rear] = frameNumber;
    rear = (rear + 1) % TOTAL_FRAMES;
}

int dequeue() {
    int frameNumber = frameQueue[front];
    front = (front + 1) % TOTAL_FRAMES;
    return frameNumber;
}

int isMemoryFull() {
    for (int i = 0; i < TOTAL_FRAMES; i++) {
        if (!memory[i].occupied)
            return 0;
    }
    return 1;
}

void printMemoryUsage() {
    int occupiedFrames = 0;
    for (int i = 0; i < TOTAL_FRAMES; i++) {
        if (memory[i].occupied)
            occupiedFrames++;
    }

    float usagePercent = ((float)occupiedFrames / TOTAL_FRAMES) * 100;
    printf("Memory Usage: %d/%d frames occupied (%.2f%%)\n", occupiedFrames, TOTAL_FRAMES, usagePercent);
    printf("------------------------------------------------------------\n");
}

// Comparison function for sorting processes by arrival time
int compareProcesses(const void *a, const void *b) {
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;
    return p1->arrivalTime - p2->arrivalTime;
}

void allocateProcess(Process *p) {
    int pagesToAllocate = p->pendingPages > 0 ? p->pendingPages : 
                          (p->pagesNeeded > MAX_PAGES_PER_PROCESS ? MAX_PAGES_PER_PROCESS : p->pagesNeeded);
    
    printf("\n=== Allocating memory for Process %d (Size: %d MB, Arrival Time: %d) ===\n",
           p->processId, p->sizeMB, p->arrivalTime);

    if (p->pendingPages > 0) {
        printf("Continuing allocation for oversized process (Remaining pages: %d)\n", p->pendingPages);
    }

    // Print header for this process
    printf("| %-10s | %-12s | %-20s | %-10s |\n", "Frame ID", "Process ID", "Arrival Time", "Process Size");

    int framesAllocated = 0;
    while (pagesToAllocate > 0) {
        int allocated = 0;
        for (int i = 0; i < TOTAL_FRAMES; i++) {
            if (!memory[i].occupied) {
                memory[i].occupied = 1;
                memory[i].processId = p->processId;
                memory[i].arrivalTime = p->arrivalTime;
                enqueue(i);
                printf("| %-10d | %-12d | %-20d | %-10d |\n", i, p->processId, p->arrivalTime, p->sizeMB);
                pagesToAllocate--;
                p->pagesAllocated++;
                framesAllocated++;
                allocated = 1;
                if (pagesToAllocate == 0)
                    break;
            }
        }

        // If memory is full and still pages are left
        if (!allocated && pagesToAllocate > 0) {
            int frameToFree = dequeue();
            printf("Memory full! Removing Process %d from Frame %d\n",
                   memory[frameToFree].processId, frameToFree);
            
            // Find which process this frame belongs to and decrement its allocated pages
            for (int i = 0; i < MAX_PROCESSES; i++) {
                if (memory[frameToFree].processId == i + 1) {
                    // This would need the processes array to be accessible here
                    // For now, we just acknowledge the deallocation
                    break;
                }
            }
            
            memory[frameToFree].occupied = 0;
            memory[frameToFree].processId = -1;
            memory[frameToFree].arrivalTime = -1;
        }
    }

    // Update pending pages for oversized processes
    if (p->pagesNeeded > MAX_PAGES_PER_PROCESS && p->pendingPages == 0) {
        p->pendingPages = p->pagesNeeded - MAX_PAGES_PER_PROCESS;
    } else if (p->pendingPages > 0) {
        p->pendingPages -= framesAllocated;
    }

    // Only print memory usage after allocation
    if (framesAllocated > 0) {
        printMemoryUsage();
    }
}

void loadProcessesFromFile(const char *filename, Process processes[], int *processCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    *processCount = 0;

    while (fgets(line, sizeof(line), file)) {
        int arrival, sizeMB;
        if (sscanf(line, "%d %d", &arrival, &sizeMB) != 2) {
            printf("Error: Invalid format in file.\n");
            fclose(file);
            exit(1);
        }

        processes[*processCount].arrivalTime = arrival;
        processes[*processCount].sizeMB = sizeMB;
        processes[*processCount].processId = *processCount + 1;
        processes[*processCount].pagesNeeded = (sizeMB + FRAME_SIZE_MB - 1) / FRAME_SIZE_MB;
        processes[*processCount].pagesAllocated = 0;
        processes[*processCount].pendingPages = 0;
        
        (*processCount)++;

        if (*processCount >= MAX_PROCESSES) {
            printf("Warning: Maximum process limit reached. Some processes ignored.\n");
            break;
        }
    }

    fclose(file);
}

int main() {
    Process processes[MAX_PROCESSES];
    int processCount = 0;

    initializeMemory();
    loadProcessesFromFile("processes_file.txt", processes, &processCount);

    // Sort processes by arrival time
    qsort(processes, processCount, sizeof(Process), compareProcesses);

    printf("============================================================\n");
    printf("                    MEMORY MODULE OF OS\n");
    printf("============================================================\n");
    printf("Total Main Memory: %d MB\n", TOTAL_MEMORY_MB);
    printf("Memory Management Technique Used: Paging\n");
    printf("Frame Size: %d MB\n", FRAME_SIZE_MB);
    printf("Maximum Pages Per Process: %d\n", MAX_PAGES_PER_PROCESS);
    printf("Scheduling Algorithm: First Come First Served (FCFS)\n");
    printf("------------------------------------------------------------\n");

    // Allocate memory to each process in arrival time order
    for (int i = 0; i < processCount; i++) {
        allocateProcess(&processes[i]);
    }
    
    // Handle remaining pages for oversized processes
    int pendingProcesses = 0;
    do {
        pendingProcesses = 0;
        for (int i = 0; i < processCount; i++) {
            if (processes[i].pendingPages > 0) {
                allocateProcess(&processes[i]);
                pendingProcesses++;
            }
        }
    } while (pendingProcesses > 0);

    // Calculate and print Process Size Summary
    int totalProcesses = processCount;
    int maxSize = processes[0].sizeMB;
    int minSize = processes[0].sizeMB;
    int totalSize = 0;

    for (int i = 0; i < processCount; i++) {
        if (processes[i].sizeMB > maxSize)
            maxSize = processes[i].sizeMB;
        if (processes[i].sizeMB < minSize)
            minSize = processes[i].sizeMB;
        totalSize += processes[i].sizeMB;
    }
    float averageSize = (float)totalSize / totalProcesses;

    printf("\n------------------------------------------------------------\n");
    printf("                    Process Size Summary:\n");
    printf("------------------------------------------------------------\n");
    printf("Total Processes: %d\n", totalProcesses);
    printf("Max Process Size: %d MB\n", maxSize);
    printf("Min Process Size: %d MB\n", minSize);
    printf("Average Process Size: %.2f MB\n", averageSize);
    printf("------------------------------------------------------------\n");

    return 0;
}