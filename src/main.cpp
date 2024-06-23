#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

const char* ssid = "MERCUSYS_22E9";
const char* password = "Kausaki@23";
const char* googleScriptURL = "YOUR_GOOGLE_SCRIPT_URL"; // Replace with your Google Script URL

#define RGB_BUILTIN 21
#define RGB_BRIGHTNESS 255

AsyncWebServer server(80);

void blinkLED() {
  neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS); // Turn the LED on
  delay(2000); // Wait for 2 seconds
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Turn the LED off
}

void sendToGoogleSheet(const String& ipAddress, const String& name, const String& wishDate, const String& wishItem, const String& wishWhen, const String& remarks) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(googleScriptURL);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "ipAddress=" + ipAddress + "&name=" + name + "&wishDate=" + wishDate + "&wishItem=" + wishItem + "&wishWhen=" + wishWhen + "&remarks=" + remarks;

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize the LED pin
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Turn the LED off
  
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
        <title>Wish Submission</title>
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
          textarea {
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
        <h1>Submit Your Wish</h1>
        <input type="text" id="name" placeholder="Enter your name">
        <input type="date" id="wishDate" placeholder="Date of wish">
        <input type="text" id="wishItem" placeholder="What do you wish for?">
        <input type="text" id="wishWhen" placeholder="When do you want it?">
        <textarea id="remarks" placeholder="Any remarks"></textarea>
        <button onclick="submitWish()">Submit</button>
        <div class="response" id="response"></div>

        <script>
          function submitWish() {
            const name = document.getElementById('name').value;
            const wishDate = document.getElementById('wishDate').value;
            const wishItem = document.getElementById('wishItem').value;
            const wishWhen = document.getElementById('wishWhen').value;
            const remarks = document.getElementById('remarks').value;
            fetch(`/submit?name=${name}&wishDate=${wishDate}&wishItem=${wishItem}&wishWhen=${wishWhen}&remarks=${remarks}`)
              .then(response => response.text())
              .then(data => {
                const responseDiv = document.getElementById('response');
                responseDiv.innerHTML = `<p>${data}</p>`;
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
  server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("name") && request->hasParam("wishDate") && request->hasParam("wishItem") && request->hasParam("wishWhen") && request->hasParam("remarks")) {
      String name = request->getParam("name")->value();
      String wishDate = request->getParam("wishDate")->value();
      String wishItem = request->getParam("wishItem")->value();
      String wishWhen = request->getParam("wishWhen")->value();
      String remarks = request->getParam("remarks")->value();

      String ipAddress = request->client()->remoteIP().toString();

      // Send data to Google Sheets
      sendToGoogleSheet(ipAddress, name, wishDate, wishItem, wishWhen, remarks);

      request->send(200, "text/plain", "Wish Submitted Successfully");

      // Blink the LED on successful response
      blinkLED();
    } else {
      request->send(400, "text/plain", "Bad Request: Missing required parameters");
    }
  });

  // Start the server
  server.begin();
}

void loop() {
  // Nothing to do here
}
