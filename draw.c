#include "main.h"
//#include "dolphin1k.h"
//#include "moth2.h"
//#include "boate.h"
//#include "moby2.h"
#define T_OFFSET 0.09765625

model_data *model = NULL;
void import_model();

SDL_Window* mywindow = NULL;				// the usual window
SDL_Renderer* myrenderer = NULL;
SDL_GLContext gContext;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern node nodes[GRIDL][GRIDW];
extern spring springv[GRIDL - 1][GRIDW];
extern spring springh[GRIDL][GRIDW - 1];
extern node nodes[GRIDL][GRIDW];
extern widget* widgets[];
extern int no_of_widget;
vector3 colours[ELS * 10];			// colours for the vertices in each triangle fan
GLuint model_buffer = 0;
GLsizei no_of_verts;
GLsizei stride;				// buffers for vbo
GLfloat hud_r = 0.9;
GLfloat hud_g = 0.3;
GLfloat hud_b = 0.5;
TTF_Font	*active_font = NULL;		// TODO get set sonts and free in close sdl
TTF_Font	*hud_font= NULL;
TTF_Font	*menu_font = NULL;
TTF_Font	*ed_font = NULL;	


typedef struct tex_index{GLfloat coords[8];}tex_index;

const tex_index tsq[4] = {	
	{{0.25, 0, 0.25, 1, 0, 0, 0, 1}},
	{{0.5, 0, 0.5, 1, 0.25, 0, 0.25, 1}},
	{{0.75, 0, 0.75, 1, 0.5, 0, 0.5, 1}},
	{{1, 0, 1, 1, 0.75, 0, 0.75, 1}}
};
const tex_index t_nums[11] = {
//	L-LEFT				U-LEFT				L-RIGHT				U-RIGHT	
 	{{0, 0,				0, 1,				T_OFFSET, 0,		T_OFFSET, 1 	}},	
	{{T_OFFSET, 0,		T_OFFSET, 1, 		T_OFFSET * 2, 0, 	T_OFFSET * 2, 1 }},
	{{T_OFFSET * 2, 0, 	T_OFFSET * 2, 1,	T_OFFSET * 3, 0, 	T_OFFSET * 3, 1 }},
	{{T_OFFSET * 3, 0, 	T_OFFSET * 3, 1,	T_OFFSET * 4, 0, 	T_OFFSET * 4, 1 }},
	{{T_OFFSET * 4, 0, 	T_OFFSET * 4, 1,	T_OFFSET * 5, 0, 	T_OFFSET * 5, 1 }},
	{{T_OFFSET * 5, 0, 	T_OFFSET * 5, 1,	T_OFFSET * 6, 0, 	T_OFFSET * 6, 1 }},
	{{T_OFFSET * 6, 0, 	T_OFFSET * 6, 1,	T_OFFSET * 7, 0, 	T_OFFSET * 7, 1 }},
	{{T_OFFSET * 7, 0, 	T_OFFSET * 7, 1,	T_OFFSET * 8, 0, 	T_OFFSET * 8, 1 }},
	{{T_OFFSET * 8, 0, 	T_OFFSET * 8, 1,	T_OFFSET * 9, 0, 	T_OFFSET * 9, 1 }},
	{{T_OFFSET * 7, 1,	T_OFFSET * 7, 0,	T_OFFSET * 6, 1,	T_OFFSET * 6, 0	}},
	{{T_OFFSET * 9, 0, 	T_OFFSET * 9, 1, 	T_OFFSET * 10, 0, 	T_OFFSET * 10, 1 }}
};

int power_two(int n);

void import_model(){
	
	no_of_verts = 2712;
	stride = sizeof(model_data);
	//model = &import[0];
	SDL_RWops* file = SDL_RWFromFile("data/new_model.dat", "w+b");
	SDL_RWwrite(file, &no_of_verts, sizeof(int), 1);
	SDL_RWwrite(file, model, sizeof(model_data), no_of_verts);
	SDL_RWclose(file);
}

