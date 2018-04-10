import processing.serial.*;


int axesCount = 1;
int[] colors = {#FF0000, #00FF00, #0000FF};
String[] names = {"x", "y", "z"};

PVector origin;
PVector scaleFactor = new PVector(10, 3);

int HORIZONTAL_LINES_NUM = 10;
int VERTICAL_LINES_NUM = 10;

int maxLength;
int prevWidth = 0;
IntList[] graphs = {new IntList(), new IntList(), new IntList()};

Serial myPort;        // The serial port
String comPortName = "/dev/ttyUSB0"; //"/dev/rfcomm0";/

boolean needOffset = false;
int addedValues = 0;

PGraphics pg;


void setup() {
  myPort = new Serial(this, comPortName, 57600);
  myPort.bufferUntil('\n');
  
  size(1000, 700);
  surface.setResizable(true);
  
  pg = createGraphics(width, height);
  
  frameRate(100);
  
  // left bottom corner
  origin = new PVector(0, height);
}

void draw() {
  if (myPort.available() == 0)
    return;
  
  String incomingString = myPort.readStringUntil('\n');
  if (incomingString == null)
    return; 
  
  boolean needResize = (origin.y != height || prevWidth != width);
  if (origin.y != height) {
    origin.y = height;
  }
    
  if (prevWidth != width) {
    prevWidth = width;
    maxLength = int(prevWidth / scaleFactor.x) + 1;
  }
  
  if (needResize) {
    pg = createGraphics(width, height); //<>//
  }
  
  int[] newValues = int(split(incomingString, ','));
  addNewValues(newValues);
  
  pg.beginDraw();
  drawBackground();   // background resets on new frame 
  drawGraphs();
  drawLegend();
  pg.endDraw();
  image(pg, 0, 0);
}



void addNewValues(int[] newValues) {
  for (int i = 0; i < axesCount; i++) {
    addNewValue(graphs[i], newValues[i]);
  }
  
  addedValues = (addedValues + 1) % (width);
}


void addNewValue(IntList graph, int newValue) {
  graph.append(newValue);
  needOffset = (graph.size() >= maxLength);
  while (graph.size() >= maxLength) {
    graph.remove(0);
  }
}


void drawGraphs() {
  // save current transformation
  pg.pushMatrix();
  
  pg.translate(origin.x, origin.y);
  pg.scale(1, -1); // flip y axis
  
  for (int i = 0; i < axesCount; i++) {
    pg.stroke(colors[i]);
    drawGraph(graphs[i]);
  }
  
  // release current transformation
  pg.popMatrix();
}


void drawGraph(IntList graph) {
  float prevX = 0;
  float prevY = (float)graph.get(0) * scaleFactor.y;
  for (int t = 1; t < graph.size(); t++) {
    float curX = t * scaleFactor.x;
    float curY = (float)graph.get(t) * scaleFactor.y;
    pg.line(prevX, prevY, curX, curY);
    prevX = curX;
    prevY = curY;
  }
}


void drawBackground() {
  pg.background(255);
  
  // save current transformation
  pg.pushMatrix();
  
  pg.translate(origin.x, origin.y);
  pg.scale(1, -1);  // flip y axis
  
  drawOrigin();
  pg.stroke(200);  // color
  drawVerticalLines();
  drawHorizontalLines();
  
  // release current transformation
  pg.popMatrix(); 
}


void drawOrigin() {
  // draw graph -- center on origin
  pg.stroke(0);
  pg.line(0, 0, width, 0);
}


void drawHorizontalLines() {
  float yDiff = height / HORIZONTAL_LINES_NUM;
  for (int y = 0; y < HORIZONTAL_LINES_NUM; y++) {
    float linePos = y * yDiff;
    pg.line(0, linePos, width, linePos);
    
    pg.pushMatrix();
    pg.scale(1, -1);
    int displayValue = int(y * yDiff / scaleFactor.y);
    float xPos = 10;
    float yPos = linePos + 5;
    pg.text(displayValue, xPos, -yPos);
    pg.popMatrix();
  }
}


void drawVerticalLines() {
  pg.pushMatrix();
  
  float xDiff = width / VERTICAL_LINES_NUM;
  
  if (needOffset) {
    float offset = addedValues * scaleFactor.x;
    offset %= xDiff;    
    pg.translate(-offset, 0);
  }
  
  for (int t = 0; t < VERTICAL_LINES_NUM + 1; t++) {
    float linePos = t * xDiff;
    pg.line(linePos, 0, linePos, height);
  }
  
  pg.popMatrix();
}


void drawLegend() {
  for (int i = 0; i < axesCount; i++) {
    pg.fill(colors[i]);  //set the rect color to red
    pg.rect(30, 10 + i * 20, 10, 10);
    
    pg.fill(0);  //set the text color to black
    String value = 
      String.format("%s (%d)", names[i], graphs[i].get(graphs[i].size() - 1));
    pg.text(value, 45, 20 + i * 20);
  }
}