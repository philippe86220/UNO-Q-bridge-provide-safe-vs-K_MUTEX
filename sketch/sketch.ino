#include <Arduino.h>
#include <Arduino_RouterBridge.h>
#include <zephyr/kernel.h>

volatile int counter = 0;
volatile int doubleValue = 0;

K_MUTEX_DEFINE(data_mutex);
K_MUTEX_DEFINE(monitor_mutex);

void updateCounter(int x) {
  k_mutex_lock(&data_mutex, K_FOREVER);

  counter = x;
  delay(5);
  doubleValue = x * 2;

  k_mutex_unlock(&data_mutex);

  k_mutex_lock(&monitor_mutex, K_FOREVER);
  Monitor.print("RPC update: counter=");
  Monitor.print(counter);
  Monitor.print(" double=");
  Monitor.println(doubleValue);
  k_mutex_unlock(&monitor_mutex);
}

void setup() {
  Bridge.begin();
  Monitor.begin();

  Bridge.provide("update", updateCounter);
}

void loop() {
  int c;
  int d;

  k_mutex_lock(&data_mutex, K_FOREVER);
  c = counter;
  d = doubleValue;
  k_mutex_unlock(&data_mutex);

  if (d != c * 2) {
    k_mutex_lock(&monitor_mutex, K_FOREVER);
    Monitor.print("ERROR inconsistent state: counter=");
    Monitor.print(c);
    Monitor.print(" double=");
    Monitor.println(d);
    k_mutex_unlock(&monitor_mutex);
  }

  delay(100);
}
