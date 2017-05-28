#include "includes.h"
#include <string.h>


/* CONSTANTS */ 

#define  TASK_STK_SIZE  64
#define  N_TASKS         7


OS_STK InitTaskStk[TASK_STK_SIZE];
OS_STK TaskStk[N_TASKS][TASK_STK_SIZE];


/* FUNCTION PROTOTYPES */


void   InitTask(void *args) KCREENTRANT;
void   Task1(void *args) KCREENTRANT;
void   Task2(void *args) KCREENTRANT;
void   Task3(void *args) KCREENTRANT;
void 		returnSet(char);
int		 setStructVariable();


/* Global Variables */


char *nLine = "\r\n";
INT8U i;
INT8U numTasks = 7;
char *message = "Hello"; 
INT8U  err; 
void  *msg;
INT8U temp = 1;
char key;
char set;
INT8U keyCode;
char keyMap[16] = { '1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', 'E', '0', 'F', 'D'};
const char * array[] = { "Task7", "Task1", "Task2", "Task3", "Task4", "Task5",  "Task6"};

/* STRUCTURS */

typedef struct TASK_PARAM{		
	
	char *id;
	INT8U priority;
	INT8U wait;
	OS_EVENT *Sem;
	OS_EVENT *CommQ;
	INT8U *error; 
	INT8U err; 
	INT16U qSize;
	void *CommMsg[10];
	
}taskParam;	
taskParam tasks[7];


/*
 * MAIN
 */

void main (void) { 
	
	OSInit();
	SendString("Inintialsing Keypad \n");
	KeyInit();	
	setStructVariable();
	
	//craeate task related functions
	for(i = 1; i < numTasks; i++){
		
		//Create my Semaphores
		/*if( (tasks[i].Sem = OSSemCreate( 1 )) != (void *)0){
			SendString( "Semaphore for " );
			SendString( tasks[i].id );
			SendString( " Created" );
			SendString( nLine );
		}
		else  {
			SendString( "Semaphore for " );
			SendString( tasks[i].id );
			SendString( " Not Created" );
			SendString( nLine );
		}	*/
		
		//create ques for each task
		if(( tasks[i].CommQ = OSQCreate(&tasks[i].CommMsg[0], tasks[i].qSize )) != (OS_EVENT *)0){
			SendString( "Que for " );
			SendString( tasks[i].id );
			SendString( " Created" );
			SendString( nLine );
		}
		else  {
			SendString( "Que for " );
			SendString( tasks[i].id );
			SendString( " Not Created" );
			SendString( nLine );
		}	
	}

	OSTaskCreate(InitTask, (void *)0, (void *)&InitTaskStk[TASK_STK_SIZE - 1], 0);
	OSStart();
}

/*
 * STARTUP TASK
 */
void InitTask (void *args) KCREENTRANT
{
	args = args;
	SetupComms();
  /* init the system */
	initSystem();
	
	//crate main task all other tasks are pending on
	if(OSTaskCreate(Task1, (void *) &tasks[0], (void *)&TaskStk[0][TASK_STK_SIZE - 1], tasks[0].priority) == OS_NO_ERR){
		SendString( tasks[0].id );
		SendString( " Created" );
		SendString( nLine );
	}else{
		SendString( tasks[0].id );
		SendString( " Not Created" );
		SendString( nLine );
	}

	for(i = 1; i < numTasks ; i++){
		
		if(OSTaskCreate(Task3, (void *) &tasks[i], (void *)&TaskStk[i][TASK_STK_SIZE - 1], tasks[i].priority) == OS_NO_ERR){
			SendString( tasks[i].id );
			SendString( " Created" );
			SendString( nLine );
		}else{
			SendString( tasks[i].id );
			SendString( " Not Created" );
			SendString( nLine );
		}	
	}
	

	
	while(1)
	{
		OSCtxSwCtr = 0;
		OSTimeDlyHMSM(0,0,1,0);
	}
}


/*
 * TASKS
 */
void Task1(void *args) KCREENTRANT
{
	args = args;

	SendString( "\033[2J"); 

	while(1)
	{
		
		//SendString("Waiting for keypress \n");
		if(KeyHit()){
			
			keyCode = KeyGetKey(0);
			key = keyMap[keyCode];
			
			returnSet(key);	
		}
	 OSTimeDlyHMSM(0,0,0,50);                             
	}
}



   

void Task2(void * args) KCREENTRANT
{
	args = args;

	while(1)
	{
		
		/*OSSemPend (((taskParam *)args)->Sem, ((taskParam *)args)->wait, ((taskParam *)args)->error );
		if( *((taskParam *)args)->error == 10){
			SendChar('T');
			SendString( nLine );
		}else{
			SendString( ((taskParam *)args)->id ); 
			SendString( nLine );
		}

		if (strcmp( ((taskParam *)args)->id, str1) == 0) {
			//P1_1 = ~P1_1;
		//SendString( "Strcmp1 " ); 
			OSTimeDly(10);  
			
		}else if(strcmp(((taskParam *)args)->id, str2) == 0){
			//P1_2 = ~P1_2; 
		 //SendString( "Strcmp2 " );
			OSTimeDly(10); 
		}else if(strcmp(((taskParam *)args)->id, str3) == 0){
		  //SendString( "Strcmp3 " );
			OSTimeDly(10);
		}		else{
			SendString( "Error" );
		}
		OSTimeDly(10); */                             
	}
}

