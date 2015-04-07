//--------------------------------------------------------------------------
/// \file	main.h
/// \author	Kemal Duran [kemalduran06@gmail.com]
/// \date	27-3-2015
/// \brief	Main source codes of paint program
/// 
//--------------------------------------------------------------------------

#ifdef WIN32 
	#include <windows.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <pu.h>
#include "texture.h"
#include <math.h>
#include "ImageSaver.cpp"

const float DEG2RAD = 3.14159 / 180;

#define WIDTHH 480
#define HEIGHTT 270
unsigned char saveImage[WIDTHH][WIDTHH][3];

// window screen
int width = 680;
int height = 480;

struct Point{
	int X;
	int Y;
};

struct Rectanglee{
	float r;
	float g;
	float b;
	float a;
	Point p1;
	Point p2;
};
Rectanglee *rects;
int rectCount = 0;
Point currRect1;
Point currRect2;

struct Line{
	float r;
	float g;
	float b;
	float a;
	float size;
	Point p1;
	Point p2;
};
Point currLine1;
Point currLine2;
Line *lines;
int lineCount = 0;

struct  Brush {
	float r;
	float g;
	float b;
	float a;
	float size;
	Point p;
};
Brush *brushes;
int brCount = 0;
float brushSize = 1.0f;

struct Polygonn{
	float r;
	float g;
	float b;
	float a;
	Point points[20];
	int pCount=0;
};
Polygonn *polys;
int polyCount = 0;
Polygonn currPoly;

struct Circle{
	float r;
	float g;
	float b;
	float a;
	float size;
	float radius;
	Point center;
	Point p1; // mouse down point
	Point p2; // mouse up point
};
Circle currCircle;
int circleCount = 0;
Circle *circles;

struct FCircle{
	float r;
	float g;
	float b;
	float a;
	float size;
	float radius;
	Point center;
	Point p1; // mouse down point
	Point p2; // mouse up point
};
FCircle currFCircle;
int fCircleCount = 0;
FCircle *fCircles;

bool isPointBrush = false;
bool isLineDraw = false;
bool isRectDraw = false;
bool isCircleDraw = false;
bool isFCircleDraw = false;
bool isPolygonDraw = false;

void File_Selector_Callback(puObject *pWidget);
void OnReshape(int w, int h);
void textureInit(char *imgPath);
void OnExit();
void closeAllBrush();
void initButtons();
void textureDisplay();

bool isImageLoaded = false;


/// this will hold the openGL texture object for the loaded texture
unsigned int g_Texture = 0;

/* read pixels by glReadPixels() */
void renderImage(void)
{
	// margins
	int x1 = (680 - WIDTHH) / 2;
	int y1 = (480 - HEIGHTT) / 2;
	
	int i, j;
	for (j = 0; j < HEIGHTT; j++) { // satýr
		for (i = 0; i < WIDTHH; i++) { // sütun
			//saveImage[j][i][0] = saveImage[j][i][1] = saveImage[j][i][2] = (((i & 8) == 0) ^ ((j & 8) == 0)) * 255;
			//saveImage[j][i][0] = saveImage[j][i][1] = saveImage[j][i][2] = 235;
			
			GLubyte pixel_color[3];
			glReadPixels(i+x1, j+y1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel_color[0]);
			
			saveImage[j][i][0] = pixel_color[2];
			saveImage[j][i][1] = pixel_color[1];
			saveImage[j][i][2] = pixel_color[0];
			
		}
		printf("%d / %d completed \n" , j, HEIGHTT);
	}

}

/* called when open menu item selected */
void Open_CB(puObject *pWidget)
{
	/* x margin , y margin , width, height, start file */
	puFileSelector* fs = new puFileSelector(5, height-350-20, 250, 350, "");
	fs->setCallback(File_Selector_Callback); /* set callback for when user clicks OK or cancel */
}

