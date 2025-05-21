// переменные двигателей
#define RM_1 5    // IN1 - правый мотор, первый вывод
#define RM_2 3    // IN2 - правый мотор, второй вывод
#define LM_1 9    // IN3 - левый мотор, первый вывод
#define LM_2 6    // IN4 - левый мотор, второй вывод

// переменные кнопки
#define POS_1 4 // Позиция 1 - кнопка выбора автоматического режима
#define POS_2 2 // Позиция 2 - кнопка выбора ручного режима

// переменные ультразвуковых дальномеров
#include <Ultrasonic.h>
//переименование пинов УЗ датчика (левый)
#define TRIG_L 11 // Пин TRIG левого датчика
#define ECHO_L 10 // Пин ECHO левого датчика
//переименование пинов УЗ датчика (правый)
#define TRIG_R 13 // Пин TRIG правого датчика
#define ECHO_R 12 // Пин ECHO правого датчика
long  cm_R, cm_L; // Переменные для хранения расстояний (в см) с датчиков
int duration;     // Переменная для хранения длительности импульса

// Параметры управления
int time=1800;    // Время выполнения поворота (мс)
int speedR=255;   // Скорость правого мотора (0-255)
int speedL=243;   // Скорость левого мотора (0-255)
int n=3;          // Количество витков спирали в ручном режиме

void setup() {
  Serial.begin(9600); // Инициализация последовательного порта

  // Настройка пинов моторов как выходы
  pinMode(LM_1, OUTPUT);
  pinMode(LM_2, OUTPUT);
  pinMode(RM_1, OUTPUT);
  pinMode(RM_2, OUTPUT);

  // Настройка пинов ультразвуковых датчиков
  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);
  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);

  // Настройка пинов кнопок с подтяжкой к питанию
  pinMode(POS_1, INPUT_PULLUP);
  pinMode(POS_2, INPUT_PULLUP);

  delay(1000); // Пауза для стабилизации
}

// Функция измерения расстояний ультразвуковыми датчиками
void UZ(){
  // Измерение правым датчиком
  digitalWrite(TRIG_R, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_R, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_R, LOW);
  duration = pulseIn(ECHO_R, HIGH); // Измерение длительности отраженного сигнала
  cm_R = duration*0.034/2; // Перевод в сантиметры

  // Вывод расстояния справа
  Serial.print("Расстояние до препятствия справа= ");
  Serial.print(cm_R);
  Serial.println(" см");
  delay(15);

  // Измерение левым датчиком
  digitalWrite(TRIG_L, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_L, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_L, LOW);
  duration = pulseIn(ECHO_L, HIGH);
  cm_L = duration*0.034/2;

  // Вывод расстояния слева
  Serial.print("Расстояние до препятствия слева= ");
  Serial.print(cm_L);
  Serial.println(" см");
  delay(15);

  Serial.println(); // Пустая строка для разделения
  delay(1000); // Пауза между измерениями
}

// Функция поворота вправо:
void R(){
  // Правый мотор назад, левый вперед
  analogWrite(RM_1, 0);
  analogWrite(RM_2, speedR);
  analogWrite(LM_1, speedL);
  analogWrite(LM_2, 0);
  delay(time); // Время поворота
  OFF(); // Остановка моторов
}

// Функция поворота влево:
void L(){
  // Правый мотор вперед, левый назад
  analogWrite(RM_1, speedR);
  analogWrite(RM_2, 0);
  analogWrite(LM_1, 0);
  analogWrite(LM_2, speedL);
  delay(time); // Время поворота
  OFF(); // Остановка моторов
}

// Функция движения вперед:
void V(){
  // Оба мотора вперед
  analogWrite(RM_1, speedR);
  analogWrite(RM_2, 0);
  analogWrite(LM_1, speedL);
  analogWrite(LM_2, 0);
}

// Функция движения назад:
void N(){
  // Оба мотора назад
  analogWrite(RM_1, 0);
  analogWrite(RM_2, speedR);
  analogWrite(LM_1, 0);
  analogWrite(LM_2, speedL);
}

// Функция выключения двигателей:
void OFF(){
  // Все выводы моторов в 0
  analogWrite(RM_1, 0);
  analogWrite(RM_2, 0);
  analogWrite(LM_1, 0);
  analogWrite(LM_2, 0);
}

// Функция разворота при препятствии
void RZ(){
  N(); delay(time); // Движение назад
  OFF(); delay(time); // Пауза
  R(); R(); // Два поворота вправо (разворот)
}

// Автоматический режим работы
void auto_mode(){
  UZ(); // Измерение расстояний

  // Нет препятствия - движение вперед
  if (cm_L >= 10 && cm_R >= 10) {
    V();
  }

  // Препятствие слева - объезд вправо
  if (cm_R > 25 && cm_L < 25) {
    R();
  }

  // Препятствие справа - объезд влево
  if (cm_R < 25 && cm_L > 25) {
    L();
  }

  // Препятствие впереди - разворот
  if (cm_L < 25 && cm_R < 25) {
    RZ();
  }
}

// Ручной режим работы (движение по спирали)
void manual_mode(){
  // Движение от центра по спирали
  for(int i=1; i<=n; i++){
    for(int f=0; f<i; f++){
      V(); // Движение вперед
      delay(1000);
    }
    R(); // Поворот направо
    for(int f=0; f<i; f++){
      V(); // Движение вперед
      delay(1000);
    }
    R(); // Поворот направо
  }

  // Разворот
  R();

  // Движение к центру по спирали
  for(int i=n; i>=1; i--){
    for(int f=0; f<i; f++){
      V(); // Движение вперед
      delay(1000);
    }
    L(); // Поворот налево
    for(int f=0; f<i; f++){
      V(); // Движение вперед
      delay(1000);
    }
    L(); // Поворот налево
  }
}

void loop() {
  // Проверка состояния кнопок и выбор режима работы
  if(!digitalRead(POS_1)==1){    // Если нажата кнопка POS_1
    auto_mode();                 // Включить автоматический режим
  }
  else if(!digitalRead(POS_2)==1){ // Если нажата кнопка POS_2
    manual_mode();                // Включить ручной режим
  }
  else{
    OFF();                       // Иначе - остановка
  }
}
