//
// Classe PushButton
//

#include "PushButton.hpp"

    PushButton::PushButton(const uint8_t pin):
    _pin(pin)
    {
         pinMode(_pin, INPUT_PULLUP);
    }

    bool PushButton::pressBtn(){
      bool stateButton = digitalRead(_pin);
      return stateButton;
    }

    bool PushButton::clickBtn(){
      static bool oldState = true;
      bool stateBtn = digitalRead(_pin);
      bool value = false;

      if(oldState && !stateBtn){
        value = true;
      }
      oldState = stateBtn;

      return value;
    }