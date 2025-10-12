// Code ALARME V1 Zinck Edouard
//Capteur Activation alarme : capteur ultrason, bouton, faux code 3 fois, mauvais UID 3 fois 
//Alarme succès : Bon code et bon UID 

// DEBUT CODE 

// Bibliothèques
#include <MFRC522.h> // RFID
#include <SPI.h>
#include <LiquidCrystal.h> // LCD
#include <Keypad.h> // CLAVIER
#include "SR04.h" // ULTRASON
#include <Wire.h>


//PIN ENTREE
#define TRIG_PIN 6
#define ECHO_PIN 8
#define rouge1 3
#define rouge2 4
#define bouton 5  
#define alarme 46
#define green1 7
#define green2 2
LiquidCrystal lcd(33, 35, 37, 39, 41, 43);
#define SS_PIN 53
#define RST_PIN 5

//initialisation instance RFID et SR04
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;
MFRC522 mfrc522(SS_PIN, RST_PIN);



// NOMBRE DE COLONNE PLUS LIGNE PLUS ETAT CLAVIER
const byte ROWS = 4; 
const byte COLS = 4; 
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {30, 32, 34, 36}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {38, 40, 42, 44}; //connect to the column pinouts of the keypad



//initialisation instance clavier
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


//FONCTIONS ANNEXES


//POUR COMPARER DEUX LISTES CHAR, UTILISER PAR CLAVIER
bool comparer(char A[], char B[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (A[i] != B[i]) return false;
    }
    return true;
}

