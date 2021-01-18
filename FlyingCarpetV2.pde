import processing.serial.*;
import ddf.minim.*;
Minim minim;
AudioPlayer base;
AudioPlayer chime100;
AudioPlayer chime10;
AudioPlayer chime1;
AudioPlayer reel;
//
Serial myPort;  // The serial port:
PImage bg;
PImage img [];
int y=205;
int x0=639;
int x1=737;
int x2=835;
int x3=933;
//
// Highest Score Variable
int highscore;
int highscoreones;
int highscoretens;
int highscorehundreds;
int highscorethous;
//
int d;
int thous;
int hundreds;
int tens;
int ones; 
int oldthous;
int oldhundreds;
int oldtens;
int oldones; 
int oldscore;
int newscore;
int add;
int reset;
//
//
void setup()
{
  minim = new Minim(this);
  base = minim.loadFile("base.wav");
  chime100 = minim.loadFile("chime100.wav");
  chime10 = minim.loadFile("chime10.wav");
  chime1 = minim.loadFile("chime1.wav");
  reel=minim.loadFile("reel.wav");
  //
  myPort = new Serial(this, Serial.list()[0], 9600);
  //size(978,1080);
  //bg =   loadImage("FC1.png");
   size(1080,1193);
   bg =   loadImage("FC.png");
  newscore = 0;
  int nPics = 10;
  img = new PImage [nPics];
  for (int i = 0; i <nPics; i++) 
  {
    img[i] = loadImage(+i+".jpg");
  }
  background(bg);
//
}
//
void display()
{
   oldthous = thous;
   oldhundreds = hundreds;
   oldtens = tens;
   oldones = ones;    
   oldscore = newscore;
   if(oldscore>9999)oldscore=0;
   thous = int(oldscore/1000); 
   hundreds = int(oldscore/100);
   hundreds = hundreds-(int(hundreds/10)*10);
   tens = int(oldscore/10);
   tens = tens-(int(tens/10)*10);
   ones = oldscore-(int(oldscore/10)*10);
   // 
   image(img[ones], x3, y);
   image(img[tens], x2, y);
   image(img[hundreds], x1, y);
   image(img[thous], x0, y);
   delay(20);
}
//  
void draw() 
  {
    if (reset==1)
    {
      int total=ones+(10*tens)+(100*hundreds)+(1000*thous);
      if (total == 0) 
      {
        reset=0;
        ones=0;
        tens=0;
        hundreds=0;
        thous=0;
        newscore=0;
        oldscore=0;
      }
//      
      if (ones != 0) 
      {
        ones=ones+1;
        if (ones==10)ones=0;
        image(img[ones], x3, y);
        reel.rewind();
        reel.play();
        delay(75);
      }
//      
      if (tens != 0) 
      {
        tens=tens+1;
        if (tens==10)tens=0;
        image(img[tens], x2, y);
        reel.rewind();
        reel.play();
        delay(75); 
      }
//     
      if (hundreds != 0) 
      {
        hundreds=hundreds+1;
        if (hundreds==10)hundreds=0;
        image(img[hundreds], x1, y);
        reel.rewind();
        reel.play();
        delay(75);  
      }
      if (thous != 0) 
      {
        thous=thous+1;
        if (thous==10)thous=0;
        image(img[thous], x0, y);
        reel.rewind();
        reel.play();
        delay(75);  
      }
  }
//  Process any inputs
   while (myPort.available() > 0) 
   {
    char inByte = myPort.readChar();
//  
    if (inByte == 'x') 
    {
      base.rewind();
      base.play();
    }
    if (inByte == 'g') 
    {
     reset = 1;
     base.rewind();
     base.play();
     add=0;
    }
  if (inByte == 'a')
  {
    add = 1;
    chime1.rewind();
    chime1.play();
  }
  if (inByte == 'b')
  {
    add = 10;
    chime10.rewind();
    chime10.play(); 
  }
  if (inByte == 'c')
  {
   add = 100; 
   chime100.rewind();
   chime100.play();
  }
  if (add>0)
   {
     oldscore = newscore; 
     newscore = newscore+add;
     add=0;
   }
   display();
  }
}
