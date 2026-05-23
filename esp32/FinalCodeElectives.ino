#include <WiFi.h>
#include <HTTPClient.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TFT_eSPI.h>
#include <vector>
#include <math.h>

// ===================== WIFI =====================
const char* ssid = "Jceuu";
const char* password = "jejejeje";

// ===================== SERVER =====================
const char* serverUrlRegister =
    "http://192.168.106.104/rfid_project/register2.php";

const char* serverUrlVoucher =
    "http://192.168.106.104/rfid_project/get_voucher.php";

// ===================== RFID =====================
#define SS_PIN 21
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ===================== TFT =====================
TFT_eSPI tft = TFT_eSPI();

// ===================== KEYPAD =====================
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 26, 25};
byte colPins[COLS] = {33, 32, 16, 4};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ===================== STATES =====================
enum State {
    STATE_MAIN_MENU,
    STATE_ENTER_STUDENT_NUMBER,
    STATE_ENTER_RFID,
    STATE_FETCH_VOUCHER,
    STATE_SCHOOL_PROGRAM_SELECTION
};

State currentState = STATE_MAIN_MENU;
std::vector<State> stateStack;

// ===================== VARIABLES =====================
String studentNumber = "";
String selectedSchoolProgram = "";

#define NUM_DOTS 12
int dotPositions[NUM_DOTS][2];
unsigned long previousMillis = 0;

// =====================================================
// UI FUNCTIONS
// =====================================================

void setupDotPositions() {
    float angle = 0;

    for (int i = 0; i < NUM_DOTS; i++) {
        dotPositions[i][0] = 225 + 20 * cos(angle);
        dotPositions[i][1] = 170 + 20 * sin(angle);
        angle += 2 * PI / NUM_DOTS;
    }
}

void drawConnectingAnimation() {
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(110,110);
    tft.println("Connecting to Server");

    for(int i=0;i<NUM_DOTS;i++){
        tft.fillCircle(dotPositions[i][0],dotPositions[i][1],3,TFT_WHITE);
        delay(50);
        tft.fillCircle(dotPositions[i][0],dotPositions[i][1],3,TFT_BLACK);
    }
}

void drawBorder() {
    tft.drawRoundRect(0,0,tft.width(),tft.height(),10,TFT_DARKGREY);
    tft.drawRoundRect(2,2,tft.width()-4,tft.height()-4,10,TFT_WHITE);
}

void drawOptionBox(int x,int y,String text){
    int w = tft.textWidth(text)+20;

    tft.fillRoundRect(x-10,y-10,w,30,5,TFT_BLACK);
    tft.drawRoundRect(x-10,y-10,w,30,5,TFT_WHITE);

    tft.setCursor(x,y);
    tft.println(text);
}

void drawBackKey(){
    drawOptionBox(40,tft.height()-50,"D: Back");
}

void clearUIArea(){
    tft.fillRect(0,60,tft.width(),tft.height()-60,TFT_BLACK);
    drawBorder();
}

void drawGradient(){
    for(int y=0;y<tft.height();y++){
        uint8_t c = map(y,0,tft.height(),0,255);
        tft.drawFastHLine(0,y,tft.width(),tft.color565(0,0,c));
    }
}

void drawUI(){

    drawGradient();

    tft.setTextColor(TFT_DARKCYAN);
    tft.setTextSize(3);

    if(currentState==STATE_MAIN_MENU){

        tft.setCursor(160,25);
        tft.println("Main Menu");

        drawBorder();

        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);

        drawOptionBox(125,150,"1. Register your ID");
        drawOptionBox(150,200,"2. Voucher Code");
    }

    else if(currentState==STATE_ENTER_STUDENT_NUMBER){

        tft.setCursor(160,25);
        tft.println("Registration");

        drawBorder();

        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);

        tft.setCursor(30,80);
        tft.println("Enter Student Number:");

        tft.setCursor(30,110);
        tft.println(studentNumber);

        drawOptionBox(40,180,"A: Submit");
        drawOptionBox(190,180,"B: Clear");
        drawOptionBox(330,180,"C: Delete");

        drawBackKey();
    }

    else if(currentState==STATE_SCHOOL_PROGRAM_SELECTION){

        tft.setCursor(160,25);
        tft.println("Registration");

        drawBorder();

        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);

        tft.setCursor(30,80);
        tft.println("Select School Program:");

        drawOptionBox(40,150,"1. SHS");
        drawOptionBox(180,150,"2. SET");
        drawOptionBox(320,150,"3. SBA");

        drawOptionBox(40,230,"4. SAS");
        drawOptionBox(175,230,"5. STHM");
        drawOptionBox(320,230,"6. SOA");

        drawBackKey();
    }
}

