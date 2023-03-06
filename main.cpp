#include <SFML/Graphics.hpp>
#include <time.h>
#include<iostream>
#include<string>
using namespace sf;

bool youLost = false;
const int M = 20;
const int N = 10;
//This is used to define the floor! where the blocks should stop going down
int field[M][N] = {0};

bool newLine = false;
int countLines = 0;
int level = 1.;
double speedFactor = 0.3 - 0.00140175*level + 0.0015093*level*level;
Text textInfo;

int colorNum1; int colorNum2;
int offsetX = N + 3;int offsetY = M/2 + 1;
// Declare and load a font
Font font;

struct Point
  {int x,y;} a[4], b[4], c[4];

int figures[7][4] =
{
  1,3,5,7, // I
  2,4,5,7, // Z
  3,5,4,6, // S
  3,5,4,7, // T
  2,3,5,7, // L
  3,5,7,6, // J
  2,3,4,5, // O
};


//OSS: This is not to let the blocks go out of the window
bool checkBorders(){
   for(int i=0;i<4;i++)
      if ( a[i].x<0 || a[i].x>=N || a[i].y>=M) return 0;
      else if(field[a[i].y][a[i].x]) return 0;
  return 1;
};

bool checkLost(){
   for(int i=0;i<4;i++)
      if ( a[i].y<0 ){ std::cout<<"ciao"<<std::endl;return 1;}
   return 0;
};

void reset(Event event){

  youLost= false;
  countLines = 0;
  level = 1;
  speedFactor = 0.3 - 0.00140175*level + 0.0015093*level*level;

  colorNum1=1+rand()%7;
  int n=rand()%7;
  for(int i=0;i<4;i++){
     a[i].x = (figures[n][i] % 2) + 4;
     a[i].y = figures[n][i] / 2;
  }
  int m=rand()%7;
  colorNum2=1+rand()%7;
  for(int i=0;i<4;i++){
       c[i].x = (figures[m][i] % 2) + offsetX;
       c[i].y = figures[m][i] / 2 + offsetY;
  }

}


