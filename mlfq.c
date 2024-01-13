/*
    COMP3511 Fall 2023 
    PA2: Simplified Multi-Level Feedback Queue (MLFQ)

    Your name: Lam Yeung Kong Sunny
    Your ITSC email:     ykslam      @connect.ust.hk 

    Declaration:

    I declare that I am not involved in plagiarism
    I understand that both parties (i.e., students providing the codes and students copying the codes) will receive 0 marks. 

*/

// Note: Necessary header files are included
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define MAX_* constants
#define MAX_NUM_PROCESS 10
#define MAX_QUEUE_SIZE 10
#define MAX_PROCESS_NAME 5
#define MAX_GANTT_CHART 300

// Keywords (to be used when parsing the input)
#define KEYWORD_TQ0 "tq0"
#define KEYWORD_TQ1 "tq1"
#define KEYWORD_PROCESS_TABLE_SIZE "process_table_size"
#define KEYWORD_PROCESS_TABLE "process_table"

// Assume that we only need to support 2 types of space characters: 
// " " (space), "\t" (tab)
#define SPACE_CHARS " \t"

// Process data structure
// Helper functions:
//  process_init: initialize a process entry
//  process_table_print: Display the process table
struct Process {
    char name[MAX_PROCESS_NAME];
    int arrival_time ;
    int burst_time;
    int remain_time; // remain_time is needed in the intermediate steps of MLFQ 
};
void process_init(struct Process* p, char name[MAX_PROCESS_NAME], int arrival_time, int burst_time) {
    strcpy(p->name, name);
    p->arrival_time = arrival_time;
    p->burst_time = burst_time;
    p->remain_time = 0;
}
void process_table_print(struct Process* p, int size) {
    int i;
    printf("Process\tArrival\tBurst\n");
    for (i=0; i<size; i++) {
        printf("%s\t%d\t%d\n", p[i].name, p[i].arrival_time, p[i].burst_time);
    }
}

// A simple GanttChart structure
// Helper functions:
//   gantt_chart_append: append one item to the end of the chart (or update the last item if the new item is the same as the last item)
//   gantt_chart_print: display the current chart
struct GanttChartItem {
    char name[MAX_PROCESS_NAME];
    int duration;
};
void gantt_chart_update(struct GanttChartItem chart[MAX_GANTT_CHART], int* n, char name[MAX_PROCESS_NAME], int duration) {
    int i;
    i = *n;
    // The new item is the same as the last item
    if ( i > 0 && strcmp(chart[i-1].name, name) == 0) 
    {
        chart[i-1].duration += duration; // update duration
    } 
    else
    {
        strcpy(chart[i].name, name);
        chart[i].duration = duration;
        *n = i+1;
    }
}
void gantt_chart_print(struct GanttChartItem chart[MAX_GANTT_CHART], int n) {
    int t = 0;
    int i = 0;
    printf("Gantt Chart = ");
    printf("%d ", t);
    for (i=0; i<n; i++) {
        t = t + chart[i].duration;     
        printf("%s %d ", chart[i].name, t);
    }
    printf("\n");
}

// Global variables
int tq0 = 0, tq1 = 0;
int process_table_size = 0;
struct Process process_table[MAX_NUM_PROCESS];



// Helper function: Check whether the line is a blank line (for input parsing)
int is_blank(char *line) {
    char *ch = line;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) )
            return 0;
        ch++;
    }
    return 1;
}
// Helper function: Check whether the input line should be skipped
int is_skip(char *line) {
    if ( is_blank(line) )
        return 1;
    char *ch = line ;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) && *ch == '#')
            return 1;
        ch++;
    }
    return 0;
}
// Helper: parse_tokens function
void parse_tokens(char **argv, char *line, int *numTokens, char *delimiter) {
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}