void draw_setup_tiles(vector3 pos, int type, int angle){


	static const GLfloat square[] = { 	-GRID_SPACING * 2, 	0,	-GRID_SPACING * 2,
										-GRID_SPACING * 2, 	0, 	GRID_SPACING * 2,
										GRID_SPACING * 2, 	0, 	-GRID_SPACING * 2,
										GRID_SPACING * 2, 	0,	GRID_SPACING * 2 };
	glColor4f(1.0, 1.0, 1.0, 1.0);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, square);

	glTexCoordPointer(2, GL_FLOAT, 0,  &tsq[type]);
	glPushMatrix();
		// tile
		glTranslatef(pos.x, pos.y + MODHEIGHT, pos.z);
		glRotatef(angle, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, t_cards);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// Shadow
		
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
}

int init_sdl(){
	//import_model();
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("init error! sdl: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );	//Use OpenGL 2.1
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	mywindow = SDL_CreateWindow("spring grid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*2, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	if (mywindow == NULL){
		printf("error creating window! sdl:%s\n", SDL_GetError());
		return 1;
	}

	gContext = SDL_GL_CreateContext(mywindow); //Create context
	if(gContext == NULL)
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		
	SDL_GL_SetSwapInterval(1);					//Use Vsync
	initGL();

	int flags = IMG_INIT_PNG;

	if (!(IMG_Init(flags) & flags)){
		printf("png init error! sdl: %s\n", SDL_GetError());
		return 1;
	}

	if(TTF_Init() == -1){
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
		return 1;
	}
glewInit();
	//initGL();
	set_viewport();
	ed_font = TTF_OpenFont("./Ultra.ttf", (int)(SCREEN_HEIGHT * 0.02));
	hud_font = TTF_OpenFont("./njnaruto.ttf", (int)(SCREEN_HEIGHT * 0.06));
	if(!hud_font) {
    	printf("TTF_OpenFont: %s\n", TTF_GetError());
	}
	assert(ed_font != NULL);
	assert(hud_font != NULL);
	return 0; 


}	// end of sdl_init

void initGL(){

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBlendFunc(GL_SRC_COLOR, GL_ONE);
	
	glEnable(GL_DEPTH_TEST); 
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor( 0.f, 0.f, 0.f, 1.f );

	glEnable(GL_LIGHT0);
	// Create light components
	GLfloat ambientLight[] = { 0.8f, 0.2f, 0.8f, 1.0f };
	GLfloat diffuseLight[] = { 0.2f, 0.2f, 0.2, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { -2.5f, 1.0f, -1.0f, 1.0f };
	// Assign created components to
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
		
}

void set_lights(vector3 col){

	glEnable(GL_LIGHT0);
	float i = 0.3;
	GLfloat ambientLight[] = {col.x, col.y, col.z, 1.0f};
	GLfloat diffuseLight[] = {col.x * i, col.y * i, col.z * i, 1.0f};
	//GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	//GLfloat position[] = { -2.5f, 1.0f, -1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	//glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_hud_colour(vector3 col){

	hud_r = col.x;
	hud_g = col.y;
	hud_b = col.z;
}

void set_viewport(){
	
	//SDL_DisplayMode dm;
	//SDL_GetCurrentDisplayMode(0, &dm);

	//glViewport(0, 0, dm.w, dm.h);
	//SCREEN_WIDTH = dm.w;
	//SCREEN_HEIGHT = dm.h;
	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glViewport(SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glViewport(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);


	glLoadIdentity();
	//gluPerspective(45.0f,(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,0.1f,10.0f);
	float fW, fH;
	float zNear = 0.1;
	float zFar = 10;
	float aspect = (float)(SCREEN_WIDTH)/SCREEN_HEIGHT;//dm.w/dm.h;
	float fovY = 45.0;
	fH = tan( fovY / 360 * PI ) * zNear;
	fW = fH * aspect;
	glFrustum( -fW, fW, -fH, fH, zNear, zFar );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLenum error = GL_NO_ERROR;
	error = glGetError();
	if (error != GL_NO_ERROR){
		printf( "Error in set_veiwport! %d\n", error);
	}
}


GLuint load_shaders(const char *v, const char *f){

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);

	// LOAD VERTEX SHADER
	SDL_RWops* vprog = SDL_RWFromFile(v, "r");
    Sint64 length = SDL_RWseek(vprog, 0, RW_SEEK_END);		// get size of file
	SDL_RWseek(vprog, 0, RW_SEEK_SET);						// seek back to start of file

	char* buffer;										// allocate memory for buffer
	buffer = malloc((length + 1) * sizeof(char));
	
	SDL_RWread(vprog, buffer, length, 1);				// read file into buffer
	buffer[length] = '\0';								// null terminate the source code string
	char const * p = buffer;
	glShaderSource(vshader, 1, &p, NULL);
	glCompileShader(vshader);
	SDL_RWclose(vprog);									// cleanup
	free(buffer);

	// LOAD FRAGMENT SHADER
	SDL_RWops* fprog = SDL_RWFromFile(f, "r");
	length = SDL_RWseek(fprog, 0, RW_SEEK_END);		// get size of file
	SDL_RWseek(fprog, 0, RW_SEEK_SET);						// seek back to start of file
										// allocate memory for buffer
	buffer = malloc((length + 1) * sizeof(char));
	
	SDL_RWread(fprog, buffer, length, 1);				// read file into buffer
	buffer[length] = '\0';								// null terminate the source code string
	p = buffer;
	glShaderSource(fshader, 1, &p, NULL);
	glCompileShader(fshader);
	SDL_RWclose(fprog);									// cleanup
	free(buffer);

	GLuint Program = glCreateProgram();					// linking
    glAttachShader(Program, vshader);
    glAttachShader(Program, fshader);
    glLinkProgram(Program);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return Program;
}

void close_sdl(){

	for (int i=0;i<MAX_SOUNDS;i++){
		if (sound[i].effect != NULL){
			Mix_FreeChunk(sound[i].effect);
		}
	}
	Mix_HookMusicFinished(NULL);
	free_music();
 	Mix_CloseAudio();
	
	TTF_CloseFont(ed_font);// TTF_CloseFont(title_font);TTF_CloseFont(menu_font);
	ed_font = NULL;
	TTF_CloseFont(hud_font);// TTF_CloseFont(title_font);TTF_CloseFont(menu_font);
	hud_font = NULL;
 	TTF_Quit();

 	//glDeleteTextures(1, &t_grenade);
 	//glDeleteTextures(1, &t_torpedo);
 	glDeleteTextures(1, &t_parts);
 	//glDeleteTextures(1, &t_shadow);
 	//glDeleteTextures(1, &t_badman);
 	//glDeleteTextures(1, &t_badcat);
	glDeleteTextures(1, &t_cards);

	IMG_Quit();
 	
 	if (model_buffer != 0)
		glDeleteBuffers(1, &model_buffer);
	
	SDL_DestroyRenderer(myrenderer);
	myrenderer = NULL;
	SDL_DestroyWindow(mywindow);
	mywindow = NULL;

	SDL_Quit();
}

GLuint load_texture(const char *s, int type){

	GLuint newtexture;
	newtexture = 0;
	SDL_Surface* loaded = IMG_Load(s);
	if (loaded == NULL){
		printf("file not here? sdl: %s\n", SDL_GetError());
	}

	else{
		glGenTextures(1, &newtexture);
		glBindTexture(GL_TEXTURE_2D, newtexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
		if (type == 8){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, loaded->w, loaded->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded->pixels);
		}
		if (type == 32){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loaded->w, loaded->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded->pixels);
		}
		SDL_FreeSurface(loaded);
	}
	return newtexture;
}

void grid_colours(float x, float y, float z){

	int p = 0;  
	// set the colours
		for (int j = 0; j < ELS; ++j){
			for (int i = 0; i < 10; ++i){
				// random colour at the center of each tile
				if (i == 0){
					colours[p].x = (float)rand()/(float)RAND_MAX;
					colours[p].y = (float)rand()/(float)RAND_MAX;
					colours[p].z = 0.8; //(float)rand()/(float)RAND_MAX;
					//colours[p].w = 1 - j * 0.0009;
				// outer colour
				}else{
					colours[p].x = x;//0.2;// + (j%3) * 0.2;
					colours[p].y = y;//0.069;// - (j%13) * 0.015;
					colours[p].z = z;//0.07;// - (j%19) * 0.007;
					//colours[p].w = 1 - j * 0.005;
				}
				++p;
			}
		}
}

void draw_grid(){

	int n = 0;
	int m = 0;
	
	GLfloat vertices[ELS * 30];							// no. of fans * vertex per fan
	//GLint startsv[ELS];	
	//GLsizei counts[ELS];

	// set the colours
	/*for (int j = 0; j < els; ++j){
		for (int i = 0; i < 10; ++i){
			// random colour at the center of each tile
			if (i == 0){
				colours[p].x = 1; //(float)rand()/(float)RAND_MAX;
				colours[p].y = (float)rand()/(float)RAND_MAX;
				colours[p].z = (float)rand()/(float)RAND_MAX;
				colours[p].w = 1 - j * 0.0009;
			// outer colour
			}else{
				colours[p].x = 0.2;// + (j%3) * 0.2;
				colours[p].y = 0.069;// - (j%13) * 0.015;
				colours[p].z = 0.07;// - (j%19) * 0.007;
				colours[p].w = 1 - j * 0.005;
			}
			++p;
		}
	} */

	// SHOULD BE ABLE TO USE NODES AS AN INDEXED VBO W/TRI STRIPS
	// RUNNING LENGTHWISE FOR GRIDW/2 DRAW CALLS!!!!!!
	// ON ANDROID WIREFRAME MODE NOT EVEN AN OPTION
	for (int i = 1; i < GRIDL; i += 2){
		for (int j = 1; j < GRIDW; j += 2){
			
			//startsv[m] = n/3;
			//counts[m] = 10;

			++m;
			vertices[n] = nodes[i][j].pos.x;
			++n;
			vertices[n] = nodes[i][j].pos.y;
			++n;
			vertices[n] = nodes[i][j].pos.z; 
			++n;
			vertices[n] = nodes[i][j-1].pos.x;
			++n;
			vertices[n] = nodes[i][j-1].pos.y;
			++n;
			vertices[n] = nodes[i][j-1].pos.z;
			++n;
			vertices[n] = nodes[i-1][j-1].pos.x;
			++n;
			vertices[n] = nodes[i-1][j-1].pos.y;
			++n;
			vertices[n] = nodes[i-1][j-1].pos.z;
			++n;
			vertices[n] = nodes[i-1][j].pos.x;
			++n;
			vertices[n] = nodes[i-1][j].pos.y;
			++n;
			vertices[n] = nodes[i-1][j].pos.z;
			++n;
			vertices[n] = nodes[i-1][j+1].pos.x;
			++n;
			vertices[n] = nodes[i-1][j+1].pos.y;
			++n;
			vertices[n] = nodes[i-1][j+1].pos.z;
			++n;
			vertices[n] = nodes[i][j+1].pos.x;
			++n;
			vertices[n] = nodes[i][j+1].pos.y;
			++n;
			vertices[n] = nodes[i][j+1].pos.z;
			++n;
			vertices[n] = nodes[i+1][j+1].pos.x;
			++n;
			vertices[n] = nodes[i+1][j+1].pos.y;
			++n;
			vertices[n] = nodes[i+1][j+1].pos.z;
			++n;
			vertices[n] = nodes[i+1][j].pos.x;
			++n;
			vertices[n] = nodes[i+1][j].pos.y;
			++n;
			vertices[n] = nodes[i+1][j].pos.z;
			++n;
			vertices[n] = nodes[i+1][j-1].pos.x;
			++n;
			vertices[n] = nodes[i+1][j-1].pos.y;
			++n;
			vertices[n] = nodes[i+1][j-1].pos.z;
			++n;
			vertices[n] = nodes[i][j-1].pos.x;
			++n;
			vertices[n] = nodes[i][j-1].pos.y;
			++n;
			vertices[n] = nodes[i][j-1].pos.z;
			++n;
		}
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableClientState(GL_COLOR_ARRAY);	
	glEnableClientState(GL_VERTEX_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, colours);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		//glMultiDrawArrays(GL_TRIANGLE_FAN, startsv, counts, els);
		for (n = 0; n < ELS*10; n+= 10)
			glDrawArrays(GL_TRIANGLE_FAN, n, 10);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//GLenum error = glGetError();
}
 
int power_two(int n){

	unsigned int x;
	x = n;
	x--;
    x |= x >> 1;  // handle  2 bit numbers
    x |= x >> 2;  // handle  4 bit numbers
    x |= x >> 4;  // handle  8 bit numbers
    x |= x >> 8;  // handle 16 bit numbers
    x |= x >> 16; // handle 32 bit numbers
    x++;

    return (int)x;
}

void draw_stack(){

	GLsizei els = p1.no_of_tiles;	// number of cards
	int n = 0;
	int tidx = 0;

	GLfloat square[els * 12];
	
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, t_cards);
	glColor4f(1.0, 1.0, 1.0, 1.0);	
	// draw stack above waves TODO IS THIS TOO HIGH
	// float add = nodes[2][(int)(GRIDC + GRID_SPACING_INV * p1.position.x)].pos.y + 0.0625;

	for (int i = 0; i < els; ++i){

		n = 0;

		square[n++] = p1.position.x - GRID_SPACING * 2;
		square[n++]	= p1.position.y + i * 0.002 ;//+ add;
		square[n++]	= p1.position.z - GRID_SPACING * 2;
		square[n++]	= p1.position.x - GRID_SPACING * 2;
		square[n++]	= p1.position.y + i * 0.002 ;//+ add;
		square[n++]	= p1.position.z + GRID_SPACING * 2;
		square[n++]	= p1.position.x + GRID_SPACING * 2;
		square[n++]	= p1.position.y + i * 0.002 ;//+ add;
		square[n++]	= p1.position.z - GRID_SPACING * 2;
		square[n++]	= p1.position.x + GRID_SPACING * 2;
		square[n++]	= p1.position.y + i * 0.002 ;//+ add;
		square[n++]	= p1.position.z + GRID_SPACING * 2;
		
		tidx = p1.tile_type[i];
		glTexCoordPointer(2, GL_FLOAT, 0, &tsq[tidx]);
		glVertexPointer(3, GL_FLOAT, 0, square);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
	}

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);					
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void display_stack(){

	GLfloat size = 0.012;

	 GLfloat square[8] = { 	size , size,
							size, -size,
							-size, size,
							-size, -size };
	//glBlendFunc(GL_ONE, GL_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
	if (p1.no_of_tiles == 0)
		return;
	glColor4f(1, 1, 1, 0.7);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, t_cards);	
	glVertexPointer(2, GL_FLOAT, 0, square);
	int x;

	for ( int i = 0; i < p1.no_of_tiles; ++i){

		x = p1.tile_type[i];
		glTexCoordPointer(2, GL_FLOAT, 0,  &tsq[x]);

		glPushMatrix();
			// tile
			glTranslatef(-0.01 +0.003*i, -0.12, -0.4);
			//glRotatef(90, 0, 1, 0);
							// TODO switch for texture
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glPopMatrix();
		
	}

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}
void draw_active_tiles(){

	static const GLfloat square[] = { 	-GRID_SPACING * 2, 	0,	-GRID_SPACING * 2,
										-GRID_SPACING * 2, 	0, 	GRID_SPACING * 2,
										GRID_SPACING * 2, 	0, 	-GRID_SPACING * 2,
										GRID_SPACING * 2, 	0,	GRID_SPACING * 2};
	GLsizei els = p1.act.no_active;
	if (els == 0)
		return;


	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glVertexPointer(3, GL_FLOAT, 0, square);
	int x;

	for (int i = 0; i < els; ++i){

		x = p1.act.tiles[i].type;
		glTexCoordPointer(2, GL_FLOAT, 0,  &tsq[x]);

		switch (x){

			case GRENADE:
				glPushMatrix();
					// tile
					glTranslatef(((grenade *)p1.act.tiles[i].data)->position.x, ((grenade *)p1.act.tiles[i].data)->position.y, ((grenade *)p1.act.tiles[i].data)->position.z);
					glRotatef(((grenade *)p1.act.tiles[i].data)->spin, 0, 1, 0);
					glBindTexture(GL_TEXTURE_2D, t_cards);					// TODO switch for texture
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					// Shadow
					glTexCoordPointer(2, GL_FLOAT, 0,  &tsq[0]);
					glBindTexture(GL_TEXTURE_2D, t_cards);	
					glTranslatef(0, -((grenade *)p1.act.tiles[i].data)->position.y + 0.002, 0);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glPopMatrix();
				break;
			case TORPEDO:
				glPushMatrix();
					// tile
					glTranslatef(((torpedo *)p1.act.tiles[i].data)->position.x, ((torpedo *)p1.act.tiles[i].data)->position.y, ((torpedo *)p1.act.tiles[i].data)->position.z);
					glBindTexture(GL_TEXTURE_2D, t_cards);					// TODO switch for texture
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glPopMatrix();
				break;
			default:
				break;
		}
	}

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void draw_entities(){

	static int x;
	++x;
	x = x % 360;

	GLfloat square[] = { -0.04, 0.04, 0, 0, 0, 0.08, 0.04, 0.04 };
	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, t_cards);	
	glVertexPointer(2, GL_FLOAT, 0, square);

	for (int i = 0; i < en.no_active; ++i){
		
		glTexCoordPointer(2, GL_FLOAT, 0, &tsq[en.data[i].type]);
		glPushMatrix();
			glTranslatef(en.data[i].pos.x, en.data[i].pos.y, en.data[i].pos.z);
			glRotatef(x, 0, 1, 0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);		
		glPopMatrix();
		
	}
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void load_model(){

	// read model size from file
	// malloc arrays
	// create vbos for verex and nomal
	// free arrays
	if (model_buffer != 0){
		glDeleteBuffers(1, &model_buffer);
		model_buffer = 0;
	}
	glGenBuffers(1, &model_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, model_buffer);
	//								array size 				the array 
	glBufferData(GL_ARRAY_BUFFER, sizeof(model_data) * no_of_verts, model, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void draw_models(){

	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindBuffer(GL_ARRAY_BUFFER, model_buffer); 
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, 0);
	glNormalPointer(GL_FLOAT, stride, (void*)(sizeof(GLfloat)*3));

	for (int i = 0; i < en3d.no_active; ++i){

		glPushMatrix();
			glTranslatef(en3d.data[i].pos.x, en3d.data[i].pos.y, en3d.data[i].pos.z);
			glScalef(model_size, model_size, model_size);
			glRotatef(en3d.data[i].ang.z, 1, 0, 0);
			glDrawArrays(GL_TRIANGLES, 0, no_of_verts);
		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_LIGHTING);
	return;

}

void display_model(){
	
	static float angle = 90;

	angle += 0.2;
	if (angle > 360)
		angle = 0; 

	float size = 0.12;
	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindBuffer(GL_ARRAY_BUFFER, model_buffer); 
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, 0);
	glNormalPointer(GL_FLOAT, stride, (void*)(sizeof(GLfloat)*3));

	//glViewport(256, 380, 500, SCREEN_HEIGHT);
	glPushMatrix();
		glTranslatef(0.045, -0.05, -0.5);
		glScalef(size, size, size);
		glRotatef(angle, 0, 1, 0);
		glDrawArrays(GL_TRIANGLES, 0, no_of_verts);
	glPopMatrix();
	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_LIGHTING);

}

