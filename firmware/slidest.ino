/*
 * Touch slide keyboard Slidest (сенсорная слайдовая клавиатура Слайдость)
 * Version: 0.6 beta
 * Date: 2020-05-06
 * Description: https://github.com/ibnteo/slidest (soon)
 * Author: Vladimir Romanovich <ibnteo@gmail.com>
 * License: MIT
 * |1|2|
 * |3|4|
 * |5|6|
 */

#include <Keyboard.h>
#include "Adafruit_MPR121.h"

Adafruit_MPR121 sensors = Adafruit_MPR121();

struct ListTouched {
  byte index;
  uint16_t chord;
};
struct Touched {
  uint16_t last;
  uint16_t curr;
  ListTouched list[2];
};
Touched touched = {0, 0, {{0, 0}, {0, 0}}};


struct Chord {
  uint16_t chord; // 5*3 bit
  byte symbol;
};

struct Chord2 {
  uint16_t chord;
  byte symbols[4];
};

struct ChordM {
  uint16_t chord;
  String macros;
};

#define S1 0
#define S2 3
#define S3 6
#define S4 9
#define S5 12

// Consonants
#define CHORD4 sizeof(chord4) / 6
Chord2 chord4[] = {
  {(2<<S1), {'s', 'c', '2', 0}},
  {(3<<S1), {'n', 'y', 0, 0}},
  {(6<<S1), {'t', 'n', '1', 0}},
  {(1<<S1)|(2<<S2), {'h', 'd', 0, 0}},
  {(1<<S1)|(3<<S2), {'r', 'h', 0, 0}},
  {(5<<S1)|(3<<S2), {'l', 'k', 0, 0}},
  {(5<<S1)|(6<<S2), {'c', 'r', 0, 0}},
  {(3<<S1)|(5<<S2)|(6<<S3), {'p', 'g', '{', 0}},
  {(3<<S1)|(5<<S2)|(3<<S3), {'b', ',', 0, 0}},
  {(3<<S1)|(1<<S2)|(2<<S3), {'m', 'v', '[', 0}},
  {(3<<S1)|(1<<S2)|(3<<S3), {'k', 'x', 0, 0}},
  {(2<<S1)|(1<<S2)|(3<<S3), {'z', 'p', '+', 0}},
  {(2<<S1)|(1<<S2)|(2<<S3), {'j', 'q', '8', 0}},
  {(6<<S1)|(5<<S2)|(3<<S3), {'d', 'l', '-', 0}},
  {(6<<S1)|(5<<S2)|(6<<S3), {'g', 'u', '9', 0}},
  {(1<<S1)|(3<<S2)|(5<<S3)|(6<<S4), {'v', ';', 0, 0}}, // ж
  {(1<<S1)|(3<<S2)|(5<<S3)|(3<<S4), {'w', 'i', 0, 0}}, // ш
  {(1<<S1)|(3<<S2)|(1<<S3)|(2<<S4), {'f', 'a', 0, 0}}, // ф
  {(1<<S1)|(2<<S2)|(1<<S3)|(3<<S4), {0, '\'', 0, 0}}, // э
  {(5<<S1)|(3<<S2)|(1<<S3)|(2<<S4), {'x', '[', 0, 0}}, // х
  {(5<<S1)|(3<<S2)|(1<<S3)|(3<<S4), {'y', 'o', 0, 0}}, // щ
  {(5<<S1)|(3<<S2)|(5<<S3)|(6<<S4), {'q', 'w', 0, 0}}, // ц
  {(5<<S1)|(6<<S2)|(5<<S3)|(3<<S4), {0, ']', 0, 0}}, // ъ
  {(3<<S1)|(1<<S2)|(2<<S3)|(1<<S4)|(3<<S5), {'-', '-', 0, 0}},
  {(3<<S1)|(5<<S2)|(6<<S3)|(5<<S4)|(3<<S5), {'"', '@', 0, 0}},
  {(3<<S1)|(1<<S2)|(3<<S3)|(5<<S4)|(6<<S5), {'_', '_', 0, 0}},
  {(3<<S1)|(5<<S2)|(3<<S3)|(1<<S4)|(2<<S5), {'\'', '@', 0, 0}},
  {(2<<S1)|(1<<S2)|(3<<S3)|(5<<S4)|(6<<S5), {',', '?', 0, 0}},
  {(6<<S1)|(5<<S2)|(3<<S3)|(1<<S4)|(2<<S5), {'.', '/', 0, 0}},
  {(2<<S1)|(1<<S2)|(3<<S3)|(1<<S4)|(2<<S5), {'(', '(', 0, 0}},
  {(6<<S1)|(5<<S2)|(3<<S3)|(5<<S4)|(6<<S5), {')', ')', 0, 0}},
  {(2<<S1)|(1<<S2)|(3<<S3)|(5<<S4)|(3<<S5), {';', '$', 0, 0}},
  {(6<<S1)|(5<<S2)|(3<<S3)|(1<<S4)|(3<<S5), {':', '^', 0, 0}},
};

