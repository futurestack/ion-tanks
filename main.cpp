
/* Simple program:  Create a blank window, wait for keypress, quit.

   Please see the SDL documentation for details on using the SDL API:
   /Developer/Documentation/SDL/docs.html
*/
   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"

#include "fsRendererGL.h"
#include "fsPoint2i.h"
#include <OpenGL/OpenGL.h>
#include <SDL/SDL_opengl.h>

static SDL_Surface *gScreen;
static fsRendererGL * pRenderer;

static fsPoint2i mousePos;

static void initAttributes ()
{
    // Setup attributes we want for the OpenGL context
    
    int value;
    
    // Don't set color bit sizes (SDL_GL_RED_SIZE, etc)
    //    Mac OS X will always use 8-8-8-8 ARGB for 32-bit screens and
    //    5-5-5 RGB for 16-bit screens
    
    // Request a 16-bit depth buffer (without this, there is no depth buffer)
    value = 16;
    SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, value);
    
    
    // Request double-buffered OpenGL
    //     The fact that windows are double-buffered on Mac OS X has no effect
    //     on OpenGL double buffering.
    value = 1;
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, value);
}

static void printAttributes ()
{

}

static void createSurface (int fullscreen)
{
    Uint32 flags = 0;
    
    flags = SDL_OPENGL;
    if (fullscreen)
        flags |= SDL_FULLSCREEN;
    
    // Create window
    gScreen = SDL_SetVideoMode (800, 335, 0, flags);
    if (gScreen == NULL) {
		
        fprintf (stderr, "Couldn't set 640x480 OpenGL video mode: %s\n",
                 SDL_GetError());
		SDL_Quit();
		exit(2);
	}
}

static void initGL ()
{
    
    
#ifdef __FS_ENABLE_VSYNC__
	GLint swap = 1;
	CGLSetParameter ( CGLGetCurrentContext(), kCGLCPSwapInterval, &swap);
#endif
	
	
	//proper transparency setting
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	// no good : text setting glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	glAlphaFunc(GL_GREATER,0.1f);
	glEnable(GL_ALPHA_TEST);
	
	glEnable( GL_BLEND );
	
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	double zSpace = (double)viewport[3];
	glOrtho( 0.0,(double)viewport[2],(double)viewport[3],0.0,-1*zSpace,zSpace );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable (GL_LINE_SMOOTH);				// Initially Disable Line Smoothing
    
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
	
	glEnableClientState( GL_VERTEX_ARRAY );
    
    
    
    glOrtho( 0, 800, 335 , 0, 1, -1 );
	glMatrixMode(GL_PROJECTION);
	glColorMask(true, true, true, true);        
	glLoadIdentity();
    
    
}

static void drawGL ()
{
    static fsColor white(1.0,1.0,1.0,1.0);
    static fsColor black(0.0,0.0,0.0,1.0);

    pRenderer->setColor(white);
    
    pRenderer->clear(white);
    pRenderer->setColor(black);

    
    pRenderer->renderCircle(mousePos, 20);
    

    
}

static void mainLoop ()
{
    SDL_Event event;
    int done = 0;
    int fps = 24;
	int delay = 1000/fps;
    int thenTicks = -1;
    int nowTicks;
    
    
    while ( !done ) {

		/* Check for events */
		while ( SDL_PollEvent (&event) ) {
			switch (event.type) {

				case SDL_MOUSEMOTION:
                    mousePos.x = event.button.x;
					mousePos.y = event.button.y ;//,  event.button.button 
					
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_KEYDOWN:
					/* Any keypress quits the app... */
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
    
        // Draw at 24 hz
        //     This approach is not normally recommended - it is better to
        //     use time-based animation and run as fast as possible
        drawGL ();
        
        
        SDL_GL_SwapBuffers ();

        // Time how long each draw-swap-delay cycle takes
        // and adjust delay to get closer to target framerate
        if (thenTicks > 0) {
            nowTicks = SDL_GetTicks ();
            delay += (1000/fps - (nowTicks-thenTicks));
            thenTicks = nowTicks;
            if (delay < 0)
                delay = 1000/fps;
        }
        else {
            thenTicks = SDL_GetTicks ();
        }

        SDL_Delay (delay);
	}
}

int main(int argc, char *argv[])
{
	// Init SDL video subsystem
	if ( SDL_Init (SDL_INIT_VIDEO) < 0 ) {
		
        fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
		exit(1);
	}

    // Set GL context attributes
    initAttributes ();
    
    // Create GL context
    createSurface (0);
    
    // Get GL context attributes
    printAttributes ();
    
    pRenderer = new fsRendererGL;
    
    // Init GL state
    initGL ();
    
    // Draw, get events...
    mainLoop ();
    delete pRenderer;
    
    // Cleanup
	SDL_Quit();
	
    return 0;
}