void draw_particles(const particles *parts){
	
	//static GLfloat distance[] = { 1.0, 1.0, 1.00 };
//glPointParameterfv(GL_POINT_SIZE_MIN, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);					
	//glTexCoordPointer(2, GL_FLOAT, 0,  &tsq[0]);

	
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glPointSize(64 * parts->age); // TODO MOVE TO GLINIT MAKE DPI INDEPENDENT
	glColor4f(1, 0.2, 0.2, parts->age);
	glBindTexture(GL_TEXTURE_2D, t_parts); // TODO OPT TO ONCE PER FRAME?
	glVertexPointer(3, GL_FLOAT, 0, parts->vertex);

	glDrawArrays(GL_POINTS, 0, MAX_PARTS);
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_POINT_SPRITE);
	glEnable(GL_DEPTH_TEST);
}

void render_text(texture *t, char s[], int c, int size){

	//TTF_Font 		*text_font = NULL;
	SDL_Color 		col = {17, 126, 255};
	SDL_Color 		col2 = {10, 100, 200};
	//SDL_Color 		inv = {23, 23, 128 - col.b};
	SDL_Surface* 	load_text = NULL;
	// TODO ENUM FOR COLOUR AND SIZE?
	// TODO IN FINAL GAME WE WILL NOT DO THIS
	// if  (size != 0){
	// 	TTF_CloseFont(ed_font);// TTF_CloseFont(title_font);TTF_CloseFont(menu_font);
	// 	score_font = NULL;
	// 	score_font = TTF_OpenFont("Ultra.ttf", size);
	// }

	if (c == 0)
		load_text = TTF_RenderText_Blended(active_font, s, col);
	else if (c == 1)
		load_text = TTF_RenderText_Blended(active_font, s, col2);
	
	int p2x;
	int p2y;
	p2x = power_two(load_text->w);
	p2y = power_two(load_text->h);
	SDL_Surface* resized = SDL_CreateRGBSurface(0, power_two(load_text->w), power_two(load_text->h), 32, 0, 0, 0, 0);
	
	t->w = load_text->w;
	t->h = load_text->h;
	int offsetx = (p2x - t->w)/2 - 1;
	int offsety = (p2y - t->h)/2 - 1;

	glGenTextures(1, &t->t);
	glBindTexture(GL_TEXTURE_2D, t->t);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p2x, p2y, 0, GL_RGBA, GL_UNSIGNED_BYTE, resized->pixels);	
	glTexSubImage2D(GL_TEXTURE_2D, 0, offsetx, offsety, t->w, t->h, GL_RGBA, GL_UNSIGNED_BYTE, load_text->pixels);

	t->w = p2x;
	t->h = p2y;
	SDL_FreeSurface(load_text);
	SDL_FreeSurface(resized);
	load_text = NULL;
	resized = NULL;
	//text_font = NULL;
	
	return;
}

