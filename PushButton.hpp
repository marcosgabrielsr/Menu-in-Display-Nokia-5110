//
// Classe PushButtons
//

#ifndef PUSHBUTTON
#define PUSHBUTTON

#include <Arduino.h>

class PushButton {

  public:
    PushButton(const uint8_t pin);

    bool pressBtn();
    bool clickBtn();

  private:
    const uint8_t _pin;
};

#endif