/* called when save menu item selected */
void Save_CB(puObject *pWidget)
{
	renderImage();
	printf("Rendering completed...\n");
	//glReadPixels(0, width - 0, 100, 100, GL_RGB, GL_UNSIGNED_BYTE, &saveImage);
	if (SaveImage("D:\\saved_image.png", saveImage, WIDTHH, HEIGHTT)){
		printf("Image successfully saved...\n");
	}
	else{
		printf("Image not saved!!!\n");
	}
}
void Clear_CB(puObject *pWidget)
{
	closeAllBrush();
	DeleteAllTextures();
	isImageLoaded = false;
	
	fCircleCount = 0;
	circleCount = 0;
	lineCount = 0;
	polyCount = 0;
	rectCount = 0;
	brCount = 0;

}
/* called when quit menu item selected */
void Quit_CB(puObject *pWidget)
{
	exit(0);
}
/* the items in the combo box */
char *ComboBoxItems[] =
{
	"Thin 1x1",
	"Medium 5x5",
	"Thick 10x10",
	NULL
};
/* the callback for the combo box */
void ComboBox_CB(puObject *pWidget)
{
	char *selected = pWidget->getStringValue();
	bool equal1 = (std::strcmp(selected, ComboBoxItems[0]) == 0);
	bool equal2 = (std::strcmp(selected, ComboBoxItems[1]) == 0);
	bool equal3 = (std::strcmp(selected, ComboBoxItems[2]) == 0);

	if (equal1)
		brushSize = 1.0f;
	if (equal2)
		brushSize = 5.0f;
	if (equal3)
		brushSize = 10.0f;

	printf("CurrentItem = %s\n", selected);
}

/* the text items that will appear on the menu */
char *MenuItems1[] =
{
	"Quit",
	"Save",
	"Choose Image",
	NULL		/* last item must be NULL */
};
char *emptyItems[] =
{
	NULL
};
char *MenuItems2[] =
{
	"Clear Screen",
	"Hide Color Slider",
	NULL		/* last item must be NULL */
};

/* slider values */
float r = 1.0, g = 0.0, b = 0.0, a=1.0;

int slidr_w = 120;
int slidr_h = 16;
puSlider *slider_r;
puSlider *slider_g;
puSlider *slider_b;
puSlider *slider_a;
bool isColorShow = true;

void ShowColorSlider_CB(puObject *pWidget){
	if (isColorShow){
		// slider gizle
		isColorShow = false;
		slider_r->hide();
		slider_g->hide();
		slider_b->hide();
		slider_a->hide();
		pWidget->setLegend("Show Color Slider");

	}
	else{
		// slider göster
		isColorShow = true;
		slider_r->reveal();
		slider_g->reveal();
		slider_b->reveal();
		slider_a->reveal();
		pWidget->setLegend("Hide Color Slider");
	}

}

/* The functions that will be called if one of the menu items above is selected */
puCallback MenuCallbacks[] =
{
	Quit_CB,
	Save_CB,
	Open_CB,
	NULL		/* last item must be NULL */
};
puCallback MenuCallbacks2[] =
{
	Clear_CB,
	ShowColorSlider_CB,
	NULL		/* last item must be NULL */
};

/* applies a point at (x,y) */
void savePointBrush(int x, int y){
	if (brCount % 100 == 99){
		brushes = (Brush*) realloc(brushes, (100 + brCount)* sizeof(Brush));
	}
	Brush bb;
	bb.r = r;
	bb.g = g;
	bb.b = b;
	bb.a = a;
	bb.p.X = x;
	bb.p.Y = y;
	bb.size = brushSize;
	brushes[brCount] = bb;

	brCount++;
}

