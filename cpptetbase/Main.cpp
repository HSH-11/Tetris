#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <termios.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "colors.h"
#include "Matrix.h"

using namespace std;


/**************************************************************/
/**************** Linux System Functions **********************/
/**************************************************************/

char saved_key = 0;
int tty_raw(int fd);	/* put terminal into a raw mode */
int tty_reset(int fd);	/* restore terminal's mode */
  
/* Read 1 character - echo defines echo mode */
char getch() {
  char ch;
  int n;
  while (1) {
    tty_raw(0);
    n = read(0, &ch, 1);
    tty_reset(0);
    if (n > 0)
      break;
    else if (n < 0) {
      if (errno == EINTR) {
        if (saved_key != 0) {
          ch = saved_key;
          saved_key = 0;
          break;
        }
      }
    }
  }
  return ch;
}

void sigint_handler(int signo) {
  // cout << "SIGINT received!" << endl;
  // do nothing;
}

void sigalrm_handler(int signo) {
  alarm(1);
  saved_key = 's';
}

void registerInterrupt() {
  struct sigaction act, oact;
  act.sa_handler = sigint_handler;
  sigemptyset(&act.sa_mask);
#ifdef SA_INTERRUPT
  act.sa_flags = SA_INTERRUPT;
#else
  act.sa_flags = 0;
#endif
  if (sigaction(SIGINT, &act, &oact) < 0) {
    cerr << "sigaction error" << endl;
    exit(1);
  }
}

void registerAlarm() {
  struct sigaction act, oact;
  act.sa_handler = sigalrm_handler;
  sigemptyset(&act.sa_mask);
#ifdef SA_INTERRUPT
  act.sa_flags = SA_INTERRUPT;
#else
  act.sa_flags = 0;
#endif
  if (sigaction(SIGALRM, &act, &oact) < 0) {
    cerr << "sigaction error" << endl;
    exit(1);
  }
  alarm(1);
}

/**************************************************************/
/**************** Tetris Blocks Definitions *******************/
/**************************************************************/
#define MAX_BLK_TYPES 7
#define MAX_BLK_DEGREES 4

int T0D0[] = { 1, 1, 1, 1, -1 };
int T0D1[] = { 1, 1, 1, 1, -1 };
int T0D2[] = { 1, 1, 1, 1, -1 };
int T0D3[] = { 1, 1, 1, 1, -1 };

int T1D0[] = { 0, 1, 0, 1, 1, 1, 0, 0, 0, -1 };
int T1D1[] = { 0, 1, 0, 0, 1, 1, 0, 1, 0, -1 };
int T1D2[] = { 0, 0, 0, 1, 1, 1, 0, 1, 0, -1 };
int T1D3[] = { 0, 1, 0, 1, 1, 0, 0, 1, 0, -1 };

int T2D0[] = { 1, 0, 0, 1, 1, 1, 0, 0, 0, -1 };
int T2D1[] = { 0, 1, 1, 0, 1, 0, 0, 1, 0, -1 };
int T2D2[] = { 0, 0, 0, 1, 1, 1, 0, 0, 1, -1 };
int T2D3[] = { 0, 1, 0, 0, 1, 0, 1, 1, 0, -1 };

int T3D0[] = { 0, 0, 1, 1, 1, 1, 0, 0, 0, -1 };
int T3D1[] = { 0, 1, 0, 0, 1, 0, 0, 1, 1, -1 };
int T3D2[] = { 0, 0, 0, 1, 1, 1, 1, 0, 0, -1 };
int T3D3[] = { 1, 1, 0, 0, 1, 0, 0, 1, 0, -1 };

int T4D0[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, -1 };
int T4D1[] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 };
int T4D2[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, -1 };
int T4D3[] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 };

int T5D0[] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
int T5D1[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, -1 };
int T5D2[] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
int T5D3[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, -1 };

