/* getstats.c 
 *
 * CSC 360, Summer 2022
 *
 * - If run without an argument, dumps information about the PC to STDOUT.
 *
 * - If run with a process number created by the current user, 
 *   dumps information about that process to STDOUT.
 *
 * Please change the following before submission:
 *
 * Author: Salem Ait Ami
 * Login:  salemaitami@uvic.ca 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Note: You are permitted, and even encouraged, to add other
 * support functions in order to reduce duplication of code, or
 * to increase the clarity of your solution, or both.
 */


void print_process_info(char * process_num) {
    char stat[50] = "/proc/";
    strcat(stat, process_num);
    char proc[50]; char cmd[50];
    strncpy(proc, stat, sizeof(stat));
    strncpy(cmd, proc, sizeof(proc));
    strcat(cmd, "/cmdline");
    strcat(stat, "/status");
    
    FILE *status = fopen(stat, "r");
    FILE *cmdline = fopen(cmd, "r");
    
    char line[300];
    
    if(status == NULL){
        printf("process number %s not found.\n", process_num);
        exit(1);
    }
    
    //Print process number and name
    fgets(line, sizeof(line), status);
    printf("Process number: %s\n", process_num);
    printf("%s", line);
    
    //If command exists, get cmdline and, if not, ignore
    if(cmdline != NULL){
        int c;
        printf("Command Line: ");
        while(1){
            c = fgetc(cmdline);
            if(feof(cmdline)){
                printf("\n");
                break;
            }
            printf("%c", c);
        }
    }
    char threads[50] = "Threads: ";
    int v_ctxt_sw = 0;
    int nonv_ctxt_sw = 0;
    
    char attribute[50];
    char temp[50];
    while(fgets(line, sizeof(line), status)){
        sscanf(line, "%s %s\n", attribute, temp);
        
        if(strcmp(attribute, "Threads:") == 0){
            strcat(threads, temp); 
        }
        else if(strcmp(attribute, "voluntary_ctxt_switches:") == 0){
            sscanf(temp, "%d", &v_ctxt_sw);
        }
        else if(strcmp(attribute, "nonvoluntary_ctxt_switches:") == 0){
            sscanf(temp, "%d", &nonv_ctxt_sw);
        }
    }
    
    printf("%s\n", threads);
    printf("Total Context Switches: %d\n", v_ctxt_sw+nonv_ctxt_sw);
    
    
    fclose(status);
    fclose(cmdline);
} 


void print_full_info() {
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r"); 
    FILE *version = fopen("/proc/version", "r"); //File pointers
    FILE *meminfo = fopen("/proc/meminfo", "r");
    FILE *uptime = fopen("/proc/uptime", "r");
    //buffer
    char line[300];
    //# of lines until core 1 information (i.e. general info repeats for each core)
    int lcount = 26;
    
    while(lcount != 0){
        fgets(line, sizeof(line), cpuinfo);
        if(lcount == 22 || lcount == 14){
        printf("%s", line);
        }
        lcount--;
    }
    //wipe buffer for reuse
    memset(line, 0, sizeof(line));
    //Info in version is all contained on one line (i.e. no loop required)
    fgets(line, sizeof(line), version);
    printf("%s", line);
    memset(line, 0, sizeof(line));
    
    //More than one line in file, but only require first line information
    fgets(line, sizeof(line), meminfo);
    printf("%s", line);
    
    float up_totalsec = 0;
    //First value in uptime specifies how long the system has been up in seconds (i.e. The second value is the sum of how much time each core has spent idle)
    fscanf(uptime, "%f ", &up_totalsec);
    int days = up_totalsec/86400; up_totalsec -= days*86400;
    int hours = up_totalsec/3600; up_totalsec -= hours*3600;
    int minutes = up_totalsec/60; up_totalsec -= minutes*60;
    int seconds = (int) up_totalsec;
    printf("Uptime: %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
    
    
    fclose(cpuinfo);
    fclose(version);
    fclose(meminfo);
    fclose(uptime);
}


int main(int argc, char ** argv) {  
    if (argc == 1) {
        print_full_info();
    } else {
        print_process_info(argv[1]);
    }
}