// =====================================================
// SERVER FUNCTIONS
// =====================================================

void registerStudent(String studentNumber,String uid,String schoolProgram){

    if(WiFi.status()!=WL_CONNECTED) return;

    HTTPClient http;

    http.begin(serverUrlRegister);
    http.addHeader("Content-Type","application/x-www-form-urlencoded");

    String postData =
        "uid="+uid+
        "&studentNumber="+studentNumber+
        "&schoolProgram="+schoolProgram;

    int code=http.POST(postData);

    if(code>0){
        String response=http.getString();

        clearUIArea();
        drawGradient();

        tft.setCursor(30,120);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.println(response);

        delay(2000);
    }

    http.end();
}

void fetchVoucherCode(String uid){

    if(WiFi.status()!=WL_CONNECTED) return;

    HTTPClient http;

    http.begin(serverUrlVoucher);
    http.addHeader("Content-Type","application/x-www-form-urlencoded");

    String postData="uid="+uid;

    int code=http.POST(postData);

    if(code>0){

        String response=http.getString();

        clearUIArea();
        drawGradient();

        tft.setCursor(30,120);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);

        tft.println(response);

        delay(3000);
    }

    http.end();
}

// =====================================================
// SETUP
// =====================================================

void setup(){

    Serial.begin(115200);

    tft.init();
    tft.setRotation(3);

    setupDotPositions();

    drawConnectingAnimation();

    WiFi.begin(ssid,password);

    while(WiFi.status()!=WL_CONNECTED){

        if(millis()-previousMillis>=600){
            previousMillis=millis();
            drawConnectingAnimation();
        }

        Serial.println("Connecting...");
    }

    Serial.println("Connected!");

    SPI.begin();
    mfrc522.PCD_Init();

    drawUI();
}

// =====================================================
// LOOP
// =====================================================

void loop(){

    char key=keypad.getKey();

    if(key){

        if(key=='D'){
            if(!stateStack.empty()){
                currentState=stateStack.back();
                stateStack.pop_back();
            } else {
                currentState=STATE_MAIN_MENU;
            }

            drawUI();
            return;
        }

        switch(currentState){

            case STATE_MAIN_MENU:

                if(key=='1'){
                    stateStack.push_back(currentState);
                    currentState=STATE_ENTER_STUDENT_NUMBER;
                    drawUI();
                }

                else if(key=='2'){
                    stateStack.push_back(currentState);
                    currentState=STATE_FETCH_VOUCHER;
                    drawUI();
                }

                break;

            case STATE_ENTER_STUDENT_NUMBER:

                if(key=='A'){
                    currentState=STATE_SCHOOL_PROGRAM_SELECTION;
                    drawUI();
                }

                else if(key=='B'){
                    studentNumber="";
                    drawUI();
                }

                else if(key=='C'){
                    if(studentNumber.length()>0){
                        studentNumber.remove(studentNumber.length()-1);
                        drawUI();
                    }
                }

                else{
                    studentNumber+=key;
                    drawUI();
                }

                break;

            case STATE_SCHOOL_PROGRAM_SELECTION:

                if(key>='1'&&key<='6'){

                    String programs[]={
                        "SHS","SET","SBA",
                        "SAS","STHM","SOA"
                    };

                    selectedSchoolProgram=programs[key-'1'];

                    currentState=STATE_ENTER_RFID;

                    clearUIArea();

                    tft.setCursor(50,130);
                    tft.println("Tap your ID to register");
                }

                break;
        }
    }

    if(mfrc522.PICC_IsNewCardPresent() &&
       mfrc522.PICC_ReadCardSerial()){

        String uid="";

        for(byte i=0;i<mfrc522.uid.size;i++){
            uid += String(
                mfrc522.uid.uidByte[i]<0x10?"0":""
            ) + String(mfrc522.uid.uidByte[i],HEX);
        }

        uid.toUpperCase();

        if(currentState==STATE_ENTER_RFID){

            registerStudent(
                studentNumber,
                uid,
                selectedSchoolProgram
            );

            studentNumber="";
            selectedSchoolProgram="";
            currentState=STATE_MAIN_MENU;
            drawUI();
        }

        else if(currentState==STATE_FETCH_VOUCHER){

            fetchVoucherCode(uid);

            currentState=STATE_MAIN_MENU;
            drawUI();
        }
    }
}