// Vowels
#define CHORD0 sizeof(chord0) / 6
Chord2 chord0[] = {
  {(4<<S1)|(2<<S2), {'o', 'j', '0', 0}},
  {(4<<S1)|(6<<S2), {'e', 't', '3', 0}},
  {(4<<S1)|(3<<S2), {' ', ' ', ' ', 0}},
  {(4<<S1)|(2<<S2)|(1<<S3), {'i', 'b', '4', 0}},
  {(4<<S1)|(6<<S2)|(5<<S3), {'a', 'f', '5', 0}},
  {(4<<S1)|(3<<S2)|(5<<S3), {'u', 'e', '6', 0}},
  {(4<<S1)|(3<<S2)|(1<<S3), {'y', 'z', '7', 0}},
  {(4<<S1)|(3<<S2)|(1<<S3)|(2<<S4), {0, 's', 0, 0}},
  {(4<<S1)|(2<<S2)|(1<<S3)|(3<<S4), {0, 'm', '}', 0}},
  {(4<<S1)|(3<<S2)|(5<<S3)|(6<<S4), {0, '.', 0, 0}},
  {(4<<S1)|(6<<S2)|(5<<S3)|(3<<S4), {0, '`', ']', 0}},
};

// Vowels + space
#define CHORD0SP sizeof(chord0sp) / 6
Chord2 chord0sp[] = {
  {(4<<S1)|(2<<S2)|(1<<S3)|(2<<S4), {'i', 'b', '4', 0}},
  {(4<<S1)|(6<<S2)|(5<<S3)|(6<<S4), {'a', 'f', '5', 0}},
  {(4<<S1)|(3<<S2)|(5<<S3)|(3<<S4), {'u', 'e', '6', 0}},
  {(4<<S1)|(3<<S2)|(1<<S3)|(3<<S4), {'y', 'z', '7', 0}},
  {(4<<S1)|(3<<S2)|(1<<S3)|(2<<S4)|(1<<S5), {0, 's', 0, 0}},
  {(4<<S1)|(2<<S2)|(1<<S3)|(3<<S4)|(1<<S5), {0, 'm', '}', 0}},
  {(4<<S1)|(3<<S2)|(5<<S3)|(6<<S4)|(5<<S5), {0, '.', 0, 0}},
  {(4<<S1)|(6<<S2)|(5<<S3)|(3<<S4)|(5<<S5), {0, '`', ']', 0}},
};

// Macros
#define CHORDM sizeof(chordm) / 8
ChordM chordm[] = {
  {(4<<S1)|(3<<S2)|(1<<S3)|(2<<S4), "ia"},
  {(4<<S1)|(2<<S2)|(1<<S3)|(3<<S4), "ao"},
  {(4<<S1)|(3<<S2)|(5<<S3)|(6<<S4), "ue"},
  {(4<<S1)|(6<<S2)|(5<<S3)|(3<<S4), "you"},
};

#define KEY_LAYOUT 255
#define KEY_LAYOUT_NUM 254
#define KEY_RESET_MODS 253
byte layout = 0;
bool layout_num = false;
byte mods = 0;

