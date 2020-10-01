/* lab7.c
 * Logan Lewis
 * lslewis
 * ECE 2220, Fall 2016
 * MP7
 *
 * Purpose: This lab simulates a space navigation mission system that incorporates a "mission control" and three 
 *	    space crafts that are on a mission to Mars. The program utilizes the concept of forking 
 *	    to generate child processes that are copies of the code. Also, signals are used to communicate
 *          between the multiple terminal windows, and processes such as kill(), signal(), and alarm() are also used.
 *
 * Assumptions: There are no errors in the input stream given by the user for the terminal numbers.
 *		Also, the lowest numbered terminal is the first listed in the command line and 
 *		is used to represent the mission control.
 *
 * Input: (terminal associated with the Mission Control)
 *	ln - Instructs space craft n to launch its Rover. n is 1, 2, or 3.
 *	kn - Instructs space craft n to cancel its mission (the process is killed) n is 1, 2, 3.
 *	tn - Transmit 10 new way points to space craft n.
 *	q -  Terminates all child processes and exits the program.
 *
 *
 * Important example programs:
 *
 *	Openterms.c   
 *	Wait3.c
 *	Kill.c
 *	Signal2.c
 *	Signal3.c
 *	Alarm1.c
 *	Alarm2.c
 *
 * Notes: 
 *       
 *   
 * Known bugs: 
 *           
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUMTTYS 4

/* Global Variables */

	int waypoint = 10; 					// essentially the "fuel" for the space craft
	int alarm_count;
	
	int distance = 0;					// distance the space craft is from the Earth
	int Mars_dist; // = (int) (10 * drand48()) %59 + 30;	// random distance between 30 and 59
	
	int rover = 1;						// status of the rover; 1 = not launched, 0 = launched
	
	int i = 0; 						// count variable for the seconds
	
	char input[2];						// used to store the mission control commands
	int child_pids[4];					// child process identification
	
	// this block of variables accounts for writing to multiple terminals, from openterms.c
	FILE *fpt[NUMTTYS];
    	char strDev[100];
    	int Terminal[NUMTTYS];
    	int term_cnt = 0;
	int ttyindex = 0;
	
	int sc_num;		// space craft number
	int mission_status;	// status of the mission, either success or fail (3 or not 3, where 3 is the number of surviving space crafts)
	
/* End of Global Variables */

int which_process(int wait_ret, int child_pids[], int processes)	// used for determining the child process in progress (from wait3.c)
{
    int i;
    for (i = 0; i < processes; i++)
        if (child_pids[i] == wait_ret)
            return i+1;
    return -1;
}

/* Signal Handlers */

        /* SIGUSR1 Handler */
	void ln(int signum) {
            // launch the rover and return to Earth when the signal is sent at the correct time
	    if (rover == 1 && distance == Mars_dist) {
	    	rover = 0;		
	    	fprintf(fpt[(sc_num)],"\nSpace craft %d launching rover and returning to Earth\n\n", sc_num);
	    	distance--;
	    }
	    // otherwise self-destruct if the signal is sent at the wrong time
	    else if (rover == 1 && distance != Mars_dist) {
	    	fprintf(fpt[(sc_num)],"\n\tSelf-Destruct\n");    
		exit(2);
		   
	    }
	    else if (rover == 0) {
	    	fprintf(fpt[(sc_num)],"\n\tSelf-Destruct\n");
	    	exit(2);
	    }
	    
	}
	
	/* SIGUSR2 Handler */
	void tn(int signum) {		
		waypoint += 10;		// increases the specified space craft's waypoints by 10 upon each call
		
	}

	/* Alarm Handler */
	void AlarmHandler(int signum)
	{
	    
	    if (i == 0) {
	    	fprintf(fpt[(sc_num)], "\nSpace Craft %d initiating mission. Distance to Mars: %d\n\n",sc_num, Mars_dist);
	    }
	    
	    i++;
	    
	    if (distance != Mars_dist && rover == 1) {		// hasn't reached Mars yet, keep going
	    	distance++;
	    }
	    
	    if (rover == 0) {
	    	distance--;	// returning to Earth as the rover has been launched
	    }
	    if (rover == 1 ) {
	    	fprintf(fpt[(sc_num)],"waypoints: %d, distance: %d, rover status: Not Launched\n", waypoint, distance);	// status message printed every second
	    }
	    
	    else if (rover == 1 && distance == Mars_dist) { 		// craft has reached Mars and stays in orbit until rover is launched or it runs out of waypoints
	    	while(distance == Mars_dist) {
	    	
	    		if (i > 1 && i % 2 == 0) {		// only decrease waypoints every two seconds
	    			waypoint--;
	    		}
	    		
	    		if (waypoint < 5) {
	    			fprintf(fpt[(sc_num -1)],"\n\tHouston, we have a problem!\n\n");
	    		}
	    		
	    		if (waypoint == 0) {
	    			exit(2);
	    		} 
	    		
		    	// distance doesn't change
		    	// but waypoints keeps decreasing
		    	
	 		fprintf(fpt[(sc_num)],"waypoints: %d, distance: %d, rover status: Not Launched\n", waypoint, distance);
	 	
	 		i++;	// keeps track of the seconds
	    		
		}
	    }
	    else { 
	    	fprintf(fpt[(sc_num)],"waypoints: %d, distance: %d, rover status: Launched\n", waypoint, distance);	// status message
	    }
	    
	    if (distance == Mars_dist) {
	    	if (alarm_count == 0) {	// only print this message once
	    		fprintf(fpt[(sc_num)],"\nSpace craft %d to Mission Control: entered orbit.\nPlease signal when to launch Rover.\n\n", sc_num); 
    		}
	    	alarm_count++;		// allows the message to only be printed once
 		alarm(1);
	    }
	    
	    if (distance == 0 && rover == 0) {			// craft made it safely back to Earth
	    	exit(1);	// return successful exit code 
	    }
	    
	    if (i > 1 && i % 2 == 0) {		// only decrease waypoints every two seconds
	    	waypoint--;
	    }
	    
	    if (waypoint < 5) {
	    	fprintf(fpt[(sc_num)],"\n\tHouston, we have a problem!\n\n");
	    }
	    
	    if (waypoint == 0) {
	    	fprintf(fpt[(sc_num)],"\n\tSpace Craft %d lost in space\n", sc_num);
	    	exit(2);		// return failure exit code
	    }
	    alarm(1);	// reset alarm for use every second
	   
	}


