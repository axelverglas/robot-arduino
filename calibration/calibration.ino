#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define STASSID "arthur"
#define STAPSK "1234azerty"

//#define STASSID "Pixel_1960"
//#define STAPSK "88nut6skx8ah8-d"

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int moteurGauche1Pin = D1;  // Moteur gauche 1
const int moteurGauche2Pin = D7;  // Moteur gauche 2
const int moteurDroit1Pin = D0;   // Moteur droit 1
const int moteurDroit2Pin = D5;   // Moteur droit 2
const int ledD6Pin = D6;          // LED D6

int coef_gauche = 125; // initial value, middle of the range 0-255
int coef_droit = 125;  // initial value, middle of the range 0-255

String generateHTML() {
  return "<!DOCTYPE html>\
<html>\
<head>\
  <title>Motor Control</title>\
  <style>\
    body { font-family: Arial, sans-serif; text-align: center; }\
    .button {\
      display: inline-block;\
      padding: 10px 20px;\
      font-size: 20px;\
      margin: 5px;\
      cursor: pointer;\
      background-color: #4CAF50;\
      color: white;\
      border: none;\
      border-radius: 5px;\
    }\
    .button:hover { background-color: #45a049; }\
    .button-off {\
      background-color: #f44336;\
    }\
    .button-off:hover { background-color: #e53935; }\
  </style>\
</head>\
<body>\
  <h1>Motor Control Interface</h1>\
  <h2>Left Motor (Moteur Gauche 1 & 2)</h2>\
  <p>Current coefficient: <span id=\"coef_gauche\">" + String(coef_gauche) + "</span></p>\
  <button class=\"button\" onclick=\"location.href='/increase_gauche'\">Increase Coefficient</button>\
  <button class=\"button\" onclick=\"location.href='/decrease_gauche'\">Decrease Coefficient</button>\
  <h2>Right Motor (Moteur Droit 1 & 2)</h2>\
  <p>Current coefficient: <span id=\"coef_droit\">" + String(coef_droit) + "</span></p>\
  <button class=\"button\" onclick=\"location.href='/increase_droit'\">Increase Coefficient</button>\
  <button class=\"button\" onclick=\"location.href='/decrease_droit'\">Decrease Coefficient</button>\
  <br><br>\
  <button class=\"button\" onclick=\"location.href='/move_forward'\">Move Forward</button>\
  <button class=\"button\" onclick=\"location.href='/move_backward'\">Move Backward</button>\
  <button class=\"button\" onclick=\"location.href='/turn_left'\">Turn Left</button>\
  <button class=\"button\" onclick=\"location.href='/turn_right'\">Turn Right</button>\
  <br><br>\
  <button class=\"button\" onclick=\"location.href='/move_forward_1m'\">Move Forward 1m</button>\
  <button class=\"button\" onclick=\"location.href='/move_backward_1m'\">Move Backward 1m</button>\
  <button class=\"button\" onclick=\"location.href='/quarter_turn_left'\">Quarter Turn Left</button>\
  <button class=\"button\" onclick=\"location.href='/quarter_turn_right'\">Quarter Turn Right</button>\
  <br><br>\
  <button class=\"button\" onclick=\"location.href='/allumer'\">Allumer</button>\
  <button class=\"button button-off\" onclick=\"location.href='/eteindre'\">Stop</button>\
</body>\
</html>";
}

void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void increaseCoef(int &coef) {
  coef += 5;
  if (coef > 255) coef = 255;
}

void decreaseCoef(int &coef) {
  coef -= 5;
  if (coef < 0) coef = 0;
}

void applyCoef(int pin, int coef) {
  analogWrite(pin, coef);
}

void increaseGauche() {
  increaseCoef(coef_gauche);
  Serial.println("Left Motor Coefficient increased to " + String(coef_gauche));
  handleRoot();
}

void decreaseGauche() {
  decreaseCoef(coef_gauche);
  Serial.println("Left Motor Coefficient decreased to " + String(coef_gauche));
  handleRoot();
}

void increaseDroit() {
  increaseCoef(coef_droit);
  Serial.println("Right Motor Coefficient increased to " + String(coef_droit));
  handleRoot();
}

void decreaseDroit() {
  decreaseCoef(coef_droit);
  Serial.println("Right Motor Coefficient decreased to " + String(coef_droit));
  handleRoot();
}

void moveForward() {
  Serial.println("Avance");
  analogWrite(moteurGauche1Pin, 0);
  applyCoef(moteurGauche2Pin, coef_gauche);
  applyCoef(moteurDroit1Pin, coef_droit);
  analogWrite(moteurDroit2Pin, 0);
  handleRoot();
}