/* called when a mouse button is pressed or released */
void MouseButton(int button, int state, int x, int y)
{
	
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		if (isPointBrush){
			savePointBrush(x, y);
		}
		if (isLineDraw)
		{
			currLine1.X = x;
			currLine1.Y = y;
			currLine2.X = x;
			currLine2.Y = y;
		}
		if (isRectDraw){
			currRect1.X = x;
			currRect1.Y = y;
			currRect2.X = x;
			currRect2.Y = y;

		}
		if (isPolygonDraw){
			currPoly.points[currPoly.pCount].X = x;
			currPoly.points[currPoly.pCount].Y = y;
			currPoly.pCount++;
		}
		if (isCircleDraw){
			currCircle.p1.X = x;
			currCircle.p1.Y = y;
			currCircle.p2.X = x;
			currCircle.p2.Y = y;
		}
		if (isFCircleDraw){
			currFCircle.p1.X = x;
			currFCircle.p1.Y = y;
			currFCircle.p2.X = x;
			currFCircle.p2.Y = y;
		}
		std::cout << "mouse down " << x << "-" << y << std::endl;

	}
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
	{
		std::cout << "mouse up " << x << "-" << y << std::endl;
		
		if (isLineDraw)
		{
			currLine2.X = x;
			currLine2.Y = y;

			if (lineCount % 10 == 9){
				lines = (Line*) realloc(lines, (10 + lineCount)* sizeof(Line));
			}
			Line ll;
			ll.r = r;
			ll.g = g;
			ll.b = b;
			ll.a = a;
			ll.p1.X = currLine1.X;
			ll.p1.Y = currLine1.Y;
			ll.p2.X = currLine2.X;
			ll.p2.Y = currLine2.Y;
			ll.size = brushSize;
			lines[lineCount] = ll;
			lineCount++;
		}
		if (isRectDraw){
			currRect2.X = x;
			currRect2.Y = y;
			if (rectCount % 10 == 9){
				rects = (Rectanglee*) realloc(rects, (10 + rectCount)* sizeof(Rectanglee));
			}
			Rectanglee rr;
			rr.r = r;
			rr.g = g;
			rr.b = b;
			rr.a = a;
			rr.p1.X = currRect1.X;
			rr.p1.Y = currRect1.Y;
			rr.p2.X = currRect2.X;
			rr.p2.Y = currRect2.Y;
			rects[rectCount] = rr;
			rectCount++;
		}
		if (isCircleDraw){
			currCircle.p2.X = x;
			currCircle.p2.Y = y;
			currCircle.center.X = (currCircle.p1.X + currCircle.p2.X) / 2;
			currCircle.center.Y = (currCircle.p2.Y + currCircle.p2.Y) / 2;
			int absX = abs(currCircle.p1.X - currCircle.p2.X);
			int absY = abs(currCircle.p1.Y - currCircle.p2.Y);
			if (absX < absY){
				currCircle.radius = absX / 2;
			}
			else{
				currCircle.radius = absY / 2;
			}

			if (circleCount % 10 == 9){
				circles = (Circle*) realloc(circles, (10 + circleCount)* sizeof(Circle));
			}

			Circle rr;
			rr.r = r;
			rr.g = g;
			rr.b = b;
			rr.a = a;
			rr.center = currCircle.center;
			rr.radius = currCircle.radius;
			rr.p1 = currCircle.p1;
			rr.p2 = currCircle.p2;
			rr.size = brushSize;
			circles[circleCount] = rr;
			circleCount++;

			currCircle.radius = 0;
		}
		if (isFCircleDraw){
			currFCircle.p2.X = x;
			currFCircle.p2.Y = y;
			currFCircle.center.X = (currFCircle.p1.X + currFCircle.p2.X) / 2;
			currFCircle.center.Y = (currFCircle.p2.Y + currFCircle.p2.Y) / 2;
			int absX = abs(currFCircle.p1.X - currFCircle.p2.X);
			int absY = abs(currFCircle.p1.Y - currFCircle.p2.Y);
			if (absX < absY){
				currFCircle.radius = absX / 2;
			}
			else{
				currFCircle.radius = absY / 2;
			}

			if (fCircleCount % 10 == 9){
				fCircles = (FCircle*) realloc(fCircles, (10 + fCircleCount)* sizeof(FCircle));
			}

			FCircle rr;
			rr.r = r;
			rr.g = g;
			rr.b = b;
			rr.a = a;
			rr.center = currFCircle.center;
			rr.radius = currFCircle.radius;
			rr.p1 = currFCircle.p1;
			rr.p2 = currFCircle.p2;
			rr.size = brushSize;
			fCircles[fCircleCount] = rr;
			fCircleCount++;

			currFCircle.radius = 0;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		isPolygonDraw = false;
		
		if (polyCount % 10 == 9){
			polys = (Polygonn*) realloc(polys, (10 + polyCount)* sizeof(Polygonn));
		}
		polys[polyCount].pCount = currPoly.pCount;
		for (int i = 0; i < currPoly.pCount; i++)
		{
			polys[polyCount].points[i] = currPoly.points[i];
		}
		polys[polyCount].r = r;
		polys[polyCount].g = g;
		polys[polyCount].b = b;
		polys[polyCount].a = a;
		polyCount++;

		currPoly.pCount = 0;

	}

	/* see if the mouse clicked on any of the user interface controls */
	puMouse(button, state, x, y);
	glutPostRedisplay();

}

