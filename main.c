#include <stdio.h>
#include <cmsis_os2.h>
#include <stdbool.h>
#include "random.h"

// Creating data tracking arrays for Monitors
int sent1;
int sent2;

int received1;
int received2;

int overflow1;
int overflow2;


// Initializing the Queues
osMessageQueueId_t q_1;
osMessageQueueId_t q_2;

//The actual message to send

int message = 420;

//Client function
void client(void *arg) {
	
	//DELAY
	
	int delay_s = 0;
	delay_s = next_event();
	
	//gets ticks per second
	int ticks_p_s = osKernelGetTickFreq();
	
	int avg_ticks = 0;
	int nticks = 0;
	avg_ticks = (delay_s*ticks_p_s)/9;
	nticks = avg_ticks >> 16;
	
	//Infinite loop
	while(true){
		
		//for queue 1
		osDelay(nticks);
		
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
		osDelay(nticks);
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

void servers(int server) {
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

void monitor() {

}


int main(){
	osKernelInitialize();
	
	osThreadNew(client, NULL, NULL);
	
	// Figure out how to send arguments 
	osThreadNew(servers, NULL, NULL);
	osThreadNew(servers, NULL, NULL);
	
	osThreadNew(monitor, NULL, NULL);
	osKernelStart();
}
