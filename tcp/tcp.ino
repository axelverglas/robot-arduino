#include <ESP8266WiFi.h>

//#define STASSID "Pixel_1960"
//#define STAPSK "88nut6skx8ah8-d"

//#define STASSID "arthur"
//#define STAPSK "1234azerty"

#define STASSID "dcrobot"
#define STAPSK "dcrobot24"

//#define STASSID "DCGuest"
//#define STAPSK "St33veJobs!"

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "10.43.122.49"; // Remplacez par l'IP de votre serveur
const int port = 8080;

WiFiClient client;

const int moteurGauche1Pin = D1;  // Moteur gauche 1
const int moteurGauche2Pin = D7;  // Moteur gauche 2
const int moteurDroit1Pin = D0;   // Moteur droit 1
const int moteurDroit2Pin = D5;   // Moteur droit 2
const int ledD6Pin = D6;          // LED D6

int coef_gauche = 120; // initial value, maximum of the range 0-255
int coef_droit = 110;  // initial value, maximum of the range 0-255

enum State { IDLE, MOVE_FORWARD, MOVE_BACKWARD, TURN_LEFT, TURN_RIGHT };
State currentState = IDLE;
unsigned long stateStartTime = 0;
unsigned long stateDuration = 0;

void setup() {
  pinMode(moteurGauche1Pin, OUTPUT);
  pinMode(moteurGauche2Pin, OUTPUT);
  pinMode(moteurDroit1Pin, OUTPUT);
  pinMode(moteurDroit2Pin, OUTPUT);
  pinMode(ledD6Pin, OUTPUT); // Set D6 as output
  
  digitalWrite(ledD6Pin, HIGH);

  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to the server
  Serial.print("Connecting to server at ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(port);

}

void loop() {
  if (client.connected()) {
    // Read data from server
    if (client.available() >= 6) {
      String command = client.readStringUntil('\0');
      //command.trim();  // Remove any extraneous whitespace
      Serial.println("Received command: " + command);
      client.print(command);

      if (command.length() >= 4) {
        String order = command.substring(0, 3);
        int parValue = command.substring(4).toInt();  // Convert parameter to integer
        
        if (order == "MVT") {
          allumer();
       
        } else if (order == "STO") {
          eteindre();
        } else if (order == "AVA") {
          startMoveForward((parValue + 1) * 300); // 1 metre en 3 secondes
        } else if (order == "REC") {
          startMoveBackward((parValue + 1) * 300); // 1 metre en 3 secondes
        } else if (order == "GAU") {
          startTurnLeft(parValue); // Quarter or half turn based on parValue
        } else if (order == "DRO") {
          startTurnRight(parValue); // Quarter or half turn based on parValue
        }

        // Echo the command back to the server
       
      }
    }
  } else {
    Serial.println("Disconnected from server, retrying...");
    // Reconnect to the server
    if (client.connect(host, port)) {
      Serial.println("Reconnected to server");
    }
    delay(100); // Wait before retrying
  }

  // Check if the current state action is complete
  if (currentState != IDLE && millis() - stateStartTime >= stateDuration) {
    completeAction();
  }
}

void startMoveForward(unsigned long duration) {
  currentState = MOVE_FORWARD;
  stateStartTime = millis();
  stateDuration = duration;
  Serial.println("Start moving forward");
  analogWrite(moteurGauche1Pin, 0);
  applyCoef(moteurGauche2Pin, coef_gauche);
  applyCoef(moteurDroit1Pin, coef_droit);
  analogWrite(moteurDroit2Pin, 0);
}

void startMoveBackward(unsigned long duration) {
  currentState = MOVE_BACKWARD;
  stateStartTime = millis();
  stateDuration = duration;
  Serial.println("Start moving backward");
  applyCoef(moteurGauche1Pin, coef_gauche);
  analogWrite(moteurGauche2Pin, 0);
  analogWrite(moteurDroit1Pin, 0);
  applyCoef(moteurDroit2Pin, coef_droit);
}

void startTurnLeft(int parValue) {
  currentState = TURN_LEFT;
  stateStartTime = millis();
  if (parValue == 1) {
    stateDuration = 300; // Quarter turn duration in milliseconds
    Serial.println("Start turning left for a quarter turn");
  } else if (parValue == 2) {
    stateDuration = 540; // Half turn duration in milliseconds
    Serial.println("Start turning left for a half turn");
  }
  applyCoef(moteurGauche1Pin, 245);
  analogWrite(moteurGauche2Pin, 0);
  applyCoef(moteurDroit1Pin, 235);
  analogWrite(moteurDroit2Pin, 0);
}

void startTurnRight(int parValue) {
  currentState = TURN_RIGHT;
  stateStartTime = millis();
  if (parValue == 1) {
    stateDuration = 300; // Quarter turn duration in milliseconds
    Serial.println("Start turning right for a quarter turn");
  } else if (parValue == 2) {
    stateDuration = 540; // Half turn duration in milliseconds
    Serial.println("Start turning right for a half turn");
  }
  analogWrite(moteurGauche1Pin, 0);
  applyCoef(moteurGauche2Pin, 245);
  analogWrite(moteurDroit1Pin, 0);
  applyCoef(moteurDroit2Pin, 235);
}

void completeAction() {
  eteindre();
  currentState = IDLE;
  client.println("FIN-");
  Serial.println("Action completed, sent FIN-");
}

void allumer() {
  Serial.println("Allumer");
  digitalWrite(moteurGauche1Pin, HIGH);
  digitalWrite(moteurGauche2Pin, HIGH);
  digitalWrite(moteurDroit1Pin, HIGH);
  digitalWrite(moteurDroit2Pin, HIGH);
}

void eteindre() {
  Serial.println("Éteindre");
  digitalWrite(moteurGauche1Pin, LOW);
  digitalWrite(moteurGauche2Pin, LOW);
  digitalWrite(moteurDroit1Pin, LOW);
  digitalWrite(moteurDroit2Pin, LOW);
}

void applyCoef(int pin, int coef) {
  analogWrite(pin, coef);
}