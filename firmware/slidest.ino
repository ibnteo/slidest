/*
 * Touch slide keyboard Slidest (сенсорная слайдовая клавиатура Слайдость)
 * Version: 0.1 alpha
 * Date: 2020-05-03
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
uint16_t last_touched = 0;
uint16_t curr_touched = 0;
uint16_t list_touched[2] = {0};
uint8_t index_touched[2] = {0};

struct Chord {
  uint16_t c; // chord 2*5 bit
  byte s;     // symbol
};

#define S1 0
#define S2 3
#define S3 6
#define S4 9
#define S5 12

// Consonants
#define CHORD4 sizeof(chord4) / 3
Chord chord4[] = {
  {(2<<S1), 's'},
  {(3<<S1), 'n'},
  {(6<<S1), 't'},
  {(1<<S1)|(2<<S2), 'h'},
  {(1<<S1)|(3<<S2), 'r'},
  {(5<<S1)|(3<<S2), 'l'},
  {(5<<S1)|(6<<S2), 'c'},
  {(3<<S1)|(5<<S2)|(6<<S3), 'p'},
  {(3<<S1)|(5<<S2)|(3<<S3), 'b'},
  {(3<<S1)|(1<<S2)|(2<<S3), 'm'},
  {(3<<S1)|(1<<S2)|(3<<S3), 'k'},
  {(2<<S1)|(1<<S2)|(3<<S3), 'z'},
  {(2<<S1)|(1<<S2)|(2<<S3), 'j'},
  {(6<<S1)|(5<<S2)|(4<<S3), 'd'},
  {(6<<S1)|(5<<S2)|(6<<S3), 'g'},
};

// Vowels
#define CHORD0 sizeof(chord0) / 3
Chord chord0[] = {
  {(4<<S1)|(2<<S2), 'o'},
  {(4<<S1)|(6<<S2), 'e'},
  {(4<<S1)|(3<<S2), ' '},
  {(4<<S1)|(2<<S2)|(1<<S3), 'i'},
  {(4<<S1)|(6<<S2)|(5<<S3), 'a'},
  {(4<<S1)|(3<<S2)|(5<<S3), 'u'},
  {(4<<S1)|(3<<S2)|(1<<S3), 'y'},
};

// Vowels + space
#define CHORD0SP sizeof(chord0sp) / 3
Chord chord0sp[] = {
  {(4<<S1)|(2<<S2)|(1<<S3)|(2<<S4), 'i'},
  {(4<<S1)|(6<<S2)|(5<<S3)|(6<<S4), 'a'},
  {(4<<S1)|(3<<S2)|(5<<S3)|(3<<S4), 'u'},
  {(4<<S1)|(3<<S2)|(1<<S3)|(3<<S4), 'y'},
};

// Controls
#define CHORD0C sizeof(chord0c) / 3
Chord chord0c[] = {
  {(2<<S1)|(1<<S2), KEY_BACKSPACE},
  {(1<<S1)|(3<<S2), KEY_RETURN},
  {(2<<S1)|(5<<S2), KEY_DOWN_ARROW},
  {(5<<S1)|(1<<S2), KEY_UP_ARROW},
  {(5<<S1)|(6<<S2), KEY_RIGHT_ARROW},
  {(6<<S1)|(5<<S2), KEY_LEFT_ARROW},
};

void press4(uint8_t k) { // Autopress on sector 4
  Serial.print(list_touched[k], BIN);
  Serial.println(" press4");
  if ((list_touched[k] & 0b111) == 4) {
    for (uint8_t i = 0; i < CHORD0; i ++) {
      if (list_touched[k] == chord0[i].c) {
        if (chord0[i].s == ' ') {
          Keyboard.write(KEY_TAB);
        } else {
          Keyboard.write(chord0[i].s);
          Keyboard.write(' ');
        }
        break;
      }
    }
  } else {
    for (uint8_t i = 0; i < CHORD4; i ++) {
      if (list_touched[k] == chord4[i].c) {
        Keyboard.write(chord4[i].s);
        break;
      }
    }
  }
}

void press0(uint8_t k) { // Press on release
  Serial.print(list_touched[k], BIN);
  Serial.println(" press0");
  if ((list_touched[k] & 0b111) == 4) {
    for (uint8_t i = 0; i < CHORD0; i ++) {
      if (list_touched[k] == chord0[i].c) {
        Keyboard.write(chord0[i].s);
        break;
      }
    }
    for (uint8_t i = 0; i < CHORD0SP; i ++) {
      if (list_touched[k] == chord0sp[i].c) {
        Keyboard.write(chord0sp[i].s);
        Keyboard.write(' ');
        break;
      }
    }
  } else {
    for (uint8_t i = 0; i < CHORD0C; i ++) {
      if (list_touched[k] == chord0c[i].c) {
        Keyboard.write(chord0c[i].s);
        break;
      }
    }
  }
}

void setup() {
  Keyboard.begin();
  sensors.begin(0x5A);
}

void loop() {
  curr_touched = sensors.touched();
  
  for (uint8_t i=0; i<12; i++) {
    uint8_t k = 0; // Side left or right
    uint8_t j = i; // Section convert to 1-6 value on each side
    if ((curr_touched & 0b110011001100 & (1 << i)) || (last_touched & 0b110011001100 & (1 << i))) { // Right side
      k = 1;
      j = j - 2;
    }
    if (j > 1) j = j - 2;
    if (j > 3) j = j - 2;
    j ++;
    j = j & 0b111;
    
    if ((curr_touched & (1 << i)) && ! (last_touched & (1 << i)) ) { // Touch
      if (list_touched && (j == 4)) {
        press4(k);
        index_touched[k] = 3;
        list_touched[k] = 4;
      } else {
        if (index_touched[k] < 15) {
          list_touched[k] |= (j << index_touched[k]);
          index_touched[k] = index_touched[k] + 3;
        } else {
          list_touched[k] = 0;
        }
      }
    }
    if (! (curr_touched & (1 << i)) && (last_touched & (1 << i)) ) { // Release
      if (! (curr_touched & (0b1100110011 << (k + k)))) { // All release
        if (list_touched && (index_touched[k] > 3)) {
          press0(k);
        }
        index_touched[k] = 0;
        list_touched[k] = 0;
      }
    }
  }

  last_touched = curr_touched;
}
