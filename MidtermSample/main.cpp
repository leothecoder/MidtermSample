/*
Template for 2D drawings
*/

//
// To disable console window, valid only for Visual C++ 6.0
//
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )


#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400

#define  TIMER_PERIOD  20 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

int second = 0;
int per = 0;

bool gameStarted = false;

int width, height ; 
int speed = 5;

float pm = 15;
int pmR = 20;
bool pm_closing = true;
//move flags
bool up=false,down=false,left=false,right=false;

//pacman variables
float pmX = 0;
int pmDirection = 1; //1 or -1

//ghost variables
float ghX = 200;
int ghDirection = -1;
int ghEyeBlinkSpeed = 50;


// 
// Add your global variables here.
//

//
// to draw circle, center at (x,y)
//  radius r
//
void circle( int x, int y, int r )
{
#define PI 3.1415
	float angle ;

	glBegin( GL_POLYGON ) ;
	for ( int i = 0 ; i < 100 ; i++ )
	{
		angle = 2*PI*i/100;
		glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
	}
	glEnd();
}

void print(int x, int y, char *string, void *font )
{
	int len, i ;

	glRasterPos2f( x, y );
	len = (int) strlen( string );
	for ( i =0; i<len; i++ )
	{
		glutBitmapCharacter( font, string[i]);
	}
}

void drawString(const char* string)
{
	glPushMatrix();
	while(*string)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *string++);
	glPopMatrix();
}

void drawGradient(int x1, int y1, int w, int h, float r, float g, float b){
	glBegin( GL_QUADS );
	glColor3f( r,g,b );
	glVertex2f( x1, y1 );
	glVertex2f( x1 + w, y1 );
	//glColor3f( r + 0.4, g + 0.4, b + 0.4 );
	glVertex2f( x1 + w, y1 - h );
	glVertex2f( x1, y1 - h );
	glEnd();

	glLineWidth(3);
	glColor3f( 0.2, 0.8, 0.2 );
	glBegin(GL_LINE_LOOP);
	glVertex2f( x1, y1 );
	glVertex2f( x1 + w, y1 );
	glVertex2f( x1 + w, y1 - h );
	glVertex2f( x1, y1 - h );
	glEnd();
	glLineWidth(1);
}

void drawBase(){

	glColor3f(0.5,0.5,1);
	glLineWidth(3);
	for(int i =-300; i<300; i=i+20){
		glBegin( GL_LINES );
		glVertex2f(i+8,-5);
		glVertex2f(i,-15);	
		glEnd();
	}
	glLineWidth(1);
}

void drawPacman(){

	glColor3f(1,1,0);
	circle(0,0,pmR);

	int d = pmDirection;
	glColor3f(0,0,0);
	glBegin(GL_TRIANGLES);
	glVertex2f(0,0);
	glVertex2f(25*d, pm);
	glVertex2f(25*d,-pm);
	glEnd();
}

void drawGhost(){

	glColor3f(1,0.4,0.5);
	//
	//top
	//
	glBegin( GL_QUADS );
		glVertex2f(-25,20);	
		glVertex2f(-15,30);
		glVertex2f(15,30);
		glVertex2f(25,20);
	glEnd();

	//
	//middle
	//
	glBegin( GL_QUADS );
		glVertex2f(25,20);
		glVertex2f(25,-25);
		glVertex2f(-25,-25);
		glVertex2f(-25,20);	
	glEnd();

	//
	// bottom
	//
	glBegin( GL_TRIANGLES ) ;
		glVertex2f( 25,-25 ) ;
		glVertex2f( 5,-40 ) ;
		glVertex2f(  5 , -25 ) ;

		glVertex2f( 5, -25 ) ;
		glVertex2f( -10, -40 ) ;
		glVertex2f(  -10,  -25 ) ;

		glVertex2f( -10, -25 ) ;
		glVertex2f( -25, -40 ) ;
		glVertex2f(  -25,  -25 ) ;
	glEnd();

	//
	//mouth
	//
	glColor3f(0,0,0);	
	glBegin( GL_QUADS );
		glVertex2f(15,-10);	
		glVertex2f(15,-15);
		glVertex2f(-15,-15);
		glVertex2f(-15,-10);
	glEnd();

	//
	//left eye
	//
	glColor3f(1,1,1);	
	glBegin( GL_QUADS );
		glVertex2f(-5,15);	
		glVertex2f(-5,5);
		glVertex2f(-15,5);
		glVertex2f(-15,15);
	glEnd();

	//
	//left eye black dot
	//
	glColor3f(0,0,0);	
	glBegin( GL_QUADS );
		glVertex2f(-10,10);	
		glVertex2f(-10,5);
		glVertex2f(-10 + ghDirection * 5,5);
		glVertex2f(-10 + ghDirection * 5,10);
	glEnd();

	//
	//right eye
	//
	glColor3f(1,1,1);	
	glBegin( GL_QUADS );
		glVertex2f(15,15);	
		glVertex2f(15,5);
		glVertex2f(5,5);
		glVertex2f(5,15);
	glEnd();

	//
	//right eye black dot
	//
	glColor3f(0,0,0);	
	glBegin( GL_QUADS );
		glVertex2f(10,10);	
		glVertex2f(10,5);
		glVertex2f(10 + ghDirection * 5,5);
		glVertex2f(10 + ghDirection * 5,10);
	glEnd();
}

