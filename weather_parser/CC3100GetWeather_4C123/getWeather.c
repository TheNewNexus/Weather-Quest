/*
 * main.c - Example project for UT.6.02x Embedded Systems - Shape the World
 * Jonathan Valvano and Ramesh Yerraballi
 * October 27, 2015
 * Hardware requirements
     TM4C123 LaunchPad, optional Nokia5110
     CC3100 wifi booster and
     an internet access point with OPEN, WPA, or WEP security

 * derived from TI's getweather example
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Application Name     -   Get weather
 * Application Overview -   This is a sample application demonstrating how to
                            connect to openweathermap.org server and request for
              weather details of a city.
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_SLS_Get_Weather_Application
 *                          doc\examples\sls_get_weather.pdf
 */
/* CC3100 booster pack connections (unused pins can be used by user application)
Pin  Signal        Direction      Pin   Signal     Direction
P1.1  3.3 VCC         IN          P2.1  Gnd   GND      IN
P1.2  PB5 UNUSED      NA          P2.2  PB2   IRQ      OUT
P1.3  PB0 UART1_TX    OUT         P2.3  PE0   SSI2_CS  IN
P1.4  PB1 UART1_RX    IN          P2.4  PF0   UNUSED   NA
P1.5  PE4 nHIB        IN          P2.5  Reset nRESET   IN
P1.6  PE5 UNUSED      NA          P2.6  PB7  SSI2_MOSI IN
P1.7  PB4 SSI2_CLK    IN          P2.7  PB6  SSI2_MISO OUT
P1.8  PA5 UNUSED      NA          P2.8  PA4   UNUSED   NA
P1.9  PA6 UNUSED      NA          P2.9  PA3   UNUSED   NA
P1.10 PA7 UNUSED      NA          P2.10 PA2   UNUSED   NA

Pin  Signal        Direction      Pin   Signal      Direction
P3.1  +5  +5 V       IN           P4.1  PF2 UNUSED      OUT
P3.2  Gnd GND        IN           P4.2  PF3 UNUSED      OUT
P3.3  PD0 UNUSED     NA           P4.3  PB3 UNUSED      NA
P3.4  PD1 UNUSED     NA           P4.4  PC4 UART1_CTS   IN
P3.5  PD2 UNUSED     NA           P4.5  PC5 UART1_RTS   OUT
P3.6  PD3 UNUSED     NA           P4.6  PC6 UNUSED      NA
P3.7  PE1 UNUSED     NA           P4.7  PC7 NWP_LOG_TX  OUT
P3.8  PE2 UNUSED     NA           P4.8  PD6 WLAN_LOG_TX OUT
P3.9  PE3 UNUSED     NA           P4.9  PD7 UNUSED      IN (see R74)
P3.10 PF1 UNUSED     NA           P4.10 PF4 UNUSED      OUT(see R75)

UART0 (PA1, PA0) sends data to the PC via the USB debug cable, 115200 baud rate
Port A, SSI0 (PA2, PA3, PA5, PA6, PA7) sends data to Nokia5110 LCD

*/

#include "..\cc3100\simplelink\include\simplelink.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "utils/cmdline.h"
#include "application_commands.h"
#include "LED.h"
#include <string.h>
#include "ST7735.h"
//#include "../inc/tm4c123gh6pm.h"
#include "tm4c123gh6pm.h"
#include "displayWeather.h"
#include <stdbool.h>
#include "SysTick.h"
#include <ctype.h>
#include <stdlib.h> 
#include "driverlib/systick.h"




// To Do: replace the MinHe WiFiollowing three lines with your access point information
#define SSID_NAME "Robby_iPhone" /* Access point name to connect to */
#define SEC_TYPE SL_SEC_TYPE_WPA
#define PASSKEY "testtest" /* Password in case of secure AP */
#define MAXLEN 100

