#include <SPI.h>
#include <RF24.h>
#include "Data_Stuct.h"

#define CE_PIN 4
#define CSN_PIN 5

// =========nRF24=====================
enum PipeID { REMODE = 0, PLANE = 1 };
const byte pipeAddr[][6] = { "REM01" , "PLN01" };
RF24 radio_Sent(CE_PIN, CSN_PIN);
// ===================================

void setup()
{
  

}

void loop()
{

}

