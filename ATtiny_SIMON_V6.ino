// ATtiny412 ピン定義デモバージョンV6
// ブザーはPWM対応のPA6に接続
const int BUZZER_PIN = A6; 
// LEDとボタンはPA7, PA1, PA2, PA3に接続
const int PINS[] = {A7, A1, A2, A3};
const int NUM_PINS = 4;

// ゲームのシーケンスを保存する配列
#define MAX_SEQUENCE_LENGTH 10 
int gameSequence[MAX_SEQUENCE_LENGTH];
int sequenceLength = 0;

// 各LEDに対応する音の周波数
const int TONES[] = {262, 294, 330, 349}; 

int gameState;
const int STATE_GAME = 0;
const int STATE_DEMO = 1;

void setup() {
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(PINS[i], OUTPUT);
    digitalWrite(PINS[i], HIGH);
  }
  
  pinMode(BUZZER_PIN, OUTPUT);
  
  pinMode(0, INPUT);
  randomSeed(analogRead(0));
  
  gameState = STATE_GAME;
  
  allLedsFlash(3, 200);
}

void loop() {
  switch (gameState) {
    case STATE_GAME:
      // === ゲームロジック ===
      if (sequenceLength < MAX_SEQUENCE_LENGTH) {
        int nextColor = random(NUM_PINS);
        gameSequence[sequenceLength] = nextColor;
        sequenceLength++;
      }
      playSimonSequence();
      if (playerTurn()) {
        delay(1000); 
      } else {
        // ゲームオーバーまたはタイムアウト
        gameOverSignal();
        sequenceLength = 0; 
        delay(2000);
        allLedsFlash(3, 200);
      }
      break;
      
    case STATE_DEMO:
      // === デモロジック ===
      // 全てのピンをOUTPUTに設定し、入力監視を無効化
      for (int i = 0; i < NUM_PINS; i++) {
        pinMode(PINS[i], OUTPUT);
      }
      
      // 音を鳴らさずにランダムなLEDを点滅
      int nextColor = random(NUM_PINS);
      digitalWrite(PINS[nextColor], LOW);
      delay(500);
      digitalWrite(PINS[nextColor], HIGH);
      delay(500);
      break;
  }
}

void playSimonSequence() {
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(PINS[i], OUTPUT);
  }
  for (int i = 0; i < NUM_PINS; i++) {
    digitalWrite(PINS[i], HIGH);
  }
  for (int i = 0; i < sequenceLength; i++) {
    digitalWrite(PINS[gameSequence[i]], LOW);
    tone(BUZZER_PIN, TONES[gameSequence[i]]);
    delay(500); 
    
    digitalWrite(PINS[gameSequence[i]], HIGH);
    noTone(BUZZER_PIN);
    delay(500); 
  }
}

bool playerTurn() {
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(PINS[i], INPUT);
  }
  for (int i = 0; i < sequenceLength; i++) {
    unsigned long startTime = millis();
    int expectedColorIndex = gameSequence[i];
    int playerInput = -1;
    while (playerInput == -1) {
      if (millis() - startTime > 15000) {
        gameState = STATE_DEMO; 
        return false;
      }
      for (int j = 0; j < NUM_PINS; j++) {
        if (digitalRead(PINS[j]) == LOW) {
          delay(10);
          if (digitalRead(PINS[j]) == LOW) {
            playerInput = j;
            tone(BUZZER_PIN, TONES[playerInput]);
            while (digitalRead(PINS[j]) == LOW) {}
            noTone(BUZZER_PIN);
            delay(50);
            break;
          }
        }
      }
    }
    if (playerInput != expectedColorIndex) {
      return false;
    }
  }
  return true;
}

void gameOverSignal() {
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(PINS[i], OUTPUT);
  }
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < NUM_PINS; j++) {
      digitalWrite(PINS[j], LOW);
    }
    tone(BUZZER_PIN, 100); 
    delay(100);
    for (int j = 0; j < NUM_PINS; j++) {
      digitalWrite(PINS[j], HIGH);
    }
    noTone(BUZZER_PIN);
    delay(100);
  }
}

void allLedsFlash(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    for (int j = 0; j < NUM_PINS; j++) {
      digitalWrite(PINS[j], LOW);
    }
    tone(BUZZER_PIN, 500);
    delay(delayMs);
    for (int j = 0; j < NUM_PINS; j++) {
      digitalWrite(PINS[j], HIGH);
    }
    noTone(BUZZER_PIN);
    delay(delayMs);
  }
}
