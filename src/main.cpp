#include <Arduino.h>
#include <MsTimer2.h>
#include <SimpleDHT.h>

const int PIN_DHT11 = 2;
const int PIN_ALERT_KEY = 8;
const int PIN_SOUNDER = 9;

const int PIN_GP2D12 = 0;

SimpleDHT11 dht11(PIN_DHT11);

void exec_command();
void read_alert_key();

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_ALERT_KEY, INPUT_PULLUP);
  pinMode(PIN_SOUNDER, OUTPUT);
}

void loop()
{
  read_alert_key();
  exec_command();
}

void stop_sounder()
{
  analogWrite(PIN_SOUNDER, 0);
  MsTimer2::stop();
}

void alert()
{
  analogWrite(PIN_SOUNDER, 64);
  MsTimer2::set(1000, stop_sounder);
  MsTimer2::start();
}

void warning()
{
  analogWrite(PIN_SOUNDER, 128);
  MsTimer2::set(3000, stop_sounder);
  MsTimer2::start();
}

template <typename T>
void write_error(const T &err)
{
  Serial.print("err ");
  Serial.print(err);
  Serial.println("|");
}

template <typename T>
void write_params(const T &t)
{
  Serial.print(" ");
  Serial.print(t);
}

template <typename T, typename... Params>
void write_params(const T &t, const Params &... params)
{
  Serial.print(" ");
  Serial.print(t);
  write_params(params...);
}

template <typename Command>
void write_command(const Command &command)
{
  Serial.print(command);
  Serial.println("|");
}

template <typename Command, typename... Params>
void write_command(const Command &command, const Params &... params)
{
  Serial.print(command);
  write_params(params...);
  Serial.println("|");
}

void read_dht11()
{
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    write_error("DHT11 " + String(err));
    return;
  }
  write_command("DHT", (int)temperature, "C", (int)humidity, "%");
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
    write_error("broken stream");
    return "";
  }
  com_data.trim();
  com_data.remove(com_data.length() - 1);
  com_data.trim();
  return com_data;
}

void read_distance()
{
  int val = analogRead(PIN_GP2D12);
  float distance = 2547.8 / ((float)val * 0.49 - 10.41) - 0.42;
  if (distance > 80 || distance < 10)
  {
    write_error("GP2D12 over range");
  }
  distance = int(distance * 10) / 10.0;
  write_command("distance", distance, "cm");
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
    warning();
  }
  else if (com_data == "distance")
  {
    read_distance();
  }
  else
  {
    write_error("invalid command");
  }
}

void read_alert_key()
{
  if (read_key(PIN_ALERT_KEY))
  {
    write_command("alert");
    alert();
    while (read_key(PIN_ALERT_KEY))
    {
      delay(100);
    }
  }
}
