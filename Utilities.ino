
const uint32_t SwitchCount = 1;
const uint32_t ButtonCount = 2;
const uint32_t LEDCount = 4;
const uint32_t Buttons[2] = { PE_0, PD_2 };
const uint32_t LEDs[4] = { PC_6, PC_7, PD_6, RED_LED };
const uint32_t Switch[1] = {PA_6};
const uint32_t ROW1_Y = 4;
const uint32_t ROW2_Y = 12;
const uint32_t ROW3_Y = 20;
const int LOWER_BOUND = 10;
const int UPPER_BOUND = 15;
const int LOWER_MARGIN_OF_ERROR = 7;
const int UPPER_MARGIN_OF_ERROR = 3;  
const int MAX_SPEED = 5;
const int NOTE_COUNT = 9;

struct note {
  int x;
  int y;
  boolean hit;
  boolean onScreen;
};

struct note notes[NOTE_COUNT];
unsigned long timeElapsed;
unsigned long startTime;
int prevState[3];
int speed;
int points;
int missed;
unsigned long timeHit[LEDCount];

void initVariables(){  
  timeElapsed = 0;
  startTime = millis();
  
  for (int i = 0; i < 3; i++){
    prevState[i] = 0;
  }
  
  for (int i = 0; i < 4; i++){
    timeHit[i] = 0;
  }
  
  speed = 2;
  points = 0;
  missed = 0;  
}

void initBoard(){
  OrbitOledInit();
  OrbitOledClear();
  OrbitOledClearBuffer();
  OrbitOledSetFillPattern(OrbitOledGetStdPattern(iptnSolid));
  OrbitOledSetDrawMode(modOledSet);
  
  for(int i = 0; i < ButtonCount; i++){
    pinMode(Buttons[i], INPUT);
  }
  
  for(int i = 0; i < LEDCount; i++){
    pinMode(LEDs[i], OUTPUT);
  }
  
  for(int i = 0; i < SwitchCount; i++){
    pinMode(Switch[i], INPUT);
  }  
}

void drawPoints(){
  //Print the points indicator on top right
  OrbitOledMoveTo(110, 0);
  char scoreBuffer[10];
  itoa(points, scoreBuffer, 10);
  OrbitOledDrawString(scoreBuffer);
}

void drawGoalLine(){
  //Draws the goal line  
  OrbitOledMoveTo(10, 0);
  OrbitOledLineTo(10, 32);
  OrbitOledMoveTo(15, 0);
  OrbitOledLineTo(15, 32);    
}

void initNotes(){
  for (int i = 0; i < NOTE_COUNT; i ++){
    notes[i].x = -1;
    notes[i].y = -1;
    notes[i].hit = 0;
    notes[i].onScreen = false;
  }
}

void generateNotes(){
  //for each note with x < 0, randomly generate a new x and y and reset
  for (int i = 0; i < NOTE_COUNT - MAX_SPEED + speed; i ++){
    if(notes[i].x < 0){      
      
      int randomx;      
      int randomy = (rand() % 3);      
      boolean repeat = false;
      
      do
      {
        //generate a new x coordinate to the right of the screen for each note such that every note is spaced out by at least 15 pixels
        randomx =  (1 + (rand() % 11)) * 15 + 120;
        repeat = false;
        for(int i=0; i<NOTE_COUNT;i++){
          if(timeElapsed < 20000){
            if(abs(randomx-notes[i].x)<=20){
              repeat = true;
              break;
            }
          }
          else{
            if(abs(randomx-notes[i].x)<=20 && randomx!=notes[i].x){
              repeat = true;
              break;
            }
          }
        }
      }while(repeat);

      notes[i].x = randomx;

      //generate a new y coordinate for each note
      switch(randomy){
        case 0:
          notes[i].y = ROW1_Y;    
          break;
        case 1:
          notes[i].y = ROW2_Y;
          break;
        case 2:
          notes[i].y = ROW3_Y;
          break;
      }
      
      notes[i].hit = false;
      notes[i].onScreen = true;
    }
  }
}

void changeSpeed(){
  //increment speed every 10 seconds (capped at MAX_SPEED)
  if(speed <= MAX_SPEED){
    speed =+ timeElapsed/10000;  
  }
}

void moveNotes(){
  for(int i = 0; i < NOTE_COUNT; i ++){
      notes[i].x -= speed;
  }
}

void drawNotes(){
  //draw all notes on screen
  for (int i = 0; i < NOTE_COUNT; i++){
    if(notes[i].onScreen){
        OrbitOledMoveTo(notes[i].x, notes[i].y);
        OrbitOledDrawString("o");
    }
  }
}

