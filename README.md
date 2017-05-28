# uCOS


Assignemnt2
Create four tasks. Three tasks pend on separate semaphores that are posted
by the forth task which detects changes of input in Port 1.
A positive transition on bit0 posts to semaphore 1, which task 1 is pending.
A negative transition on bit1 posts to semaphore 2, which task 2 is pending.
Either a positive or negative transition on bit2 post to semaphore 3, which task 3 is pending.
When task 1,2,3 receives the semaphore it sends its task number to the serial port.
Task 1and 2 will wait indefinitely until the semaphore arrives, but task 3 will only wait one
second before exiting the pend and outputting 'T' to the serial port. However, if the semaphore arrives within the second the task number is outputted. 

Assignemt3
1. Use the key.c + key.h  driver to interface to the 8051 (NO'K's) Development Board keypad.
2. Create 6 QUEUES, each which can hold 10 void *.
3. Create a single task (mainTask) to takes input from the keypad and based on the number of the key received,  send a message to one of six different tasks (Task1…Task 6) via unique QUEUE.
4. This is done by the mainTask checking the number it receives from the keypad (make sure AUTO REPEAT is working) to see if it is in the range 1 to 6 and posting the number to the appropriate QUEUE and ignoring numbers outside this range by printing "invalid number".
5. Task 1..6 issues the system call: “PEND”, which tells the operating system that Task 1..6 is pending on their appropriate QUEUE for a message to be received. Use different timeouts (SECOND) for each QUEUE.
6. When a message is posted to the appropriate QUEUE  Task 1..6 will echo the character to the console screen plus the number of times the key has been pressed otherwise will print a 'Timeout Error'.
7. Create the 6 pending task using a for loop and pass the QUEUE, TO and ID parameters in the OSTaskCreate API call. 

Assignemnt4
Create the following application using the uCOS-II 2.84 kernel and DIO,  Clock and  Keypad driver code.
1. Configure the application for 8 DI, 8 DO and 4X4 keypad.
2. A change of state on any of the 8 DI will result in the Date, Time, Day Of Week, DI signal number and state (on/off) been displayed on the serial terminal.
3. Demonstrate Disjunctive and Conjunctive Synchronization using the Event Flag Group by triggering the Digital Outputs in Asynchronous and Synchronous blinking modes when the inputs are switched to the desired specification for OSFlagPend.
Use four of the inputs for Disjunctive Synchronization and the other four for Conjunctive Synchronization.
Four outputs should be setup in Synchronous mode and the other four outputs in Asynchronous mode.
Blinking is only enabled after a defined pattern (ARMED) is entered by the keypad.
Blinking is disabled after a defined pattern (UN-ARMED) is entered by the keypad.
4. Use the kernel timer to send a message (“ I am the timer”) to the serial port every 5 seconds.
5. Set the Time using the Keypad, the Date can be hard coded into the clock driver.
