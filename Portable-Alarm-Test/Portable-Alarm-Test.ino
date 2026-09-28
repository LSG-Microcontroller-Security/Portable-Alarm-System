/*
 Name:		PortableAlarmTest.ino
 Created:	6/27/2020 3:39:19 PM
 Author:	luigi.santagada
*/

// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	return;
	Serial.begin(9600);
	setPinsMode();
}

// the loop function runs over and over again until power down or reset
void loop() {
	return;
	checkUsb1();
	checkUsb2();
	checkUsb3(); 

}

void setPinsMode()
{
	 
}


void checkUsb2()
{
	pinMode(A2, OUTPUT);
	delay(500);
	digitalWrite(A2, HIGH);
	delay(500);
	digitalWrite(A2, LOW);
}

void checkUsb3()
{
	pinMode(5, OUTPUT);
	delay(500);
	digitalWrite(5, HIGH);
	delay(500);
	digitalWrite(5, LOW);
}

void checkUsb1()
{
	pinMode(A4, OUTPUT);
	pinMode(A5, OUTPUT);

	delay(500);
	digitalWrite(A5, HIGH);
	delay(500);
	digitalWrite(A5, LOW);
	delay(500);
	digitalWrite(A4, HIGH);
	delay(500);
	digitalWrite(A4, LOW);
}