/* called when the mouse moves */
void MouseMove(int x, int y)
{
	std::cout << "mouse move " << x << "-" << y << std::endl;
	
	if (isPointBrush){
		savePointBrush(x, y);
	}
	if (isLineDraw){
		currLine2.X = x;
		currLine2.Y = y;
	}
	if (isRectDraw){
		currRect2.X = x;
		currRect2.Y = y;
	}
	if (isCircleDraw){
		currCircle.p2.X = x;
		currCircle.p2.Y = y;
		currCircle.center.X = (currCircle.p1.X + currCircle.p2.X) / 2;
		currCircle.center.Y = (currCircle.p2.Y + currCircle.p2.Y) / 2;
		int absX = abs(currCircle.p1.X - currCircle.p2.X);
		int absY = abs(currCircle.p1.Y - currCircle.p2.Y);
		if (absX < absY){
			currCircle.radius = absX / 2;
		}
		else{
			currCircle.radius = absY / 2;
		}
	}
	if (isFCircleDraw){
		currFCircle.p2.X = x;
		currFCircle.p2.Y = y;
		currFCircle.center.X = (currFCircle.p1.X + currFCircle.p2.X) / 2;
		currFCircle.center.Y = (currFCircle.p2.Y + currFCircle.p2.Y) / 2;
		int absX = abs(currFCircle.p1.X - currFCircle.p2.X);
		int absY = abs(currFCircle.p1.Y - currFCircle.p2.Y);
		if (absX < absY){
			currFCircle.radius = absX / 2;
		}
		else{
			currFCircle.radius = absY / 2;
		}

	}
	/* Pass information about the position of the mouse to pui */
	puMouse(x, y);
	glutPostRedisplay();
}

// OK veya cancel týklandýðýnda çalýþýr
void File_Selector_Callback(puObject *pWidget)
{
	/* get the filename held in the widget */
	char *filename = pWidget->getStringValue();

	/* if cancel was pressed then filename will be "", or "\0" */
	if (filename[0] == NULL)
	{
		printf("Cancel was specified\n");
	}
	else
	{
		isImageLoaded = true;
		printf("File to load = \"%s\"\n", filename);
		textureInit(filename);
		if (isImageLoaded)
			textureDisplay();

	}
	/* delete the file selector widget */
	puDeleteObject(pWidget);
}

