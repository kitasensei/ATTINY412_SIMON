// ATtiny412 ピン定義
// ブザーはPWM対応のPA6に接続
const int BUZZER_PIN = A6; 
// LEDとボタンはPA1, PA2, PA7, PA3に接続
const int PINS[] = {A1, A2, A7, A3};
const int NUM_PINS = 4;

// ゲームのシーケンスを保存する配列
#define MAX_SEQUENCE_LENGTH 10 
int gameSequence[MAX_SEQUENCE_LENGTH];
int sequenceLength = 0;

// 各LEDに対応する音の周波数
const int TONES[] = {262, 294, 330, 349}; 

void setup() {
  // すべてのピンを出力に設定
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(PINS[i], OUTPUT);
    digitalWrite(PINS[i], HIGH);
  }
  
  // ブザーピンを出力に設定
  pinMode(BUZZER_PIN, OUTPUT);
  
  // ランダムシードの設定
  pinMode(0, INPUT);
  randomSeed(analogRead(0));
  
  // ゲーム開始の合図
  allLedsFlash(3, 200);
}

void loop() {
  if (sequenceLength < MAX_SEQUENCE_LENGTH) {
    int nextColor = random(NUM_PINS);
    gameSequence[sequenceLength] = nextColor;
    sequenceLength++;
  }

  playSimonSequence();

  if (playerTurn()) {
    delay(1000); 
  } else {
    gameOverSignal();
    sequenceLength = 0; 
    delay(2000);
    allLedsFlash(3, 200);
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
    int expectedColorIndex = gameSequence[i];
    int playerInput = -1;
    while (playerInput == -1) {
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
