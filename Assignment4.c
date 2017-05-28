#include "includes.h"
#include <stdio.h>



/* CONSTANTS */

#define  TASK_STK_SIZE  86
#define  N_TASKS        5


OS_STK InitTaskStk[TASK_STK_SIZE];
OS_STK TaskStk[N_TASKS][TASK_STK_SIZE];


/* FUNCTION PROTOTYPES */
void   InitTask(void *args);
void   Task1(void *args);
void   Task2(void *args);
void   Task3(void *args);
void   Task4(void *args); 
void   Task5(void *args); 


int 	 timerCreate(); 
void   kernelTimerMessage( void * callback_arg);
int    configureDO(INT8U arg);
int 	 changeMode(INT8U state);


/* STRUCTURS */
typedef struct TASK_PARAM
{
		char time[20];
		TS ts;
    char *id;
    INT8U priority;
    INT8U wait;
    OS_EVENT *Sem;
    INT8U err;

} taskParam;
taskParam tasks[5];	//Must be same as below
INT8U numStructs = 5;	//Must corrospond to above number of structures


/* GLOBAL VARIABLES */
char keyMap[16] = { '1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', 'E', '0', 'F', 'D'};
OS_FLAG_GRP *LedStatusC;
OS_FLAG_GRP *LedStatusD;
char key;
INT8U returned = 0;
INT8U blinkState = 0;
INT16S dioReturn = 0;
INT16S dioPrevious = 0xFF;
INT8S temp;
INT8S compare  = 0x80;
INT8U j =0;
INT8U notFirst = 0;
INT8U PASSSTATE = 0;
INT8U TIMESTATE = 0;
INT8U timeArray[6];
INT8U hr;
INT8U min;
INT8U sec;
INT8U  err;
char  timeOut= '0';
OS_FLAGS  value = 0;

void main(void)
{

    OSInit();
    setStructVariable();

		if( (tasks[0].Sem = OSSemCreate( 1 ) ) != (void *)0){
			SendString( "Semaphore NOT created, error" );
		}
		
		if( ( LedStatusC = OSFlagCreate(0x00, &err) ) == (void *)0) { //initial value to store in flag group.
        SendString( "Flag group not creted" );
    }
		
		if( ( LedStatusD = OSFlagCreate(0x00, &err) ) == (void *)0) { //initial value to store in flag group.
        SendString( "Flag group not creted" );
    }
			
	
    if(OSTaskCreate(InitTask, (void *)0, (void *)&InitTaskStk[TASK_STK_SIZE - 1], 0) != OS_NO_ERR) {
        SendString( "Init task not creted" );
    }

		
    DIOInit();
		configureDO(0);
    ClkInit();
    KeyInit();
	/*	TmrInit(); 
	  TmrCfgFnct(0, kernelTimerMessage, (void*)0 );				
		TmrSetMST(0, 0 ,5, 0); 		*/												//Specify timer number and tenths			
		
		
    OSStart();
		TmrStart(0);
}

/*
* STARTUP TASK
*/
void InitTask (void *ppdata){
	
    ppdata = ppdata;
	
    SetupComms();
    initSystem();
		

    //Create task1 
    if(OSTaskCreate(Task1, (void *) &tasks[0], (void *)&TaskStk[0][TASK_STK_SIZE - 1], tasks[0].priority) != OS_NO_ERR) {
        SendString( tasks[0].id );
        SendString( " Not Created\n" );
    }
		
    //Create task2 
    if(OSTaskCreate(Task2, (void *) &tasks[1], (void *)&TaskStk[1][TASK_STK_SIZE - 1], tasks[1].priority) != OS_NO_ERR) {
        SendString( tasks[1].id );
        SendString( " Not Created\n" );
    }

    //Create task3 
    if(OSTaskCreate(Task3, (void *) &tasks[2], (void *)&TaskStk[2][TASK_STK_SIZE - 1], tasks[2].priority) != OS_NO_ERR) {
        SendString( tasks[2].id );
        SendString( " Not Created\n" );
    }

    //Create task4
    if(OSTaskCreate(Task4, (void *) &tasks[3], (void *)&TaskStk[3][TASK_STK_SIZE - 1], tasks[3].priority) != OS_NO_ERR) {
        SendString( tasks[3].id );
        SendString( " Not Created\n" );
    }
		
		  //Create task5
    if(OSTaskCreate(Task5, (void *) &tasks[3], (void *)&TaskStk[4][TASK_STK_SIZE - 1], tasks[4].priority) != OS_NO_ERR) {
        SendString( tasks[4].id );
        SendString( " Not Created\n" );
    }

    while(1)
    {
        OSCtxSwCtr = 0;
        OSTimeDlyHMSM(0,0,2,0);
    }
}


