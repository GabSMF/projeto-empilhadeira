#include "esp_camera.h"
#include <WiFi.h>

// ===========================
// WiFi credentials
// ===========================
const char *ssid = "Projeto";
const char *password = "2022-11-07";

void startCameraServer();
void setupLedFlash();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  
  camera_config_t config = {
    .pin_pwdn = -1, 
    .pin_reset = -1,
    .pin_xclk = 15, 
    .pin_sscb_sda = 4,
    .pin_sscb_scl = 5,
    .pin_d7 = 16, 
    .pin_d6 = 17,
    .pin_d5 = 18, 
    .pin_d4 = 12,
    .pin_d3 = 10, 
    .pin_d2 = 8,
    .pin_d1 = 9, 
    .pin_d0 = 11,
    .pin_vsync = 6, 
    .pin_href = 7,
    .pin_pclk = 13,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    // *** IMPORTANTE ***
    // GRAYSCALE, resolução baixa (QVGA)
    .pixel_format = PIXFORMAT_GRAYSCALE,
    .frame_size = FRAMESIZE_HQVGA,

    // Só funciona com JPEG, então deixado comentado
    // .jpeg_quality = 10,
    
    .fb_count = 2,
    .grab_mode = CAMERA_GRAB_LATEST
  };
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // ==========================
  // Ajustes do sensor
  // ==========================
  sensor_t *s = esp_camera_sensor_get();

  // Contraste (-2 a +2)
  s->set_contrast(s, -2);  // 2 = contraste alto

  // Brilho (-2 a +2)
  s->set_brightness(s, 0); // leve aumento de brilho

  // Saturação (-2 a +2) — sem efeito em grayscale, mas seguro
  s->set_saturation(s, -2);

  // Special Effect (0=none, 2=grayscale)
  // Você já está usando grayscale nativo, então deixar none
  s->set_special_effect(s, 0);

  // Obs: s->set_quality() só funciona com JPEG
  // Como você está usando GRAYSCALE, jpeg_quality não é usado.

  // ==========================
  // WiFi
  // ==========================
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  delay(10000);
}