// Helper: parse the input file
void parse_input() {
    FILE *fp = stdin;
    char *line = NULL;
    ssize_t nread;
    size_t len = 0;
    char *two_tokens[2]; // buffer for 2 tokens
    int numTokens = 0, i=0;
    char equal_plus_spaces_delimiters[5] = "";
    char process_name[MAX_PROCESS_NAME];
    int process_arrival_time = 0;
    int process_burst_time = 0;

    strcpy(equal_plus_spaces_delimiters, "=");
    strcat(equal_plus_spaces_delimiters,SPACE_CHARS);

    while ( (nread = getline(&line, &len, fp)) != -1 ) {
        if ( is_skip(line) == 0)  {
            line = strtok(line,"\n");

            if (strstr(line, KEYWORD_TQ0)) {
                // parse tq0
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &tq0);
                }
            } 
            else if (strstr(line, KEYWORD_TQ1)) {
                // parse tq0
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &tq1);
                }
            }
            else if (strstr(line, KEYWORD_PROCESS_TABLE_SIZE)) {
                // parse process_table_size
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &process_table_size);
                }
            } 
            else if (strstr(line, KEYWORD_PROCESS_TABLE)) {

                // parse process_table
                for (i=0; i<process_table_size; i++) {

                    getline(&line, &len, fp);
                    line = strtok(line,"\n");  

                    sscanf(line, "%s %d %d", process_name, &process_arrival_time, &process_burst_time);
                    process_init(&process_table[i], process_name, process_arrival_time, process_burst_time);

                }
            }

        }
        
    }
}
// Helper: Display the parsed values
void print_parsed_values() {
    printf("%s = %d\n", KEYWORD_TQ0, tq0);
    printf("%s = %d\n", KEYWORD_TQ1, tq1);
    printf("%s = \n", KEYWORD_PROCESS_TABLE);
    process_table_print(process_table, process_table_size);
}

//Helper Function: Move element queue forward one position 
void queueMoveForward(struct Process** queue, int size){ //this is the previous size before remove the first element in queue
    if(size > 1)
        for(int i = 0; i < size; i++){
            if(i != size-1)
            {
                queue[i] = queue[i+1];
            }
        }
}

void remainingTimeQueue_MoveForward(int* queue, int size){
    if(size > 1)
        for(int i = 0; i < size; i++){
            if(i == size-1)
            {
                queue[i] = queue[i+1];
            }
        }
}


int totalTimeCal(struct Process* p, int process_table_size){
    int sum = 0;
    for(int i = 0; i < process_table_size; i++){
        sum += p[i].burst_time;
    }
    return sum;
}

void processTable_RemainingTime_Init(struct Process* p, int process_table_size){
    for(int i = 0; i < process_table_size; i++){
        p[i].remain_time = p[i].burst_time;
    }
}