/*------------------------------------------------------------------------------------------------
																																														TASK 4
------------------------------------------------------------------------------------------------*/
void Task5(void *ppdata){
    ppdata = ppdata;
		SetupComms();
    for (;;) {
			
			
					OSSemPend ( tasks[0].Sem, 0, &err );
					if(err == 10){
						SendChar('T');
					}					 
					SendString( "\033[1;0H");
					SendString( "\033[K" );
					SendString("Timer countdown: ");
					SendChar(timeOut);
					timeOut ++;
					TmrReset(0);
					TmrStart(0);
					 OSTimeDlyHMSM(0,0,0,500);
        } 
}
   



/*------------------------------------------------------------------------------------------------
																																														TASK 4
------------------------------------------------------------------------------------------------*/
void Task4(void *ppdata){
    ppdata = ppdata;
	  SetupComms();
    for (;;) {
 
        value = OSFlagPend( LedStatusD,						//flag group
														LED5 + LED6 + LED7 +LED8,								
                            OS_FLAG_WAIT_SET_ANY , //check ANY bit in LedStatus to be set (1)
                            100,										//timeout
                            &err);
        			switch (err) {
							case OS_NO_ERR:
											DOSet(4, 1);
											DOSet(5, 1);
											DOSet(6, 1);
											DOSet(7, 1);
							break;
							case OS_TIMEOUT:							
											DOSet(4, 0);
											DOSet(5, 0);
											DOSet(6, 0);
											DOSet(7, 0);
							break;
							case OS_ERR_EVENT_TYPE:
								SendString("OS_ERR_EVENT_TYPE flag");
							break;
							case OS_FLAG_ERR_WAIT_TYPE:
								SendString("OS_FLAG_ERR_WAIT_TYPE flag");
							break;
        }
			OSTimeDlyHMSM(0,0,0,500);
    }
   
}


/*------------------------------------------------------------------------------------------------
																																														TASK 3
------------------------------------------------------------------------------------------------*/
void Task3(void *ppdata){
    ppdata = ppdata;
		SetupComms();
    for(;;) {
 
        value = OSFlagPend( LedStatusC,						//flag group
														LED1 + LED2 + LED3 + LED4,								
                            OS_FLAG_WAIT_SET_ALL  , //check ANY bit in LedStatus to be set (1)
                            100,										//timeout
                            &err);
        			switch (err) {
							case OS_NO_ERR:
											DOSet(0, 1);
											DOSet(1, 1);
											DOSet(2, 1);
											DOSet(3, 1);
							break;
							case OS_TIMEOUT:
											DOSet(0, 0);
											DOSet(1, 0);
											DOSet(2, 0);
											DOSet(3, 0);
							break;
							case OS_ERR_EVENT_TYPE:
								SendString("OS_ERR_EVENT_TYPE flag");
							break;
							case OS_FLAG_ERR_WAIT_TYPE:
								SendString("OS_FLAG_ERR_WAIT_TYPE flag");
							break;
        }
							
		
				
        OSTimeDlyHMSM(0,0,0,500);
    }
}


