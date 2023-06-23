
#include <avr/io.h>             // This header file includes the appropriate Input/output definitions for the device 
#include <util/delay.h>         // to use delay function we need to include this library
#include <stdlib.h>             // we'll be using itoa() function to convert integer to character array that resides in this library
#include <avr/interrupt.h>

#define F_CPU 8000000L


#define US_TRIG_1_POS	PC0         // Trigger pin of Ultrasonic 1 is connected to the Port C pin 0
#define US_ECHO_1_POS	PC1         // Echo pin of Ultrasonic 1 is connected to the Port C pin 1

#define US_TRIG_2_POS   PC2         // Trigger pin of Ultrasonic 2 is connected to the Port C pin 2
#define US_ECHO_2_POS   PC3         // Echo pin of Ultrasonic 2 is connected to the Port C pin 3


#define US_1_ERROR		-1      // Defining four more variables two know if the ultrasonic sensor is working or not 
#define	US_1_NO_OBSTACLE -2
#define US_2_ERROR  -1
#define US_2_NO_OBSTACLE -2

int distance_1, distance_2;      // variable for store distance readings of two ultrsonic sensors

void HCSR04Init();        // Call to Initialize the ultrasonic sensors
void HCSR04_1_Trigger();  // Call to send a trigger signal to ultrasonic sensor 1
void HCSR04_2_Trigger();  // Call to send a trigger signal to ultrasonic sensor 2

void HCSR04Init()
{
	
	DDRC|=(1<<US_TRIG_1_POS);  // Ultrasonic sensor 1 Trigger pin set as output
	DDRC|=(1<<US_TRIG_2_POS);  // Ultrasonic sensor 2 Trigger pin set as output
	
}



void HCSR04_1_Trigger()
{   // this function will generate ultrasonic sound wave for 15 microseconds 
		
	PORTC|=(1<<US_TRIG_1_POS);	//high
	
	_delay_us(15);				//wait 15uS
	
	PORTC &=~(1<<US_TRIG_1_POS);	//low
}

void HCSR04_2_Trigger()
{   // this function will generate ultrasonic sound wave for 15 microseconds 
	
	
	PORTC|=(1<<US_TRIG_2_POS);	//high
	
	_delay_us(15);				//wait 15uS
	
	PORTC &=~(1<<US_TRIG_2_POS);	//low
}



uint16_t GetPulseWidth_1()
{
	// this function will be used to measure the pulse duration. When the ultra sound echo back after hitting an object
	// the microcontroller will read the pulse using the echo pin of the ultrasonic sensor connected to it. 
	
	uint32_t i,result;

	// Section - 1: the following lines of code before the section - 2 is checking if the ultrasonic is working or not
	// it check the echo pin for a certain amount of time. If there is no signal it means the sensor is not working or not connect properly
	for(i=0;i<600000;i++)
	{
		if(!(PINC & (1<<US_ECHO_1_POS)))   // if echo pulse not receive continue and watch until receive a pulse, if receive break the loop and goto measure the pulse duration
		continue;	//Line is still low, so wait
		else
		break;		//High edge detected, so break.
	}

	if(i==600000)
	return US_1_ERROR;	//Indicates time out
	
	//High Edge Found
	
	// Section -2 : This section is all about preparing the timer for counting the time of the pulse. Timers in microcontrllers is used for timimg operation
	//Setup Timer1
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);	// This line sets the resolution of the timer. Maximum of how much value it should count.
	TCNT1=0x00;			// This line start the counter to start counting time

	// Section -3 : This section checks weather the there is any object or not
	for(i=0;i<600000;i++)                // the 600000 value is used randomly to denote a very small amount of time, almost 40 miliseconds 
	{
		if(PINC & (1<<US_ECHO_1_POS))
		{
			if(TCNT1 > 60000) break; else continue;   // if the TCNT1 value gets higher than 60000 it means there is not object in the range of the sensor
		}
		else
		break;
	}

	if(i==600000)
	return US_1_NO_OBSTACLE;	//Indicates time out

	//Falling edge found

	result=TCNT1;          // microcontroller stores the the value of the counted pulse time in the TCNT1 register. So, we're returning this value to the 
	                       // main function for utilizing it later 

	//Stop Timer
	TCCR1B=0x00;

	if(result > 60000)
	return US_1_NO_OBSTACLE;	//No obstacle
	else
	return (result>>1);
}


