//incluindo bibliotecas
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "PushButton.hpp"

//definindo pinos dos botões
#define pinBtn1 10
#define pinBtn2 9
#define pinBtn3 8

//criando objetos PushButton
PushButton buttonL(pinBtn1);
PushButton buttonX(pinBtn2);
PushButton buttonR(pinBtn3);

//criando objeto display
Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 6, 7);

//criando listas de itens
String fruits[] = {"Morango", "Pera", "Banana", "Cereja", "sair"};
byte quantidades[] = {5, 2, 10, 8};
byte valorNutri[] = {10, 20, 25, 12};

int8_t fome = 100;

struct menu{
  //nome da aba
  String name;
  //primeira coluna, a qual vai deter o nome dos itens ou opções
  String colum1[10];
  //segunda coluna, a qual vai deter valores inteiros indicando quantidades ou indices de porcentagem
  int8_t colum2[10];
  //valor adcional, sera o valor utilizado no segundo plano para contas
  int8_t valorAdd[10];
  //valor minimo, nesse caso se refere a menor quantidad de itens
  int8_t vleMin;
  //valor máximo, representa a quantidade total de itens inseridos
  int8_t vleMax;
  //representa o indice referente ao array colum1
  int8_t index;
  //True para apresentar a caixa de notificação e false para não apresenta-la
  bool notifBox;
  };

struct box{
  //nome da caixa de notificação
  String nameBox;
  //valor que sera apresentado na caixa de notificação
  int8_t valueBox;  
};

//Setando os dados em uma struct do tipo menu
menu Cardapio = { "Cardapio",
                  {"Morango", "Pera", "Banana", "Cereja", "exit"},
                  {5, 2, 10, 8},
                  {10, 20, 25, 12},
                  0, 
                  4,
                  1,
                  true};

//Variaveis para controle de tempo
unsigned long delay1 = 0;
unsigned long delay2 = 0;

//serve para fazer controle do que será apresentado
bool showCardapio = true;

void setup(){
  //Iniciando monitor serial
  Serial.begin(9600);

  //Iniciando display e definindo seu contraste
  display.begin();
  display.setContrast(57);
  display.clearDisplay();
}

void loop(){
  //Serve para desenhar o menu
  drawMenu();
  
  //Atualiza o display
  display.clearDisplay();
  display.display();
}


int8_t navegation(int8_t min, int8_t max, int8_t ind){
  static int8_t i[2] = {0, 0};

  if((millis() - delay1) > 100){
      if(buttonL.clickBtn()){
          if((i[ind] >= min) && (i[ind] < max)){
              i[ind] = i[ind] + 1;
          } else {
              i[ind] = i[ind];
          }
        
          delay1 = millis();
      }

      if(buttonR.clickBtn()){
          if((i[ind] <= max) && (i[ind] > min)){
              i[ind] = i[ind] - 1;
          } else {
              i[ind] = i[ind];
          }

          delay1 = millis();
      }

  }
  return i[ind];
}

void reduct(int8_t estoque){
  if((millis() - delay2) > 100){
      if(buttonX.clickBtn() &&
        (quantidades[estoque] > 0) &&
        (fome > 0)
        ){
          quantidades[estoque] -= 1;
          fome = fome - valorNutri[estoque];
          delay2 = millis();
      }      
  }
}

bool exit(bool show, int8_t vle, int8_t Max){
  if(buttonX.clickBtn() && vle == Max){
    show = !show;
  }
  return show;
}

void drawMenu(){
  while(showCardapio == true){
    
    int mult = 0;

    int8_t value = navegation(Cardapio.vleMin, Cardapio.vleMax, Cardapio.index);
    reduct(value);
    showCardapio = exit(showCardapio, value, Cardapio.vleMax);

    static int8_t ip = 0;

    if((value - 2) > 0){
        ip = value - 2;
    } else {
      ip = 0;
    }

    display.drawRect(0, 0, 84, 11, BLACK);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(10,2);
    display.println("Cardapio");

    display.setCursor(0, 12);

    for(int i = ip; i < (3 + ip); i++){
      
      if(value == i){
        display.print("-> ");display.println(fruits[i]);
      } else {
        display.print("> ");display.println(fruits[i]);
      }

    }

    for(int i = ip; i < (3 + ip); i++){

      int8_t y = 12 + (8 * mult);

      display.setCursor(65, y);    
      
      if(i == Cardapio.vleMax){
        display.print("");
      } else {
        if(quantidades[i] < 10){
          display.print("x0"); display.println(quantidades[i]);
        } else {
          display.print("x"); display.println(quantidades[i]);
        }
      }

      mult += 1;

    }
    drawNotfBox();

    display.display();
    display.clearDisplay();
  }
}

void drawNotfBox(){
  display.setTextSize(1);
  display.drawRect(0, 37, 84, 11, BLACK);
  display.setCursor(33, 39);
  display.print("Fome:"); display.println(fome);
}