//--------------------------------------------------//
//													//	
//    Learning OpenGL: ÅéóáãùãéêÜ ÌáèÞìáôá OpenGL   //
//													//
//    Ðëáôöüñìá áíÜðôõîçò ðáñáäåéãìÜôùí				//
//													//
//													//
//		ÌÜèçìá 4ï									//
//--------------------------------------------------//


#include <stdio.h>     // - Just for some ASCII messages
#include <stdlib.h>     /* srand, rand */
#include "gl/glut.h"   // - An interface and windows  management library
#include "visuals.h"   // Header file for our OpenGL functions




////////////////// State Variables ////////////////////////
float t=0.0; ///Time

/////////////// Main Program ///////////////////////////

int main(int argc, char* argv[])
{ 
  // initialize GLUT library state
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  
  
  // Define the main window size and initial position 
  // ( upper left corner, boundaries included )
  glutInitWindowSize(1200,1200);
  glutInitWindowPosition(50,50);
  
  // Create and label the main window
  glutCreateWindow("Project");
  
  // Configure various properties of the OpenGL rendering context
  Setup();
  
  // Callbacks for the GL and GLUT events:

  // The rendering function 
  glutKeyboardFunc(KeyboardPressed);
  glutKeyboardUpFunc(KeyboardUnPressed); // Tell GLUT to use the method "keyUp" for key up events 
  glutDisplayFunc(Render);
  glutReshapeFunc(Resize);
  glutIdleFunc(Idle);
  glEnable(GL_DEPTH_TEST);
  //Enter main event handling loop
  glutMainLoop();
  return 0;
}  