// standard ASCII symbols
#define CR 0x0D
#define LF 0x0A
#define BS 0x08
#define SP 0x20
#define NVIC_EN0_UART0 0x20     // UART0 IRQ number 5

//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART0_Init(bool RxInt, bool TxInt){
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // activate UART0
  if ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOA)==0) {
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;	// Activate D clocks
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOA)==0){};
	}
  UART0_CTL_R &= ~UART_CTL_UARTEN;  // Disable UART
  UART0_IBRD_R = 27;                    // IBRD = int(50,000,000 / (16 * 115,200)) = int(27.1267)
  UART0_FBRD_R = 8;                     // FBRD = int(0.1267 * 64 + 0.5) = 8
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
	//clear FIFO
  UART0_CC_R = 0x0;
	
	if ( RxInt | TxInt) {
		NVIC_PRI1_R = (NVIC_PRI1_R&~0x0000E000)|0x0000A000; // bits 15-13, priority 5
		NVIC_EN0_R = NVIC_EN0_UART0;           // enable UART0 interrupt in NVIC
		if (RxInt) {
			UART0_IM_R |= UART_IM_RXIM;         // Enable RX interrupt
		}
		
		if (TxInt) {
			UART0_IM_R |= UART_IM_TXIM;         // Enable TX interrupt
		}
	}

	
  UART0_CTL_R |= UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN; // enable Tx, RX and UART
  GPIO_PORTA_AFSEL_R |= 0x03;                                   // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= 0x03;                                     // enable digital I/O on PA1-0
                                                                // configure PA1-0 as UART
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03; // disable analog functionality on PA
	UART0_CTL_R |= UART_CTL_UARTEN;  // Enable UART
}

//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(char *pt)
{
  while (*pt)
  {
    while ((UART0_FR_R & UART_FR_TXFF) != 0)
      ;
    UART0_DR_R = *pt;
    pt++;
  }
}

//------------UART0_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
uint8_t UART0_InChar(void)
{
  while ((UART0_FR_R & UART_FR_RXFE) != 0)
    ;
  return ((uint8_t)(UART0_DR_R & 0xFF));
}

