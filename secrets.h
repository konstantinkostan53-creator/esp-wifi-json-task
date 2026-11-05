#include <ESP8266WiFi.h>        // Если используете ESP32, замените на <WiFi.h>
#include <ESP8266HTTPClient.h>  // Если используете ESP32, замените на <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>        // Убедитесь, что библиотека установлена в Arduino IDE
#include "secrets.h"            // Подключаем наш секретный файл с SSID и паролем

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1. Подключение к Wi-Fi
  Serial.println();
  Serial.print("Подключение к Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Проверка ошибки подключения по таймауту
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 40) { // Если не подключились за ~20 секунд
      Serial.println("\n[ОШИБКА] Не удалось подключиться к Wi-Fi! Проверьте SSID и пароль.");
      return;
    }
  }

  Serial.println("\n[УСПЕХ] Wi-Fi успешно подключен!");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());

  // 2. Инициализация HTTPS-клиента
  WiFiClientSecure client;
  client.setInsecure(); // Пропускаем строгую проверку SSL-сертификата (для тестов)

  HTTPClient https;

  Serial.print("Отправка HTTPS GET-запроса по адресу: ");
  Serial.println(jsonUrl);

  // 3. Выполнение HTTPS-запроса и обработка ошибок
  if (https.begin(client, jsonUrl)) {
    int httpCode = https.GET(); // Выполняем GET-запрос

    if (httpCode > 0) {
      Serial.printf("[HTTPS] Код ответа сервера: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = https.getString(); // Читаем тело ответа (JSON)
        
        // 4. Парсинг JSON-данных
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("[ОШИБКА] Не удалось разобрать JSON: ");
          Serial.println(error.c_str());
        } else {
          // Замените "value" на имя нужного ключа из вашего JSON (например, "temperature", "status" и т.д.)
          const char* targetValue = doc["value"]; 
          
          Serial.println("----------------------------------------");
          Serial.print("Полученное значение из JSON: ");
          Serial.println(targetValue);
          Serial.println("----------------------------------------");
        }
      } else {
        Serial.printf("[ОШИБКА] Сервер вернул код ошибки: %d\n", httpCode);
      }
    } else {
      Serial.printf("[ОШИБКА] Не удалось выполнить GET-запрос: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end(); // Закрываем HTTPS-соединение
  } else {
    Serial.println("[ОШИБКА] Не удалось установить HTTPS-соединение с сервером.");
  }
}

void loop() {
  // Код выполняется один раз в setup(), поэтому loop пустой
}