// Controls
#define CHORD0C sizeof(chord0c) / 3
Chord chord0c[] = {
  {(2<<S1)|(1<<S2), KEY_BACKSPACE},
  {(1<<S1)|(3<<S2), KEY_RETURN},
  {(3<<S1)|(1<<S2), KEY_LEFT_SHIFT},
  {(3<<S1)|(1<<S2)|(2<<S3), KEY_LEFT_CTRL},
  {(3<<S1)|(1<<S2)|(3<<S3), KEY_LEFT_ALT},
  {(3<<S1)|(1<<S2)|(3<<S3)|(5<<S4), KEY_LEFT_GUI},
  {(3<<S1)|(5<<S2), KEY_DOWN_ARROW},
  {(5<<S1)|(3<<S2), KEY_UP_ARROW},
  {(5<<S1)|(6<<S2), KEY_RIGHT_ARROW},
  {(6<<S1)|(5<<S2), KEY_LEFT_ARROW},
  {(1<<S1)|(2<<S2), KEY_LAYOUT_NUM},
  {(1<<S1)|(2<<S2)|(1<<S3), KEY_LAYOUT},
  {(3<<S1)|(5<<S2)|(3<<S3), KEY_PAGE_DOWN},
  {(5<<S1)|(3<<S2)|(5<<S3), KEY_PAGE_UP},
  {(5<<S1)|(6<<S2)|(5<<S3)|(6<<S4), KEY_END},
  {(6<<S1)|(5<<S2)|(6<<S3)|(5<<S4), KEY_HOME},
  {(1<<S1)|(3<<S2)|(5<<S3), KEY_ESC},
  {(1<<S1)|(3<<S2)|(5<<S3)|(3<<S4), KEY_RESET_MODS},
};

// Ctrl + Controls
#define CHORD0CC sizeof(chord0cc) / 3
Chord chord0cc[] = {
  {(2<<S1)|(1<<S2)|(2<<S3), KEY_BACKSPACE},
  {(1<<S1)|(3<<S2)|(1<<S3), KEY_RETURN},
  {(5<<S1)|(6<<S2)|(5<<S3), KEY_RIGHT_ARROW},
  {(6<<S1)|(5<<S2)|(6<<S3), KEY_LEFT_ARROW},
};

#define LED_LAYOUT LED_BUILTIN_RX
#define LINUX   0
#define WINDOWS 1
#define MACOS   2
#define OS LINUX
void layout2(byte layer) {
  if (layout != layer) {
    Keyboard.releaseAll();
    #if OS == MACOS
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(' ');
    #elif OS == WINDOWS
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
    #else
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_SHIFT);
    #endif
    Keyboard.releaseAll();
  }
  layout = layer;
  digitalWrite(LED_LAYOUT, layout == 0 ? HIGH : LOW); // LOW = light
}

struct Space4 {
  byte symbol;
  bool space;
};
Space4 buff4[2] = {{0, false}, {0, false}};

void releaseMods() {
    Keyboard.releaseAll();
    mods = 0;
    digitalWrite(LED_LAYOUT, layout == 0 ? HIGH : LOW); // LOW = light
}

void press4(byte k) { // Autopress on sector 4
  byte l = layout_num ? 2 : layout;
  if ((touched.list[k].chord & 0b111) == 4) { // Start 4 and End 4
    if (buff4[k].space || buff4[k].symbol) {
      pressb(k);
    }
    for (byte i = 0; i < CHORD0; i ++) {
      if (touched.list[k].chord == chord0[i].chord) {
        buff4[k] = {chord0[i].symbols[l], true};
        break;
      }
    }
  } else {
    for (uint8_t i = 0; i < CHORD4; i ++) {
      if (touched.list[k].chord == chord4[i].chord) {
        buff4[k] = {chord4[i].symbols[l], false};
        break;
      }
    }
  }
}

void pressb(byte k) {
  if (buff4[k].symbol == 0 && buff4[k].space) { // Macros
    for (byte j = 0; j < CHORDM; j ++) {
      if (touched.list[k].chord == chordm[j].chord) {
        Keyboard.print(chordm[j].macros);
        releaseMods();
        break;
      }
    }
  } else if (buff4[k].symbol == ' ' && buff4[k].space) { // Tab
    Keyboard.write(KEY_TAB);
  } else if (buff4[k].space) { // Consonant + Space
    Keyboard.write(buff4[k].symbol);
    releaseMods();
    Keyboard.write(' ');
  } else {
    Keyboard.write(buff4[k].symbol);
    releaseMods();
  }
  buff4[k] = {0, false};
}