void draw_text(char *s, int just, int x, int y){

	texture t;
	render_text(&t, s, 1, 0);
	GLfloat vertex[8];

	if (just == CENTER){
		vertex[0] = x-t.w/2;
		vertex[1] = y;
		vertex[2] = x+t.w/2;
		vertex[3] = y;
		vertex[4] = x+t.w/2; 
		vertex[5] = y+t.h;
		vertex[6] = x-t.w/2;
		vertex[7] = y+t.h;
	}
	else if (just == LEFT){
		vertex[0] = x;
		vertex[1] = y;
		vertex[2] = x+t.w;
		vertex[3] = y;
		vertex[4] = x+t.w; 
		vertex[5] = y+t.h;
		vertex[6] = x;
		vertex[7] = y+t.h;
	}
	else if (just == RIGHT){
		vertex[0] = x-t.w;
		vertex[1] = y;
		vertex[2] = x;
		vertex[3] = y;
		vertex[4] = x; 
		vertex[5] = y+t.h;
		vertex[6] = x-t.w;
		vertex[7] = y+t.h;
	}

	glColor4f(1, 1, 1, 1);
	GLfloat texver[8] = {0, 0, 1, 0, 1, 1, 0, 1};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, t.t);
	glVertexPointer(2, GL_FLOAT, 0, vertex);
	glTexCoordPointer(2, GL_FLOAT, 0, texver);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDeleteTextures(1, &t.t);
	glDisable(GL_TEXTURE_2D);

}