//------------UART0_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(uint8_t data)
{
  while ((UART0_FR_R & UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}

void UART0_NextLine(void)
{
  UART0_OutChar(CR);
  UART0_OutChar(LF);
}

//------------UART_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
//    when max length is reach, no more input will be accepted
//    the display will wait for the <enter> key to be pressed.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void UART0_InString(uint8_t *bufPt, uint16_t max)
{
  int length = 0;
  char character;
  character = UART0_InChar();
  while (character != CR)
  {
    if (character == BS)
    { // back space
      if (length)
      {
        bufPt--;
        length--;
        UART0_OutChar(BS);
      }
    }
    else if (length < max)
    {
      *bufPt = character;
      bufPt++;
      length++;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  *bufPt = 0; // adding null terminator to the end of the string.
}


#define MAX_RECV_BUFF_SIZE 1024
#define MAX_SEND_BUFF_SIZE 512
#define MAX_HOSTNAME_SIZE 40
#define MAX_PASSKEY_SIZE 32
#define MAX_SSID_SIZE 32

#define SUCCESS 0

#define CONNECTION_STATUS_BIT 0
#define IP_AQUIRED_STATUS_BIT 1

/* Application specific status/error codes */
typedef enum
{
  DEVICE_NOT_IN_STATION_MODE = -0x7D0, /* Choosing this number to avoid overlap w/ host-driver's error codes */

  STATUS_CODE_MAX = -0xBB8
} e_AppStatusCodes;

/* Status bits - These are used to set/reset the corresponding bits in 'g_Status' */
typedef enum
{
  STATUS_BIT_CONNECTION = 0, /* If this bit is:
                              *      1 in 'g_Status', the device is connected to the AP
                              *      0 in 'g_Status', the device is not connected to the AP
                              */

  STATUS_BIT_IP_AQUIRED, /* If this bit is:
                          *      1 in 'g_Status', the device has acquired an IP
                          *      0 in 'g_Status', the device has not acquired an IP
                          */

} e_StatusBits;

#define SET_STATUS_BIT(status_variable, bit) status_variable |= (1 << (bit))
#define CLR_STATUS_BIT(status_variable, bit) status_variable &= ~(1 << (bit))
#define GET_STATUS_BIT(status_variable, bit) (0 != (status_variable & (1 << (bit))))
#define IS_CONNECTED(status_variable) GET_STATUS_BIT(status_variable, \
                                                     STATUS_BIT_CONNECTION)
#define IS_IP_AQUIRED(status_variable) GET_STATUS_BIT(status_variable, \
                                                      STATUS_BIT_IP_AQUIRED)

typedef struct
{
  UINT8 SSID[MAX_SSID_SIZE];
  INT32 encryption;
  UINT8 password[MAX_PASSKEY_SIZE];
} UserInfo;

/*
 * GLOBAL VARIABLES -- Start
 */

volatile int currentIcon = 0;
volatile bool updateIcon = false;
volatile bool iconFrame = false;
bool weatherDataReceived = false; // Flag to indicate if weather data is ready
unsigned long lastUpdateTime = 0;         // Time of the last frame update
bool endOfString = false;
uint8_t string[MAXLEN];
volatile unsigned long strIndex = 0;		   // Index for string

WeatherCondition icon = WEATHER_CLEAR;
char Recvbuff[MAX_RECV_BUFF_SIZE];
char SendBuff[MAX_SEND_BUFF_SIZE];
char HostName[MAX_HOSTNAME_SIZE];
unsigned long DestinationIP;
int SockID;
int frame = 0;
int32_t retVal;
SlSecParams_t secParams;
char *pConfig = NULL;
INT32 ASize = 0;
SlSockAddrIn_t Addr;
char *pt = NULL;
/*
   char option;
    char location[MAXLEN] = {0};
    char request[MAX_SEND_BUFF_SIZE];
    char encoded_location[MAXLEN * 3] = {0};
*/
//char option;
static char option = 0;     // Store the user option
//static char location[MAXLEN]; // Store the location input
static bool inputComplete = false; // To track if input is complete
//char option;
char City[MAXLEN];
char Temperature[MAXLEN];
char MinTemperature[MAXLEN];
char MaxTemperature[MAXLEN];
char Humidity[MAXLEN];
char Weather[MAXLEN];
char mainDescription[MAXLEN];
WeatherCondition currentWeather;
uint8_t i;
char request[MAXLEN];
char location[MAXLEN] = {0}; // Declare and initialize location variable

typedef enum
{
  CONNECTED = 0x01,
  IP_AQUIRED = 0x02,
  IP_LEASED = 0x04,
  PING_DONE = 0x08

} e_Status;
UINT32 g_Status = 0;
/*
 * GLOBAL VARIABLES -- End
 */

/*
 * STATIC FUNCTION DEFINITIONS  -- Start
 */

static int32_t configureSimpleLinkToDefaultState(char *);

/*
 * STATIC FUNCTION DEFINITIONS -- End
 */

int32_t UART_inchar(void);
void UART_instring(char *string, uint16_t maxLen);
void getWeather(void);
void parseWeatherData(void);
int sendWeatherRequest(char *request);

void Crash(uint32_t time)
{
  while (1)
  {
    for (int i = time; i; i--)
    {
    };
    LED_RedToggle();
  }
}

/*
 * Application's entry point
 */
// 1) change Austin Texas to your city
// 2) metric(for celsius), imperial(for fahrenheit)
// api.openweathermap.org/data/2.5/weather?q={city name},{state code}&appid={API key}
#define REQUEST "GET /data/2.5/weather?q=Long%20Beach&APPID=GET_API_FROM_ACCOUNT&units=imperial HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n"
#define API_BASE "GET /data/2.5/weather?"
#define API_KEY "&APPID=GET_API_FROM_ACCOUNT&units=imperial"
#define API_SUFFIX " HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n"

// 1) go to http://openweathermap.org/appid#use
// 2) Register on the Sign up page
// 3) get an API key (APPID) replace the 7907b2abac2053aed180a74b9310b119 with your APPID
int main(void)
{
	SysTick_Init(); // with interrupt crash
	//SysTickDisable();
	
  initClk();   // PLL 50 MHz
  UART0_Init(false,false); // Send data to PC, 115200 bps
  LED_Init();  // initialize LaunchPad I/O
  ST7735_InitR(INITR_REDTAB);  
	UART0_NextLine();
  UART_OutString("Weather App\n\r");
  ST7735_OutString("Weather App\n\r");

  WeatherCondition currentWeather = WEATHER_CLEAR;     // This would be updated dynamically in real scenarios
  retVal = configureSimpleLinkToDefaultState(pConfig); // set policies
  if (retVal < 0)
    Crash(4000000);
  retVal = sl_Start(0, pConfig, 0);
  if ((retVal < 0) || (ROLE_STA != retVal))
    Crash(8000000);
  secParams.Key = PASSKEY;
  secParams.KeyLen = strlen(PASSKEY);
  secParams.Type = SEC_TYPE; // OPEN, WPA, or WEP
  sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
  while ((0 == (g_Status & CONNECTED)) || (0 == (g_Status & IP_AQUIRED)))
  {
    _SlNonOsMainLoopTask();
  }
  
	LED_GreenOn();
  UART_OutString("Connected\n\r");
  ST7735_OutString("Connected\n\r");
	weatherDataReceived = false;
	SysTickIntRegister(SysTick_Handler);
	//SysTickPeriodSet(ONE_MS_RELOAD);
	//SysTickEnable();
	SysTickIntEnable();
	Delay_1ms(50);
  // Wait for a short time to allow systick_count to change
	//SysTick_Wait(50);

 srand(SysTickValueGet());

while (1) {
    // Fetch weather data if not received
    if(!updateIcon){
    getWeather();
		}
    // Draw random shapes
    int random_num = rand() % 4;
    switch (random_num) {
        case 0:
            ST7735_DrawCircle(60, 130, 10, ST7735_YELLOW);
            break;
        case 1:
            ST7735_DrawLine(ST7735_TFTWIDTH/2 - 20, 130, 100, 100, ST7735_YELLOW);
            break;
        case 2:
            ST7735_DrawFastVLine(60, 120, 40, ST7735_YELLOW);
            break;
        case 3:
            ST7735_DrawFastHLine(ST7735_TFTWIDTH/2 - 20, 130, 40, ST7735_YELLOW);
            break;
    }
		

  

   
}

}

void SysTick_Handler(void){
	 // Animate weather icon
		if(updateIcon){
    if (iconFrame) {
        WeatherIcon_Draw1(icon, 48, 112);
    } else {
        WeatherIcon_Draw2(icon, 48, 112);
    }
    iconFrame = !iconFrame; // Toggle icon frame
    Delay_1ms(125); // Delay for animation speed
		  // Check for user input non-blockingly
    if (UARTCharsAvail(UART0_BASE)) {
        // Read the character from the UART
        char c = (char)UARTCharGetNonBlocking(UART0_BASE);
        // Check if the input character is 'u' or 'U'
        if (c == 'u' || c == 'U') {
            updateIcon = false; // Trigger weather update
            UART_OutString("\nUpdating weather...\n");
					
        }
    }
	}
}

int32_t UART_inchar(void) {
    int32_t c;
    do {
        c = UARTCharGetNonBlocking(UART0_BASE);
    } while(c == -1);
    return c;
}

void UART_instring(char *string, uint16_t maxLen) {
    int32_t c;
    uint16_t count = 0;

    while (count < maxLen - 1) {
        c = UARTCharGetNonBlocking(UART0_BASE);
        if (c == -1) {
            // No character available, you might want to add a small delay here
						Delay_1ms(10);
            continue;
        }
        if (c == '\r' || c == '\n') {
            break;  // End of line
        }

        string[count++] = (char)c;
        
        // Echo the character back to the terminal
        UARTCharPutNonBlocking(UART0_BASE, c);
    }
    string[count] = '\0';  // Null-terminate the string
}
void getWeather(void){
// Display menu and get user input
UART_OutString("\n\rWelcome to my Embedded Weather Quester!");
UART_OutString("\n\rPlease choose your query criteria:");
UART0_NextLine();
UART_OutString("1. City Name");
UART0_NextLine();
UART_OutString("2. City ID");
UART0_NextLine();
UART_OutString("3. Geographic Coordinates");
UART0_NextLine();
UART_OutString("4. Zip Code");
UART0_NextLine();

	UART_OutString(">"); 
	option = UART0_InChar();	
  UART0_OutChar(option);
  UART0_NextLine();
  UART_OutString("Enter the location: ");
  UART0_InString((uint8_t*)location, MAXLEN);
  UART0_NextLine();

  // Construct API request based on user input
  char request[MAX_SEND_BUFF_SIZE];
  // URL-encode the location (replace spaces with %20)
  char encoded_location[MAXLEN * 3] = {0}; // Allow extra space for encoding
  int j = 0;
  for (int i = 0; location[i]; i++)
  {
    if (location[i] == ' ')
    {
      encoded_location[j++] = '%';
      encoded_location[j++] = '2';
      encoded_location[j++] = '0';
    }
    else
    {
      encoded_location[j++] = location[i];
    }
  }
  encoded_location[j] = 0;

  // Construct API request based on user input
  strcpy(request, "GET /data/2.5/weather?");
  switch (option)
  {
  case '1':
    strcat(request, "q=");
    strcat(request, encoded_location);
    break;
  case '2':
    strcat(request, "id=");
    strcat(request, encoded_location);
    break;
  case '3':
    strcat(request, "lat=");
    char *lon = strchr((char *)location, ',');
    if (lon)
    {
      // Temporarily replace comma with null terminator
      *lon = '\0';
      lon++; // Move past the comma

      // Trim leading/trailing spaces from latitude
      char *lat_start = (char *)location;
      char *lat_end = lon - 2; // Point to character before the null terminator
      while (*lat_start == ' ')
        lat_start++;
      while (lat_end > lat_start && *lat_end == ' ')
        lat_end--;
      *(lat_end + 1) = '\0';

      // Trim leading/trailing spaces from longitude
      while (*lon == ' ')
        lon++;
      char *lon_end = lon + strlen(lon) - 1;
      while (lon_end > lon && *lon_end == ' ')
      {
        *lon_end = '\0';
        lon_end--;
      }

      // Construct the request
      strcat(request, lon); // Longitude first in the API
      strcat(request, "&lon=");
      strcat(request, lat_start); // Latitude second in the API
    }
    else
    {
      UART_OutString("Invalid geographic coordinates. Please provide latitude and longitude separated by a comma.\n\r");
      // continue;
    }
    break;

  case '4':
    strcat(request, "zip=");
    strcat(request, encoded_location);
    break;
  default:
    UART_OutString("Invalid option. Please try again.\n\r");
    // continue;
  }
  strcat(request, "&APPID=a2b8b0d8ab4bbca7f946581ca034730a&units=imperial HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n");
  // Use the constructed request instead of the hardcoded REQUEST
  strcpy(SendBuff, request);

  strcpy(HostName, "api.openweathermap.org");
  retVal = sl_NetAppDnsGetHostByName(HostName,
                                     strlen(HostName), &DestinationIP, SL_AF_INET);
  if (retVal == 0)
  {
    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons(80);
    Addr.sin_addr.s_addr = sl_Htonl(DestinationIP); // IP to big endian
    ASize = sizeof(SlSockAddrIn_t);
    SockID = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
    if (SockID >= 0)
    {
      retVal = sl_Connect(SockID, (SlSockAddr_t *)&Addr, ASize);
    }
    if ((SockID >= 0) && (retVal >= 0))
    {
      // strcpy(SendBuff, REQUEST);
      strcpy(SendBuff, request);
      sl_Send(SockID, SendBuff, strlen(SendBuff), 0);   // Send the HTTP GET
      sl_Recv(SockID, Recvbuff, MAX_RECV_BUFF_SIZE, 0); // Receive response
      sl_Close(SockID);
      UART_OutString("\r\n\r\n");
      UART_OutString(Recvbuff);
      UART_OutString("\r\n");

      // process received weather information
      /* find ticker name in response*/
      pt = strstr(Recvbuff, "\"name\"");
      i = 0;
      if (NULL != pt)
      {
        pt = pt + 8; // skip over "name":"
        while ((i < MAXLEN) && (*pt) && (*pt != '\"'))
        {
          City[i] = *pt; // copy into City string
          pt++;
          i++;
        }
      }
      City[i] = 0;

      /* find Temperature Value in response */
      pt = strstr(Recvbuff, "\"temp\"");
      i = 0;
      if (NULL != pt)
      {
        pt = pt + 7; // skip over "temp":
        while ((i < MAXLEN) && (*pt) && (*pt != ','))
        {
          Temperature[i] = *pt; // copy into Temperature string
          pt++;
          i++;
        }
      }
      Temperature[i++] = 'F'; // Append 'F' for Fahrenheit
      Temperature[i] = '\0';

      // Extract minimum temperature
      pt = strstr(Recvbuff, "\"temp_min\"");
      i = 0;
      if (NULL != pt)
      {
        pt = pt + 11; // skip over "temp_min":
        while ((i < MAXLEN) && (*pt) && (*pt != ','))
        {
          MinTemperature[i] = *pt;
          pt++;
          i++;
        }
      }
      MinTemperature[i++] = 'F'; // Append 'F' for Fahrenheit
      MinTemperature[i] = '\0';

      // Extract maximum temperature
      pt = strstr(Recvbuff, "\"temp_max\"");
      i = 0;
      if (NULL != pt)
      {
        pt = pt + 11; // skip over "temp_max":
        while ((i < MAXLEN) && (*pt) && (*pt != ','))
        {
          MaxTemperature[i] = *pt;
          pt++;
          i++;
        }
      }
      MaxTemperature[i++] = 'F'; // Append 'F' for Fahrenheit
      MaxTemperature[i] = '\0';

      // Humidity
      pt = strstr(Recvbuff, "\"humidity\"");
      i = 0;
      if (NULL != pt)
      {
        pt = pt + 11; // skip over "humidity":
        while ((i < MAXLEN) && (*pt) && (*pt != ','))
        {
          Humidity[i] = *pt; // copy into Humidity string
          pt++;
          i++;
        }
      }
      Humidity[i++] = '%'; // Append '%' for percentage
      Humidity[i] = '\0';

      /* find weather in response */
      pt = strstr(Recvbuff, "\"description\"");
      i = 0;
      if (NULL != pt)
      {
        pt = pt + 15; // skip over "description":"
        while ((i < MAXLEN) && (*pt) && (*pt != '\"'))
        {
          Weather[i] = *pt; // copy into weather string
          pt++;
          i++;
        }
      }
      Weather[i] = 0;

      /* find main description in response */
      pt = strstr(Recvbuff, "\"main\"");
      i = 0;
      if (NULL != pt)
      {
        pt = pt + 8; // skip over "main":"
        while ((i < MAXLEN) && (*pt) && (*pt != '\"'))
        {
          mainDescription[i] = *pt; // copy into weather string
          pt++;
          i++;
        }
      }
      mainDescription[i] =  '\0';

      // Parse weather condition from the main description
// Update currentWeather variable based on OpenWeatherMap API main weather conditions
if (strcmp(mainDescription, "Clear") == 0)
{
  currentWeather = WEATHER_CLEAR;
}
else if (strcmp(mainDescription, "Clouds") == 0 || 
         strcmp(mainDescription, "Mist") == 0 ||
         strcmp(mainDescription, "Smoke") == 0 ||
         strcmp(mainDescription, "Haze") == 0 ||
         strcmp(mainDescription, "Dust") == 0 ||
         strcmp(mainDescription, "Fog") == 0 ||
         strcmp(mainDescription, "Sand") == 0 ||
         strcmp(mainDescription, "Ash") == 0 ||
         strcmp(mainDescription, "Squall") == 0 ||
         strcmp(mainDescription, "Tornado") == 0)
{
  currentWeather = WEATHER_CLOUDY;
}
else if (strcmp(mainDescription, "Rain") == 0 ||
         strcmp(mainDescription, "Drizzle") == 0 ||
         strcmp(mainDescription, "Snow") == 0 ||
         strcmp(mainDescription, "Thunderstorm") == 0)
{
  currentWeather = WEATHER_RAINY;
}

icon = currentWeather;


	 //print on UART for debugging
//Getting stuck here
	UART0_NextLine();
  UART_OutString("City: ");
  UART_OutString(City);
  UART0_NextLine(); //CR LF
  UART_OutString("Min Temp: ");
  UART_OutString(MinTemperature);
  UART0_NextLine();
  UART_OutString("Max Temp: ");
  UART_OutString(MaxTemperature);
  UART0_NextLine();
  UART_OutString("Current Temp: ");
  UART_OutString(Temperature);
  UART0_NextLine();
  UART_OutString("Humidity: ");
  UART_OutString(Humidity);
  UART0_NextLine();
  UART_OutString("Weather: ");
  UART_OutString(mainDescription);
  UART0_NextLine();
   // // update ST7735 LCD with weather information
    UpdateWeather(City, Temperature, MinTemperature, MaxTemperature, Humidity, mainDescription);
		updateIcon = true;

    }
  }
}





/*!
    \brief This function puts the device in its default state. It:
           - Set the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregister mDNS services

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static int32_t configureSimpleLinkToDefaultState(char *pConfig)
{
  SlVersionFull ver = {0};
  UINT8 val = 1;
  UINT8 configOpt = 0;
  UINT8 configLen = 0;
  UINT8 power = 0;

  INT32 retVal = -1;
  INT32 mode = -1;

  mode = sl_Start(0, pConfig, 0);

  /* If the device is not in station-mode, try putting it in station-mode */
  if (ROLE_STA != mode)
  {
    if (ROLE_AP == mode)
    {
      /* If the device is in AP mode, we need to wait for this event before doing anything */
      while (!IS_IP_AQUIRED(g_Status))
        ;
    }

    /* Switch to STA role and restart */
    retVal = sl_WlanSetMode(ROLE_STA);

    retVal = sl_Stop(0xFF);

    retVal = sl_Start(0, pConfig, 0);

    /* Check if the device is in station again */
    if (ROLE_STA != retVal)
    {
      /* We don't want to proceed if the device is not coming up in station-mode */
      return DEVICE_NOT_IN_STATION_MODE;
    }
  }
  /* Get the device's version-information */
  configOpt = SL_DEVICE_GENERAL_VERSION;
  configLen = sizeof(ver);
  retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));

  /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
  retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);

  /* Remove all profiles */
  retVal = sl_WlanProfileDel(0xFF);

  /*
   * Device in station-mode. Disconnect previous connection if any
   * The function returns 0 if 'Disconnected done', negative number if already disconnected
   * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
   */
  retVal = sl_WlanDisconnect();
  if (0 == retVal)
  {
    /* Wait */
    while (IS_CONNECTED(g_Status))
      ;
  }

  /* Enable DHCP client*/
  retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &val);

  /* Disable scan */
  configOpt = SL_SCAN_POLICY(0);
  retVal = sl_WlanPolicySet(SL_POLICY_SCAN, configOpt, NULL, 0);

  /* Set Tx power level for station mode
     Number between 0-15, as dB offset from max power - 0 will set maximum power */
  power = 0;
  retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);

  /* Set PM policy to normal */
  retVal = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);

  /* TBD - Unregister mDNS services */
  retVal = sl_NetAppMDNSUnRegisterService(0, 0);

  retVal = sl_Stop(0xFF);

  g_Status = 0;
  memset(&Recvbuff, 0, MAX_RECV_BUFF_SIZE);
  memset(&SendBuff, 0, MAX_SEND_BUFF_SIZE);
  memset(&HostName, 0, MAX_HOSTNAME_SIZE);
  DestinationIP = 0;
  ;
  SockID = 0;

  return retVal; /* Success */
}