/*
*	This function is called after the openGL window has been created.
*	It allows you to set up user interface controls within the main
*	window.
*/
void initSliders(){
	/* create a new horizontal slider. int min = 0, int max = 100 */
	slider_r = new puSlider(0, 100, 50);
	slider_r->setValuator(&r);
	slider_r->setLegend("Red");
	slider_r->setSize(slidr_w, slidr_h);
	slider_r->setPosition(5, 65);

	/* create a new horizontal slider. int min = 0, int max = 100 */
	slider_g = new puSlider(0, 100, 50);
	slider_g->setValuator(&g);
	slider_g->setLegend("Green");
	slider_g->setSize(slidr_w, slidr_h);
	slider_g->setPosition(5, 45);

	/* create a new horizontal slider. int min = 0, int max = 100 */
	slider_b = new puSlider(0, 100, 30);
	slider_b->setValuator(&b);
	slider_b->setLegend("Blue");
	slider_b->setSize(slidr_w, slidr_h);
	slider_b->setPosition(5, 25);

	slider_a = new puSlider(0, 100, 30);
	slider_a->setValuator(&a);
	slider_a->setLegend("Alpha");
	slider_a->setSize(slidr_w, slidr_h);
	slider_a->setPosition(5, 5);
}
void initMenuBar(){
	/* create a new menu */
	puMenuBar *menu = new puMenuBar(-1);

	/* add the first menu "File" */
	menu->add_submenu("File", MenuItems1, MenuCallbacks);
	menu->add_submenu("|", emptyItems, NULL);
	menu->add_submenu("Window", MenuItems2, MenuCallbacks2);


	/* This MUST be called after adding your menus */
	menu->close();
}
void closeAllBrush(){
	isLineDraw = false;
	isPointBrush = false;
	isRectDraw = false;
	isCircleDraw = false;
	isPolygonDraw = false;
	isFCircleDraw = false;
}
void btnPen(puObject *pWidget)
{
	if (isPointBrush)
		isPointBrush = false;
	else{
		closeAllBrush();
		isPointBrush = true;
	}
	printf("Pen\n");
}
void btnLine(puObject *pWidget)
{
	if (isLineDraw)
		isLineDraw = false;
	else{
		closeAllBrush();
		isLineDraw = true;
	}
	printf("Line\n");
}
void btnRect(puObject *pWidget)
{
	if (isRectDraw)
		isRectDraw = false;
	else{
		closeAllBrush();
		isRectDraw = true;
	}
	printf("Rect\n");
}
void btnCircle(puObject *pWidget)
{
	if (isCircleDraw)
		isCircleDraw = false;
	else{
		closeAllBrush();
		isCircleDraw = true;
	}
	printf("Circle\n");
}
void btnFillCircle(puObject *pWidget)
{
	if (isFCircleDraw)
		isFCircleDraw = false;
	else{
		closeAllBrush();
		isFCircleDraw = true;
	}
	printf("Filled Circle\n");
}
void btnPolygon(puObject *pWidget)
{
	if (isPolygonDraw)
		isPolygonDraw = false;
	else{
		closeAllBrush();
		isPolygonDraw = true;
	}
	printf("Polygon\n");
}
void btnNone(puObject *pWidget)
{	
	closeAllBrush();	
}
/* puOneShot = button */
puOneShot *b1, *b2, *b3, *b4, *b5, *b6, *b7;
/* initialize shape library buttons */
void initButtons(){
	b1 = new puOneShot(width - 90, height - 40, width - 10, height - 10);
	b1->setLegend("Brush");
	b1->setCallback(btnPen);

	b2 = new puOneShot(width - 90, height - 70, width - 10, height - 40);
	b2->setLegend("Line");
	b2->setCallback(btnLine);

	b3 = new puOneShot(width - 90, height - 100, width - 10, height - 70);
	b3->setLegend("Rectangle");
	b3->setCallback(btnRect);

	b4 = new puOneShot(width - 90, height - 130, width - 10, height - 100);
	b4->setLegend("Circle");
	b4->setCallback(btnCircle);

	b5 = new puOneShot(width - 90, height - 160, width - 10, height - 130);
	b5->setLegend("Filled Circle");
	b5->setLegendFont(PUFONT_HELVETICA_12);
	b5->setCallback(btnFillCircle);

	b6 = new puOneShot(width - 90, height - 190, width - 10, height - 160);
	b6->setLegend("Polygon");
	b6->setCallback(btnPolygon);

	b7 = new puOneShot(width - 90, height - 220, width - 10, height - 190);
	b7->setLegend("None");
	b7->setCallback(btnNone);

}
puComboBox *box;
void InitUI()
{
	puSetDefaultStyle(PUSTYLE_SMALL_SHADED);
	puSetDefaultColourScheme( 0.1f, 0.5f, 0.96f );	/* theme color */

	/* create a new bit of text */
	puText* Text = new puText( 5, 5 );

	/* set the text in the control */
	Text->setLabel("Kemal Duran");

	initMenuBar();
	initSliders();
	initButtons();

	/* create a new combo box */
	box = new puComboBox(width-230, height-40, width-100, height-10, ComboBoxItems, FALSE);
	box->setCallback(ComboBox_CB);
}

/* custom OpenGL Initialisation */
void Init()
{
	glClearColor(0.8,0.0,0.8,0.9); // background

	// Set your projection matrix
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, width, height, 0, -1, 1); // sol üst köþe 0,0 
	// Restore the default matrix mode
	glMatrixMode(GL_MODELVIEW);

	/* point lerin yuvarlak olmasý için */
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//------------------------------------------------------------	OnReshape()
//
void OnReshape(int w, int h)
{
	// prevent a division by 0 when minimises
	if (h == 0)
		h = 1;
	width = w;
	height = h;

	//glOrtho(0, width, height, 0, -1, 1); // sol üst köþe 0,0 

	// change button positions
	b1->setPosition(width - 90, height - 40);
	b2->setPosition(width - 90, height - 70);
	b3->setPosition(width - 90, height - 100);
	b4->setPosition(width - 90, height - 130);
	b5->setPosition(width - 90, height - 160);
	b6->setPosition(width - 90, height - 190);
	b7->setPosition(width - 90, height - 220);
	box->setPosition(width - 230, height - 40);

	std::cout << "OnReshape "<<w<<"-"<<h<<std::endl;

	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
//------------------------------------------------------------	OnExit()
//
void OnExit() {

	// deletes all loaded texture
	DeleteAllTextures();
}

//------------------------------------------------------------	OnInit()
// load texture
void textureInit(char *imgPath) {

	// enable the depth test and 2D texturing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	// load texture as compressed
	g_Texture = LoadTexture(imgPath);

	// outputs the size of the texture
	std::cout << "Texture Size=" << (GetTextureSize(g_Texture) / 1024.0f) << "Kb" << std::endl;
}
//------------------------------------------------------------	OnDraw()
// draw texture
void textureDisplay() {

	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // ***** önemli
	// binds the texture to any geometry about to be rendered
	glBindTexture(GL_TEXTURE_2D, g_Texture);

	float x = 15;
	float y = 11;

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-x, y);

	glTexCoord2f(0, 1);
	glVertex2f(-x, -y);

	glTexCoord2f(1, 1);
	glVertex2f(x, -y);

	glTexCoord2f(1, 0);
	glVertex2f(x, y);
	
	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20, 20, 20, -20, -20, 20);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* draws all points drawn */