/* End of Signal Handlers */

void MissionInput(void) {
	
	/* Mission Control Commands */ 
   
	   
	    while(1) {
	    
		    scanf("%s",input);		// scans the mission control terminal for input
		    
		    /* l commands */
		    
			    if (strcmp(input,"l1") == 0)
			    	kill(child_pids[0],SIGUSR1);		//launch rover 1
			    	
			    else if (strcmp(input,"l2") == 0)
			       kill(child_pids[1],SIGUSR1);		//launch rover 2
			      
			    else if (strcmp(input,"l3") == 0)	//launch rover 3
			       kill(child_pids[2],SIGUSR1);
			     
		    /* k commands */
		     
			    else if (strcmp(input,"k1") == 0) {
			    	kill(child_pids[0],1);
			    	printf("\n\tTerminated Space Craft 1\n");	//destroy space craft 1 
			    }
			    
			    else if (strcmp(input,"k2") == 0) {
			    	kill(child_pids[1],1);
			    	printf("\n\tTerminated Space Craft 2\n");	//destroy space craft 2
			    }
			    
			    else if (strcmp(input,"k3") == 0) {
			    	kill(child_pids[2],1);
			    	printf("\n\tTerminated Space Craft 3\n");	//destroy space craft 3
			    }
		    
		    /* t commands */
		    
			    else if (strcmp(input,"t1") == 0)
			    	kill(child_pids[0],SIGUSR2);			//transmit 10 way points to sc 1
			       
			    else if (strcmp(input,"t2") == 0)		//transmit 10 way points to sc 2
			    	kill(child_pids[1],SIGUSR2);
			       
			    else if (strcmp(input,"t3") == 0)		//transmit 10 way points to sc 3
			    	kill(child_pids[2],SIGUSR2);
			       
		    /* quit */
			  
			    else if (strcmp(input,"q") == 0) {		//kill child processes and exit
			    	kill(child_pids[0],1);
			    	kill(child_pids[1],1);
			    	kill(child_pids[2],1);
			    	kill(child_pids[3],1); 	// mission control is child_pids[3]
			    }
			    
		    /* invalid commands */
			    else {
			    	printf("Invalid command\n");
			    }  
	    }                      
    
    /* End of Mission Control Commands */

}

/*----------------------------------------------------------*/