int T6D0[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
int T6D1[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
int T6D2[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
int T6D3[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
  
int *setOfBlockArrays[] = {
  T0D0, T0D1, T0D2, T0D3,
  T1D0, T1D1, T1D2, T1D3,
  T2D0, T2D1, T2D2, T2D3,
  T3D0, T3D1, T3D2, T3D3,
  T4D0, T4D1, T4D2, T4D3,
  T5D0, T5D1, T5D2, T5D3,
  T6D0, T6D1, T6D2, T6D3,
};

void drawScreen(Matrix *screen, int wall_depth)
{
  int dy = screen->get_dy();
  int dx = screen->get_dx();
  int dw = wall_depth;
  int **array = screen->get_array();

  for (int y = 0; y < dy - dw + 1; y++) {
    for (int x = dw - 1; x < dx - dw + 1; x++) {
      if (array[y][x] == 0)
	      cout << "□ ";
      else if (array[y][x] == 1)
	      cout << "■ ";
      else if (array[y][x] == 10)
	      cout << "◈ ";
      else if (array[y][x] == 20)
	      cout << "★ ";
      else if (array[y][x] == 30)
	      cout << "● ";
      else if (array[y][x] == 40)
	      cout << "◆ ";
      else if (array[y][x] == 50)
	      cout << "▲ ";
      else if (array[y][x] == 60)
	      cout << "♣ ";
      else if (array[y][x] == 70)
	      cout << "♥ ";
      else
	      cout << "X ";
    }
    cout << endl;
  }
}
  
/**************************************************************/
/******************** Tetris Main Loop ************************/
/**************************************************************/

#define SCREEN_DY  10
#define SCREEN_DX  10
#define SCREEN_DW  3 
#define ARRAY_DY (SCREEN_DY + SCREEN_DW)
#define ARRAY_DX (SCREEN_DX + 2*SCREEN_DW)

int arrayScreen[ARRAY_DY][ARRAY_DX] = {
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 ,1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  

};
//
void deleteFullLines(Matrix *oScreen){
  Matrix *temp_Screen = new Matrix((int *)arrayScreen, ARRAY_DY, ARRAY_DX);
  Matrix *sub = temp_Screen->clip(0,0,1,temp_Screen->get_dx());
  
    for (int i = 9; i >= 0; i--){
      Matrix *line = oScreen->clip(i,0,i+1,oScreen->get_dx());
      if (line->sum() == oScreen->get_dx()){
          Matrix *temp = oScreen->clip(0,0,i,oScreen->get_dx());
          oScreen->paste(temp,1,0);
          oScreen->paste(sub,0,0);
          i++;
          delete temp;
      }
    delete line;
    }
  delete temp_Screen;
  delete sub;
}
int main(int argc, char *argv[]) {
  char key;
  int top = 0, left = 6;
  //난수 발생 함수
  srand((unsigned int)time(NULL));
  //최초 난수 발생
  int blkType = rand() % MAX_BLK_TYPES;
  int blkDegree = 0;
  bool newBlock = false;
  //Matrix *setOfBlockObjects[7][4] 배열을 정의하고, 28개 객체를 new 로 할당 받아, 각 원소에 지정할 것.
  Matrix *setOfBlockObjects[7][4];
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 4; j++) {
      if (i == 0)
      {setOfBlockObjects[i][j] = new Matrix((int *)setOfBlockArrays[i*4+j],2,2);}
      else if(i < 6 && i > 0)
      {setOfBlockObjects[i][j] = new Matrix((int *)setOfBlockArrays[i*4+j],3,3);}
      else
      {setOfBlockObjects[i][j] = new Matrix((int *)setOfBlockArrays[i*4+j],4,4);}
    }
  }
 
  Matrix *iScreen = new Matrix((int *) arrayScreen, ARRAY_DY, ARRAY_DX); //생성자 함수는 1차원 받기로 해서 arrayscreen을 1차원 주소로 넘겨주기 위해 포인터로 형변환
  Matrix *currBlk = setOfBlockObjects[blkType][blkDegree];
  Matrix *tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
  Matrix *tempBlk2 = tempBlk->add(currBlk);
  delete tempBlk;
  
  Matrix *oScreen = new Matrix(iScreen);
 
  oScreen->paste(tempBlk2, top, left);
  delete tempBlk2;  
 
  drawScreen(oScreen, SCREEN_DW);
  delete oScreen;
  
  while ((key = getch()) != 'q')  {
    switch (key) {
      case 'a': left--; break;
      case 'd': left++; break;
      case 's': top++; break;
      case 'w':{
        blkDegree++;
        int idxBlockDegree = (blkDegree) % 4;
        currBlk = setOfBlockObjects[blkType][idxBlockDegree];
        break;
      }
      case ' ': {
        while(1){
          top++;
          tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
          tempBlk2 = tempBlk->add(currBlk);
          delete tempBlk;
          if (tempBlk2->anyGreaterThan(1)){
            delete tempBlk2;
            break;
          }
          delete tempBlk2;
        }
      break;}
      default: cout << "wrong key input" << endl;
    }
    tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
    tempBlk2 = tempBlk->add(currBlk); 
    delete tempBlk;
    
    if (tempBlk2->anyGreaterThan(1)){
      delete tempBlk2; //덧셈의 결과 중 하나라도 2가 있다면 원상복구
      switch (key) {
      case 'a': left++; break;
      case 'd': left--; break;
      case 's': {top--; 
        newBlock = true; 
        break;
      }
      case 'w': {
        blkDegree--;
        int idxBlockDegree = (blkDegree) % 4;
        currBlk = setOfBlockObjects[blkType][idxBlockDegree];
        break;
      } //회전을 하고 충돌 테스트를 했는데 충돌이면 degree를 -1해서 원상 복구 시키기
      case ' ': {
        top--;
        newBlock = true;
        break;} //drawscreen을 매번 하지 않기
      default: cout << "wrong key input" << endl;
    }
    tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx()); //바꿔준 좌표를 이용해 뜯어냄
    tempBlk2 = tempBlk->add(currBlk);
    delete tempBlk;
    
    }
 
  oScreen = new Matrix(iScreen);
  oScreen->paste(tempBlk2, top, left);
  delete tempBlk2;
  drawScreen(oScreen, SCREEN_DW);
  //마지막 라인 지우는 거는 만약 맨 밑줄이 다 채워 졌다하면 그 줄의 위 전체를 클립해서 붙여넣기 함
  if (newBlock == true){
    deleteFullLines(oScreen);
    delete iScreen;
    iScreen = new Matrix(oScreen);
    delete oScreen;
    top = 0;
    left = 6;
    newBlock = false;
    blkType = rand() % MAX_BLK_TYPES;
    blkDegree = 0;

    currBlk = setOfBlockObjects[blkType][blkDegree];
    tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
    tempBlk2 = tempBlk->add(currBlk);
    delete tempBlk;
    if (tempBlk2->anyGreaterThan(1)){
      cout << "Game Over" << endl;
      delete tempBlk2;
      break;
    }
    oScreen = new Matrix(iScreen);
    oScreen->paste(tempBlk2, top, left);
    delete tempBlk2;
    drawScreen(oScreen, SCREEN_DW); 
  }
  delete oScreen;
} 
  for (int i = 0; i < 7; i++){
    for (int j = 0; j < 4; j++){
      delete setOfBlockObjects[i][j];
    }
  }
  delete iScreen;
  cout << "(nAlloc, nFree) = (" << Matrix::get_nAlloc() << ',' << Matrix::get_nFree() << ")" << endl;  
  cout << "Program terminated!" << endl;

  return 0;
}