void pointsDisplay() {
	for (int i = 0; i < brCount; i++)
	{
		glColor4f(brushes[i].r, brushes[i].g, brushes[i].b, brushes[i].a);  // brush rengi
		glPointSize(brushes[i].size); // ÇAP

		glBegin(GL_POINTS);
		glVertex2i(brushes[i].p.X, brushes[i].p.Y);
		glEnd();
	}
	
	//glFlush(); // GLUT_SINGLE buffer için
	//glutSwapBuffers(); // GLUT_DOUBLE buffer için
}
/* draws color slider result */
void colorRectDisplay(){
	//glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(r,g,b,a);  // brush rengi
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); glVertex2i(130, 480-85);
	glTexCoord2f(0.0, 1.0); glVertex2i(130, 480-5);
	glTexCoord2f(1.0, 1.0); glVertex2i(160, 480-5);
	glTexCoord2f(1.0, 0.0); glVertex2i(160, 480-85);
	glEnd();
}
/* draws all lines drawn */
void lineDisplay(){
	// þu an çizilen
	glColor4f(r, g, b, a);  // brush rengi
	glLineWidth(brushSize);
	glBegin(GL_LINES);
	glVertex2i(currLine1.X, currLine1.Y);
	glVertex2i(currLine2.X, currLine2.Y);
	glEnd();

	// önceki çizilmiþler
	for (int i = 0; i < lineCount; i++)
	{
		glColor4f(lines[i].r, lines[i].g, lines[i].b, lines[i].a);  // brush rengi
		glLineWidth(lines[i].size); // ÇAP

		glBegin(GL_LINES);
		glVertex2i(lines[i].p1.X, lines[i].p1.Y);
		glVertex2i(lines[i].p2.X, lines[i].p2.Y);
		glEnd();
	}
}
/* draws all rectangles drawn */
void rectDisplay(){
	// draw current
	glColor4f(r, g, b, a);  // brush rengi
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(currRect1.X, currRect1.Y);
	glTexCoord2i(0, 1); glVertex2i(currRect1.X, currRect2.Y);
	glTexCoord2i(1, 1); glVertex2i(currRect2.X, currRect2.Y);
	glTexCoord2i(1, 0); glVertex2i(currRect2.X, currRect1.Y);
	glEnd();

	for (int i = 0; i < rectCount; i++)
	{
		glColor4f(rects[i].r, rects[i].g, rects[i].b, rects[i].a);  // brush rengi

		glBegin(GL_QUADS);
		glTexCoord2i(0.0, 0.0); glVertex2i(rects[i].p1.X, rects[i].p1.Y);
		glTexCoord2i(0.0, 1.0); glVertex2i(rects[i].p1.X, rects[i].p2.Y);
		glTexCoord2i(1.0, 1.0); glVertex2i(rects[i].p2.X, rects[i].p2.Y);
		glTexCoord2i(1.0, 0.0); glVertex2i(rects[i].p2.X, rects[i].p1.Y);

		glEnd();
	}
}
/* draws all polygons drawn */
void polyDisplay(){
	// draw current
	glColor4f(r, g, b, a);  // brush rengi
	glBegin(GL_POLYGON);
	for (int i = 0; i < currPoly.pCount; i++)
	{
		glVertex2i(currPoly.points[i].X, currPoly.points[i].Y);
	}
	glEnd();

	for (int i = 0; i < polyCount; i++)
	{
		glColor4f(polys[i].r, polys[i].g, polys[i].b, polys[i].a);  // brush rengi
		glBegin(GL_POLYGON);
		for (int j = 0; j < polys[i].pCount; j++)
		{
			glVertex2i(polys[i].points[j].X, polys[i].points[j].Y);
		}
		glEnd();
	}
}
/* draws all circles drawn */
void circleDisplay()
{
	// draw current
	glLineWidth(brushSize);
	glColor4f(r,g,b,a);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i<360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex2f(cos(degInRad)*currCircle.radius + currCircle.center.X, 
			sin(degInRad)*currCircle.radius + currCircle.center.Y);
	}
	glEnd();

	for (int i = 0; i < circleCount; i++)
	{
		Circle cc = circles[i];
		glLineWidth(cc.size);
		glColor4f(cc.r, cc.g, cc.b, cc.a);  // brush rengi
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i<360; i++)
		{
			float degInRad = i*DEG2RAD;
			glVertex2f(cos(degInRad)*cc.radius + cc.center.X,
				sin(degInRad)*cc.radius + cc.center.Y);
		}
		glEnd();
	}
}
void filledCircleDisplay(){
	//filled circle
	float x1, y1, x2, y2;
	float angle;
	x1 = 0.5, y1 = 0.6;
	glColor4f(r,g,b,a);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x1 + currFCircle.center.X, y1 + currFCircle.center.Y);

	for (angle = 1.0f; angle<361.0f; angle += 0.2)
	{
		x2 = x1 + sin(angle)*currFCircle.radius;
		y2 = y1 + cos(angle)*currFCircle.radius;
		glVertex2f(x2 + currFCircle.center.X, y2 + currFCircle.center.Y);
	}
	glEnd();

	for (int i = 0; i < fCircleCount; i++)
	{
		FCircle cc = fCircles[i];
		glLineWidth(cc.size);
		glColor4f(cc.r, cc.g, cc.b, cc.a);  // brush rengi
		glBegin(GL_TRIANGLE_FAN);
		for (angle = 1.0f; angle<361.0f; angle += 0.2)
		{
			x2 = x1 + sin(angle)*cc.radius;
			y2 = y1 + cos(angle)*cc.radius;
			glVertex2f(x2 + cc.center.X, y2 + cc.center.Y);
		}
		glEnd();
	}
}
void drawAreaDisplay(){
	glColor3f(1, 1, 1);  // beyaz çizim alaný
	glBegin(GL_QUADS);

	int x1 = (680 - WIDTHH) / 2;
	int y1 = (480 - HEIGHTT) / 2;
	int x2 = x1+WIDTHH;
	int y2 = y1+HEIGHTT;

	glTexCoord2i(0, 0); glVertex2i(x1, y1);
	glTexCoord2i(0, 1); glVertex2i(x1, y2);
	glTexCoord2i(1, 1); glVertex2i(x2, y2);
	glTexCoord2i(1, 0); glVertex2i(x2, y1);
	glEnd();
}