void Task3(void *args) KCREENTRANT
{
	args = args;

	
	
	SetupComms();

	while(1)
	{ 
		
		msg = OSQPend( ((taskParam *)args)->CommQ,((taskParam *)args)->wait, &err ); 
		if (err == OS_NO_ERR) {


			
			if (strcmp( ((taskParam *)args)->id, array[1]) == 0) {
		
				SendString( "\033[1;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tChar " ); 
				SendString( msg );	
			}else if(strcmp(((taskParam *)args)->id, array[2]) == 0){
				SendString( "\033[2;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tChar " ); 
				SendString( msg );	
			}else if(strcmp(((taskParam *)args)->id, array[3]) == 0){
				SendString( "\033[3;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tChar " ); 
				SendString( msg );	
			}else if(strcmp(((taskParam *)args)->id, array[4]) == 0){
				SendString( "\033[4;0H" );
				SendString( "\033[K" );				
				SendString( ((taskParam *)args)->id );
				SendString( "\tChar " ); 
				SendString( msg );	
			}else if(strcmp(((taskParam *)args)->id, array[5]) == 0){
				SendString( "\033[5;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tChar " ); 
				SendString( msg );	
			}else if(strcmp(((taskParam *)args)->id, array[6]) == 0){
				SendString( "\033[6;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tChar " ); 
				SendString( msg );	
			}	
			
		}
		else if(err == OS_TIMEOUT){
			
			if (strcmp( ((taskParam *)args)->id, array[1]) == 0) {	
				SendString( "\033[1;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tTimeout" ); 
			}else if(strcmp(((taskParam *)args)->id, array[2]) == 0){
				SendString( "\033[2;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tTimeout" ); 
			}else if(strcmp(((taskParam *)args)->id, array[3]) == 0){
				SendString( "\033[3;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tTimeout" ); 
			}else if(strcmp(((taskParam *)args)->id, array[4]) == 0){
				SendString( "\033[4;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tTimeout" ); 
			}else if(strcmp(((taskParam *)args)->id, array[5]) == 0){
				SendString( "\033[5;0H" ); 
				SendString( "\033[K" );
				SendString( ((taskParam *)args)->id );
				SendString( "\tTimeout" ); 
			}else if(strcmp(((taskParam *)args)->id, array[6]) == 0){
				SendString( "\033[6;0H" );
				SendString( "\033[K" );				
				SendString( ((taskParam *)args)->id );
				SendString( "\tTimeout" ); 
			}
			
		}	
		else { 
			SendString( "\033[8;0H " ); 
			SendString( "Error Message" );  
		}
	
		 OSTimeDlyHMSM(0,0,0,400);            
	}
}

//Set up my structure variables
int setStructVariable(){
	


	for (i = 0; i<numTasks; i++){
		
		tasks[i].id = array[i];
		tasks[i].priority = i+1;
		tasks[i].qSize = 10;
		
	}
	
	
	//tasks[0].wait = 10000000;
	tasks[1].wait = 55535;					//Max timeout.
	tasks[2].wait = 65535;
	tasks[3].wait = 15535;
	tasks[4].wait = 25535;
	tasks[5].wait = 35535;
	tasks[6].wait = 45535;
	
	return 1;
}

void returnSet( char key ){
	
	
		key = key;
	
		switch (key) { 
				case '1': 
					
						err = OSQPost(tasks[1].CommQ, &key); 
						switch (err) { 
						case OS_NO_ERR: 
								break; 
						case OS_Q_FULL: 
								SendString("Que Full");
								break; 
						} 
						break;
				case '2': 
						err = OSQPost(tasks[2].CommQ, &key); 
						switch (err) { 
						case OS_NO_ERR: 
								break; 
						case OS_Q_FULL: 
								SendString("Que Full");
								break; 
						}
						break;	
				case '3': 
						err = OSQPost(tasks[3].CommQ, &key); 
						switch (err) { 
						case OS_NO_ERR: 
								break; 
						case OS_Q_FULL: 
								SendString("Que Full");
								break; 
						}
						break;	
				case '4':
						err = OSQPost(tasks[4].CommQ, &key); 
						switch (err) { 
						case OS_NO_ERR: 
								break; 
						case OS_Q_FULL: 
								SendString("Que Full");
								break; 
						}
						break;
				case '5': 
						err = OSQPost(tasks[5].CommQ, &key); 
						switch (err) { 
						case OS_NO_ERR: 
								break; 
						case OS_Q_FULL: 
								SendString("Que Full");
								break; 
						}
						break;	
				case '6': 
						err = OSQPost(tasks[6].CommQ, &key); 
						switch (err) { 
						case OS_NO_ERR: 
								break; 
						case OS_Q_FULL: 
								SendString("Que Full");
								break; 
						}
						break;	
						
				default:					
						SendString("\033[7;0H");
						SendString("Invalid_Key");
						OSTimeDlyHMSM(0,0,0,10);
						SendString("\033[7;0H");
						SendString( "\033[K" );	
				
		} 		
}