int main(){
  //Not sure why I need this
   srand(time(0));
//Open a window: VideoMode(int width,int height,int bitsPerPixel)
   RenderWindow window(VideoMode(320,480), "The Game!");
//Texture is an image living in the graphics
   Texture t1,t2,t3,t4;
   t1.loadFromFile("Images/tiles.png");
   t2.loadFromFile("Images/background.png");
   t3.loadFromFile("Images/frame.png");

   font.loadFromFile("arial/arial.ttf");


//Sprite is a Drawable representation of a texture, with its own transformations, color, etc.
   Sprite s(t1), background(t2), frame(t3);
   //s.setTextureRect(IntRect(0,0,18,18));

   //this is to move the tetris blocks
   int dx=0; bool rotate=0;
   float timer=0,delay=0.3;

   Clock clock;

  //FDG: Added this to generate the first and second random tetris figure
    int n=rand()%7;
    colorNum1=1+rand()%7;
    for(int i=0;i<4;i++){
       a[i].x = (figures[n][i] % 2) + 4;
       a[i].y = figures[n][i] / 2;
    }
    int m=rand()%7;
    colorNum2=1+rand()%7;
    for(int i=0;i<4;i++){
       c[i].x = (figures[m][i] % 2) + offsetX;
       c[i].y = figures[m][i] / 2 + offsetY;
    }

   while(window.isOpen()){
     //asSeconds() is a method of the class Time, getElapsedTime() return a Time object
     //this 3 lines are to count time in seconds
      float time = clock.getElapsedTime().asSeconds();
      clock.restart();
      timer+=time;


      RectangleShape infoBorder(Vector2f(4.5*18,7*18));
      infoBorder.setFillColor(Color(0,0,0,150));
      infoBorder.setOutlineThickness(0);
      infoBorder.setPosition((N+3)*18,(M/2-7)*18);

      textInfo.setString("Lines:\n   " + std::to_string(countLines) +"\n"
                        + "Level:\n   " + std::to_string(level));
      textInfo.setFont(font);
      textInfo.setCharacterSize(27);
      textInfo.setStyle(sf::Text::Bold);
      textInfo.setFillColor(sf::Color::Red);
      textInfo.setPosition((N+3)*18,(M/2-7)*18);

      RectangleShape infoNewPiece(Vector2f(4.5*18,4.5*18));
      infoNewPiece.setFillColor(Color(0,0,0,150));
      infoNewPiece.setOutlineThickness(0);
      infoNewPiece.setPosition((N+3)*18,(M/2+3)*18);

      Event e;
// pollEvent check the list of events in the queue
      while (window.pollEvent(e)){
	// enumeration types in class Event. Closed is the event when the user requested to close a window
         if(youLost)
          if(e.type == Event::KeyPressed)
            if(e.key.code ==Keyboard::Return)
              reset(e);

	       if (e.type == Event::Closed)
	         window.close();

         if(e.type == Event::KeyPressed)
            if(e.key.code == Keyboard::Up) rotate = true;
            else if(e.key.code==Keyboard::Left) dx=-1;
            else if(e.key.code==Keyboard::Right) dx=1;
      }
      //If I press down I go down faster, I must reset delay=0.3 after!
      if (Keyboard::isKeyPressed(Keyboard::Down)) delay= delay/5.;
      // <----Move----> //
      for(int i=0;i<4;i++) {b[i]=a[i]; a[i].x+=dx;}
      //If I'm going out of the window, don't move, same for rotate!
      if(!checkBorders()) for(int i=0;i<4;i++) a[i] = b[i];

      // <----Rotate---> //
      if(rotate){
        Point p = a[1]; //center of rotation
        for(int i=0;i<4;i++){
            int x = a[i].y-p.y;
            int y = a[i].x-p.x;
            a[i].x = p.x - x;
            a[i].y = p.y + y;
        }
        if(!checkBorders()) for(int i=0;i<4;i++) a[i]=b[i];
      }

      //<----Tick--->//
      // everytime that timer>0.3 seconds the tetris block goes down
      if(timer>delay){
         for(int i=0;i<4;i++) {b[i]=a[i]; a[i].y+=1;}
         //If going down you reach the floor or another block, generate a new block and set field as occupied
         if(!checkBorders()){
           for(int i=0;i<4;i++) field[b[i].y][b[i].x]=colorNum1	;
           //it generates a new tetris block (I think!) with a random color!
	   colorNum1 = colorNum2;
           colorNum2=1+rand()%7;
           int n=rand()%7;
           for(int i=0;i<4;i++){
	     //Set the new tetris block to be the random one I generated already and shown in the box
	     a[i].x = c[i].x - offsetX + 4;
	     a[i].y = c[i].y - offsetY;
	     //Generate a new random block to show in the box
             c[i].x = figures[n][i] % 2 + offsetX;
             c[i].y = figures[n][i] / 2 + offsetY;
		
           }
           //if the field where you create the new block is already occupied-->you lost
           for(int i=0;i<4;i++)
            if(field[a[i].y][a[i].x]) {
              youLost=true;
            }
         }

         timer=0;

      }
      /* check lines  from bottom to top except the first line//
      count in each line how many blocks are occupied, if the line is all occupied
      then move all the lines on top of it of one floor down!!
      */
      int k=M-1;
      for (int i=M-1;i>0;i--){
        int count=0;
        for(int j=0;j<N;j++){
          if(field[i][j]) count++;
          field[k][j]=field[i][j];
        }
        if(count<N) k--;
        if(count==N){ countLines++; newLine = true;}
      }

      if(countLines!=0 && countLines%10==0 && newLine){
        level++;
        speedFactor = 0.3 - 0.00140175*level + 0.0015093*level*level;
        newLine = false;
      }

      dx = 0; rotate = 0; delay = speedFactor;

      //<---Draw--->//
    //clear: clear the entire window with a single color
      window.clear( Color::White);
      window.draw(background);
      window.draw(frame);
      window.draw(infoBorder);
      window.draw(textInfo);

      window.draw(infoNewPiece);

     if(!youLost){
      // where the field is occupied, draw the block and keep it there!
      for(int i=0;i<M;i++)
        for(int j=0;j<N;j++){
          if(field[i][j]==0) continue;
          s.setTextureRect(IntRect(field[i][j]*18,0,18,18));
          s.setPosition(j*18,i*18);
          s.move(28,31); 
          window.draw(s);
        }
      for(int i=0;i<4;i++){
          s.setTextureRect(IntRect(colorNum2*18,0,18,18));
          s.setPosition(c[i].x*18,c[i].y*18);
          s.move(19,41); //offset due to the frame inserted
          window.draw(s);
      }

    //draw the Texture via the Sprite
      for(int i=0;i<4;i++){
          s.setTextureRect(IntRect(colorNum1*18,0,18,18));
          s.setPosition(a[i].x*18,a[i].y*18);
          s.move(28,31); //offset due to the frame inserted
          window.draw(s);
      }
    } else{
      for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
          field[i][j]=0;

          //Drawing the text
          sf::Text textGO("GAME OVER", font);
          textGO.setCharacterSize(30);
          textGO.setStyle(sf::Text::Bold);
          textGO.setFillColor(sf::Color::Black);
          textGO.setPosition(N/(2+1)*18,M/2*18);
          // Draw it
          window.draw(textGO);
      }
    //display all the rendering
      window.display();
   }

   return 0;

}
