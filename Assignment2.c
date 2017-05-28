#include "includes.h"
#include <string.h>
//#include <"key.h" 

/* CONSTANTS */    
#define  TASK_STK_SIZE  64
#define  N_TASKS        4

/* VARIABLES */


OS_STK InitTaskStk[TASK_STK_SIZE];
OS_STK TaskStk[N_TASKS][TASK_STK_SIZE];

/* FUNCTION PROTOTYPES */
void   InitTask(void *args) KCREENTRANT;
void   Task1(void *args) KCREENTRANT;
void   Task2(void *args) KCREENTRANT;
void   Task3(void *args) KCREENTRANT;

sbit P1_0  = P1^0;
sbit P1_1  = P1^1;
sbit P1_2  = P1^2;
sbit P1_3  = P1^3;

unsigned char pval1;
unsigned char pval2;
unsigned char pval3; 

typedef struct TASK_PARAM{		
	char * id;
	INT8U wait;
	OS_EVENT * Sem;
	INT8U * error; 
	INT8U  err; 
	INT8U  taskError;
}taskParam;	

taskParam t1;	
taskParam t2;
taskParam t3;

char *nLine = "\r\n";
char str1[10] = "Task1";
char str2[10] = "Task2";
char str3[10] = "Task3";
/*
 * MAIN
 */

void main (void) {  
 	OSInit();


	t1.id = "Task1";
	t2.id = "Task2";
	t3.id = "Task3";  

    t1.wait = 0;
	t2.wait = 0;
	t3.wait = 10;

	//Create my Semaphores
	t1.Sem = OSSemCreate( 1 );
	if(t1.Sem != (void *)0){
		SendString( "Semaphore 1 created" );
		SendString( nLine );
	}
	else if(t1.Sem == (void *)0){
		SendString( "Semaphore 1 NOT created, error" );
		SendString( nLine );
	}

	t2.Sem = OSSemCreate( 1 );
	if(t2.Sem != (void *)0){
		SendString( "Semaphore 2 created" );
		SendString( nLine );
	}
	else if(t2.Sem == (void *)0){
		SendString( "Semaphore 2 NOT created, error" );
		SendString( nLine );
	}

	t3.Sem = OSSemCreate( 1 ); 
	if(t3.Sem != (void *)0){
			
			SendString( "Semaphore 3 created" );
				SendString( nLine );
	}
	else if(t3.Sem == (void *)0){
			SendString( "Semaphore 3 NOT created, error" );
				SendString( nLine );
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

  /* init the system */
  initSystem();





	//Name of function,	passed void pointer to task, the second parameter is data passed to void pointer[stack], priority.
	t1.taskError = OSTaskCreate(Task2, (void *)&t1, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
	if(t1.taskError == 0){
		SendString( "Task1 Created" );
		SendString( nLine );
	}
	else if(t1.taskError > 0){
		SendString( "Error creating  Task1" );
	}

	t2.taskError = OSTaskCreate(Task2, (void *)&t2, (void *)&TaskStk[1][TASK_STK_SIZE - 1], 2);
	if(t2.taskError == 0){
		SendString( "Task2 Created" );
		SendString( nLine );
	}
	else if(t2.taskError > 0){
		SendString( "Error creating  Task2" );
	}

	t3.taskError =  OSTaskCreate(Task2, (void *)&t3, (void *)&TaskStk[2][TASK_STK_SIZE - 1], 3);
	if(t3.taskError == 0){
		SendString( "Task3 Created" );
		SendString( nLine );
	}
	else if(t3.taskError > 0){
		SendString( "Error creating  Task3" );
	}

 //OSTaskCreate(Task1, (void *)&t1, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
 //OSTaskCreate(Task2, (void *)&t2, (void *)&TaskStk[1][TASK_STK_SIZE - 1], 2);
 //OSTaskCreate(Task3, (void *)&t3, (void *)&TaskStk[2][TASK_STK_SIZE - 1], 3);
  pval3 = P1_2;
  while(1)
  {
	OSCtxSwCtr = 0;   
	if(P1_0 == 1 && pval1 == 0){
		pval1 = P1_0;
		if((t1.err=OSSemPost(t1.Sem)) == 0){
			//SendString( "Post created for Task1" );
			//SendString( nLine );
		} 
		else {
			SendString( "Error creating Post for Task1" );
			SendString( nLine );
		}
	} 
	else if(P1_0 == 0){
		pval1 = P1_0;
	}

	if(P1_1 == 0 && pval2 == 1){
			pval2 = P1_1;
	
		 	if((t2.err=OSSemPost(t2.Sem)) == 0){
				//SendString( "Post created for Task2" );
				//SendString( nLine );
			} 
			else {
				SendString( "Error creating Post for Task2" );
				SendString( nLine );
			}
	} 
	else if(P1_1 == 1){
		pval2 = P1_1;
	}
	

	if(P1_2 != pval3){
		pval3 = P1_2;
		if((t3.err=OSSemPost(t3.Sem)) == 0){
		//	SendString( "Post created for Task3" );
		//	SendString( nLine );
		} 
		else {
			SendString( "Error creating Post for Task3" );
		SendString( nLine );
		}
	}

    //OSCtxSwCtr = 0;
	P1_0 = ~P1_0;
    OSTimeDlyHMSM(0,0,1,0);
  }
}

/*
 * TASKS
 */
void Task1(void *args) KCREENTRANT
{
  args = args;

  while(1)
  {
  
 	OSSemPend (((taskParam *)args)->Sem, ((taskParam *)args)->wait, ((taskParam *)args)->error );
	if(*((taskParam *)args)->error == 10){
		SendChar('T');
		SendString( nLine );
	}else{
	  	SendString( ((taskParam *)args)->id ); 
		SendString( nLine );
	}	
    OSTimeDly(10);                          
  }
}


void Task2(void *args) KCREENTRANT
{
  args = args;

  while(1)
  {

  	OSSemPend (((taskParam *)args)->Sem, ((taskParam *)args)->wait, ((taskParam *)args)->error );
	if(*((taskParam *)args)->error == 10){
		SendChar('T');
		SendString( nLine );
	}else{
	  	SendString( ((taskParam *)args)->id ); 
		SendString( nLine );
	}

   if (strcmp( ((taskParam *)args)->id, str1) == 0) {
   		P1_1 = ~P1_1;
		//SendString( "Strcmp1 " ); 
		 OSTimeDly(10);         
  }else if(strcmp(((taskParam *)args)->id, str2) == 0){
  	     P1_2 = ~P1_2; 
		 //SendString( "Strcmp2 " );
		 OSTimeDly(10); 
  }else if(strcmp(((taskParam *)args)->id, str3) == 0){
		  //SendString( "Strcmp3 " );
		  OSTimeDly(10);
  }	 
    //P1_2 = ~P1_2; 
    //OSTimeDly(10);                              
  }
}

void Task3(void *args) KCREENTRANT
{
  args = args;

  SetupComms();

  while(1)
  { 
    P1_3 = ~P1_3;
    OSTimeDly(100);            
  }
}