void Display()
{
	std::cout << "display" << std::endl;

	glClearColor(0.9, 0.9, 0.9, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (isImageLoaded)
	    textureDisplay(); // only image
	else
	{
		// draw all paint shapes
		glClear(GL_COLOR_BUFFER_BIT);
		drawAreaDisplay();
		polyDisplay();
		rectDisplay();
		filledCircleDisplay();
		circleDisplay();
		lineDisplay();
		pointsDisplay(); // brush çizdir
		colorRectDisplay();
	}
	/* display the user interface */
	puDisplay();

	glutSwapBuffers();
	
}

int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize(width,height);
	glutInitWindowPosition(320,0);
	glutCreateWindow("Paint Terk - Kemal Duran");

	glutMouseFunc(MouseButton); // menünün mouse ile açýlmasý için
	glutMotionFunc(MouseMove);
	
	// initialize shapes array
	brushes = (Brush*) malloc(100 * sizeof(Brush));
	lines = (Line*) malloc(10 * sizeof(Line));
	rects = (Rectanglee*) malloc(10 * sizeof(Rectanglee));
	polys = (Polygonn*) malloc(10 * sizeof(Polygonn));
	circles = (Circle*) malloc(10 * sizeof(Circle));
	fCircles = (FCircle*) malloc(10 * sizeof(FCircle));

	glutDisplayFunc(Display);	// set Display
	glutReshapeFunc(OnReshape); // OnReshape called when window is resized
	atexit(OnExit);

	puInit();	/* initialise pui */
	InitUI();	/* call custom UI for PUI initialisation function */
	Init();		/* call custom openGL initialisation function */

	glutMainLoop();

	return 0;
}
