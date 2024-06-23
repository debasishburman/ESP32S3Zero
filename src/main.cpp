#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

const char* ssid = "MERCUSYS_22E9";
const char* password = "Kausaki@23";

#ifdef RGB_BUILTIN
#undef RGB_BUILTIN
#endif
#define RGB_BUILTIN 21

AsyncWebServer server(80);

void blinkLED() {
  neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,RGB_BRIGHTNESS,RGB_BRIGHTNESS); // Turn the LED on
  delay(2000); // Wait for 2 seconds
  neopixelWrite(RGB_BUILTIN,0,0,0); // Turn the LED off
}

void setup() {
  Serial.begin(115200);

  // Initialize the LED pin
  neopixelWrite(RGB_BUILTIN,0,0,0); // Turn the LED off
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <title>Number Converter</title>
        <style>
          body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            background-color: #f0f8ff;
            margin: 0;
          }
          h1 {
            color: #4682b4;
          }
          input {
            padding: 10px;
            font-size: 16px;
            margin: 5px;
          }
          button {
            padding: 10px 20px;
            font-size: 16px;
            color: white;
            background-color: #4682b4;
            border: none;
            cursor: pointer;
            margin: 5px;
          }
          button:hover {
            background-color: #5a9bd4;
          }
          .response {
            margin-top: 20px;
            font-size: 18px;
            color: #333;
          }
        </style>
      </head>
      <body>
        <h1>Number Converter</h1>
        <input type="number" id="decimalInput" placeholder="Enter a decimal number">
        <button onclick="convertDecimalToBinary()">Convert to Binary</button>
        <input type="text" id="binaryInput" placeholder="Enter a binary number">
        <button onclick="convertBinaryToDecimal()">Convert to Decimal</button>
        <div class="response" id="response"></div>

        <script>
          function convertDecimalToBinary() {
            const decimal = document.getElementById('decimalInput').value;
            fetch(`/convert?type=decimalToBinary&number=${decimal}`)
              .then(response => response.json())
              .then(data => {
                const responseDiv = document.getElementById('response');
                if (data.error) {
                  responseDiv.innerHTML = `<p style="color: red;">${data.message}</p>`;
                } else {
                  responseDiv.innerHTML = `<p>Decimal: ${data.decimal}</p><p>Binary: ${data.binary}</p>`;
                }
              })
              .catch(error => {
                console.error('Error:', error);
              });
          }

          function convertBinaryToDecimal() {
            const binary = document.getElementById('binaryInput').value;
            fetch(`/convert?type=binaryToDecimal&number=${binary}`)
              .then(response => response.json())
              .then(data => {
                const responseDiv = document.getElementById('response');
                if (data.error) {
                  responseDiv.innerHTML = `<p style="color: red;">${data.message}</p>`;
                } else {
                  responseDiv.innerHTML = `<p>Binary: ${data.binary}</p><p>Decimal: ${data.decimal}</p>`;
                }
              })
              .catch(error => {
                console.error('Error:', error);
              });
          }
        </script>
      </body>
      </html>
    )rawliteral");
  });

  // Define the REST API endpoint
  server.on("/convert", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("type") && request->hasParam("number")) {
      String type = request->getParam("type")->value();
      String numberStr = request->getParam("number")->value();
      DynamicJsonDocument doc(200);

      if (type == "decimalToBinary") {
        int decimal = numberStr.toInt();
        String binaryStr = String(decimal, BIN);
        doc["decimal"] = decimal;
        doc["binary"] = binaryStr;
      } else if (type == "binaryToDecimal") {
        int decimal = strtol(numberStr.c_str(), NULL, 2);
        doc["binary"] = numberStr;
        doc["decimal"] = decimal;
      } else {
        doc["error"] = "Bad Request";
        doc["message"] = "Invalid conversion type";
        String response;
        serializeJson(doc, response);
        request->send(400, "application/json", response);
        return;
      }

      String response;
      serializeJson(doc, response);
      request->send(200, "application/json", response);
      
      // Blink the LED on successful response
      blinkLED();
    } else {
      DynamicJsonDocument doc(200);
      doc["error"] = "Bad Request";
      doc["message"] = "Parameters 'type' and 'number' are missing";

      String response;
      serializeJson(doc, response);
      request->send(400, "application/json", response);
    }
  });

  // Start the server
  server.begin();
}

void loop() {
  // Nothing to do here
}