int main(int argc, char *argv[])
{      
    /* install the signal handlers */
    void ln(int signum);
    signal(SIGUSR1,ln);
    
    void tn(int signum);
    signal(SIGUSR2,tn);
    
    signal(SIGALRM, AlarmHandler);

    
    /* Terminal Error Check */

	    if (argc != NUMTTYS+1) {
		printf("Usage: ./lab7 4 17 18 19, where the numbers are the terminal pts values\n");
		exit(1);
	    }	
	
	    // determine which ttys are open and available to this user
	    for (term_cnt = 0; term_cnt < NUMTTYS; term_cnt++)
	    { 
		ttyindex = -1;
		ttyindex = atoi(argv[term_cnt+1]);
		if (ttyindex < 1) {
		    printf("invalid terminal number %s\n", argv[term_cnt+1]);
		    exit(1);
		}
		        
		sprintf(strDev, "/dev/pts/%d", ttyindex);

		if ((fpt[term_cnt] = fopen(strDev,"w")) != NULL) {
		    Terminal[term_cnt] = ttyindex;
		} else {
		    printf("failed to open terminal %s\n", strDev);
		    exit(1);
		}
	    }
	    
   /* End of Terminal Error Check */
   
   /* Basic print indicating Mission Control and open Terminals */
   
	   printf("[Mission Control Terminal]\n\n");
	   
	   for (ttyindex=0; ttyindex < term_cnt; ttyindex++) {
	   	
	   	if (ttyindex > 0) {
			printf("Space Craft %d = Terminal %d\n", ttyindex, Terminal[ttyindex]);	
			
		}
	   }
	   
   /* End of Basic Print */

    int wait_ret = 0;		// return value of the wait() function
    int process = 0;		// the child process currently running
    
    int distances[3];		// random distances to Mars for each space craft
    int j;
    for(j = 0; j < 3; j++) {
    	distances[j] = rand() % 30 + 30;
    }
   
    int child_status = 0;
    unsigned char exit_code;   // returned from the child process, used to determine the status of each craft, and the final mission status
    
    /* Process code, handles the child processes and parent based on pid values given by fork(), based on code from wait3.c */
    
	    child_pids[process] = fork();
	    
	    if (child_pids[process]!= 0) {
	    	process++;
	    	child_pids[process] = fork();
	    	
		    if (child_pids[process] != 0) {
			// parent process
			process++;
			child_pids[process] = fork();
		
			if (child_pids[process] != 0) {
			    // parent process
			    process++;
			    child_pids[process] = fork();
			    
			    if (child_pids[process] != 0) {	// mission control checks for return values of each space craft
				// parent process
				process++;
				
				wait_ret = wait(&child_status);
				sc_num = which_process(wait_ret, child_pids, process); // keeps track of the space craft number
			
				exit_code = child_status >> 8;
				
				if (exit_code == 2 || exit_code == 0 ) {
					printf("\nVaya Con Dios, Space craft %d\n", sc_num);
					// failed mission
					// update mission status
				}
				else if (exit_code == 1) {
					printf("\nWelcome home, Space craft %d\n", sc_num);
					mission_status++;
					// completed mission
					// update mission status
				}
				
				wait_ret = wait(&child_status);
				sc_num = which_process(wait_ret, child_pids, process);
			
				exit_code = child_status >> 8;
				
				if (exit_code == 2 || exit_code == 0 ) {
					printf("\nVaya Con Dios, Space craft %d\n", sc_num);
					// failed mission
					// update mission status
				}
				else if (exit_code == 1) {
					printf("\nWelcome home, Space craft %d\n", sc_num);
					mission_status++;
					// completed mission 
					// update mission status
				}
				
				wait_ret = wait(&child_status);
				sc_num = which_process(wait_ret, child_pids, process);
			
				exit_code = child_status >> 8;
				
				if (exit_code == 2 || exit_code == 0 ) {
					printf("\nVaya Con Dios, Space craft %d\n", sc_num);
					// failed mission
					// update mission status
				}
				else if (exit_code == 1) {
					printf("\nWelcome home, Space craft %d\n", sc_num);
					mission_status++;
					// completed mission
					// update mission status
				}
			
				if (mission_status == 3) { 			//mission success if all three crafts completed the mission
					printf("\n\tCongratulations team: Mission successful\n");
					
					// close all terminals and kill all child processes
					for (ttyindex=0; ttyindex < term_cnt; ttyindex++) { 
						fclose(fpt[ttyindex]);
						kill(child_pids[ttyindex],1);
		    			}
					kill(getpid(),1);	// finally kill the parent process, produces a Hangup
				}
			
				else  {	// mission fail same as above
					printf("\n\tMission Failed\n");
					// print message
					// close all terminals and kill all child processes
					for (ttyindex=0; ttyindex < term_cnt; ttyindex++) { 
						fclose(fpt[ttyindex]);
						kill(child_pids[ttyindex],1);
		    			}
					kill(getpid(),1);	// finally kill the parent process, produces a Hangup
				}
				
			    } // else statements handle the child processes and implement the functionality of their code
			    else {   MissionInput();   }  //4 --> mission control listener for input commands, calls the input function with an infinite while loop
			} 
		
			/* Space Craft functionality, child process cases */
			// each space craft needs to install the three signal handlers, call alarm(1) need to know the terminal and keep track of the sc_num
		
			else  {		//3 --> space craft 3
				sc_num = 3;
	    			Mars_dist = distances[0];
				signal(SIGUSR1,ln);
				signal(SIGUSR2,tn);
				signal(SIGALRM, AlarmHandler);
				
				alarm(1);
				while(1){}
			}
		
		    } 
		    else {		//2 --> space craft 2
		    	sc_num = 2;
	    		Mars_dist = distances[1];
		    	signal(SIGUSR1,ln);
			signal(SIGUSR2,tn);
			signal(SIGALRM, AlarmHandler);
			
			alarm(1);
			while(1){}
		    }
	    }
	    else {			//1 --> space craft 1		
	    	sc_num = 1;;
	    	Mars_dist = distances[2];
	    	signal(SIGUSR1,ln);
		signal(SIGUSR2,tn);
		signal(SIGALRM, AlarmHandler);
		
		alarm(1);
		while(1){}
	    }
    
} // end of main()

