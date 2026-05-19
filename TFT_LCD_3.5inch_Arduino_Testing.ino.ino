#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSansBold12pt7b.h>

MCUFRIEND_kbv tft;

// --- COLORS ---
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20
#define PINK    0xF81F
#define LIGHTPINK 0xFE19 // softer pink for glow

// --- GLOBALS ---
String incomingText = "";
bool listening = false;
unsigned long lastBlink = 0;
unsigned long lastGlow = 0;
bool cheeksBright = false;

// --- DRAW ONE EYE ---
void drawEye(int x, int y) {
  tft.drawCircle(x, y, 40, MAGENTA);       // Outer purple ring
  tft.fillCircle(x, y, 30, BLACK);         // Inner black eye
  tft.fillCircle(x - 10, y - 10, 6, WHITE);// Sparkle highlight
  tft.fillCircle(x + 10, y + 10, 8, MAGENTA); // Purple crescent
}

// --- DRAW CUTE HALLOWEEN FACE ---
void drawCuteFace(String mood) {
  tft.fillScreen(BLACK);
  int cx = tft.width() / 2;
  int cy = tft.height() / 2 + 20;

  // Eye positions
  int leftEyeX = cx - 70, rightEyeX = cx + 70, eyeY = cy - 30;

  // Draw kawaii eyes
  drawEye(leftEyeX, eyeY);
  drawEye(rightEyeX, eyeY);

  // Lashes
  for (int i = 0; i < 5; i++) {
    tft.drawLine(leftEyeX - 40 + i*8, eyeY - 40, leftEyeX - 30 + i*8, eyeY - 60, WHITE);
    tft.drawLine(rightEyeX - 40 + i*8, eyeY - 40, rightEyeX - 30 + i*8, eyeY - 60, WHITE);
  }

  // Cheeks (glow effect)
  uint16_t cheekColor = cheeksBright ? LIGHTPINK : RED;
  tft.fillCircle(leftEyeX, eyeY + 50, 15, cheekColor);
  tft.fillCircle(rightEyeX, eyeY + 50, 15, cheekColor);

  // Mouth (cute smile + Halloween fangs)
  tft.fillTriangle(cx - 20, cy + 40, cx + 20, cy + 40, cx, cy + 60, RED);
  tft.fillCircle(cx, cy + 55, 8, MAGENTA); // tongue
  // Tiny fangs
  tft.fillTriangle(cx - 10, cy + 40, cx - 5, cy + 40, cx - 7, cy + 50, WHITE);
  tft.fillTriangle(cx + 10, cy + 40, cx + 5, cy + 40, cx + 7, cy + 50, WHITE);

  // Status text based on mood
  tft.setTextSize(2);
  if (mood == "rest") {
    tft.setTextColor(WHITE);
    tft.setCursor(cx - 110, 60);
    tft.print("Waiting for speech...");
  } else if (mood == "listen") {
    tft.setTextColor(CYAN);
    tft.setCursor(cx - 70, 60);
    tft.print("Listening...");
  } else if (mood == "thinking") {
    tft.setTextColor(ORANGE);
    tft.setCursor(cx - 65, 60);
    tft.print("Thinking...");
  }
}

// --- BLINK ANIMATION ---
void blinkEyes() {
  int cx = tft.width() / 2;
  int cy = tft.height() / 2 + 20;
  int leftEyeX = cx - 70, rightEyeX = cx + 70, eyeY = cy - 30;

  // Cover eyes with black rectangles for blink
  tft.fillRect(leftEyeX - 30, eyeY - 30, 60, 60, BLACK);
  tft.fillRect(rightEyeX - 30, eyeY - 30, 60, 60, BLACK);
  delay(150);
  drawCuteFace("rest"); // redraw face after blink
}

// --- DISPLAY TEXT ---
void displayText(String text) {
  tft.fillScreen(BLACK);
  tft.setFont(&FreeSansBold12pt7b);
  tft.setTextColor(YELLOW);
  tft.setCursor(40, 80);
  tft.print("You said:");

  tft.setFont(); // default
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 150);
  tft.println(text);
}

// --- SETUP ---
void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(3); // 0 = normal, 2 = upside down
  drawCuteFace("rest");
}

// --- LOOP ---
void loop() {
  // Handle serial input
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '*') {
      listening = true;
      drawCuteFace("listen");
      delay(400);
    } 
    else if (c == '#') {
      drawCuteFace("thinking");
      delay(400);
      drawCuteFace("rest");
      listening = false;
    } 
    else if (c == '\n') {
      displayText(incomingText);
      delay(3000);
      drawCuteFace("rest");
      incomingText = "";
    } 
    else {
      incomingText += c;
    }
  }

  // Blinking every 5 seconds
  if (millis() - lastBlink > 5000) {
    blinkEyes();
    lastBlink = millis();
  }

  // Cheek glow every 1 second
  if (millis() - lastGlow > 1000) {
    cheeksBright = !cheeksBright;
    drawCuteFace("rest");
    lastGlow = millis();
  }
}