void detectNotes(){
  
  int buttonPressed[ButtonCount];
  int switchOn[SwitchCount];
  
  for (int i = 0; i < SwitchCount; i++){
    switchOn[i] = digitalRead(Switch[i]);
  }
  
  for (int i = 0; i < ButtonCount; i++){
    buttonPressed[i] = digitalRead(Buttons[i]);
  }

  //turns red LED off after 100 ms
  for (int i = 0; i < LEDCount; i++){
    if(timeElapsed - timeHit[i] > 100){
      digitalWrite(LEDs[i], LOW);
    }
  }

  // when button is pressed, check if a note can be scored
  for (int i = 0; i < NOTE_COUNT; i ++){
    
    // turn red LED on if note is missed
    if(notes[i].x < LOWER_BOUND - LOWER_MARGIN_OF_ERROR && notes[i].x >= 0 && !notes[i].hit){
      missed++;
      digitalWrite(LEDs[3], HIGH);  
      timeHit[3] = timeElapsed;
      notes[i].hit = true;
    }

    // detects for properly scored notes and accumulate points
    switch(notes[i].y){
      case ROW1_Y:
        if(buttonPressed[0] && (prevState[0]!= buttonPressed[0]) && (notes[i].x > LOWER_BOUND - LOWER_MARGIN_OF_ERROR) 
        && (notes[i].x < UPPER_BOUND + UPPER_MARGIN_OF_ERROR) && notes[i].hit == false){
          digitalWrite(LEDs[0], HIGH);
          timeHit[0] = timeElapsed;
          notes[i].hit = true;
          notes[i].onScreen = false;
          points += 1;
        }
        break;
      
      case ROW2_Y:
        if(buttonPressed[1] && (prevState[1]!= buttonPressed[1]) && (notes[i].x > LOWER_BOUND - LOWER_MARGIN_OF_ERROR) 
        && (notes[i].x < UPPER_BOUND + UPPER_MARGIN_OF_ERROR) && notes[i].hit == false){
          digitalWrite(LEDs[1], HIGH);
          timeHit[1] = timeElapsed;
          notes[i].hit = true;
          notes[i].onScreen = false;
          points += 1;
        }
        break;
        
      case ROW3_Y:
        if(prevState[2]!= switchOn[0] && (notes[i].x > LOWER_BOUND - LOWER_MARGIN_OF_ERROR) 
          && (notes[i].x < UPPER_BOUND + UPPER_MARGIN_OF_ERROR) && notes[i].hit == false){
            digitalWrite(LEDs[2], HIGH);
            timeHit[2] = timeElapsed;
            notes[i].hit = true;
            notes[i].onScreen = false;
            points += 1;
        }
        break; 
    }      
  }

  // assigns current state to prevState so that player can't hold down a button to score points
  prevState[0] = buttonPressed[0];
  prevState[1] = buttonPressed[1];
  prevState[2] = switchOn[0];
  
}

void handleGameOver(){
  digitalWrite(LEDs[3], LOW); 
  
  int buttonPressed = digitalRead(Buttons[1]);
  char scoreBuffer[10];
  char gameOverString[] = "Your score: ";
  strcat(gameOverString, itoa(points, scoreBuffer, 10));
  
  OrbitOledMoveTo(5, 0);
  OrbitOledDrawString(gameOverString);

  OrbitOledMoveTo(5, 15);
  OrbitOledDrawString("Restart: BTN1");
  OrbitOledUpdate();
  
  if(buttonPressed)
  {
    OrbitOledClearBuffer();
    OrbitOledClear();
    CurrentPage = Game;
    initNotes();
    initVariables();
  }  
}

void handlePageInstructions(){
  OrbitOledMoveTo(5, 0);
  OrbitOledDrawString("BTN2 - TOP");
  OrbitOledMoveTo(5, 12);
  OrbitOledDrawString("BTN1 - MID");
  OrbitOledMoveTo(5, 24);
  OrbitOledDrawString("SW2 - BOT");
  OrbitOledUpdate();
  
  int buttonPressed = digitalRead(Buttons[1]);
  
  if(buttonPressed && !prevState[0])
  {
    OrbitOledClearBuffer();
    OrbitOledClear();
    OrbitOledUpdate();
    CurrentPage = Game;
    initVariables();
  }
  
  prevState[0] = buttonPressed;  
}

void handlePageWelcome(){
  
  OrbitOledMoveTo(25, 0);
  OrbitOledDrawString("Welcome to");
  OrbitOledMoveTo(25, 12);
  OrbitOledDrawString("OrbitNotes");
  OrbitOledMoveTo(12, 24);
  OrbitOledDrawString("BTN1 to start");
  
  int buttonPressed = digitalRead(Buttons[1]);
  OrbitOledUpdate();

  if(buttonPressed)
  {
    OrbitOledClearBuffer();
    OrbitOledClear();
    OrbitOledUpdate();
    CurrentPage = Instructions;
    prevState[0] = buttonPressed;
  }  
}

void handleGame(){
  timeElapsed = millis() - startTime;
  changeSpeed();
  generateNotes();
  detectNotes();
  
  if(timeElapsed % 70 == 0){
      OrbitOledClearBuffer();
      OrbitOledClear();   
      moveNotes();
      drawNotes();
      drawGoalLine();
      drawPoints();
      OrbitOledUpdate(); 
  }
  
  if(missed >= 5){
    CurrentPage = GameOver;
    OrbitOledClearBuffer();  
    OrbitOledClear();
    OrbitOledUpdate();
  }
}
