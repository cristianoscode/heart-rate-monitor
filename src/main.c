#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"

#define LED_IR 2
#define LDR_ADC 26
#define LED_STATUS 15
#define BLUETOOTH_TX 0

#define SAMPLE_RATE 100
#define BUFFER_SIZE 100

uint16_t signal_buffer[BUFFER_SIZE];
uint8_t index = 0;

void init_hardware() {
  stdio_init_all();
  gpio_init(LED_IR);
  gpio_set_dir(LED_IR, GPIO_OUT);
  gpio_init(LED_STATUS);
  gpio_set_dir(LED_STATUS, GPIO_OUT);
  adc_init();
  adc_gpio_init(LDR_ADC);
  adc_select_input(0);
}

uint16_t read_ldr() {
  return adc_read();
}

void filter_signal(uint16_t *buffer) {
  // filtro passa-baixa
  for (int i = 1; i < BUFFER_SIZE; i++) {
    buffer[i] = (buffer[i - 1] + buffer[i]) / 2;
  }
}

uint8_t detect_peaks(uint16_t *buffer) {
  uint8_t peak_count = 0;
  for (int i = 1; i < BUFFER_SIZE - 1; i++) {
    if (buffer[i] > buffer[i - 1] && buffer[i] > buffer[i + 1]) {
      peak_count++;
    }
  }
  return peak_count;
}

uint16_t calculate_bpm(uint8_t peaks) {
  return peaks * (60 / (BUFFER_SIZE / SAMPLE_RATE));
}

void send_bluetooth(uint16_t bpm) {
  // simulando envio via Bluetooth
  printf("BPM: %d\n", bpm);
}

int main() {
  init_hardware();
  while (1) {
    gpio_put(LED_IR, 1);
    sleep_ms(10);
    signal_buffer[index] = read_ldr();
    gpio_put(LED_IR, 0);

    if (++index >= BUFFER_SIZE) {
      filter_signal(signal_buffer);
      uint8_t peaks = detect_peaks(signal_buffer);
      uint16_t bpm = calculate_bpm(peaks);

      if (bpm > 0) {
        gpio_put(LED_STATUS, 1);
        sleep_ms(100);
        gpio_put(LED_STATUS, 0);
        send_bluetooth(bpm);
      }
      index = 0;
    }
    sleep_ms(1000 / SAMPLE_RATE);
  }
}
