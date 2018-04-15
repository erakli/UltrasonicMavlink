import processing.serial.*;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.Date;
import java.text.*;


int axesCount = 5;
int[] colors = {#FF0000, #00FF00, #0000FF, #00FFFF, #FF00FF};
String[] names = {"FRONT", "RIGHT", "BACK", "LEFT", "BOTTOM"};

PVector origin;
PVector scaleFactor = new PVector(10, 3);

int HORIZONTAL_LINES_NUM = 10;
int VERTICAL_LINES_NUM = 10;

int maxLength;
int prevWidth = 0;
IntList[] graphs = new IntList[axesCount];

Serial myPort;        // The serial port
String[] comPortName = {"/dev/rfcomm0", "/dev/ttyUSB0"};
int COM_PORT_NUM = 0;

int VALUES_START_IDX = 1;

boolean needOffset = false;
int addedValues = 0;

PGraphics pg;

DateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd_HH:mm:ss");
Date date = new Date();

BufferedWriter output = null;
String SKETCH_PATH;
String fileName;


void setup() {
  for (int i = 0; i < axesCount; i++)
    graphs[i] = new IntList(); //<>//
  
  // should not be executed outside Processing functions, because it points 
  // to processing root, not to sketch root
  SKETCH_PATH = sketchPath("");
  fileName = SKETCH_PATH + dateFormat.format(date) + ".csv";
  
  myPort = new Serial(this, comPortName[COM_PORT_NUM], 57600);
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
  
  writeValues(incomingString);
  
  boolean needResize = (origin.y != height || prevWidth != width);
  if (origin.y != height) {
    origin.y = height;
  }
    
  if (prevWidth != width) {
    prevWidth = width;
    maxLength = int(prevWidth / scaleFactor.x) + 1;
  }
  
  if (needResize) {
    pg = createGraphics(width, height);
  }
  
  int[] newValues = int(split(incomingString, ',')); //<>//
  
  // check on values with time as first argument
  if (newValues.length <= axesCount + VALUES_START_IDX)
    return;
    
  addNewValues(newValues, VALUES_START_IDX);  // add time value
  
  pg.beginDraw();
  drawBackground();   // background resets on new frame 
  drawGraphs();
  drawLegend();
  pg.endDraw();
  image(pg, 0, 0);
  
  println(millis());
}



void addNewValues(int[] newValues, int valuesStartIdx) {
  for (int i = 0; i < axesCount; i++) {
    addNewValue(graphs[i], newValues[i + valuesStartIdx]);
  }
  
  addedValues = (addedValues + 1) % (width);
}


void addNewValue(IntList graph, int newValue) {
  graph.append(newValue); //<>//
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
  for (int t = 0; t < graph.size(); t++) {
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



void writeValues(String values) {
  try {
    //the true will append the new data
    output = new BufferedWriter(new FileWriter(fileName, true));
    output.write(values);
  }
  catch (IOException e) {
    println("It Broke");
    e.printStackTrace();
  }
  finally {
    if (output != null) {
      try {
        output.close();
      } catch (IOException e) {
        println("Error while closing the writer");
      }
    }
  }
}