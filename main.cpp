#include <DHT11.h>
#include <WiFi.h>

int ledred = 26; 
int ledyellow = 25;
int ledgreen = 33;
int pin_umidade_solo = 34;
int modo = 0;
int temperatura = 0;
int umidade_ar = 0;

const char* ssid = "XXXXXX";
const char* password = "XXXXXXXX";
int temp = 2;
WiFiServer server(80);
DHT11 dht11(2);

void setup() {
  Serial.begin(9600);
  pinMode(temp, INPUT);
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(ledyellow, OUTPUT);
  pinMode(pin_umidade_solo, INPUT);

  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectada.");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  int umidade_solo = map(analogRead(pin_umidade_solo), 4095,700,0,100) ;
  int result = dht11.readTemperatureHumidity(temperatura, umidade_ar);

  if (umidade_solo <= 40 || umidade_solo >= 90 || temperatura <= 10 || temperatura >= 35) {
    digitalWrite(ledred, HIGH);
    digitalWrite(ledyellow, LOW);
    digitalWrite(ledgreen, LOW);
    modo = 0;
  } else {
    if (umidade_solo < 65 || umidade_solo > 75 || temperatura < 18 || temperatura > 30) {
      digitalWrite(ledred, LOW);
      digitalWrite(ledyellow, HIGH);
      digitalWrite(ledgreen, LOW);
      modo = 1;
    } else {
      if (umidade_solo >= 65 || umidade_solo <= 75 || temperatura >= 18 || temperatura <= 30) {
        digitalWrite(ledred, LOW);
        digitalWrite(ledyellow, LOW);
        digitalWrite(ledgreen, HIGH);
        modo = 2; 	                              
      }
    }
  }

  Serial.print("  Temperatura: ");
  Serial.println(temperatura);
  Serial.print("Umidade Ar: ");
  Serial.println(umidade_ar);
  Serial.print("Umidade solo: ");
  Serial.println(umidade_solo);
  Serial.print("  Modo: ");
  Serial.println(modo);
  delay(100);

  WiFiClient client = server.available();
  if (client) {
    Serial.println("Novo cliente.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type: application/json");
            client.println();

            
            String jsonResponse = "{";
            jsonResponse += "\"temperatura\": " + String(temperatura) + ",";
            jsonResponse += "\"umidade_do_ar\": " + String(umidade_ar) + ",";
            jsonResponse += "\"umidade_do_solo\": " + String(umidade_solo) + ",";
            jsonResponse += "\"modo\": " + String(modo);
            jsonResponse += "}";

            
            client.println(jsonResponse);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}

