#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
LiquidCrystal lcd(8,9,4,5,6,7);
const int geigerPin=21; //Μπορούν να χρησιμοποιηθούν μόνο τα πινς 2,3,18,19,20,21 που υποστηρίζουν interrupt
volatile unsigned int counts=0;
volatile unsigned long add=0;
unsigned long previousMillis=0;
unsigned long currentMillis = 0;
unsigned long intervals[]={100,250,500,750,1000,2000,3000,4000,5000}; //Από εδώ ρυθμίζεται το χρονικό διάστημα κάθε μέτρησης(ms)
int intervalIndex=0;
unsigned int measurementcount=0;
bool measuring = false;
int buttonValue=analogRead(A0);

void setup()
{
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Geiger Counter");
  delay(1000);
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(geigerPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(geigerPin),countPulse,RISING); //Ανιχνεύει παλμό και τρέχει τη συνάρτηση countPulse όταν ανιχνευτεί σήμα
  displayInterval();
}

void loop()
{
    
  checkButtons();
  if(measuring){   
    currentMillis=millis();
    if(currentMillis-previousMillis>=intervals[intervalIndex]){
    previousMillis=currentMillis;
    //unsigned int k=counts/intervals[intervalIndex];
//Απεικόνιση στη σειριακή και στο .txt
    Serial.print(counts);
    Serial.print("\n");
    Serial1.print(counts);   
    Serial1.print("\n");
//Απεικόνιση στην οθόνη
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Meas: ");
    lcd.print(measurementcount+1);
    lcd.setCursor(0,1);
    lcd.print("Counts:");
    lcd.print(counts);
    
    add=add+counts;  //αθροίζει τα counts/unit of time μετά από κάθε μέτρηση
    counts=0; //ξαναμηδενίζει πριν την επόμενη μέτρηση
    measurementcount++; //αυξάνει κατά 1 μετά από κάθε μέτρηση
}
}
}
//Συνάρτηση διακοπής για την καταμέτρηση μέσω του ανιχνευτή
  void countPulse(){
    counts++;
}
void displayInterval(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Interval:");
  lcd.setCursor(0,1);
  lcd.print(intervals[intervalIndex]);
  lcd.print("ms");

}

//Συνάρτηση για το χειρισμό των κουμπιών στην οθόνη
void checkButtons() {
  int buttonValue = analogRead(A0);

  if (buttonValue < 100) {
    if(!measuring){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Up/Down Please!");}
  } else if (buttonValue < 200) {
    if (!measuring) {
      intervalIndex = (intervalIndex + 1) % (sizeof(intervals) / sizeof(intervals[0]));
      displayInterval();
      delay(200); 
    }
  } else if (buttonValue < 400) {
    if (!measuring) {
      intervalIndex = (intervalIndex - 1 + (sizeof(intervals) / sizeof(intervals[0]))) % (sizeof(intervals) / sizeof(intervals[0]));
      displayInterval();
      delay(200);
    }
     } else if (buttonValue < 600) {
    if(!measuring) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Up/Down Please!"); }
  } else if (buttonValue < 800) {
    if (!measuring) {
      measuring = true;
      counts = 0;
      measurementcount=0;
      add=0;
      previousMillis = millis();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Measuring...");
      lcd.setCursor(0,1);
      lcd.print("Interval:");
      lcd.print(intervals[intervalIndex]);
      lcd.print("ms");
    } else {
      measuring=false;
      Serial.print("Total Counts k = ");
      Serial.print(add);
      Serial.print("\n");
      Serial.print("Total Measurements N = ");
      Serial.print(measurementcount);
      Serial.print("\n");
      Serial.print("k bar = ");
      if (measurementcount>0){
        float average=(float)add/measurementcount;
        Serial.print(average,4);
      }else{
        Serial.print("0");
      }
      Serial.print("\n");
      Serial.print("Sample Rate: ");
      Serial.print((float)1000/intervals[intervalIndex]); 
      Serial.print(" Hz");
      Serial1.print("Total Counts k = ");
      Serial1.print(add);
      Serial1.print("\n");           
      Serial1.print("Total Measurements N = ");
      Serial1.print(measurementcount);
      Serial1.print("\n");
      Serial1.print("k bar = ");
      if (measurementcount>0){
        float average=(float)add/measurementcount;
        Serial1.print(average,4);
      }else{
        Serial1.print("0");
      }    
      Serial1.print("\n");
      Serial1.print("Sample Rate: ");
      Serial1.print((float)1000/intervals[intervalIndex]); 
      Serial1.print("Hz");      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Total C :");
      lcd.setCursor(0,1);
      lcd.print(add);
      delay(2000);
      displayInterval();
    }
    delay(200); 
  }
}