/*------------------------------------------------------------------------------------------------
																																														TASK 2
------------------------------------------------------------------------------------------------*/
void Task2(void *ppdata){
		SetupComms();
    ppdata = ppdata;

    for(;;){

        dioReturn = DioChange();
        if( dioReturn   == 0x100){} //Do nothing.
        else if (notFirst++ > 0){
					  SendString( "\033[2;0H");
				    SendString( "\033[K" );
            if( (temp = dioPrevious - dioReturn) < 0){
                temp =  temp * -1;	

								if(temp == compare) {
									 if(j++ == 0){
											SendString("High ");
											err = OSFlagPost(LedStatusD, temp,  OS_FLAG_SET,	 &err);											 
									 }else{
										  SendString("Low ");								 
											err = OSFlagPost(LedStatusD, temp,  OS_FLAG_CLR,	 &err);	
											j=0;
									 }
								}else{
									SendString("High ");						
									err = OSFlagPost(LedStatusC, temp,  OS_FLAG_SET,	 &err);									 
									err = OSFlagPost(LedStatusD, temp,  OS_FLAG_SET,	 &err);	
								}									
																
            }
            else{
                SendString("Low ");
							
								err = OSFlagPost(LedStatusC, temp,  OS_FLAG_CLR,	 &err);									 
								err = OSFlagPost(LedStatusD, temp,  OS_FLAG_CLR,	 &err);									
            }
            switch(temp) {
            case 0:
                SendChar('0'); 
                break;
            case 1:
                SendChar('1');
                break;
            case 2:
                SendChar('2');
                break;
            case 4:
                SendChar('3');
                break;
            case 8:
                SendChar('4');
                break;
            case 16:
                SendChar('5');
                break;
            case 32:
                SendChar('6');
                break;
            case 64:
                SendChar('7');
                break;
            case 128:
                SendChar('8');
                break;
            }

            dioPrevious = dioReturn;
						tsConvPrint();
        }
        else {
            dioPrevious = dioReturn;
        }


        OSTimeDlyHMSM(0,0,0,250);
    }
}




/*------------------------------------------------------------------------------------------------
																																														TASK 1
------------------------------------------------------------------------------------------------*/
void Task1(void *ppdata){
	  
		INT8U i;
    ppdata = ppdata;
		SetupComms();
    i = 0;
    for(;;) {
        if(KeyHit()) {

            returned =  KeyGetKey(0);
            switch(PASSSTATE) {

            case 0:
                if(returned == 0) {
										SendString( "\033[3;0H");
										SendString( "\033[K" );
                    SendChar('*');

                    PASSSTATE = DIGIT1;
                }
                else if(returned == 3) {
										SendString( "\033[3;0H");
										SendString( "\033[K" );
									  SendString("SetTime: HHMMSS");

                    PASSSTATE = DIGIT4;
                }
                break;
            case 1:
                if(returned == 1) {									
										SendString( "\033[3;0H");
                    SendChar('*');
                    PASSSTATE = DIGIT2;
                }
                else  {
										SendString( "\033[3;0H");
                    SendChar('*');
                    PASSSTATE = PASSWORD;
                }
                break;

            case 2:

                if(returned == 2) {								
										SendString( "\033[3;0H");
                    SendChar('*');
                    PASSSTATE = DIGIT3;
                }
                else {					
										SendString( "\033[3;0H");
                    SendChar('x');
                    PASSSTATE = PASSWORD;
                }
                break;

            case 3:
                if(returned == 3) {
									if( blinkState == 0){									
										SendString( "\033[3;0H");
										SendString("ARMED");
										changeMode( DO_MODE_BLINK_SYNC );	
										blinkState++;			
									}else if ( blinkState == 1){										
										SendString( "\033[3;0H");
										SendString("UN-ARMED");
										changeMode( DO_MODE_DIRECT );
										blinkState--;
									}
                    PASSSTATE = PASSWORD;
                }
                else  {								
										SendString( "\033[2;0H");
                    SendChar('x');
                    PASSSTATE = PASSWORD;
                }
                break;
            case 4:

                if( (returned >= 0 && returned < 3) || (returned > 3 && returned < 7) || (returned > 7 && returned < 11) || (returned == 13) ) {
										key = keyMap[returned];
                    switch( TIMESTATE ) {
                    case 0:
												SendString( "\033[3;0H");
												SendString( "\033[K" );
                        if(returned < 2)  {
                            timeArray[TIMESTATE] = returned + 1;
                            SendChar(key);
                            TIMESTATE = Num2;
                        }
                        break;
                    case 1:
												SendString( "\033[3;0H");
                        timeArray[TIMESTATE] = returned + 1;
                        TIMESTATE = Num3;
                        SendChar(key);
                        break;
                    case 2:
                        if(returned < 7) {
														SendString( "\033[3;0H");
                            timeArray[TIMESTATE] = returned + 1;
                            TIMESTATE = Num4;
                            SendChar(key);
                        }
                        break;
                    case 3:
												SendString( "\033[3;0H");
                        timeArray[TIMESTATE] = returned + 1;
                        TIMESTATE = Num5;
                        SendChar(key);
                        break;
                    case 4:
                        if(returned < 7) {
														SendString( "\033[3;0H");
                            timeArray[TIMESTATE] = returned + 1;
                            TIMESTATE = Num6;
                            SendChar(key);
                        }
                        break;
                    case 5:
												SendString( "\033[3;0H");
                        timeArray[TIMESTATE] = returned + 1;
                        SendString("Time Set");
                        TIMESTATE = Num1;
                        PASSSTATE = PASSWORD;
                        setClockTIme();
                        break;

                    }
                }
                else if(returned == 3)  {
									SendString( "\033[3;0H");
									SendString( "\033[K" );
                    SendString("Cancel");
                    PASSSTATE = PASSWORD;
                }
                break;
            }

        }
        OSTimeDlyHMSM(0,0,0,250);
    }
}


