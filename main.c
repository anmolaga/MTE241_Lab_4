#include <stdio.h>
#include <cmsis_os2.h>
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


void client() {

}

void servers() {

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
