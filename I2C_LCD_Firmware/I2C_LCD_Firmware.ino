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
char recv_buffer[RECV_BUFFER_SIZE + 1];
bool recv_flag = false;

void setup()
{
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(handleI2C);
}

void handleI2C(int bytes)
{
  recv_flag = true;
}

bool stringStartsWith(const char *str, const char *prefix)
{
  if(strlen(str) == 0 || strlen(prefix) == 0)
    return false;
  
  return (strncmp(str, prefix, strlen(prefix)) == 0);
}

int getParamAtIndex(char *str, unsigned index)
{
  int extracted = -1;
  int len = strlen(str) + 1;
  char *copy = malloc(len);

  if(copy == NULL)
    return;
  
  strcpy(copy, str);
  char *match = strtok(copy, "_");

  while(match != 0)
  {
    if(extracted == index)
    {
      Serial.println(atoi(match));
      return atoi(match);
    }
      
    match = strtok(0, "_");
    extracted++;
  }

  Serial.println("0");
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
    while(Wire.available() > 0 && received < RECV_BUFFER_SIZE)
    {
      char current = (char) Wire.read();

      if(current != '\0')
        recv_buffer[received++] = current;
      else
        break;
    }

    if(received > 0)
    {
      recv_buffer[received] = '\0';
      
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
        lcd.print(recv_buffer);
      }
    }
    else
    {
      recv_flag = false;
    }
  }
}
