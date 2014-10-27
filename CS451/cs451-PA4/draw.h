//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------

#pragma once


#if ( (defined(__MACH__)) && (defined(__APPLE__)) )   
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
//#include <GL/glext.h>
#endif

extern CS451_PA4_RENDERING_TYPE cs451_pa4_render_type; //new in CS451 PA4, defined in main.h

#include <GL/gli.h>
#include <GL/gliLight.h>
#include <time.h>

#include <map>
using namespace std;

#include "Basic.h"
#include "model.h"
using namespace mathtool;

#include "texture.h"
#include "shader.h"
Shader shader;
extern Texture color_texture;

//-----------------------------------------------------------------------------
//variables used in rendering

bool showWire=false; //on/off wireframe
bool showSharpEdgeOnly=false;
bool randomColor=false;
bool background=false; //on/off background
bool light=true; //on/off background

//Store display IDs and model colors
map<model*,int> model_solid_gids;
map<model*,int> model_wire_gids;
map<model*,Vector3d> model_colors;

inline void DisplayModel(model& M, bool randcolor=false)
{
	//draw
	if (randcolor){
		if (model_colors.find(&M) == model_colors.end())
			model_colors[&M] = Vector3d(drand48() + 0.5, drand48() + 0.5, drand48(), +0.5).normalize() + Vector3d(0.25, 0.25, 0.25);
		glColor3dv(model_colors[&M].get());
	}

    {
        const Point3d& O=COM;

		//Draw facets
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 0.5f, 0.5f );
		glBegin(GL_TRIANGLES);
		for(unsigned int i=0;i<M.t_size;i++)
		{
            const triangle & t=M.tris[i];
            for(int k=0;k<3;k++)
			{
				vertex& v=M.vertices[t.v[k]];
				const Vector2d& tc = t.texcoord[k];
				glNormal3dv(v.n.get());
				glTexCoord2d(tc[0], tc[1]);
                const Point3d& pt=v.p;
                glVertex3d(pt[0]-O[0],pt[1]-O[1],pt[2]-O[2]);
            }
		}
		glEnd();
		glDisable( GL_POLYGON_OFFSET_FILL );
    }
}

inline void DisplayModelWireFrame(model& M, bool randcolor=false)
{
    //Draw Edges
    if(showWire){

	    if(model_wire_gids.find(&M)==model_wire_gids.end())
    	{
	        const Point3d& O=COM;
	        model_wire_gids[&M]=glGenLists(1);
			glNewList(model_wire_gids[&M],GL_COMPILE);
			glBegin(GL_LINES);
            for(uint i=0;i<M.e_size;i++){
                glColor3f(0,0,0);
                const edge & e=M.edges[i];
                if(e.fid.size()==2){//normal case, check if e is sharp
                    triangle& f1=M.tris[e.fid.front()];
                    triangle& f2=M.tris[e.fid.back()];
                    if(fabs(1-f1.n*f2.n)<1e-2){
                        if(showSharpEdgeOnly) continue; //not sharp
                        else
                            glColor3f(0.7f,0.7f,0.7f);
                    }
                }

                Point3d& p1=M.vertices[e.vid[0]].p;
                Point3d& p2=M.vertices[e.vid[1]].p;
                glVertex3d(p1[0]-O[0],p1[1]-O[1],p1[2]-O[2]);
                glVertex3d(p2[0]-O[0],p2[1]-O[1],p2[2]-O[2]);
            }
            glEnd();
			glEndList();
		}

		glCallList(model_wire_gids[&M]);
    }
}


//copied from meshlab
void DisplayBackground(void)
{
	float topcolor[]={1,1,1};
	float bottomcolor[]={1,1,0.5};
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
		glColor3fv(topcolor);  	glVertex2f(-1, 1);
		glColor3fv(bottomcolor);	glVertex2f(-1,-1);
		glColor3fv(topcolor);	glVertex2f( 1, 1);
		glColor3fv(bottomcolor);	glVertex2f( 1,-1);
	glEnd();
	
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawAll()
{
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        DisplayModel(*i,randomColor);
    }
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        DisplayModelWireFrame(*i);
    }
}

