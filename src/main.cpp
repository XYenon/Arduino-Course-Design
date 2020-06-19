#include <Arduino.h>
#include <SimpleDHT.h>

const int PIN_DHT11 = 2;
const int PIN_ALERT_KEY = 8;
SimpleDHT11 dht11(PIN_DHT11);

void exec_command();
void read_alert_key();

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_ALERT_KEY, INPUT_PULLUP);
}

void loop()
{
  read_alert_key();
  exec_command();

  // delay(1000);
}

void read_dht11()
{
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("err ");
    Serial.print(err);
    Serial.println("|");
    return;
  }

  Serial.print("DHT ");
  Serial.print((int)temperature);
  Serial.print(" ");
  Serial.print((int)humidity);
  Serial.println("|");
}

bool read_key(int pin_key)
{
  if (digitalRead(pin_key) == 0)
  {
    delay(10);
    if (digitalRead(pin_key) == 0)
    {
      return true;
    }
  }
  return false;
}

String read_serial()
{
  String com_data = "";
  while (Serial.available() > 0 && !com_data.endsWith("|"))
  {
    com_data += char(Serial.read());
    delay(2);
  }
  if (com_data.length() <= 0)
  {
    return com_data;
  }
  if (!com_data.endsWith("|"))
  {
    Serial.print("err ");
    Serial.print("broken stream");
    Serial.println("|");
    return "";
  }
  com_data.trim();
  com_data.remove(com_data.length() - 1);
  com_data.trim();
  return com_data;
}

void exec_command()
{
  String com_data = read_serial();
  if (com_data.length() <= 0)
  {
    return;
  }
  if (com_data == "DHT")
  {
    read_dht11();
  }
  else if (com_data == "warning")
  {
    // TODO warning
  }
}

void read_alert_key()
{
  bool key_down = false;
  if (read_key(PIN_ALERT_KEY))
  {
    key_down = true;
    // TODO alert
    while (key_down)
    {
      delay(100);
    }
    key_down = false;
  }
}