void draw_widgets(){

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	for (int i = 0; i < no_of_widget; ++i){

		if (widgets[i]->state == 0)
			glColor4f(1, 1, 1, 0.5);
		else
			glColor4f(1, 1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, widgets[i]->t);
		glVertexPointer(2, GL_FLOAT, 0, widgets[i]->vertex);
		glTexCoordPointer(2, GL_FLOAT, 0, widgets[i]->texver);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);	
		glDisable(GL_TEXTURE_2D);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		if (widgets[i]->type == SLIDER){
			GLfloat slider[] = {widgets[i]->vertex[0] + widgets[i]->state, 
								widgets[i]->vertex[1],
								widgets[i]->vertex[0] + widgets[i]->state,
								widgets[i]->vertex[5]};
			glVertexPointer(2, GL_FLOAT, 0, slider);
			glDrawArrays(GL_LINES, 0, 2);
		}
		glEnable(GL_TEXTURE_2D);
	}
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void set_ortho(){

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -10, 10);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void unset_ortho(){
	
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();	
	glMatrixMode(GL_MODELVIEW);

}

void draw_score(int score){

#define off 32
#define begin 144
	static const GLfloat square[] = {	

		begin, 48,				begin, 16,				begin + off * 1, 48,	begin + off * 1, 16,
		begin + off * 1, 48,	begin + off * 1, 16,	begin + off * 2, 48,	begin + off * 2, 16,
		begin + off * 2, 48,	begin + off * 2, 16,	begin + off * 3, 48,	begin + off * 3, 16,
		begin + off * 3, 48,	begin + off * 3, 16,	begin + off * 4, 48,	begin + off * 4, 16,
		begin + off * 4, 48,	begin + off * 4, 16,	begin + off * 5, 48,	begin + off * 5, 16,
		begin + off * 5, 48,	begin + off * 5, 16,	begin + off * 6, 48,	begin + off * 6, 16,

		470 - 32 * 1, 48,		470 - 32 * 1, 16, 		470, 48,				470, 16,
		470 - 32 * 2, 48,		470 - 32 * 2, 16, 		470 - 32 * 1, 48,		470 - 32 * 1, 16, 
		470 - 32 * 3, 48,		470 - 32 * 3, 16, 		470 - 32 * 2, 48,		470 - 32 * 2, 16,					
	};
	// TODO MAKE THESE ONLY UPDATE WHEN NECESSARY
	int h = 65;
	GLfloat bar[16];
	bar[0] = 10;	bar[1] = 43;	bar[2] = 10; 	bar[3] = 18;	bar[4] = 106; 	bar[5] = 18;	bar[6] = 106;	bar[7] = 43;
	bar[8] = 13;	bar[9] = 40;	bar[10] = 13; 	bar[11] = 21;	bar[12] = 13+h; 	bar[13] = 40;	bar[14] = 13+h;	bar[15] = 21;

	char digits[7];
	int len = sprintf(digits, "%d", score);
	tex_index coords[9];
	int idx = 0;
	len = 5 - len;
	
	for(int i = 0; i < 6; ++i){
		if (i > len)
			coords[i] = t_nums[(int)digits[idx++] - 48];
		else
			coords[i] = t_nums[0];
	}
	for (int i = 6; i < lives + 6; ++i){
		coords[i] = t_nums[10];
	}

	glColor4f(hud_r, hud_g, hud_b, 0.7);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, t_score);	
	glVertexPointer(2, GL_FLOAT, 0, square);
	glTexCoordPointer(2, GL_FLOAT, 0, &coords[0]);

	for (int i = 0; i < 6 + lives; ++i){		
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);		
	}
	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, bar);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}