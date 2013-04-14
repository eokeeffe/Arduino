/*
  X's and O's for arduino
 */

#include<ctypes.h>

char C_board[9];
int N_board[9];

boolean human=false;

void setup()
{
  int i;
  for( i=0;i<9;i++)
  {
    C_board[i]=0;
    N_board[i]=0; 
  }

  //Begin Serial Communications
  Serial.begin(9600);

  Serial.println("Initializing Game");
}

void loop()
{
  int pos=0;

  if(human)
  {
    human = false;
    pos = playerMove();
    refreshBoard(pos);
  }
  else
  {
    human = true;
    pos = computerMove(); 
    refreshBoard(pos);
  }

}

int playerMove()
{
  int player = 0;
  int pos = 0;
  
  Serial.print("Please enter a move");
  
  while(player!='\r')
  {
    player = Serial.read();
    
    if(player!='\r')
    {
       pos = player; 
    }
  }
  Serial.println(" ");
  
  pos -= 48;
  
  if(
  pos != 0 && 
  pos != 1 &&
  pos != 2 &&
  pos != 3 &&
  pos != 4 &&
  pos != 5 &&
  pos != 6 &&
  pos != 7 &&
  pos != 8 
  )
  {
     Serial.println("Invalid character, please repeat");
     playerMove(); 
  }
  
  return pos;
}

void refreshBoard(int pos)
{

  Serial.print("Pos=");
  Serial.println(pos);
  //print the first line
  Serial.print('|');
  Serial.print(C_board[0]);
  Serial.print('|');
  Serial.print(C_board[1]);
  Serial.print('|');
  Serial.print(C_board[2]);
  Serial.println('|');
  //print the second line
  Serial.print('|');
  Serial.print(C_board[3]);
  Serial.print('|');
  Serial.print(C_board[4]);
  Serial.print('|');
  Serial.print(C_board[5]);
  Serial.println('|');
  //print the third line
  Serial.print('|');
  Serial.print(C_board[6]);
  Serial.print('|');
  Serial.print(C_board[7]);
  Serial.print('|');
  Serial.print(C_board[8]);
  Serial.println('|');
}

int computerMove()
{
  int i;

  for(i=0;i<3;i++)
  {

  }

  return 0;
}