uint16_t GetPulseWidth_2()
{
	// this function will be used to measure the pulse duration. When the ultra sound echo back after hitting an object
	// the microcontroller will read the pulse using the echo pin of the ultrasonic sensor connected to it. 
	
	uint32_t i,result;

	// Section - 1: the following lines of code before the section - 2 is checking if the ultrasonic is working or not
	// it check the echo pin for a certain amount of time. If there is no signal it means the sensor is not working or not connect properly
	for(i=0;i<600000;i++)
	{
		if(!(PINC & (1<<US_ECHO_2_POS)))    // if echo pulse not receive continue and watch until receive a pulse, if receive break the loop and goto measure the pulse duration
		continue;	//Line is still low, so wait
		else
		break;		//High edge detected, so break.
	}

	if(i==600000)
	return US_2_ERROR;	//Indicates time out
	
	//High Edge Found
	
	// Section -2 : This section is all about preparing the timer for counting the time of the pulse. Timers in microcontrllers is used for timimg operation
	//Setup Timer1
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);	// This line sets the resolution of the timer. Maximum of how much value it should count.
	TCNT1=0x00;			// This line start the counter to start counting time

	// Section -3 : This section checks weather the there is any object or not
	for(i=0;i<600000;i++)                // the 600000 value is used randomly to denote a very small amount of time, almost 40 miliseconds 
	{
		if(PINC & (1<<US_ECHO_2_POS))
		{
			if(TCNT1 > 60000) break; else continue;   // if the TCNT1 value gets higher than 60000 it means there is not object in the range of the sensor
		}
		else
		break;
	}

	if(i==600000)
	return US_2_NO_OBSTACLE;	//Indicates time out

	//Falling edge found

	result=TCNT1;          // microcontroller stores the the value of the counted pulse time in the TCNT1 register. So, we're returning this value to the 
	                       // main function for utilizing it later 

	//Stop Timer
	TCCR1B=0x00;

	if(result > 60000)
	return US_2_NO_OBSTACLE;	//No obstacle
	else
	return (result>>1);
}

//****************** main loop *************************************************************************************
	
int main()
{



	while(1) {
		
		uint16_t r1;   // 16 bit two variables for save ultrasonic pulse duration
		uint16_t r2;
		
		_delay_ms(100);	

		
	
		HCSR04Init();  // Initialize ultrasonic sensors


	
		while(1)
		{
			
			
			
			//Send a trigger pulse
			HCSR04_1_Trigger();               // calling the ultrasonic sound wave generator function

			//Measure the width of pulse
			r1=GetPulseWidth_1();             // getting the duration of the ultrasound took to echo back after hitting the object

			//Handle Errors
			
			
			HCSR04_2_Trigger();
			
			r2=GetPulseWidth_2();
	
			distance_1=(r1*0.034/2.0);	// This will give the distance in centimeters of 1st sensor
				
			distance_2=(r2*0.034/2.0);   // This will give the distance in centimeters of 2nd sensor

			
	
	
	
				
		if((distance_1 < 50)||(distance_2 <50)){  // if detects any object by any ultrasonic sensor
			
			PORTC &= ~(1<< DRIVE_MOTOR);   // turn off the drive motor
			PORTC |= (1<< BUZZER);         // Turn on the buzzer
			_delay_ms(60000);              // Wait 1 minute
			
			PORTC |= (1<< DRIVE_MOTOR);    // Start to go forward again
			PORTC &= ~(1<< BUZZER);        // Turn off the buzzer
		}	