// TODO: Implementation of MLFQ algorithm
void mlfq() {
        // Initialize the gantt chart
    struct GanttChartItem gantt_chart[MAX_GANTT_CHART];
    int chart_size = 0;

    // TODO: implement your MLFQ algorithm here

    struct Process* queueRR0[MAX_QUEUE_SIZE];
    struct Process* queueRR1[MAX_QUEUE_SIZE];
    struct Process* queueFCFS[MAX_QUEUE_SIZE];
    int queueRR0_Size = 0;
    int queueRR1_Size = 0;
    int queueFCFS_Size = 0;


    int totalTime = totalTimeCal(process_table,process_table_size);
    int currentTime = 0;

    struct Process* currentProcess = NULL;
    int* burstTimeRemain = NULL;
    int* TqRemain = NULL;
    int currentLevel = 0;


    int arrivePosition = 0;

    
    // Tips: A simple array is good enough to implement a queue
    while(chart_size < MAX_GANTT_CHART && currentTime < totalTime){
            
        if(process_table[arrivePosition].arrival_time == currentTime){ //check arrive time
            queueRR0[queueRR0_Size++] = &process_table[arrivePosition++];
            if(currentLevel == 0){
                if(TqRemain != NULL){
                    if(*burstTimeRemain == 0){ //current process is not finished 
                        queueMoveForward(queueRR0, queueRR0_Size);
                        queueRR0_Size--;
                        currentProcess = queueRR0[0];
                        burstTimeRemain = &(currentProcess->burst_time);
                        TqRemain = &(currentProcess->remain_time);
                        *TqRemain = tq0;
                        
                    }
                    else if(*TqRemain == 0){
                        *TqRemain = tq1;
                        queueRR1[queueRR1_Size] = currentProcess;
                        queueRR1_Size++;
                        queueMoveForward(queueRR0, queueRR0_Size);
                        queueRR0_Size--;
                        currentProcess = queueRR0[0];
                        burstTimeRemain = &(currentProcess->burst_time);
                        TqRemain = &(currentProcess->remain_time);
                        *TqRemain = tq0;
                    }
                }
                else{
                    currentProcess = queueRR0[0];
                    burstTimeRemain = &(currentProcess->burst_time);
                    TqRemain = &(currentProcess->remain_time);
                    *TqRemain = tq0;
                }  
            }
            else if(currentLevel == 1){
                if(*burstTimeRemain == 0){ //current process is not finished 
                   queueMoveForward(queueRR1, queueRR1_Size);
                   queueRR1_Size--;
                }
                else if(*TqRemain == 0){
                    queueFCFS[queueFCFS_Size] = currentProcess;
                    queueFCFS_Size++;
                    queueMoveForward(queueRR1, queueRR1_Size);
                    queueRR1_Size--;
                }

                currentProcess = queueRR0[0];
                burstTimeRemain = &(currentProcess->burst_time);
                TqRemain = &(currentProcess->remain_time);
                *TqRemain = tq0;
                currentLevel = 0; //go back to level 0
            }
            else{
                if(*burstTimeRemain == 0){
                    queueMoveForward(queueFCFS, queueFCFS_Size);
                    queueFCFS_Size--;
                }

                currentProcess = queueRR0[0];
                burstTimeRemain = &(currentProcess->burst_time);
                TqRemain = &(currentProcess->remain_time);
                *TqRemain = tq0;
                currentLevel = 0; //go back to level 0
            }
        }
        
        else if(*burstTimeRemain == 0){ //if burst time of the process finish
            if(currentLevel == 0){
                queueMoveForward(queueRR0, queueRR0_Size);
                queueRR0_Size--;
            }
            else if(currentLevel == 1){
                queueMoveForward(queueRR1, queueRR1_Size);
                queueRR1_Size--;
            }
            else{
                queueMoveForward(queueFCFS, queueFCFS_Size);
                queueFCFS_Size--;
            }

            if(queueRR0_Size != 0){
                currentProcess = queueRR0[0];
                burstTimeRemain = &(currentProcess->burst_time);
                TqRemain = &(currentProcess->remain_time);
                currentLevel = 0;
            }
            else if(queueRR1_Size != 0){
                currentProcess = queueRR1[0];
                burstTimeRemain = &(currentProcess->burst_time);
                TqRemain = &(currentProcess->remain_time);
                currentLevel = 1;
            }
            else if(queueFCFS_Size != 0){ 
                currentProcess = queueFCFS[0];
                burstTimeRemain = &(currentProcess->burst_time);
                TqRemain = &(currentProcess->remain_time);
                currentLevel = 2;
            }
        }

        else if(*TqRemain == 0 && currentLevel != 2){//if the time quantum finish

            if(currentLevel == 0){
                *TqRemain = tq1;
                queueRR1[queueRR1_Size] = currentProcess;
                queueRR1_Size++;
                queueMoveForward(queueRR0, queueRR0_Size);
                queueRR0_Size--;
            }
            else{
                queueFCFS[queueFCFS_Size] = currentProcess;
                queueFCFS_Size++;
                queueMoveForward(queueRR1, queueRR1_Size);
                queueRR1_Size--;
            }

            if(queueRR0_Size != 0){ 
                currentProcess = queueRR0[0];
                burstTimeRemain = &(currentProcess->burst_time);
                TqRemain = &(currentProcess->remain_time);
                *TqRemain = tq0;
                currentLevel = 0;
            }
            else if(queueRR1_Size != 0){
                currentProcess = queueRR1[0];
                burstTimeRemain = &(currentProcess->burst_time);
                TqRemain = &(currentProcess->remain_time);
                currentLevel = 1;
            }
            else{
                currentProcess = queueFCFS[0];
                burstTimeRemain = &(currentProcess->burst_time);
                currentLevel = 2;
            }
        }
        // printf("burst time = %d, remaining time = %d , the current level = %d, current process = %s \n", 
        //  *burstTimeRemain, *TqRemain, currentLevel, currentProcess->name);
        // fflush(stdout);
        gantt_chart_update(gantt_chart, &chart_size, currentProcess->name, 1);
        if(currentLevel == 0 || currentLevel == 1){
            (*TqRemain)--;
        }
        (*burstTimeRemain)--;
        currentTime++;
    }
    // At the end, display the final Gantt chart
    gantt_chart_print(gantt_chart, chart_size);
}


int main() {
    parse_input();
    print_parsed_values();
    mlfq();
    return 0;
}