/*
 * * ASYNCHRONOUS EVENT HANDLERS -- Start
 */

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
  switch (pWlanEvent->Event)
  {
  case SL_WLAN_CONNECT_EVENT:
  {
    SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

    /*
     * Information about the connected AP (like name, MAC etc) will be
     * available in 'sl_protocol_wlanConnectAsyncResponse_t' - Applications
     * can use it if required
     *
     * sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
     * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
     *
     */
  }
  break;

  case SL_WLAN_DISCONNECT_EVENT:
  {
    sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;

    CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
    CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

    pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

    /* If the user has initiated 'Disconnect' request, 'reason_code' is SL_USER_INITIATED_DISCONNECTION */
    if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
    {
      UART_OutString(" Device disconnected from the AP on application's request \r\n");
    }
    else
    {
      UART_OutString(" Device disconnected from the AP on an ERROR..!! \r\n");
    }
  }
  break;

  default:
  {
    UART_OutString(" [WLAN EVENT] Unexpected event \r\n");
  }
  break;
  }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
  switch (pNetAppEvent->Event)
  {
  case SL_NETAPP_IPV4_ACQUIRED:
  {

    SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);
    /*
     * Information about the connected AP's ip, gateway, DNS etc
     * will be available in 'SlIpV4AcquiredAsync_t' - Applications
     * can use it if required
     *
     * SlIpV4AcquiredAsync_t *pEventData = NULL;
     * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
     * <gateway_ip> = pEventData->gateway;
     *
     */
  }
  break;

  default:
  {
    UART_OutString(" [NETAPP EVENT] Unexpected event \r\n");
  }
  break;
  }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pServerEvent - Contains the relevant event information
    \param[in]      pServerResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
  /*
   * This application doesn't work with HTTP server - Hence these
   * events are not handled here
   */
  UART_OutString(" [HTTP EVENT] Unexpected event \r\n");
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
  /*
   * Most of the general errors are not FATAL are are to be handled
   * appropriately by the application
   */
  UART_OutString(" [GENERAL EVENT] \r\n");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
  switch (pSock->Event)
  {
  case SL_NETAPP_SOCKET_TX_FAILED:
  {
    /*
     * TX Failed
     *
     * Information about the socket descriptor and status will be
     * available in 'SlSockEventData_t' - Applications can use it if
     * required
     *
     * SlSockEventData_t *pEventData = NULL;
     * pEventData = & pSock->EventData;
     */
    switch (pSock->EventData.status)
    {
    case SL_ECLOSE:
      UART_OutString(" [SOCK EVENT] Close socket operation failed to transmit all queued packets\r\n");
      break;

    default:
      UART_OutString(" [SOCK EVENT] Unexpected event \r\n");
      break;
    }
  }
  break;

  default:
    UART_OutString(" [SOCK EVENT] Unexpected event \r\n");
    break;
  }
}
/*
 * * ASYNCHRONOUS EVENT HANDLERS -- End
 */
