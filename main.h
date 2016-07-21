#include <GL/glew.h>
//#include <GL/glu.h>
#include <SDL_mixer.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <SDL_opengl.h>
#include <unistd.h>
#include <stdlib.h> // needed for malloc
#include <assert.h>

#define PI 3.141592653
#define DEGTORAD 0.0174532
#define GRIDW 33
#define GRIDL 101
#define GRID_SPACING 0.02
#define GRID_SPACING_INV 1/GRID_SPACING
#define GRIDC GRIDW/2 - 1
#define NODES 25
#define NODE_MASS 1
#define NODE_DAMPING 0.94
#define SPRING_STIFFNESS 0.1		// affects speed of ripples
#define SPRING_DAMPING 0.0012
#define MAX_TILES 32
#define GRAVITY 0.0001
#define MAX_ENTITIES 32
#define MAX_PARTS 9
#define ELS ((GRIDW - 1)/2) * ((GRIDL - 1)/2)	// no. of triangle fans fir grid
#define MAX_3DENTITIES 32

typedef struct Sound{
	Mix_Chunk *effect;
}Sound;
typedef struct Music{
	Mix_Music *music;
}Music;

enum{				//  SOUND EFFECTS
	SPLASH,
	WHOOSH,
	CYMBOL_SOUND,
	KILL_SOUND,	
	MAX_SOUNDS
};
enum{				// MUSIC
	LOOP1,
	LOOP2,
	LOOP3,
	LOOP4,
	LOOP5,
	LOOP6,
	LOOP7,
	LOOP8,
};
enum{				// OBJECT/CARD TYPES
	SHADOW,
	GRENADE,
	HOOK,
	TORPEDO,
	LIFE,
	MINE,
	MODELS,			// DELIMITS 2D AND 3D OBJECTS
	SINE_Z,
	LINEAR,
	SHIP
};


enum{				// WIDGET TYPES
	LEFT,
	RIGHT,
	CENTER,
	TEXT,
	BUTTON,
	SLIDER,
	SWITCH
};

enum{					// EVENTS
	ALL_CLEAR,			// no of entity == 0, active cards == 0, 
	ALL_DEAD,			// no of entity == 0
	SWITCH_MUSIC,		// CHANGE TRACK AT END OF CURRENT LOOP
	KILLED_ENTITIES,	// no of entity has decreased by X
	GRID_MOD,			// due to trickery order here is critical
	E_TIMER,
	END_LEVEL			// conditions to end level, the final event.
};

extern Sound sound[MAX_SOUNDS];
extern Music music[8];

typedef struct texture {
				int w;
				int h;
				GLuint t;
			}texture;

typedef struct {GLfloat x; GLfloat y; GLfloat z;}vector3;

typedef struct {GLfloat x; GLfloat y; GLfloat z; GLfloat w;}vector4;

typedef struct {GLfloat x; GLfloat y;}vector2;
// GRID COMPONENTS
typedef struct spring{
						vector2 n1;
						vector2 n2;
						float k;			// stiffness
						float d;			// damping
						float len;		// target length, less than node distance!
					}spring;

typedef struct node{
						vector3 pos;
						vector3 vel;
						vector3 accel;
						float imass;
						float damping;
					}node;
// GAME OBJECTS						

typedef struct tile{								// type and pointer to structure for that type
						int type;
						void *data;					// pointer to a tiles data i.e. grenade
					}tile;

typedef struct active_tiles{						// tiles that are currently in play or being updated by physics
						int no_active;
						tile tiles[MAX_TILES];
					}active_tiles;
									
typedef struct entity{
						int type;
						int multiply;
						vector3 pos;
					}entity;

typedef struct entity_array{
						int no_active;
						entity data[MAX_ENTITIES];
					}entity_array;

typedef struct entity3d{
						int type;
						int weapon;			// Its initial state
						float speed;			// sets velocity 0.05
						int health;
						void (*update)(int);	//
						vector3 vel;
						vector3 ang;
						vector3 pos;
				}entity3d;

typedef struct entity3d_array{
						int no_active;
						entity3d data[MAX_3DENTITIES];
					}entity3d_array;

typedef struct particles{
						float age;
						float velocity[3*MAX_PARTS];
						GLfloat vertex[3*MAX_PARTS];
						void *next;
					}particles;

