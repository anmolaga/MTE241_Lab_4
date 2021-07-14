#include <stdio.h>
#include <cmsis_os2.h>
#include <stdbool.h>
#include "random.h"

// Creating data tracking arrays for Monitors
int sent1 = 0;
int sent2 = 0;

int received1 = 0;
int received2 = 0;

int overflow1 = 0;
int overflow2 = 0;


// Initializing the Queues
osMessageQueueId_t q_1;
osMessageQueueId_t q_2;

int server1 = 1;
int server2 = 2;

//The actual message to send

int message = 420;

//Client function
void client(void *arg) {
	
	int serverDelay = 0;
	int ticksToSeconds = osKernelGetTickFreq();
	
	
	//Infinite Loop
	while(true){
		
		//DELAY
		// Use the next_event function from the random file to get delay in seconds
		serverDelay = next_event();
		//Convert from seconds to ticks by multiplying by ticks/second
		serverDelay = serverDelay*ticksToSeconds;
		// Divide by our average rate for a server, which is 10Hz
		serverDelay = serverDelay/9;
		//Right shift the delay by 16 bits
		serverDelay = serverDelay >> 16;
		
		//for queue 1
		osDelay(serverDelay);
		
		osStatus_t status1;
		status1 = osMessageQueuePut(q_1, &message, 0, 0);
		if (status1 == osOK)
		{
			sent1++;
		}
		else if (status1 == osErrorResource)
		{
			overflow1++;
		}
		
		//for queue 2
		osDelay(serverDelay);
		
		osStatus_t status2;
		status2 = osMessageQueuePut(q_2, &message, 0, 0);
		if (status2 == osOK)
		{
			sent2++;
		}
		else if (status2 == osErrorResource)
		{
			overflow2++;
		}
	}
	
}

void servers(void *argument) {
	int *serverPtr = (int *) argument;
	int server = *serverPtr;
	int serverDelay = 0;
	int ticksToSeconds = osKernelGetTickFreq();
	while(true) {
		// Use the next_event function from the random file to get delay in seconds
		serverDelay = next_event();
		//Convert from seconds to ticks by multiplying by ticks/second
		serverDelay = serverDelay*ticksToSeconds;
		// Divide by our average rate for a server, which is 10Hz
		serverDelay = serverDelay/10;
		//Right shift the delay by 16 bits
		serverDelay = serverDelay >> 16;
		
		//Run the delay
		osDelay(serverDelay);
		
		//Now we retreive messages from the queue
		if(server == 1) {
			osMessageQueueGet(q_1, &message, NULL, osWaitForever);
			received1++;
		}else if(server == 2) {
			osMessageQueueGet(q_2, &message, NULL, osWaitForever);
			received2++;
		}
	}
}

void serverStart() {
	// Initializing our queues
	q_1 = osMessageQueueNew(10,sizeof(int),NULL);
	q_2 = osMessageQueueNew(10,sizeof(int),NULL);
	
	//Starting up the server threads
	osThreadNew(servers, &server1,NULL);
	osThreadNew(servers, &server2,NULL);
}

void monitor() {
	
	//number of ticks in a second
	int ticks_p_s = osKernelGetTickFreq();
	//for reference, setting seconds to 1
	int sec = 1;
	
	while(true){
		//delay for one second
		osDelay(ticks_p_s);
		
		//Displaying information
		printf("Elapsed Time: %d ", sec);
		
		printf("Sent Queue 1: %d ", sent1);
		printf("Sent Queue 2: %d ", sent2);
		printf("Received Queue 1: %d ", received1);
		printf("Received Queue 2: %d ", received2);
		printf("Overflow Queue 1: %d ", overflow1);
		printf("Overflow Queue 2: %d ", overflow2);
		printf("\n");
		
		sec++;
	}
}


int main(){
	osKernelInitialize();
	
	serverStart();
	
	osThreadNew(client, NULL, NULL);
	
	osThreadNew(monitor, NULL, NULL);

	osKernelStart();
}