//Set up my structure variables
int setStructVariable(){
	
	INT8U i;
	const char * array[] = { "Task1", "Task2", "Task3", "Task4", "Task5",  "Task6" ,  "Task7"};
	const int   wait [] = { 0, 0, 0, 0, 0, 0, 0};
	
	for (i = 0; i < numStructs; i++) {

		tasks[i].id = array[i];
		tasks[i].priority = i+1;
		tasks[i].wait = wait[i];
	}

	return 0;
}


//Function to change state of Digital Output P3, blinking = 0 and continous = 1
int configureDO(INT8U state){
	
	INT8U i;

	for( i = 0; i < 4 ; i++){
		 
		
		DOCfgMode (i, DO_MODE_DIRECT, FALSE);		
		DOCfgBlink(i, DO_BLINK_EN_NORMAL, 10, 1);

	}
	

		
	  DOCfgMode (4, DO_MODE_DIRECT, FALSE);		
		DOCfgBlink(4, DO_BLINK_EN_NORMAL, 1, 10);			
		DOCfgMode (5, DO_MODE_DIRECT, FALSE);		
		DOCfgBlink(5, DO_BLINK_EN_NORMAL, 10, 15);			
		
		DOCfgMode (6, DO_MODE_DIRECT, FALSE);		
		DOCfgBlink(6, DO_BLINK_EN_NORMAL, 25, 30);			
		
	  DOCfgMode (7, DO_MODE_DIRECT, FALSE);		
		DOCfgBlink(7, DO_BLINK_EN_NORMAL, 15, 20);			
		
	

	return 0;
}

int changeMode(INT8U state){
	
	INT8U i;

	for( i = 0; i < 4 ; i++){
		
			DOCfgMode (i, state, FALSE);				
		
	}
	
	if(state == DO_MODE_DIRECT){
		
		DOCfgMode (4, state , FALSE);
		DOCfgMode (5, state, FALSE);
		DOCfgMode (6, state , FALSE);
		DOCfgMode (7, state , FALSE);
		
	}else{
		
		DOCfgMode (4, state + 1, FALSE);
		DOCfgMode (5, state + 1, FALSE);
		DOCfgMode (6, state + 1, FALSE);
		DOCfgMode (7, state + 1, FALSE);		
	}
	
	return 0;
}

int setClockTIme(){
	
		hr   = (timeArray[0]*10) + (timeArray[1]) ;
		min  = (timeArray[2]*10) + (timeArray[3]);
		sec  = (timeArray[4]*10) + (timeArray[5]);
		ClkSetTime(hr, min, sec);
		return 0;
}


//function to  get timestamp convert into ASCII string and print to terminal
int tsConvPrint(){
	
		static  char * ClkDOWTbl[] = {          /* NAME FOR EACH DAY OF THE WEEK                                */
		"Sunday ",
		"Monday ",
		"Tuesday ",
		"Wednesday ",
		"Thursday ",
		"Friday ",
		"Saturday "
		};

		tasks[0].ts = ClkGetTS();
		ClkFormatTS( 2, tasks[0].ts, &tasks[0].time );
		SendString("  ");	
		SendString(tasks[0].time); 	
		SendString("  ");				
		SendString( ClkDOWTbl[ tasks[0].time[9]-'2']);

		SendString("\n");
	
	return 0;
}

	
	
	//Function that prints aout a message to the UART whenever it is called, created to be used with the timer.
	void kernelTimerMessage( void * callback_arg){
	
		
		//TmrReset(0);
		//TmrStart(0);
		//P2 &=  0x80;
		/*if( (err = OSSemPost(tasks[0].Sem)) != 0){
				SendString( "Error creating Post for Task2" ); 
			}*/
	 
	}