// TILE TYPES				
typedef struct grenade{
						float spin;
						vector3 position;
						vector3 velocity;
						vector3 acceleration;
					}grenade;					

typedef struct torpedo{
						vector3 position;
						vector3 velocity;
					}torpedo;

typedef struct player{	
						int no_of_tiles;					// no of tiles in players stack
						int tile_type[MAX_TILES];			// array of the types of each tile
						void (*activate[MAX_TILES])(float);	// fp for tile function
						vector3 position;					// players position
						active_tiles act;					// no tiles in play
					}player;

typedef struct widget{	
						int type;
						int state;
						GLuint t;
						void (*handle)(int);
						GLfloat vertex[8];
						GLfloat texver[8];
					}widget;

// LEVEL DATA STRUCTURES

typedef struct object{
						int type;			// A game object (ENEMY OR CARD ENUM)
						int weapon;			// Its initial state
						int speed;
						int health;
						vector3 pos;	// Its starting position
					}object;

typedef struct state{
						int no_of_object;
						object* objects;
						float model_size;
						char model[16];			// make buffer overflow option!
						char loops[16];
						char message[64];
					}state;

typedef struct event{
						int type;			// Type of event
						int val1;			// event variables
						int val2;
						int val3;			// music track?
						void (*handle)();	// fuction pointer
					}event;

typedef struct level_data{
						int index;				// current state
						int ev_idx;
						int no_of_state;
						int no_of_event;
						vector3 gcolour;
						vector3 lcolour;
						vector3 hcolour;
						event* events;			// pointer to event array that will advance state 
						state* states;			// pointer to array of pointers to array of objects
					}level_data;

typedef struct model_data{
					  GLfloat vertex[3];
					  GLfloat normal[3];
					}model_data;

extern GLsizei no_of_verts;
extern GLsizei stride;
extern GLfloat hud_r;
extern GLfloat hud_g;
extern GLfloat hud_b;
extern player p1;
extern entity_array en;
extern entity3d_array en3d;
extern int level;
extern model_data *model;
extern float model_size;
extern GLuint t_parts;
extern GLuint t_cards;
extern GLuint t_score;
extern int lives;
extern int health;
extern int start_tile_types[MAX_TILES];
extern int start_no_of_tile;

int init_sdl();
void initGL();
void set_viewport();
void close_sdl();
void init_sound();
void init_grid();
void load_model();
void play_sound(int index, int loop);
void free_music();
void run_game();

// RENDER STUFF
GLuint load_texture(const char *s, int type);
void render_text(texture *t, char s[], int c, int size);
void draw_grid();
void draw_stack();
void draw_active_tiles();
void draw_entities();
void draw_models();
void draw_particles(const particles *parts);
void grid_colours(float x, float y, float z);
void set_lights(vector3 col);
void draw_setup_tiles(vector3 pos, int type, int angle);
void draw_score(int score);
void set_hud_colour(vector3 col);

// VECTOR STUFF
float v3_magnitude(vector3 *v);
float v3_magnitude2(vector3 *v);
void v3_normalise(vector3 *v);
void v3_reverse(vector3 *v);
vector3 v3_add(vector3 *v1, vector3 *v2);
vector3 v3_add(vector3 *v1, vector3 *v2);
void v3_scaler_multiply(vector3 *v, float num);
void v3_scaler_divide(vector3 *v, float num);
vector3 v3_cross(vector3 *v1, vector3 *v2);
float v3_dot(vector3 *v1, vector3 *v2);
vector3 v3_sub(vector3 *v1, vector3 *v2);

// FILE I/O STUFF
void read_leveldata(char  s[]);
void write_leveldata(char  s[]);
void print_leveldata();
void free_resources();
void read_leveldata(char  s[]);
void write_leveldata(char  s[]);
void read_model(char  s[]);
void write_model(char  s[]);
void read_tracks(char s[]);
void write_tracks(char s[]);

// GUI STUFF
void level_editor();
void init_gui();
void close_gui();
void draw_text(char *s, int justru, int x, int y);
void draw_widgets();
void gui_select();
void set_colours();
void set_ortho();
void unset_ortho();
void display_model();
void display_stack();

// EVENTS
void e_all_dead();
void e_end_level();	// notset, but automatically appended
void e_alt_music();
void e_all_clear();
void e_grid_mod();
void e_numkilled();
void e_timer();
void e_grid_mod();