//-----------------------------------------------------------------------------
void Display( void )
{
    //Init Draw
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    if(background) DisplayBackground();
 
	glColor3d(1,0,0);

	shader.bind();
	
	//Bind textures
	if (cs451_pa4_render_type == PA4_TEXTURE_RENDERING || cs451_pa4_render_type == PA4_NORAMALMAP_RENDERING || cs451_pa4_render_type == PA4_RELIEFMAP_RENDERING)
	{
		color_texture.bind(shader.id());
	}

	if (cs451_pa4_render_type == PA4_NORAMALMAP_RENDERING || cs451_pa4_render_type == PA4_RELIEFMAP_RENDERING)
	{
		normalmap_texture.bind(shader.id());
	}

	if (cs451_pa4_render_type == PA4_NORAMALMAP_RENDERING || cs451_pa4_render_type == PA4_RELIEFMAP_RENDERING)
	{
		reliefmap_texture.bind(shader.id());
	}

	//Draw model
	drawAll();

	//Unbind textures
	if (cs451_pa4_render_type == PA4_TEXTURE_RENDERING || cs451_pa4_render_type == PA4_NORAMALMAP_RENDERING || cs451_pa4_render_type == PA4_RELIEFMAP_RENDERING)
	{
		color_texture.unbind();
	}

	if (cs451_pa4_render_type == PA4_NORAMALMAP_RENDERING || cs451_pa4_render_type == PA4_RELIEFMAP_RENDERING)
	{
		normalmap_texture.unbind();
	}

	if (cs451_pa4_render_type == PA4_NORAMALMAP_RENDERING || cs451_pa4_render_type == PA4_RELIEFMAP_RENDERING)
	{
		reliefmap_texture.unbind();
	}

	shader.unbind();
}

void setupLight()
{
	//Let's have light!
	light0_position[0] = COM[1] + 100;
	light0_position[1] = COM[2] + 100;
	light0_position[2] = COM[3] + 100;

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);

	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_high_shininess);

	//setup parameters for spotlight
	// Definig spotlight attributes
	GLfloat spotDir[] = { -1, -1, -1 };
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 60.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 20.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
	//--------------------------------------------------

	glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------
// regular openGL callback functions
bool InitGL()
{
    // transparent
    glShadeModel(GL_SMOOTH);
    //glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    // others
    glEnable( GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
    glClearColor(0.5, 0.5, 0.5, 1.0);

    //Let's have light!
    setupLight();

#if ( (defined(__MACH__)) && (defined(__APPLE__)) )
#else
    glewInit();
#endif

        //initialize your shader
	if (cs451_pa4_render_type == PA4_SPOTLIGHT_RENDERING)
	{
		shader.init("spotlight_shading.vert", "spotlight_shading.frag");
	}
	else if (cs451_pa4_render_type == PA4_TEXTURE_RENDERING)
	{
		shader.init("texturing.vert", "texturing.frag");
	}
	else if (cs451_pa4_render_type == PA4_NORAMALMAP_RENDERING)
	{
		shader.init("normalmapping.vert", "normalmapping.frag");
	}
	else if (cs451_pa4_render_type == PA4_RELIEFMAP_RENDERING)
	{
		shader.init("reliefmapping.vert", "reliefmapping.frag");
	}

    return true;
}

void Reshape( int w, int h)
{
    glViewport( 0, 0, (GLsizei)w, (GLsizei)h );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

	//Perspective view
    gluPerspective( 60, 1.0f*w/h, R/100, R*100 );

	//Othogonal view
	//glOrtho(-R * 1.5, R * 1.5, -R * 1.5, R * 1.5, -R * 100, R * 100);


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

//Used for simulation/anitmation. 
void TimerCallback(int value)
{
    //in simuation state
    glutPostRedisplay();
    glutTimerFunc(30, TimerCallback,value);
}

//Handle keyboard events
void Keyboard( unsigned char key, int x, int y )
{
    // find closest colorPt3D if ctrl is pressed...
    switch( key ){
        case 27: exit(0);
        case 'w' : showWire=!showWire; break;
        case 'r' : randomColor=!randomColor; break;
		case 'R' : model_colors.clear(); break;
		case 'L' : light=!light; break;
		case 'b' : background=!background; break;
		case 'S' : showSharpEdgeOnly=!showSharpEdgeOnly;
		           for(map<model*,int>::iterator i=model_wire_gids.begin();i!=model_wire_gids.end();i++) glDeleteLists(i->second,1);
		           model_wire_gids.clear();
		           break;
    }
    glutPostRedisplay();
}