//Afficher le tableau et ses indices
void printArray(char arr[], int size) {
  Serial.println("Le tableau est :");
  for (int i = 0; i < size; i++) {
    Serial.print("Élément ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(arr[i]);
  }
}

//CODE SETUP
void setup() {
  pinMode(rouge1, OUTPUT);
  pinMode(rouge2, OUTPUT);
  pinMode(alarme, OUTPUT);
  pinMode(bouton, INPUT_PULLUP);
  digitalWrite(rouge1, LOW);
  digitalWrite(rouge2,  LOW);
  Serial.begin(9600);
  //delay(1000);
  pinMode(green1, OUTPUT);
  pinMode(green2, OUTPUT);
  analogWrite(green1, LOW);
  analogWrite(green2,  LOW);
  lcd.begin(16, 2);
  lcd.print("Entrez votre PIN");
  SPI.begin();
  mfrc522.PCD_Init();


}


//INITIALISATION VARIABLES
int rfidalarme = 0;
int compteur = 0;
int rougevaleur = 255; 
int compteur2 = 0;
char PIN[5] = {'1','2','3','4','A'};
char CODE[5] = {'0','0','0','0','0'};
byte* uidTableau = (byte*) malloc(mfrc522.uid.size);
int compteurrfid = 1;
byte uidAutorise[] = {0x8D, 0x18, 0xC9, 0x01};
byte tailleUID = 4;

int compteurcarte = 0;

// LOOP BOUCLE PRINCIPALE
void loop() {
 
  // CODE POUR CLAVIER 
  lcd.setCursor(0, 1);
  char customKey = customKeypad.getKey();
  a=sr04.Distance();
  // PIN -------------------------------------------
  lcd.setCursor(0,0);
  lcd.print("Entrez votre PIN");
  if (customKey){
    CODE[compteur] = customKey;
    Serial.println(customKey);
    printArray(CODE,5);
    compteur = compteur + 1;
    Serial.println(compteur);
    lcd.setCursor(compteur, 1);
    lcd.print('*');
    
  }
  if ((comparer(PIN,CODE,5) == true) && (compteur == 5))  {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bienvenue !");
      delay(1000);
      digitalWrite(alarme, HIGH);
      analogWrite(green1 , 255);
      analogWrite(green2 , 255);
      delay(1000);
      digitalWrite(alarme, LOW);
      analogWrite(green1 , 0);
      analogWrite(green2 , 0);
      compteur = 0;
      for(int i = 0; i<5 ; i += 1 ){
        CODE[i] = '0' ;
      }
      lcd.clear();
      
      delay(2000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Entrez votre PIN");
  }
  else if ((comparer(PIN,CODE,5) == false) && (compteur == 5)){
    compteur2 += 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Il vous reste ");
    lcd.setCursor(0,1);
    lcd.print(3 - compteur2);
    lcd.print(" essaies");
    digitalWrite(rouge1, HIGH);
    digitalWrite(rouge2, HIGH);
    delay(500);
    digitalWrite(rouge1, LOW);
    digitalWrite(rouge2, LOW);
    delay(500);
    digitalWrite(rouge1, HIGH);
    digitalWrite(rouge2, HIGH);
    delay(500);
    digitalWrite(rouge1, LOW);
    digitalWrite(rouge2, LOW);
    delay(1000);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Entrez votre PIN");
    compteur = 0;


  }

  // FIN CLAVIER 
  // DEBUT RFID

    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    bool carteValide = true;

    // Vérifie si la taille de l'UID correspond
    if (mfrc522.uid.size != tailleUID) {
      carteValide = false;
    } else {
      for (byte i = 0; i < tailleUID; i++) {
        if (mfrc522.uid.uidByte[i] != uidAutorise[i]) {
          carteValide = false;
          break;
        }
      }
    }
    

    lcd.clear();
    lcd.setCursor(0, 0);
    if (carteValide) {
      lcd.print("Carte autorisee:");
          lcd.setCursor(0, 1);
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      lcd.print(mfrc522.uid.uidByte[i], HEX);
      lcd.print(" ");
    }

    analogWrite(green1, 255);
    analogWrite(green2, 255);
    digitalWrite(alarme, HIGH);
    delay(200);
    analogWrite(green1, 0);
    analogWrite(green2, 0);
    digitalWrite(alarme, LOW);
    delay(200);
    analogWrite(green1, 255);
    analogWrite(green2, 255);
    digitalWrite(alarme, HIGH);
    delay(200);
    analogWrite(green1, 0);
    analogWrite(green2, 0);
    digitalWrite(alarme, LOW);
    delay(2000);

    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Carte inconnue:");
          lcd.setCursor(0, 1);
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      lcd.print(mfrc522.uid.uidByte[i], HEX);
      lcd.print(" ");
    }
      rfidalarme += 1;
    delay(2000);

          lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Il vous reste ");
    lcd.setCursor(0,1);
    lcd.print(3 - rfidalarme);
    lcd.print(" essaies");
    digitalWrite(rouge1, HIGH);
    digitalWrite(rouge2, HIGH);
        digitalWrite(alarme, HIGH);
    delay(500);
    digitalWrite(rouge1, LOW);
    digitalWrite(rouge2, LOW);
        digitalWrite(alarme, LOW);
    delay(500);
    digitalWrite(rouge1, HIGH);
    digitalWrite(rouge2, HIGH);
        digitalWrite(alarme, HIGH);
    delay(500);
    digitalWrite(rouge1, LOW);
    digitalWrite(rouge2, LOW);
        digitalWrite(alarme, LOW);
    delay(1000);
       }

    lcd.clear();
  }
  // FIN RFID 
  //DISTANCE CAPTEUR U
  
  a=sr04.Distance();

  // FIN DISTANCE CAPTEUR U 
  // DECLANCHEMENT ALARME : 

  if  ((digitalRead(bouton) == LOW )  || (a<5) || (compteur2 == 3) || (rfidalarme == 3) ){
  while(1<2){

  for(int i = 0; i <80 ; i += 1 ){
    digitalWrite(alarme, HIGH);
    analogWrite(rouge1, 0);
    analogWrite(rouge2, 0);
    

    delay(100);
    digitalWrite(alarme, LOW);
    analogWrite(rouge1, 255);
    analogWrite(rouge2, 255);
    delay(100);
  }


  
  for(int i = 0; i < 255 ; i +=1 ){
    rougevaleur -= 1 ;
    analogWrite(rouge1, rougevaleur);
    analogWrite(rouge2, rougevaleur);
    delay(1);
  }
  rougevaleur = 0;

  for(int i = 0; i < 255 ; i +=1 ){
    rougevaleur += 1 ;
    analogWrite(rouge1, rougevaleur);
    analogWrite(rouge2, rougevaleur);
    delay(1);
  }
  rougevaleur = 255 ; 
  }
  }
}


// FIN CODE 



