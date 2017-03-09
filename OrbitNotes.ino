#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>

#include <stdbool.h>
#include <string.h>

void initBoard();
void initNotes();
void handlePageWelcome();
void handlePageInstructions();
void handleGame();
void handleGameOver();

static enum GamePages
{
  Welcome = 0,
  Game = 1,
  Instructions = 2,
  GameOver = 3,
} CurrentPage = Welcome;

void setup() {
  initBoard();
  initNotes();
}

void loop() {
  switch(CurrentPage)
  {
  case Welcome:
    handlePageWelcome();
    break;
  case Instructions:
    handlePageInstructions();
    break;    
  case Game:
    handleGame();
    break;
  case GameOver:
    handleGameOver();
    break;
  }
}