void moveBackward() {
  Serial.println("Recule");
  applyCoef(moteurGauche1Pin, coef_gauche);
  analogWrite(moteurGauche2Pin, 0);
  analogWrite(moteurDroit1Pin, 0);
  applyCoef(moteurDroit2Pin, coef_droit);
  handleRoot();
}

void turnLeft() {
  Serial.println("Tourne à gauche");
  applyCoef(moteurGauche1Pin, coef_gauche);
  analogWrite(moteurGauche2Pin, 0);
  applyCoef(moteurDroit1Pin, coef_droit);
  analogWrite(moteurDroit2Pin, 0);
  handleRoot();
}

void turnRight() {
  Serial.println("Tourne à droite");
  analogWrite(moteurGauche1Pin, 0);
  applyCoef(moteurGauche2Pin, coef_gauche);
  analogWrite(moteurDroit1Pin, 0);
  applyCoef(moteurDroit2Pin, coef_droit);
  handleRoot();
}

void moveForward1m() {
  Serial.println("Avance de 1 mètre en 3 secondes");
  analogWrite(moteurGauche1Pin, 0);
  analogWrite(moteurDroit2Pin, 0);
  applyCoef(moteurGauche2Pin, 120); // 1/3 of 255
  applyCoef(moteurDroit1Pin, 110);  // 1/3 of 255
  delay(3000);
  eteindre();
  handleRoot();
}

void moveBackward1m() {
  Serial.println("Recule de 1 mètre en 3 secondes");
  analogWrite(moteurGauche2Pin, 0);
  analogWrite(moteurDroit1Pin, 0);
  applyCoef(moteurGauche1Pin, 120); // 1/3 of 255
  applyCoef(moteurDroit2Pin, 110);  // 1/3 of 255
  delay(3000);
  eteindre();
  handleRoot();
}

void quarterTurnLeft() {
  Serial.println("1/4 de tour à gauche");
  applyCoef(moteurGauche1Pin, 255);
  analogWrite(moteurGauche2Pin, 0);
  applyCoef(moteurDroit1Pin, 255);
  analogWrite(moteurDroit2Pin, 0);
  delay(260);
  eteindre();
  handleRoot();
}

void quarterTurnRight() {
  Serial.println("1/4 de tour à droite");
  analogWrite(moteurGauche1Pin, 0);
  applyCoef(moteurGauche2Pin, 255);
  applyCoef(moteurDroit1Pin, 0);
  analogWrite(moteurDroit2Pin, 255);
  delay(260);
  eteindre();
  handleRoot();
}

void allumer() {
  Serial.println("Allumer");
  digitalWrite(moteurGauche1Pin, HIGH);
  digitalWrite(moteurGauche2Pin, HIGH);
  digitalWrite(moteurDroit1Pin, HIGH);
  digitalWrite(moteurDroit2Pin, HIGH);
  handleRoot();
}

void eteindre() {
  Serial.println("Éteindre");
  analogWrite(moteurGauche1Pin, 0);
  analogWrite(moteurGauche2Pin, 0);
  analogWrite(moteurDroit1Pin, 0);
  analogWrite(moteurDroit2Pin, 0);
  handleRoot();
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(moteurGauche1Pin, OUTPUT);
  pinMode(moteurGauche2Pin, OUTPUT);
  pinMode(moteurDroit1Pin, OUTPUT);
  pinMode(moteurDroit2Pin, OUTPUT);
  pinMode(ledD6Pin, OUTPUT); // Set D6 as output
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(ledD6Pin, HIGH); // Set LED on D6 to HIGH
  
  server.on("/", handleRoot);
  server.on("/increase_gauche", increaseGauche);
  server.on("/decrease_gauche", decreaseGauche);
  server.on("/increase_droit", increaseDroit);
  server.on("/decrease_droit", decreaseDroit);
  server.on("/move_forward", moveForward);
  server.on("/move_backward", moveBackward);
  server.on("/turn_left", turnLeft);
  server.on("/turn_right", turnRight);
  server.on("/move_forward_1m", moveForward1m);
  server.on("/move_backward_1m", moveBackward1m);
  server.on("/quarter_turn_left", quarterTurnLeft);
  server.on("/quarter_turn_right", quarterTurnRight);
  server.on("/allumer", allumer); // Ajout de la route pour allumer le robot
  server.on("/eteindre", eteindre); // Ajout de la route pour éteindre le robot

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
