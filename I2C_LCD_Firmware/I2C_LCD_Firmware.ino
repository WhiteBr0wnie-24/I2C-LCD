// ------------------------------------------
// ----    LCD I2C Interface Firmware    ----
// ------------------------------------------
//
// This is the firmware that runs on the MCU
// you put in the adapter board. The MCU
// receives commands via I2C and then interprets
// them. For a list of valid commands see the
// GitHub repository.
//
// nerdhut.de; 2021

#include <LiquidCrystal.h>
#include <Wire.h>

#define RS 7
#define EN 8
#define D4 12
#define D5 11
#define D6 10
#define D7 9
#define I2C_ADDRESS 0x1F
#define RECV_BUFFER_SIZE 64

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
char recv_buffer[RECV_BUFFER_SIZE + 1];     // This buffer holds the data received over I2C. The +1 space is reserved for the string end character \0
bool recv_flag = false;                     // The I2C receive handler sets this variable to true, the loop method unsets it once it's done

void setup()
{
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(handleI2C);
}

void handleI2C(int bytes)
{
  recv_flag = true;
}

// A short helper method that checks whether a given string str
// starts with the string called prefix.
bool stringStartsWith(const char *str, const char *prefix)
{
  if(strlen(str) == 0 || strlen(prefix) == 0)
    return false;
  
  return (strncmp(str, prefix, strlen(prefix)) == 0);
}

// This function takes a string and an index. The string is assumed to
// be of the form 'COMMAND_X_Y_ ... _Z where X, Y, ..., Z are up to n
// integer parameters starting at index 0. The function the scans the
// entire string until it either finds a parameter with the supplied
// index or reaches the end of the string. The function the returns
// the integer value of the parameter or 0 if it didn't find anything.
int getParamAtIndex(char *str, unsigned index)
{
  // Start at index -1 to ignore the command itself
  int extracted = -1;
  int len = strlen(str) + 1;
  // Allocate some space for a copy of the input string
  char *copy = malloc(len);

  // malloc couldn't allocate enough space
  if(copy == NULL)
    return;
  
  // Copy the input string because strtok destroys the
  // original string. This way, you can call getParamAtIndex
  // multiple times using the same string as an input without
  // destroying the string.
  strcpy(copy, str);
  
  // Then, split the copy at the given delimiter
  char *match = strtok(copy, "_");

  // And look at each substring
  while(match != 0)
  {
    if(extracted == index)
    {
      Serial.println(atoi(match));
	  free(copy);
      return atoi(match);
    }
      
    match = strtok(0, "_");
    extracted++;
  }
  
  // Don't forget to free the previously allocated space!
  free(copy);
  
  return 0;
}

void setCursorTo(unsigned x, unsigned y)
{
  lcd.setCursor(x, y);
}

void initDisplay(unsigned width, unsigned height)
{
  lcd.begin(width, height);
}

void loop()
{
  int received = 0;
  
  if(recv_flag)
  {
	// Bytes are available to read and the buffer isn't full yet
    while(Wire.available() > 0 && received < RECV_BUFFER_SIZE)
    {
      char current = (char) Wire.read();

      // If we reach the end of the current string, break out
	  // of the loop. This allows the sender to combine multiple
	  // command strings using a single I2C send cycle
      if(current != '\0')
        recv_buffer[received++] = current;
      else
        break;
    }

    if(received > 0)
    {
      // Append the String end character to the buffer
      recv_buffer[received] = '\0';
      
	  // Then check the received command and act
      if(strncmp(recv_buffer, "CLEAR", RECV_BUFFER_SIZE) == 0)
      {
        lcd.clear();
      }
      else if(stringStartsWith(recv_buffer, "INIT_"))
      {
        int x = getParamAtIndex(recv_buffer, 0);
        int y = getParamAtIndex(recv_buffer, 1);
    
        initDisplay(x, y);
      }
      else if(stringStartsWith(recv_buffer, "CURSOR_"))
      {
        int x = getParamAtIndex(recv_buffer, 0);
        int y = getParamAtIndex(recv_buffer, 1);
    
        setCursorTo(x, y);
      }
      else
      {
		// If none of the command-patterns matched, the sender
		// must've transmitted a simple text string. Just
		// display it at the current cursor position.
        lcd.print(recv_buffer);
      }
    }
    else
    {
      recv_flag = false;
    }
  }
}
