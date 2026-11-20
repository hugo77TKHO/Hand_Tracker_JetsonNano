#include <WiFi.h>
#include <WiFiUdp.h>

// --- CONFIGURATION WI-FI ---
const char* ssid = "LYEX";
const char* password = "LYEX2026";
unsigned int localPort = 4210;
WiFiUDP udp;
char packetBuffer[255]; 

// --- CONFIGURATION 9 LEDS ---
// Les 9 broches choisies pour les LEDs (Broches sûres sur l'ESP32)
const int ledPins[9] = {13, 15, 21, 25, 26, 32, 33, 22, 23};

// --- CONFIGURATION AFFICHEUR 7 SEGMENTS ---
const int segA = 12;
const int segB = 2;
const int segC = 4;
const int segD = 18;
const int segE = 19;
const int segF = 14;
const int segG = 27;

// Tableau pour allumer les bons segments
const byte chiffres[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

void setup() {
  Serial.begin(115200);
  
  // Initialisation des 9 LEDs
  for(int i = 0; i < 9; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Éteint tout au démarrage
  }
  
  // Initialisation de l'afficheur
  int pins[] = {segA, segB, segC, segD, segE, segF, segG};
  for(int i = 0; i < 7; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW); 
  }

  // Connexion Wi-Fi
  Serial.print("Connexion au Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté !");
  Serial.print("Adresse IP de l'ESP32 : ");
  Serial.println(WiFi.localIP()); 

  // Démarrage du serveur UDP
  udp.begin(localPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0; 
    }
    
    String message = String(packetBuffer);
    
    // Ordre de comptage (Afficheur + LEDs)
    if (message.startsWith("C:")) {
      String countStr = message.substring(2);
      int count = countStr.toInt();
      if (count >= 0 && count <= 9) {
        afficherChiffre(count);
        allumerLeds(count);
      }
    }
  }
}

// Fonction pour allumer les segments
void afficherChiffre(int chiffre) {
  int pins[] = {segA, segB, segC, segD, segE, segF, segG};
  for(int i = 0; i < 7; i++) {
    digitalWrite(pins[i], chiffres[chiffre][i]);
  }
}

// Fonction pour allumer le bon nombre de LEDs
void allumerLeds(int nombre) {
  for(int i = 0; i < 9; i++) {
    if (i < nombre) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
}