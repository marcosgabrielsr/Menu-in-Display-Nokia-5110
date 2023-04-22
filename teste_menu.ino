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

struct Menu{
  //nome da aba
  char name[10];
  //primeira coluna, a qual vai deter o nome dos itens ou opções
  char colum1[10][10];
  //segunda coluna, a qual vai deter valores inteiros indicando quantidades ou indices de porcentagem
  int8_t colum2[10];
  //valor adcional, sera o valor utilizado no segundo plano para contas
  int8_t valorAdd[10];
  //representa o indice referente ao array colum1, ou seja, qual opção selecionada
  int8_t index;
  //True para apresentar a caixa de notificação e false para não apresenta-la
  bool notifBox;
  //Define o tipo do menu
  char type;
};

struct Box{
  //nome da caixa de notificação
  char messageBox[10];
  //valor que sera apresentado na caixa de notificação
  int8_t valueBox;  
  //Eixo x do cursor
  int8_t cursorX;
  //Eixo y do cursor
  int8_t cursorY;
};

//Setando os dados em uma struct do tipo menu
Menu cardapio = { "Cardapio",
                  {"Morango", "Pera", "Banana", "Cereja", "sair"},
                  {5, 2, 10, 8},
                  {10, 20, 25, 12},
                  1,
                  true,
                  'D'
                };

Box card = { "Fome:",
                  100,
                  33,
                  39                 
                };

//Variaveis para controle de tempo
unsigned long delay1 = 0;
unsigned long delay2 = 0;

//serve para fazer controle do que será apresentado
bool showCardapio = true;

//declarando funções
void drawNotfBox(Box& notf);

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
  drawMenu(cardapio, card);
  
  //Atualiza o display
  display.clearDisplay();
  display.display();

  //Escreve esta mensagem quando o usuário sai do menu
  Serial.println("Finish");
}

//Esta função serve para navegar entre as funções do menu, lembrando que elas são definidas nas colunas da estrutura anterior
int8_t navegation(int8_t max, int8_t ind){
  //Esta variável serve para salvar os dados que serão retornados para cada função segundo o seu indíce
  static int8_t i[2] = {0, 0};

  //Nesta parte verificamos se o botão esquerdo foi apertado, para isso primeiro ele verifica se há um intervalo entre o clique anterior até
  //o momento atual
  if((millis() - delay1) > 100){
      if(buttonL.clickBtn()){
          if((i[ind] >= 0) && (i[ind] < max)){
              i[ind] = i[ind] + 1;
          } else {
              i[ind] = i[ind];
          }
        
          delay1 = millis();
      }

      if(buttonR.clickBtn()){
          if((i[ind] <= max) && (i[ind] > 0)){
              i[ind] = i[ind] - 1;
          } else {
              i[ind] = i[ind];
          }

          delay1 = millis();
      }
  }
  //retorna um valor de acordo com indice do menu
  return i[ind];
}

//Essa função serve para reduzir os valores da coluna 2 referente a um índice
void reduct(int8_t estoque, Box& v, Menu& n){
  //
  if((millis() - delay2) > 100){
      if(buttonX.clickBtn() &&
        (n.colum2[estoque] > 0) &&
        (v.valueBox > 0)
        ){
          n.colum2[estoque] -= 1;
          v.valueBox = v.valueBox - n.valorAdd[estoque];
          delay2 = millis();
      }      
  }
}

bool sair(bool show, int8_t vle, int8_t max){
  if((vle == max) && (buttonX.clickBtn())){
    Serial.println("Tentando Desligar");
    show = !show;
  }
  return show;
}

void drawMenu(Menu& m, Box& n){

  int8_t max = (sizeof(m.colum2)/sizeof(int)) - 1;
  Serial.println(max);

  while(showCardapio == true){
    int8_t value = navegation(max, m.index);

    reduct(value, n, m);

    display.drawRect(0, 0, 84, 11, BLACK);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(10,2);
    display.println(m.name);

    int8_t i;
    int8_t lim;

    if(m.notifBox == true){
      if(value <= 2){
        i = 0;
      } else {
        i = 2 + 1 * int8_t(value / 3);
      }

      lim = i + 2;
      drawNotfBox(n);

    } else {
      if(value <= 3){
        i = 0;
      } else {
        i = 2 + 2 * int8_t(value / 4);
      }
      lim = i + 3;
      Serial.print(i);
    }

    display.setCursor(0, 12);

    while(i <= lim){
      if(i <= max){
        if(value == i){
          display.print("-> "); display.print(m.colum1[i]);
          if(i == max){
            display.print("");
          } else {
            if(m.colum2[i] < 10){
              display.print(" x0"); display.println(m.colum2[i]);            
            } else {
              display.print(" x"); display.println(m.colum2[i]);
            }
          }
        } else if(value != i){
          display.print("> "); display.println(m.colum1[i]);
        }
      } else {
        display.print("");
      }
      i++;
    }

    showCardapio = sair(showCardapio, value, max);
    //Serial.print("teste string.length: ");
    //Serial.println(m.colum1[0].length());

    display.display();
    display.clearDisplay();
  }
}

void drawNotfBox(Box& notf){
  display.setTextSize(1);
  display.drawRect(0, 37, 84, 11, BLACK);
  display.setCursor(notf.cursorX, notf.cursorY);
  display.print(notf.messageBox); display.println(notf.valueBox);
}