//
// To display onto window using OpenGL commands
//
void display()
{
	glClearColor( 0, 0 , 0 , 0 );
	glClear( GL_COLOR_BUFFER_BIT );

	glLoadIdentity();

	glColor3f(1,1,0);
	char tmp[] =  "PRESS SPACEBAR TO START";
	print(-120,150, tmp, GLUT_BITMAP_HELVETICA_18);

	drawBase();
	drawGradient(-300,5,600,10,0.5,1,0.5);

	glPushMatrix();
	glTranslatef(pmX,27,0);
	drawPacman();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(ghX,27,0);
	glScalef(0.5,0.5,0.5);
	drawGhost();
	glPopMatrix();


	glutSwapBuffers();


}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void ASCIIKeyDown(unsigned char key, int x, int y )
{
	if ( key == 27 )
		exit(0);

	if( key == ' ' ){
		gameStarted = true;
	}
}

void ASCIIKeyUp ( unsigned char key, int x, int y )
{
}


//
// Special Key like F1, F2, F3, Arrow Keys, Page UP, ...
//
void SpecialKeyDown( int key, int x, int y )
{
	switch(key)
	{
	case GLUT_KEY_UP: up=true; break;
	case GLUT_KEY_DOWN: down=true; break;
	case GLUT_KEY_RIGHT: right=true; break;
	case GLUT_KEY_LEFT: left=true; break;	
	}
}


void SpecialKeyUp( int key , int x, int y )
{
	switch(key)
	{
	case GLUT_KEY_UP: up=false; break;
	case GLUT_KEY_DOWN: down=false; break;
	case GLUT_KEY_RIGHT: right=false; break;
	case GLUT_KEY_LEFT: left=false; break;	
	}
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void MouseClick( int button, int stat, int x, int y )
{

}


void MouseMove( int x, int y )
{
}


void MouseDownMove( int x, int y )
{
}


//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void reshape( int w, int h )
{
	width = w ;
	height = h ;
	glViewport( 0,0,w,h) ;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -w/2, w/2, -h/2, h/2, -1, 1);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();

}

#if TIMER_ON == 1
void onTimer( int v ) {

	glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
	// Write your codes here.,

	//
	//pacman mouth animation
	//

	if(pm_closing){	
		pm --;
		if(pm==0)
			pm_closing = false;
	}else {	
		pm ++;
		if(pm==15)
			pm_closing = true;

	}

	if( gameStarted ){

		//pacman moves 
		if(left){
			pmX -=speed;
			pmDirection = -1;
			if(pmX-pmR <= -300)
				pmX = 280;
		}
		if(right){ 
			pmX +=speed;
			pmDirection = 1;
			if(pmX+pmR >= 300)
				pmX = -280;
		}

	}
	else{
		ghEyeBlinkSpeed --;
		if( ghEyeBlinkSpeed == 0){
			ghEyeBlinkSpeed = 50;
			ghDirection -= ghDirection * 2;
		}
	}

	

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif


void main( int argc, char *argv[] )
{

	glutInit(&argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow( "Template" ) ;

	glutDisplayFunc( display ) ;
	glutReshapeFunc( reshape );
	//
	// keyboard registration
	//
	glutKeyboardFunc( ASCIIKeyDown ) ;
	glutKeyboardUpFunc( ASCIIKeyUp ) ;

	glutSpecialFunc( SpecialKeyDown ) ;
	glutSpecialUpFunc( SpecialKeyUp ) ;
	//
	// mouse registration
	//
	glutMouseFunc( MouseClick ) ;
	glutPassiveMotionFunc( MouseMove ) ;
	glutMotionFunc( MouseDownMove ) ;

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
#endif

	glutMainLoop();
}


