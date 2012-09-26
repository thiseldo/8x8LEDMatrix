/*
 * Show messages on an 8x8 led matrix,
 * scrolling from right to left.
 *
 * Uses FrequencyTimer2 library to
 * constantly run an interrupt routine
 * at a specified frequency. This
 * refreshes the display without the
 * main loop having to do anything.
 *
 */

#include <FrequencyTimer2.h>
#include "font5x7.h"
String inputString = "";
boolean stringComplete = false;  // whether the string is complete

#define DELAY 80

#define MAX_MSG 100

char msgBuf[MAX_MSG];
char testMessage[] = "My Sex is on fire!!!";

char *msgPtr;

byte col = 0;
byte leds[8][8];

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
//int pins[17]= {-1, 5, 4, 3, 2, 14, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6};
int pins[17]= {
  -1, 17, 8, 7, 19, 9, 13, 4, 10, 6, 5, 18, 3, 2, 11, 12, 16};
//int pins[17]= {-1, 13, 12, 11, 10, 16, 17, 18, 19, 2, 3, 4, 5, 6, 7, 8, 9};

// col[xx] of leds = pin yy on led matrix
int cols[8] = {
  pins[13], pins[3], pins[4], pins[10], pins[06], pins[11], pins[15], pins[16]};

// row[xx] of leds = pin yy on led matrix
int rows[8] = {
  pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};


void setup() {
  // sets the pins as output
  for (int i = 1; i <= 16; i++) {
    pinMode(pins[i], OUTPUT);
  }

  // set up cols and rows
  for (int i = 1; i <= 8; i++) {
    digitalWrite(cols[i - 1], LOW);
  }

  for (int i = 1; i <= 8; i++) {
    digitalWrite(rows[i - 1], LOW);
  }

  clearLeds();

  // Turn off toggling of pin 11
  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(2000);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(display);

  //  Serial.begin(19200);
  /*  for( int i=' '; i<='z'; i++ ) {
   //    Serial.println(i, DEC);
   setChar( i );
   delay(100);
   }
   */
  Serial.begin(9600);
   inputString.reserve(200);
   
   strcpy(msgBuf, testMessage);
   stringComplete = true;

}

void scrollMsg( char *msg ) {
  char *ptr = msg;
  clearLeds();
  
  while( *ptr ) {
    slideChar( *ptr++, DELAY );
  }
  // A final space
  slideChar( ' ', DELAY) ;
}



void loop() {
    // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString); 
    Serial.println();
    // clear the string:
    inputString = "";
    stringComplete = false;
    scrollMsg(msgBuf);
  }
//  scrollMsg( testMessage );
  //    pattern = ++pattern % numPatterns;
  //    slidePattern(pattern, 60);
  //  delay(1000);
}

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      leds[i][j] = 0;
    }
  }
}

void setChar(char ch) {

  for (int i = 0; i < 5; i++) {
    unsigned char bt = pgm_read_byte(&(smallFont [(ch-32)*5 + i] ));
    for (int j = 0; j < 8; j++) {
      leds[j][i+1] = (bt & 0x01);
      bt = bt >> 1;
    }
  }
}

void slideChar(char ch, int del) {
  for (int l = 0; l < 5; l++) {
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 8; j++) {
        leds[j][i] = leds[j][i+1];
      }
    }
    unsigned char bt = pgm_read_byte(&(smallFont [(ch-32)*5 + l] ));
    for (int j = 0; j < 8; j++) {
      leds[j][7] = (bt & 0x01);
      bt = bt >> 1;
    }
    delay(del);
  }

  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 8; j++) {
      leds[j][i] = leds[j][i+1];
    }
  }

  for (int j = 0; j < 8; j++) {
    leds[j][7] = 0;
  }
  delay(del);
}


// Interrupt routine
void display() {
  digitalWrite(cols[col], LOW);  // Turn whole previous column off
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    //    if (leds[col][7 - row] == 1) {
    if (leds[col][ row] == 1) {
      digitalWrite(rows[row], LOW);  // Turn on this led
    }
    else {
      digitalWrite(rows[row], HIGH); // Turn off this led
    }
  }
  digitalWrite(cols[col], HIGH); // Turn whole column on at once (for equal lighting times)
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    *msgPtr++ = inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if( (inChar == '\n') || ( msgPtr >= (msgBuf + MAX_MSG - 1))) {
      *msgPtr = '\0';
      stringComplete = true;
      msgPtr = msgBuf;

    }
  }
}

