import controlP5.*;
import processing.pdf.*;

ControlP5 cp5;


int numRows = 4;
int numColums = 3;

int spaceX = 7;
int spaceY = 7;

int tamX = 100;
int tamY = 100;

int startX = 10;
int startY = 10;

ArrayList arucoImages;

PImage testGrid;

boolean record = false;

int startIndex = 0;
int id =0;

String makerType = "DICT_4X4_50";
int opencvId = 0;

//12 markers
int ids [] = {3, 10, 13, 17, 22, 30, 34, 37, 38, 40, 41, 49};

void setup() {
  size(1280, 1200);

  testGrid = loadImage("test_grid.png");

  arucoImages = new ArrayList<PImage>();

  //load ArucoImages
  for (int  i = 0; i < numRows; i++) {
    for (int j = 0; j < numColums; j++) {
      int index = i * numColums + j + startIndex;
      String path = makerType+"/"+opencvId+" "+ids[index]+".png";
      PImage image = loadImage(path);
      println(path);
      arucoImages.add(image);
      
      println(index);
    }
  }

  cp5 = new ControlP5(this);
  cp5.setAutoDraw (false);
  
  //textSize(32);

  cp5.addSlider("startX")
    .setPosition(50, 50)
    .setSize(200, 20)
    .setRange(0, 100)
    .setValue(82) //72
    ;

  cp5.addSlider("startY")
    .setPosition(50, 80)
    .setSize(200, 20)
    .setRange(0, 100)
    .setValue(32) //32
    ;


  cp5.addSlider("tamX")
    .setPosition(50, 110)
    .setSize(200, 20)
    .setRange(0, 200)
    .setValue(166.5) //184.5
    ;

  cp5.addSlider("tamY")
    .setPosition(50, 140)
    .setSize(200, 20)
    .setRange(0, 200)
    .setValue(166.5) //184.5
    ;

  cp5.addSlider("spaceX")
    .setPosition(50, 170)
    .setSize(200, 20)
    .setRange(0, 300)
    .setValue(236.5)
    ;

  cp5.addSlider("spaceY")
    .setPosition(50, 200)
    .setSize(200, 20)
    .setRange(0, 300)
    .setValue(236.5)
    ;
}

void draw() {

  if (record) {
    // Note that #### will be replaced with the frame number. Fancy!
    beginRecord(PDF, "frame_"+startIndex+"_.pdf");
  }


  background(255);

  image(testGrid, 0, 0);
  


  for (int  i = 0; i < numRows; i++) {
    for (int j = 0; j < numColums; j++) {
      int index = i * numColums + j;
      PImage im = (PImage)arucoImages.get(index);

      float x =  spaceX*i + startX;
      float y =  spaceY*j + startY;
      image(im, x, y + spaceY, tamX, tamY);
      
      fill(0);
      textSize(16);
      //text(ids[index], x - 20, y + spaceY);
    }
  }


  fill(0);
  text(startIndex+"", 30, 20);
  
  if (record) {
    endRecord();
    record = false;
    println("finished writing");
  }
}


// Use a keypress so thousands of files aren't created
void mousePressed() {
  record = true;
}