void press0(byte k) { // Press on release
  byte l = layout_num ? 2 : layout;
  if (buff4[k].space || buff4[k].symbol) {
    pressb(k);
  }
  if ((touched.list[k].chord & 0b111) == 4) { // Vowels
    for (uint8_t i = 0; i < CHORD0; i ++) {
      if (touched.list[k].chord == chord0[i].chord) {
        if (chord0[i].symbols[l] == 0) { // Macro
          for (byte j = 0; j < CHORDM; j ++) {
            if (touched.list[k].chord == chordm[j].chord) {
              releaseMods();
              Keyboard.print(chordm[j].macros);
              break;
            }
          }
        } else {
          Keyboard.write(chord0[i].symbols[l]);
          releaseMods();
          if (chord0[i].symbols[l] == ' ') {
            layout_num = false;
          }
        }
        break;
      }
    }
    for (uint8_t i = 0; i < CHORD0SP; i ++) { // Vowels + Space
      if (touched.list[k].chord == chord0sp[i].chord) {
        Keyboard.write(chord0sp[i].symbols[l]);
        releaseMods();
        Keyboard.write(' ');
        layout_num = false;
        break;
      }
    }
  } else { // Controls
    for (uint8_t i = 0; i < CHORD0C; i ++) {
      if (touched.list[k].chord == chord0c[i].chord) {
        if (chord0c[i].symbol == KEY_LAYOUT_NUM) {
          layout_num = ! layout_num;
        } else if (chord0c[i].symbol == KEY_LAYOUT) {
          layout2(layout ? 0 : 1);
        } else if (chord0c[i].symbol == KEY_RESET_MODS) {
          releaseMods();
        } else if (chord0c[i].symbol >= KEY_LEFT_CTRL && chord0c[i].symbol <= KEY_RIGHT_GUI) {
          byte mod = (1 << (chord0c[i].symbol - KEY_LEFT_CTRL));
          if (mods & mod) {
            mods &= ~ mod;
            Keyboard.release(chord0c[i].symbol);
          } else {
            mods |= mod;
            Keyboard.press(chord0c[i].symbol);
          }
          digitalWrite(LED_LAYOUT, mods || layout != 0 ? LOW : HIGH); // LOW = light
        } else {
          Keyboard.write(chord0c[i].symbol);
        }
        break;
      }
    }
    for (uint8_t i = 0; i < CHORD0CC; i ++) { // Ctrl+
      if (touched.list[k].chord == chord0cc[i].chord) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.write(chord0cc[i].symbol);
        Keyboard.release(KEY_LEFT_CTRL);
        mods &= ~ (KEY_LEFT_CTRL - 0x80);
        break;
      }
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN_TX, INPUT); // disable LED TX
  pinMode(LED_LAYOUT, OUTPUT);
  Keyboard.begin();
  sensors.begin(0x5A);
}

void loop() {
  touched.curr = sensors.touched();
  
  for (uint8_t i=0; i<12; i++) {
    uint8_t k = 0; // Side left or right
    uint8_t j = i; // Section convert to 1-6 value on each side
    if ((touched.curr & 0b110011001100 & (1 << i)) || (touched.last & 0b110011001100 & (1 << i))) { // Right side
      k = 1;
      j = j - 2;
    }
    if (j > 1) j = j - 2;
    if (j > 3) j = j - 2;
    j ++;
    j = j & 0b111;
    
    if ((touched.curr & (1 << i)) && ! (touched.last & (1 << i)) ) { // Touch
      if (touched.list[k].chord && (j == 4)) {
        press4(k);
        touched.list[k].index = 3;
        touched.list[k].chord = 4;
      } else {
        if (touched.list[k].index < 15) {
          touched.list[k].chord |= (j << touched.list[k].index);
          touched.list[k].index = touched.list[k].index + 3;
        } else {
          touched.list[k].chord = 0;
        }
      }
    }
    if (! (touched.curr & (1 << i)) && (touched.last & (1 << i)) ) { // Release
      if (! (touched.curr & (0b1100110011 << (k + k)))) { // All release
        if (touched.list[k].chord) {
          press0(k);
        }
        touched.list[k].index = 0;
        touched.list[k].chord = 0;
      }
    }
  }

  touched.last = touched.curr;
}
