#include "main.h"
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
enum{
	ADD_STATE,	// States are configurations within levels, 
	ADD_EVENT,	// states are advanced by events being triggerred
	SAVE_LEVEL,
	RUN_STATE,
	ADD_ENEMY,
	ADD_BONUS,
	RED,
	GREEN,
	BLUE,
	GRID,
	LIGHT1,
	HUD,
	CONTEXT1,	// 	These are context sensitive button in the editor GUI
	CONTEXT2,	//	Their Labels are altered according to the mode selected
	CONTEXT3,	//	in the buttons above.
	CONTEXT4,
	CONTEXT5,
	CONTEXT6,
	CONTEXT7,
	CONTEXT8,
	CONTEXT9,
	DATA_FILE,	//	data file to store level information in.
	LOOP_FILE,	//	name of sample files (prefix only!, xxxx1.ogg)
	MODEL_FILE,	//	model file for current state
	DEC_EVIDX,
	SHOW_EVIDX,
	INC_EVIDX,
	DELETE,
	DEC_STATE,
	SHOW_STATE,
	INC_STATE,
	ADD_TILE,
	TYPE_TILE,
	DEL_TILE,
	//MESSAGE
};

typedef struct files{
	char loops[16];
	char model[16];
	char data[16];
}files;

files file_names = { {'\0'},{'\0'},{'\0'} };

void add_state();
void add_event();
void add_object();
void save_level();
void toggle_colours(int idx);
void handle_click(int idx);
void set_active(int x);
void context_enemy();
void create_dolphin(int i);
void create_bonus();
void set_event_type();
void context_event();
void context_state();
void context_bonus();
void no_op();
void set_music(int i);
void set_mus_transition(int i);
void set_statecontrol(int i);
void set_timer(int i);
void set_numkilled(int i);
void set_grid_mod(int i);
void get_filepath(char* path, int x, int y);
void set_filepath(int i);
void set_eventidx(int i);
void set_enemyidx(int i);
void set_cardtype(int i);
void set_motion(int i);
void set_speed(int i);
void set_health(int i);
void set_bonus_multi(int i);
void set_bonus_type(int i);
void set_bonusidx(int i);
void set_model_size(int i);
void set_model_height(int i);
void set_state_message(int i);
void set_blend_mode(int i);
void set_stateidx(int i);
void add_card(int i);
void delete_card(int i);
void delete_event(int i);
void delete_object(int i);
void delete_state(int i);
void refresh_state();
void draw_log();
void display_error(int error);
int validate_level();

extern void init_grenade(float speed);
extern void init_torpedo(float unused);
extern void free_music();
extern void load_music();
extern void play_music();
extern void kill_entity3d(int i);
extern void create_entity3d(object *obj);
extern void kill_entity(int i);
extern void create_entity(vector3 pos, int type);
extern void load_gamestate();
extern void e_all_dead();
extern void init_player();

extern node nodes[GRIDL][GRIDW];
extern int mousex;
extern int mousey;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int editor_mode;
extern int music_index;
extern vector4 view;
extern int gridmod;
extern SDL_Window* mywindow;
extern level_data ldata;
int enemy_idx = 0;		// enemy id number
int bonus_idx = 0;		// bonus id number
int object_idx = -1;	// array indice pointed to, can be enemy or bonus
int enemy_count = 0;
int bonus_count = 0;
int no_of_widget = 0;
int cardtype = 0;
widget* create_widget(char *s, int type, int just, int x, int y);
widget *widgets[35];
int event_flag = 99;
char logmessage[7][64];
float model_y = 0;

void init_gui(){


	//Mix_HookMusicFinished(NULL);
	Mix_HaltMusic();
	active_font = ed_font;
	view.x = 0.0;
	view.y = -1.29;
	view.z = -2.94;
	view.w = 90;
	
	widgets[ADD_STATE] = create_widget("ADD STATE", BUTTON, LEFT, 20, 20);
	widgets[ADD_STATE]->handle = set_active;// was add_state;
	widgets[ADD_EVENT] = create_widget("ADD EVENT", BUTTON, LEFT, 20, 60);
	widgets[ADD_EVENT]->handle = set_active;// was add_event
	widgets[SAVE_LEVEL] = create_widget("SAVE LEVEL", BUTTON, RIGHT, SCREEN_WIDTH - 20, 20);
	widgets[SAVE_LEVEL]->handle = save_level;
	widgets[RUN_STATE] = create_widget("RUN STATE", BUTTON, RIGHT, SCREEN_WIDTH - 20, 60);
	widgets[RUN_STATE]->handle = run_game;
	widgets[ADD_ENEMY] = create_widget("ADD ENEMY", BUTTON, CENTER, SCREEN_WIDTH/2, 20);
	widgets[ADD_ENEMY]->handle = add_object;
	widgets[ADD_BONUS] = create_widget("ADD BONUS", BUTTON, CENTER, SCREEN_WIDTH/2, 60);
	widgets[ADD_BONUS]->handle = add_object;
	widgets[RED] = create_widget("---------------------------  R  ", SLIDER, LEFT, 20, 120);
	widgets[RED]->handle = set_colours;
	widgets[GREEN] = create_widget("---------------------------  G  ", SLIDER, LEFT, 20, 155);
	widgets[GREEN]->handle = set_colours;
	widgets[BLUE] = create_widget("---------------------------  B  ", SLIDER, LEFT, 20, 190);
	widgets[BLUE]->handle = set_colours;
	widgets[GRID] = create_widget("  GRID  ", BUTTON, RIGHT, SCREEN_WIDTH - 20, 120);
	widgets[GRID]->handle = toggle_colours;
	widgets[LIGHT1] = create_widget("LIGHT1", BUTTON, RIGHT, SCREEN_WIDTH - 20, 155);
	widgets[LIGHT1]->handle = toggle_colours;
	widgets[HUD] = create_widget("    HUD    ", BUTTON, RIGHT, SCREEN_WIDTH - 20, 190);
	widgets[HUD]->handle = toggle_colours;

	widgets[12] = create_widget("                ", TEXT, RIGHT, SCREEN_WIDTH -20, 250);
	widgets[12]->handle = no_op;
	widgets[13] = create_widget("                ", TEXT, RIGHT, SCREEN_WIDTH -20, 285);
	widgets[13]->handle = no_op;
	widgets[14] = create_widget("                ", TEXT, RIGHT, SCREEN_WIDTH -20, 320);
	widgets[14]->handle = no_op;
	widgets[15] = create_widget("                ", TEXT, CENTER, SCREEN_WIDTH/2, 250);
	widgets[15]->handle = no_op;
	widgets[16] = create_widget("                ", TEXT, CENTER, SCREEN_WIDTH/2, 285);
	widgets[16]->handle = no_op;
	widgets[17] = create_widget("                ", TEXT, CENTER, SCREEN_WIDTH/2, 320);
	widgets[17]->handle = no_op;
	widgets[18] = create_widget("                ", TEXT, LEFT, 20, 250);
	widgets[18]->handle = no_op;
	widgets[19] = create_widget("                ", TEXT, LEFT, 20, 285);
	widgets[19]->handle = no_op;
	widgets[20] = create_widget("                ", TEXT, LEFT, 20, 320);
	widgets[20]->handle = no_op;

	widgets[21] = create_widget("                ", TEXT, LEFT, 20, 380);
	widgets[21]->handle = set_filepath;
	widgets[22] = create_widget("                ", TEXT, LEFT, 20, 415);
	widgets[22]->handle = set_filepath;
	widgets[23] = create_widget("                ", TEXT, LEFT, 20, 450);
	widgets[23]->handle = set_filepath;

	widgets[DEC_EVIDX] = create_widget(" - ", TEXT, LEFT, 20, 510);
	widgets[DEC_EVIDX]->handle = no_op;
	widgets[SHOW_EVIDX] = create_widget("        ", TEXT, CENTER, 84, 510);
	widgets[SHOW_EVIDX]->handle = set_active;
	widgets[INC_EVIDX] = create_widget(" + ", TEXT, RIGHT, 148, 510);
	widgets[INC_EVIDX]->handle = no_op;

	widgets[DELETE] = create_widget("DELETE", TEXT, LEFT, 20, 545);
	widgets[DELETE]->handle = no_op;

	widgets[DEC_STATE] = create_widget(" - ", TEXT, LEFT, 20, 580);
	widgets[DEC_STATE]->handle = set_stateidx;
	widgets[SHOW_STATE] = create_widget("        ", TEXT, CENTER, 84, 580);
	widgets[SHOW_STATE]->handle = set_active;
	widgets[INC_STATE] = create_widget(" + ", TEXT, RIGHT, 148, 580);
	widgets[INC_STATE]->handle = set_stateidx;

	widgets[ADD_TILE] = create_widget("ADD TILE", BUTTON, LEFT, 20, 640);
	widgets[ADD_TILE]->handle = add_card;
	widgets[TYPE_TILE] = create_widget("                 ", BUTTON, LEFT, 20, 675);
	widgets[TYPE_TILE]->handle = set_cardtype;
	widgets[DEL_TILE] = create_widget("DEL TILE", BUTTON, LEFT, 20, 710);
	widgets[DEL_TILE]->handle = delete_card;



	for (int i = 0; i < MAX_TILES; ++i){		// maybe make this a seperate function?
		p1.tile_type[i] = 0;
		start_tile_types[i] = 0;
		p1.activate[i] = NULL;		
	} 
	
	p1.position.x = nodes[2][GRIDW/2].pos.x;			// center of grid
	p1.position.y = nodes[2][GRIDW/2].pos.y; 	// above the grid surface
	p1.position.z = nodes[2][GRIDW/2].pos.z;			// in front of the camera
	

	for (int i = 0; i < MAX_TILES; ++i){
		p1.act.tiles[i].type = 99;						// this is never a valid tile type!
		//p1.act.tiles[i].data = NULL;
		if (p1.act.tiles[i].data != NULL){
			free(p1.act.tiles[i].data);
			p1.act.tiles[i].data = NULL;
		}
	}
	
	widgets[ADD_STATE]->state = 1;
	widgets[SAVE_LEVEL]->state = 1;
	widgets[RUN_STATE]->state = 1;
	widgets[GRID]->state = 1;
	widgets[DEC_EVIDX]->state = 1;
	widgets[SHOW_EVIDX]->state = 1;
	widgets[INC_EVIDX]->state = 1;
	widgets[DELETE]->state = 1;
	widgets[DEC_STATE]->state = 1;
	widgets[SHOW_STATE]->state = 1;
	widgets[INC_STATE]->state = 1;
	widgets[ADD_TILE]->state = 1;
	widgets[TYPE_TILE]->state = 1;
	widgets[DEL_TILE]->state = 1;
	widgets[MODEL_FILE]->state = 1;
	set_active(ADD_STATE);

	en3d.no_active = 0;
	en.no_active = 0;

	// On reloading editor restore state
	if (file_names.data[0] != '\0'){

		read_leveldata(file_names.data);
		//read_model(ldata.states[0].model);
		init_player();
		load_gamestate();
		refresh_state();
		printf("reloaded previous data\n");
	}
	// Or load a blank config
	else {

		free_resources();
		
		p1.no_of_tiles = 0;
		start_no_of_tile = 0;
		p1.act.no_active = 0;
		// INITIAL STATE SETUP
		ldata.states = malloc(sizeof(state));
		snprintf(file_names.model, 16, "dolphin.dat");
		snprintf(ldata.states[0].model, 16, "%s", file_names.model);
		read_model(ldata.states[0].model);
		ldata.index = 0;
		ldata.ev_idx = -1;
		ldata.no_of_state = 1;
		ldata.no_of_event = 0;
		ldata.gcolour.x = 0;
		ldata.gcolour.y = 0;
		ldata.gcolour.z = 0;
		ldata.lcolour.x = 0;
		ldata.lcolour.y = 0;
		ldata.lcolour.z = 0;
		ldata.states[0].objects = NULL;	
		ldata.states[0].no_of_object = 0;
		ldata.states[0].model_size = 0.3;
		ldata.states[0].message[0] = '\0';
		init_grid();
	}
}

void no_op(){}

void add_state(int i){

	ldata.no_of_state += 1;
	ldata.index += 1;
	ldata.states = realloc(ldata.states, sizeof(state) * ldata.no_of_state);
	ldata.states[ldata.index].objects = NULL;	
	ldata.states[ldata.index].no_of_object = 0;
	ldata.states[ldata.index].model_size = 0.3;
	snprintf(ldata.states[ldata.index].model, 16, "%s", file_names.model);
	ldata.states[ldata.index].message[0] = '\0';
	set_active(i);
	refresh_state();
	printf("added state %d\n", ldata.no_of_state);
	
}

void add_event(int i){

	ldata.no_of_event += 1;
	ldata.ev_idx = ldata.no_of_event - 1;
	ldata.events = realloc(ldata.events, sizeof(event) * ldata.no_of_event);
	ldata.events[ldata.ev_idx].handle = NULL;
	ldata.events[ldata.ev_idx].type = 0;
	ldata.events[ldata.ev_idx].val1 = 0;
	ldata.events[ldata.ev_idx].val2 = 0;
	ldata.events[ldata.ev_idx].val3 = 0;

	set_active(i);
	printf("added event %d\n", ldata.no_of_event);
}

void add_object(int u){
	
	int i = ldata.states[ldata.index].no_of_object;

	if (u == ADD_ENEMY){

		++enemy_count;
		enemy_idx = enemy_count;
		object_idx = ldata.states[ldata.index].no_of_object;
		ldata.states[ldata.index].no_of_object += 1;
		ldata.states[ldata.index].objects = 
		realloc(ldata.states[ldata.index].objects, sizeof(object) * ldata.states[ldata.index].no_of_object);
		ldata.states[ldata.index].objects[i].type = SINE_Z;
		ldata.states[ldata.index].objects[i].weapon = 0;
		ldata.states[ldata.index].objects[i].speed = 5;
		ldata.states[ldata.index].objects[i].health = 1;
		ldata.states[ldata.index].objects[i].pos.x = 0;
		ldata.states[ldata.index].objects[i].pos.y = model_y;
		ldata.states[ldata.index].objects[i].pos.z = -0.5;	// TODO ALTER BY TYPE OF OBJECT TO BE SENSIBLE
		set_active(u);
		create_dolphin(0);
	}

	if (u == ADD_BONUS){

		++bonus_count;
		bonus_idx = bonus_count; 
		object_idx = ldata.states[ldata.index].no_of_object;
		ldata.states[ldata.index].no_of_object += 1;
		ldata.states[ldata.index].objects = 
		realloc(ldata.states[ldata.index].objects, sizeof(object) * ldata.states[ldata.index].no_of_object);
		ldata.states[ldata.index].objects[i].type = GRENADE;
		ldata.states[ldata.index].objects[i].health = 1;	// bunus multiplier
		ldata.states[ldata.index].objects[i].pos.x = 0;
		ldata.states[ldata.index].objects[i].pos.y = 0;
		ldata.states[ldata.index].objects[i].pos.z = -0.5;
		set_active(u);
		create_bonus();
	}
	printf("Added object %d ", ldata.states[ldata.index].no_of_object);
	printf("to state %d\n", ldata.index);
	printf("enemy_idx = %d   bonus_idx = %d   object_idx = %d\n", enemy_idx, bonus_idx, object_idx);
}

void context_menu(){
	
	int x = 0;
	char str[64];
	
	char *labels[3];
	labels[0] = "GRENADE";
	labels[1] = "  HOOK  ";
	labels[2] = "TORPEDO";

	//t = ldata.no_of_tiles;
	//ldata.tile_types[32];	
	draw_text(labels[cardtype], LEFT, 20, 675);

	if (widgets[ADD_STATE]->state == 1) x = ADD_STATE;
	else if (widgets[ADD_EVENT]->state == 1) x = ADD_EVENT;
	else if (widgets[ADD_ENEMY]->state == 1) x = ADD_ENEMY;
	else if (widgets[ADD_BONUS]->state == 1) x = ADD_BONUS;
	
	switch (x){

		case ADD_STATE:
			context_state();
			break;
		case ADD_EVENT:
			context_event();
			sprintf(str, "EV:%d", ldata.ev_idx + 1);
			draw_text(str, CENTER, 84, 510);	
			break;
		case ADD_ENEMY:
			sprintf(str, "EN:%d", enemy_idx);
			draw_text(str, CENTER, 84, 510);
			context_enemy();		
			break;
		case ADD_BONUS:	
			sprintf(str, " B:%d ", bonus_idx);
			draw_text(str, CENTER, 84, 510);
			context_bonus();
			break;
		default:
			break;
	}

	if (file_names.data[0] == '\0')
		draw_text("DATA FILE", LEFT, 20, 380);
	else
		draw_text(file_names.data, LEFT, 20, 380);

	if (file_names.loops[0] == '\0')
		draw_text("LOOPS FILE", LEFT, 20, 415);
	else
		draw_text(file_names.loops, LEFT, 20, 415);

	if (file_names.model[0] == '\0')
		draw_text("MODEL FILE", LEFT, 20, 450);
	else
		draw_text(file_names.model, LEFT, 20, 450);

	sprintf(str, "S:%d", ldata.index + 1);
	draw_text(str, CENTER, 84, 580);

	draw_log();		
}

void context_state(){

	char str[32];
	draw_text("MESSAGE", LEFT, 20, 250);

	sprintf(str, "EN SIZE: %.1f", ldata.states[ldata.index].model_size);
	draw_text(str, CENTER, SCREEN_WIDTH/2, 250);

	sprintf(str, "EN Y: %.2f", model_y);	// this needs fixing!!!!
	draw_text(str, CENTER, SCREEN_WIDTH/2, 285);

	draw_text("EN ALPHA", CENTER, SCREEN_WIDTH/2, 320);	// TODO WHATS THE PLAN HERE???
	if (ldata.states[ldata.index].message[0] != '\0'){
		snprintf(str, 10, ldata.states[ldata.index].message);
		draw_text(str, LEFT, 20, 285);
	}
}

void context_bonus(){
	
	char *b_type[5];
	char str[32];
	float val;
	float dpnx = 6.3;
	float dpny = 6.4;
	b_type[0] = "   TYPE   ";		// SETS notion
	b_type[1] = "GRENADE";
	b_type[2] = "  HOOK  ";
	b_type[3] = "TORPEDO";
	b_type[4] = "   LIFE   ";

	int o = object_idx;//ldata.states[ldata.index].no_of_object - 1;

	sprintf(str, "X :\t%.2f" ,ldata.states[ldata.index].objects[o].pos.x);
	draw_text(str, LEFT, 20, 250);
	sprintf(str, "Y :\t%.2f" ,ldata.states[ldata.index].objects[o].pos.y);
	draw_text(str, LEFT, 20, 285);
	sprintf(str, "Z :\t%.2f" ,ldata.states[ldata.index].objects[o].pos.z);
	draw_text(str, LEFT, 20, 320);
	
	if ((unsigned)(mousex - 610) < (832-610)){

		if (mousex > 723){
			val = GRID_SPACING * (mousex-723)/dpnx;
			ldata.states[ldata.index].objects[o].pos.x = val;
		}
		else if (mousex < 723){
			val = -GRID_SPACING * (723 - mousex)/dpnx;
			ldata.states[ldata.index].objects[o].pos.x = val;
		}
		else if (mousex == 723)
			ldata.states[ldata.index].objects[o].pos.x = 0;

		val = GRID_SPACING * ((800 - mousey) / dpny);
		ldata.states[ldata.index].objects[o].pos.z = -val;

		kill_entity(bonus_idx - 1);
		create_bonus(0);
		mousex = 0;
	}

	o = ldata.states[ldata.index].objects[object_idx].type;
	
	draw_text(b_type[o], CENTER, SCREEN_WIDTH/2, 250);

	sprintf(str, "   X %d   ", ldata.states[ldata.index].objects[object_idx].health);
	draw_text(str, CENTER, SCREEN_WIDTH/2, 285);
}

void context_enemy(){
	
	char *motions[3];
	motions[0] = " SINE Z ";		// SETS notion
	motions[1] = "LINEAR";
	motions[2] = "DODGE";
	
	char str[12];
	float val;
	float dpnx = 6.3;
	float dpny = 6.4;
	int o = object_idx;// ldata.states[ldata.index].no_of_object - 1;

	sprintf(str, "X :\t%.2f" ,ldata.states[ldata.index].objects[o].pos.x);
	draw_text(str, LEFT, 20, 250);
	sprintf(str, "Y :\t%.2f" ,ldata.states[ldata.index].objects[o].pos.y);
	draw_text(str, LEFT, 20, 285);
	sprintf(str, "Z :\t%.2f" ,ldata.states[ldata.index].objects[o].pos.z);
	draw_text(str, LEFT, 20, 320);

	if ((unsigned)(mousex - 610) < (832-610)){

		if (mousex > 723){
			val = GRID_SPACING * (mousex-723)/dpnx;
			ldata.states[ldata.index].objects[o].pos.x = val;
		}
		else if (mousex < 723){
			val = -GRID_SPACING * (723 - mousex)/dpnx;
			ldata.states[ldata.index].objects[o].pos.x = val;
		}
		else if (mousex == 723)
			ldata.states[ldata.index].objects[o].pos.x = 0;

		val = GRID_SPACING * ((800 - mousey) / dpny);
		ldata.states[ldata.index].objects[o].pos.z = -val;

		kill_entity3d(enemy_idx - 1);
		create_dolphin(0);
		mousex = 0;
	}
	// convert o to to motions[]
	o = ldata.states[ldata.index].objects[object_idx].type - MODELS - 1;
	draw_text(motions[o], CENTER, SCREEN_WIDTH/2, 250);

	sprintf(str, "SPEED %d", ldata.states[ldata.index].objects[object_idx].speed);
	draw_text(str, CENTER, SCREEN_WIDTH/2, 285);
	
	sprintf(str, "LIFE %d", ldata.states[ldata.index].objects[object_idx].health);
	draw_text(str, CENTER, SCREEN_WIDTH/2, 320);
}

void context_event(){

	int x;
	char *labels[7];
	labels[0] = "  FLAT  ";
	labels[1] = "CHOPPY";
	labels[2] = "VIOLENT";
	labels[3] = "";
	labels[4] = "";
	labels[5] = "";
	labels[6] = "";

	char *mlabels[3];
	mlabels[0] = "AT END";
	mlabels[1] = "   NOW   ";
	mlabels[2] = "  FADE  ";
	
	char str[64];
	draw_text("KILLED X", LEFT, 20, 250);
	draw_text("GRID MOD", LEFT, 20, 285);
	draw_text("TIMER", LEFT, 20, 320);
	draw_text("ALL CLEAR", CENTER, SCREEN_WIDTH/2, 250);
	draw_text("ALL DEAD", CENTER, SCREEN_WIDTH/2, 285);
	draw_text("ALT MUSIC", CENTER, SCREEN_WIDTH/2, 320);

	switch(ldata.events[ldata.ev_idx].type){

		case ALL_CLEAR: 
			ldata.events[ldata.ev_idx].handle = e_all_clear;
			sprintf(str, "STATE + %d", ldata.events[ldata.ev_idx].val3);
			widgets[14]->handle = set_statecontrol;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 320);	
			break;
		case ALL_DEAD:
			ldata.events[ldata.ev_idx].handle = e_all_dead;
			sprintf(str, "STATE + %d", ldata.events[ldata.ev_idx].val3);
			widgets[14]->handle = set_statecontrol;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 320);			
			break;
		case SWITCH_MUSIC:
			ldata.events[ldata.ev_idx].handle = e_alt_music;
			sprintf(str, "  LOOP  %d  ", ldata.events[ldata.ev_idx].val1 + 1);
			widgets[12]->handle = set_music;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 250);
			
			int m = ldata.events[ldata.ev_idx].val2;
			widgets[13]->handle = set_mus_transition;
			draw_text(mlabels[m], RIGHT, SCREEN_WIDTH-20, 285);

			sprintf(str, "STATE + %d", ldata.events[ldata.ev_idx].val3);
			widgets[14]->handle = set_statecontrol;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 320);			
			break;
		case KILLED_ENTITIES:
			ldata.events[ldata.ev_idx].handle = e_numkilled;
			sprintf(str, "KILLED %d", ldata.events[ldata.ev_idx].val1);
			widgets[12]->handle = set_numkilled;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 250);
			
			sprintf(str, "STATE + %d", ldata.events[ldata.ev_idx].val3);
			widgets[14]->handle = set_statecontrol;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 320);
			break;
		case GRID_MOD:
			ldata.events[ldata.ev_idx].handle = e_grid_mod;
			x = ldata.events[ldata.ev_idx].val1;
			if (x > 2) x = 0;
			draw_text(labels[x], RIGHT, SCREEN_WIDTH-20, 250);
			widgets[12]->handle = set_grid_mod;

			sprintf(str, "STATE + %d", ldata.events[ldata.ev_idx].val3);
			widgets[14]->handle = set_statecontrol;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 320);
			break;
		case E_TIMER:
			ldata.events[ldata.ev_idx].handle = e_timer;
			sprintf(str, "%d  SECS", ldata.events[ldata.ev_idx].val1);
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 250);
			widgets[12]->handle = set_timer;

			sprintf(str, "STATE + %d", ldata.events[ldata.ev_idx].val3);
			widgets[14]->handle = set_statecontrol;
			draw_text(str, RIGHT, SCREEN_WIDTH-20, 320);
			break;
		default:
			break;
	}

	for (int i = 12; i < 21; ++i)
		widgets[i]->state = 0;

	int flag = ldata.events[ldata.ev_idx].type;
	widgets[flag + 15]->state = 1;

	if (flag == SWITCH_MUSIC){
		widgets[12]->state = 1;
		widgets[13]->state = 1;
		widgets[14]->state = 1;
	}
	else if (flag == KILLED_ENTITIES){
		widgets[12]->state = 1;
		widgets[13]->state = 0;
		widgets[14]->state = 1;
	}
	else if (flag == GRID_MOD){
		widgets[12]->state = 1;
		widgets[13]->state = 0;
		widgets[14]->state = 1;
	}
	else if (flag == E_TIMER){
		widgets[12]->state = 1;
		widgets[13]->state = 0;
		widgets[14]->state = 1;
	}
	else{
		widgets[12]->state = 0;
		widgets[13]->state = 0;
		widgets[14]->state = 1;
	}
}

void set_cardtype(int i){
	
	++cardtype;
	if (cardtype > 2) cardtype = 0;
}

void add_card(int i){
	
	if (p1.no_of_tiles < MAX_TILES){

		p1.tile_type[p1.no_of_tiles] = cardtype + 1;
		start_tile_types[p1.no_of_tiles] = cardtype + 1;

		switch(cardtype + 1){

			case GRENADE:
				p1.activate[p1.no_of_tiles] = init_grenade;
				break;
			case TORPEDO:
				p1.activate[p1.no_of_tiles] = init_torpedo;
				break;
			default:
				p1.activate[p1.no_of_tiles] = init_grenade;
				break;
		}

		++p1.no_of_tiles;
		++start_no_of_tile;
	}
	printf("Added card no %d\n", p1.no_of_tiles);
	printf("Card type %d\n", cardtype+1);
}

void delete_card(int i){
	
	if (p1.no_of_tiles > 0){
		p1.activate[p1.no_of_tiles - 1] = NULL;
		p1.no_of_tiles -= 1;
		start_no_of_tile -=1;
		printf("no_of_tiles = %d\n", p1.no_of_tiles);
	}
}

void set_eventidx(int i){

	if (i == DEC_EVIDX){
		if (ldata.ev_idx > 0){	// allow dec
			--ldata.ev_idx;
			if (ldata.events[ldata.ev_idx].type == SWITCH_MUSIC){
				music_index = ldata.events[ldata.ev_idx].val1;
				Mix_VolumeMusic(ldata.events[ldata.ev_idx].val2);
				play_music();
			}
		}
	}

	if (i == INC_EVIDX){
		if (ldata.ev_idx < ldata.no_of_event - 1){	// allow inc
			++ldata.ev_idx;
			if (ldata.events[ldata.ev_idx].type == SWITCH_MUSIC){
				music_index = ldata.events[ldata.ev_idx].val1;
				Mix_VolumeMusic(ldata.events[ldata.ev_idx].val2);
				play_music();
			}
		}
		else if(ldata.ev_idx == ldata.no_of_event - 1){
			add_event(ADD_EVENT);
		}
	}	
}
// Gets the next object of type enemy in either direction
// If found sets object index and enemy index
void set_enemyidx(int i){

	// GET PREVIOUS ENEMY
	if ((i == DEC_EVIDX) && object_idx > 0){
		for (int k = object_idx - 1; k >= 0; --k){
			
			if (ldata.states[ldata.index].objects[k].type > MODELS){
				object_idx = k;
				--enemy_idx;
				printf("set_enemyidx() object idx = %d\n", object_idx);
				return;
			}
		}
	}

	// GET NEXT ENEMY
	if ((i == INC_EVIDX) && object_idx < ldata.states[ldata.index].no_of_object - 1){
		for (int k = object_idx + 1; k < ldata.states[ldata.index].no_of_object; ++k){
			
			if (ldata.states[ldata.index].objects[k].type > MODELS){
				object_idx = k;
				++enemy_idx;
				printf("set_enemyidx() object idx = %d\n", object_idx);
				return;
			}
		}
	}
}

void set_bonusidx(int i){


	// GET PREVIOUS BONUS
	if ((i == DEC_EVIDX) && object_idx > 0){
		for (int k = object_idx - 1; k >= 0; --k){
			
			if (ldata.states[ldata.index].objects[k].type < MODELS){
				object_idx = k;
				--bonus_idx;
				printf("set object idx = %d, bonus idx = %d\n", object_idx, bonus_idx);
				return;
			}
		}
	}

	// GET NEXT BONUS
	if ((i == INC_EVIDX) && object_idx < ldata.states[ldata.index].no_of_object - 1){
		for (int k = object_idx + 1; k < ldata.states[ldata.index].no_of_object; ++k){
			
			if (ldata.states[ldata.index].objects[k].type < MODELS){
				object_idx = k;
				++bonus_idx;
				printf("set object idx = %d, bonus idx = %d\n", object_idx, bonus_idx);
				return;
			}
		}
	}
}

void set_stateidx(int i){

	int repos = 0;
	
	if (i == DEC_STATE){
		if (ldata.index != 0){
			ldata.index -= 1;
			repos = 1;
		}
	}

	if (i == INC_STATE){
		if (ldata.index < ldata.no_of_state - 1){
			ldata.index += 1;
			repos = 1;
		}
		else if (ldata.index == ldata.no_of_state - 1){
			add_state(ADD_STATE);
		}
	}

	if (repos == 1){
		refresh_state();
	}
}

void delete_event(int i){

	if (ldata.ev_idx == 0)
		return;

	if (ldata.ev_idx == (ldata.no_of_event - 1)){

		--ldata.no_of_event;
		ldata.events = realloc(ldata.events, sizeof(event) * ldata.no_of_event);

		--ldata.ev_idx;
		printf("Deleted last event no:%d\n", ldata.no_of_event + 1);
		return;
	}

	event *temp = malloc(sizeof(event) * (ldata.no_of_event - 1));

	for (int x = 0; x < ldata.no_of_event; ++x){
		if (x != ldata.ev_idx)
			temp[x] = ldata.events[x];
	}

	--ldata.no_of_event;

	free(ldata.events);
	ldata.events = temp;

	printf("Deleted event no:%d\n", ldata.ev_idx);
	printf("no_of_event = %d\t event index = %d\n", ldata.no_of_event, ldata.ev_idx);
	return;
}

void delete_object(int i){

	int type = ldata.states[ldata.index].objects[object_idx].type;
	// KILL ENTITY IN GAMESPACE
	if (type > MODELS){
		if (enemy_count == 0){
			printf("D-- nothing to kill\n");
			return;
		}
		kill_entity3d(enemy_idx - 1);
		printf("kill entity3D\n");
	}
	if (type < MODELS){
		if (bonus_count == 0){
			printf("D-- nothing to kill\n");
			return;
		}
		printf(" going to kill entity\n");
		kill_entity(bonus_idx - 1);
		printf("kill entity\n");
	}

	printf("D-PRE--no_of_object = \t%d\t obj index = \t%d\n", ldata.states[ldata.index].no_of_object, object_idx);
	printf("D-PRE--bonus idx = \t%d\t enemy idx = \t%d\n", bonus_idx, enemy_idx);
	printf("D-PRE--bonus count = \t%d\t enemy count = \t%d\n", bonus_count, enemy_count);

	// KILL ENTITY IN LDATA

	// IF THERE IS ONLY ONE OBJECT
	if (ldata.states[ldata.index].no_of_object == 1){

		free(ldata.states[ldata.index].objects);
		ldata.states[ldata.index].objects = NULL;

		ldata.states[ldata.index].no_of_object = 0;
		enemy_idx = 0;		// enemy id number
		bonus_idx = 0;		// bonus id number
	 	object_idx = -1;
	 	enemy_count = 0;
	 	bonus_count = 0;
		set_active(ADD_STATE);	
	 	return;
	}

	// CHECK IF ITS THE LAST OBJECT
	if (object_idx == (ldata.states[ldata.index].no_of_object - 1)){

		ldata.states[ldata.index].no_of_object -= 1;

		ldata.states[ldata.index].objects = 
		realloc(ldata.states[ldata.index].objects, sizeof(object) * (ldata.states[ldata.index].no_of_object));

	}

	// ELSE MUST REALLOCATE WHOLE OBJECT MEMORY FOR THIS STATE
	else{
		object *temp = malloc(sizeof(object) * (ldata.states[ldata.index].no_of_object - 1));

		int temp_idx = 0;
		for (int x = 0; x < ldata.states[ldata.index].no_of_object; ++x){
			if (x != object_idx){
				temp[temp_idx] = ldata.states[ldata.index].objects[x];
				temp_idx += 1;
			}
		}
		ldata.states[ldata.index].no_of_object -= 1;
		free(ldata.states[ldata.index].objects);
		ldata.states[ldata.index].objects = temp;
	}

	// FIND NEXT OR PREV OBJECT OF CORRECT TYPE

	if (type > MODELS){
		--enemy_count;
		if (enemy_count == 0){
			set_active(ADD_STATE);		// no valid goto revert to state
			return;
		}
		set_enemyidx(INC_EVIDX);
		set_enemyidx(DEC_EVIDX);
	}
	else{
		--bonus_count;
		if (bonus_count == 0){
			set_active(ADD_STATE);		// no valid goto revert to state
			return;
		}
		set_bonusidx(INC_EVIDX);
		set_bonusidx(DEC_EVIDX);
	}

	
	printf("Del-POST--no_of_object = \t%d\t obj index = \t%d\n", ldata.states[ldata.index].no_of_object, object_idx);
	printf("Del-POST--bonus idx = \t%d\t enemy idx = \t%d\n", bonus_idx, enemy_idx);
	printf("Del-POST--bonus count = \t%d\t enemy count = \t%d\n", bonus_count, enemy_count);
	return;
}

void delete_state(int i){

	if (ldata.no_of_state == 1){
		return;
	}
	
	if (ldata.index == ldata.no_of_state - 1){

		free(ldata.states[ldata.index].objects);
		ldata.states[ldata.index].objects = NULL;

		--ldata.no_of_state;
		ldata.states = realloc(ldata.states, sizeof(state) * ldata.no_of_state);
		--ldata.index;
		refresh_state();
		
		return;
	}

	state *temp = malloc(sizeof(state) * ldata.no_of_state - 1);

	for (int i = 0; i < ldata.no_of_state; ++i){

		if (i != ldata.index){
			temp[i] = ldata.states[i];
		}

		free(ldata.states[ldata.index].objects);
		ldata.states[ldata.index].objects = NULL;
	}

	free(ldata.states);
	ldata.states = temp;
	
	--ldata.no_of_state;

	printf("Deleted state no:%d\n", ldata.index);
	printf("no_of_state = %d\t state index = %d\n", ldata.no_of_state, ldata.index);

	refresh_state();

	return;
}

void refresh_state(){
	
	int idx = ldata.index;
	// load states model
	if (ldata.states[idx].model[0] != '\0'){
		read_model(ldata.states[idx].model);
		snprintf(file_names.model, 16, "%s", ldata.states[ldata.index].model);
	}

	model_size = ldata.states[idx].model_size;

	if (ldata.states[idx].no_of_object > 0){
		model_y = ldata.states[idx].objects[0].pos.y;
	}
	// re/create states entities
	en.no_active = 0;
	en3d.no_active = 0;

	for (int i = 0; i < ldata.states[idx].no_of_object; ++i){
		if (ldata.states[idx].objects[i].type < MODELS){
			create_entity(ldata.states[idx].objects[i].pos, ldata.states[idx].objects[i].type);
		}
		else{
			create_entity3d(&ldata.states[idx].objects[i]);
		}
	}

	// SET ENEMY COUNT ANT BONUS COUNT ETC
	int done = 0;
	int last = 0;	// Previous object indexed
	object_idx = 0;
	enemy_count = 0;
	bonus_count = 0;
	enemy_idx = 0;
	bonus_idx = 0;
	// get the type of the first object in this state
	if (ldata.states[ldata.index].no_of_object > 0){
		if (ldata.states[ldata.index].objects[0].type > MODELS)
			++enemy_count;
		else
			++bonus_count;
	}

	while(!done){

		set_enemyidx(INC_EVIDX);	// en/obj_idx = next enemy or 0

		if (object_idx == last){	// if last enemy object in array quit
			done = 1;
			printf("DEBUG enemy count  = %d\n", enemy_count);
		}
		else{
			enemy_count += 1;
		}
		
		last = object_idx;
	}
	
	done = 0;
	last = 0;
	object_idx = 0;
	
	while(!done){

		set_bonusidx(INC_EVIDX);

		if (object_idx == last){
			done = 1;
		}
		else{
			bonus_count += 1;
		}
		
		last = object_idx;
	}

	object_idx = 0;
}

void set_numkilled(int i){
	
	int x = ldata.events[ldata.ev_idx].val1;
	x = (x + 1)%11;
	ldata.events[ldata.ev_idx].val1 = x;
}

void set_grid_mod(int i){

	int x = ldata.events[ldata.ev_idx].val1;
	x = (x + 1)%3;
	ldata.events[ldata.ev_idx].val1 = x;
}

void set_timer(int i){

	int x = ldata.events[ldata.ev_idx].val1;
	x = (x + 1)%11;
	ldata.events[ldata.ev_idx].val1 = x;
}

void set_music(int i){

	int x = ldata.events[ldata.ev_idx].val1;
	x = (x + 1)%8;
	music_index = x;
	ldata.events[ldata.ev_idx].val1 = x;
	play_music();
}

void set_mus_transition(int i){

	int x = ldata.events[ldata.ev_idx].val2;
	x = (x + 1)%3;
	ldata.events[ldata.ev_idx].val2 = x;
}

void set_statecontrol(int i){

	int x;

	x = ldata.events[ldata.ev_idx].val3;
	if (x == 0)
		ldata.events[ldata.ev_idx].val3 = 1;
	if (x == 1)
		ldata.events[ldata.ev_idx].val3 = -1;
	if (x == -1)
		ldata.events[ldata.ev_idx].val3 = 0;
}

void set_motion(int i){
	
	int idx = ldata.index;
	object *ob  = &ldata.states[idx].objects[object_idx];
	if (ob->type == LINEAR)
		ob->type = SINE_Z;
	else if (ob->type == SINE_Z)
		ob->type = LINEAR;
}

void set_speed(int i){
	
	int idx = ldata.index;
	object *ob  = &ldata.states[idx].objects[object_idx];
	
	ob->speed  = (ob->speed + 1) % 11;
}

void set_health(int i){

	int idx = ldata.index;
	object *ob  = &ldata.states[idx].objects[object_idx];
	ob->health = (ob->health) % 10 + 1; 
}

void set_bonus_multi(int i){
	
	int idx = ldata.index;
	object *ob  = &ldata.states[idx].objects[object_idx];
	ob->health = (ob->health) % 10 + 1; 
}

void set_bonus_type(int i){

	int idx = ldata.index;	
	object *ob  = &ldata.states[idx].objects[object_idx];
	ob->type = (ob->type) % 4 + 1; 	
	kill_entity(bonus_idx - 1);
	create_entity(ldata.states[idx].objects[object_idx].pos, ldata.states[idx].objects[object_idx].type);
}

void set_model_size(int i){

	int idx = ldata.index;
	ldata.states[idx].model_size += 0.1;
	model_size = ldata.states[idx].model_size;
	if(ldata.states[idx].model_size > 0.6)
		ldata.states[idx].model_size = 0.1;
}

void set_model_height(int i){

	model_y += 0.02;
	if (model_y > 0.1)
		model_y = -0.1;

	for (int k = 0; k < ldata.states[ldata.index].no_of_object; ++k){		
		if (ldata.states[ldata.index].objects[k].type > MODELS){
			ldata.states[ldata.index].objects[k].pos.y = model_y;
		}
	}
}

void set_state_message(int i){

	get_filepath(ldata.states[ldata.index].message, 20, 380);
}

void set_blend_mode(int i){


}

void create_dolphin(int X){

	int idx = ldata.index;
	//int i = ldata.states[idx].no_of_object - 1;
	create_entity3d(&ldata.states[idx].objects[object_idx]);
	//ldata.states[idx].objects[object_idx].type = SINE_Z;
	//ldata.states[idx].objects[i].state = 0;
	printf("CD-Dolphin in state %d\t object %d\n", idx, object_idx);
	printf("CD-Num of Dolphins = %d\n", en3d.no_active);
}

void create_bonus(){

	int idx = ldata.index;
	create_entity(ldata.states[idx].objects[object_idx].pos, ldata.states[idx].objects[object_idx].type);
	printf("CB-Bonus in state %d\t object %d\n", idx, object_idx);
	printf("CB-Num of bonus = %d\n", en.no_active);
}

void close_gui(){

	for (int i = 0; i < no_of_widget; ++i){
		if (widgets[i]->t != 0)
			glDeleteTextures(1, &widgets[i]->t);
		if (widgets[i] != NULL)
			free(widgets[i]);
	}
	no_of_widget = 0;	
}

void level_editor(){

	set_ortho();
	draw_widgets();
	context_menu();
	unset_ortho();
	display_stack();
	display_model();
}

void set_filepath(int i){

	if (i == DATA_FILE){
		get_filepath(file_names.data ,20, 380);
		if (file_names.data[0] != '\0'){
			widgets[DATA_FILE]->state = 1;
			read_leveldata(file_names.data);
			// TODO SHOUD BE IN REFRESH FUNCTION?
			if (ldata.states[0].loops != '\0'){
				strcpy(file_names.loops, ldata.states[0].loops);
				widgets[LOOP_FILE]->state = 1;
				snprintf(ldata.states[0].loops, 16, "%s", file_names.loops);
				load_music();
				play_music();
			}
			if (ldata.states[0].model != '\0'){
				strcpy(file_names.model, ldata.states[0].model);
				read_model(file_names.model);
				snprintf(ldata.states[ldata.index].model, 16, "%s", file_names.model);
			}
			if (start_no_of_tile)
				init_player();

			refresh_state();
		}
		else
			widgets[DATA_FILE]->state = 0;
	}
	if (i == LOOP_FILE){
		get_filepath(file_names.loops ,20, 415);
		if (file_names.loops[0] != '\0'){
			widgets[LOOP_FILE]->state = 1;
			snprintf(ldata.states[0].loops, 16, "%s", file_names.loops);
			load_music();
			play_music();
		}
		else
			widgets[LOOP_FILE]->state = 0;
	}
	if (i == MODEL_FILE){
		get_filepath(file_names.model ,20, 450);
		if (file_names.model[0] != '\0'){
			printf("reading model: %s\n", file_names.model);
			read_model(file_names.model);
			snprintf(ldata.states[ldata.index].model, 16, "%s", file_names.model);
		}
	}
}

void get_filepath(char *path, int x, int y){

	SDL_Event e;

	int quit = 0;
	int current = 0;	// current position in name field
	
	for (int i = 0; i < 15; ++i)
		path[i] = '_';
	path[15] = '\0';

	SDL_StartTextInput();	//Enable text input
	
	while (!quit){
				
		while (SDL_PollEvent(&e) != 0){		// check for key event
			
			if(e.type == SDL_KEYDOWN){

				if(e.key.keysym.sym == SDLK_BACKSPACE && current > 0){
					--current;					//lop off character
					path[current] = '_';
				}
				else if(e.key.keysym.sym == SDLK_RETURN){
					quit = 1;
					path[current] = '\0';
				}		
								
			}
			else if((e.type == SDL_TEXTINPUT) && (current < 15)){
					path[current] = e.text.text[0];
					++current;
			}	
		}

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		set_ortho();

		if (path[0] != '\0'){
			draw_text(path ,CENTER, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
		}
		
		unset_ortho();
		draw_grid();
		SDL_GL_SwapWindow(mywindow);
		SDL_Delay(10);
		glClear(GL_COLOR_BUFFER_BIT);
		
	}

	SDL_StopTextInput();
}

widget* create_widget(char *s, int type, int just, int x, int y){

	texture t;
	widget *w;
	w = malloc(sizeof(widget));
	w->type = type;
	w->state = 0;
	if (w->type == SLIDER)
		w->state = 100;
	render_text(&t, s, 1, 0);
	w->t = t.t;

	if (just == CENTER){
		w->vertex[0] = x-t.w/2;
		w->vertex[1] = y;
		w->vertex[2] = x+t.w/2;
		w->vertex[3] = y;
		w->vertex[4] = x+t.w/2; 
		w->vertex[5] = y+t.h;
		w->vertex[6] = x-t.w/2;
		w->vertex[7] = y+t.h;
	}
	else if (just == LEFT){
		w->vertex[0] = x;
		w->vertex[1] = y;
		w->vertex[2] = x+t.w;
		w->vertex[3] = y;
		w->vertex[4] = x+t.w; 
		w->vertex[5] = y+t.h;
		w->vertex[6] = x;
		w->vertex[7] = y+t.h;
	}
	else if (just == RIGHT){
		w->vertex[0] = x-t.w;
		w->vertex[1] = y;
		w->vertex[2] = x;
		w->vertex[3] = y;
		w->vertex[4] = x; 
		w->vertex[5] = y+t.h;
		w->vertex[6] = x-t.w;
		w->vertex[7] = y+t.h;
	}

	w->texver[0] = 0;
	w->texver[1] = 0;
	w->texver[2] = 1;
	w->texver[3] = 0;
	w->texver[4] = 1; 
	w->texver[5] = 1;
	w->texver[6] = 0;
	w->texver[7] = 1;

	++no_of_widget;
	return w;
}

void gui_select(){

	int min, max;

	for (int i = 0; i < no_of_widget; ++i){
		
		min = widgets[i]->vertex[0];
		max = widgets[i]->vertex[2];
		if ((unsigned)(mousex - min) < (max-min)){
			min = widgets[i]->vertex[1];
			max = widgets[i]->vertex[7];
			if ((unsigned)(mousey - min) < (max-min)){
				
				widgets[i]->handle(i);
				printf("%i\n",i );

			}
		}
	}
}

void toggle_colours(int i){

	widgets[GRID]->state = 0;
	widgets[LIGHT1]->state = 0;
	widgets[HUD]->state = 0;
	widgets[i]->state = 1;

	if (i == GRID){
		widgets[RED]->state = ldata.gcolour.x * 255;
		widgets[GREEN]->state = ldata.gcolour.y * 255;
		widgets[BLUE]->state = ldata.gcolour.z * 255;
	}
	else if (i == LIGHT1){
		widgets[RED]->state = ldata.lcolour.x * 255;
		widgets[GREEN]->state = ldata.lcolour.y * 255;
		widgets[BLUE]->state = ldata.lcolour.z * 255;
	}
	else if (i == HUD){
		widgets[RED]->state = ldata.hcolour.x * 255;
		widgets[GREEN]->state = ldata.hcolour.y * 255;
		widgets[BLUE]->state = ldata.hcolour.z * 255;
	}
}

void set_colours(int i){

	widgets[i]->state = mousex - 20;
	vector3 col;

	if (widgets[GRID]->state == 1){
		switch (i){
			case RED: 
				ldata.gcolour.x = (mousex - 20)/255.0;
				break;
			case GREEN: 
				ldata.gcolour.y = (mousex - 20)/255.0;
				break;
			case BLUE: 
				ldata.gcolour.z = (mousex - 20)/255.0;
				break;
		}

		grid_colours(ldata.gcolour.x, ldata.gcolour.y, ldata.gcolour.z);
	}

	else if (widgets[LIGHT1]->state == 1){
		switch (i){
			case RED: 
				ldata.lcolour.x = (mousex - 20)/255.0;
				break;
			case GREEN: 
				ldata.lcolour.y = (mousex - 20)/255.0;
				break;
			case BLUE: 
				ldata.lcolour.z = (mousex - 20)/255.0;
				break;
		}
		col.x = ldata.lcolour.x; 
		col.y = ldata.lcolour.y; 
		col.z = ldata.lcolour.z;
		set_lights(col);
	}
	else if (widgets[HUD]->state == 1){
		switch (i){
			case RED: 
				ldata.hcolour.x = (mousex - 20)/255.0;
				break;
			case GREEN: 
				ldata.hcolour.y = (mousex - 20)/255.0;
				break;
			case BLUE: 
				ldata.hcolour.z = (mousex - 20)/255.0;
				break;
		}
		col.x = ldata.hcolour.x; 
		col.y = ldata.hcolour.y; 
		col.z = ldata.hcolour.z;
		set_hud_colour(col);
	}
}

void save_level(int i){
	
	// TODO ADD ENDLEVEL HERE TO LDATA
	int error = validate_level();
	if (error != 0){
		printf("error validating level\n");
		return;
	}
	write_leveldata(file_names.data);
	//read_leveldata("new.bin");
	set_active(i);
}

void run_game(){
	
	int error = 0;

	if (ldata.no_of_event == 0){
		printf("no events!!\n");
		return;
	}
	
	error = validate_level();
	if (error != 0){
		printf("error validating level\n");
		return;
	}

	ldata.index = 0;
	ldata.ev_idx = 0;
	gridmod = 0;
	en.no_active = 0;
	en3d.no_active = 0;
	music_index = 0;
	p1.act.no_active = 0;

	view.x = 0.0;
	view.y = -0.22;
	view.z = -0.38;
	view.w = 28;

	close_gui();
	editor_mode = 0;
	active_font = hud_font;
	init_player();
	load_gamestate();
	//free_resources();
}

// SET FUNCTION POINTERS IN CONTEXT 1-6
void set_active(int x){

	if (x == SHOW_EVIDX){
		if (widgets[ADD_EVENT]->state == 1)
			x = ADD_ENEMY;
		if (widgets[ADD_ENEMY]->state == 1)
			x = ADD_BONUS;
		if (widgets[ADD_BONUS]->state == 1)
			x = ADD_EVENT;
	}

	for (int i = 0; i < 6; ++i){
		widgets[i]->state = 0;
	}
	for (int i = 12; i < 21; ++i){
		widgets[i]->handle = no_op;
	}
	widgets[DEC_EVIDX]->handle = no_op;
	widgets[INC_EVIDX]->handle = no_op;
	widgets[DELETE]->handle = no_op;
	
	widgets[x]->state = 1;

	widgets[SAVE_LEVEL]->state = 1;
	widgets[RUN_STATE]->state = 1;

	for (int i = 12; i < 21; ++i)
		widgets[i]->state = 0;
	
	switch (x){

		case ADD_STATE:
		case SHOW_STATE:
			widgets[CONTEXT4]->state = 1;
			widgets[CONTEXT4]->handle = set_model_size;
			widgets[CONTEXT5]->state = 1;
			widgets[CONTEXT5]->handle = set_model_height;
			widgets[CONTEXT6]->state = 1;
			widgets[CONTEXT6]->handle = set_blend_mode;
			widgets[CONTEXT7]->state = 1;
			widgets[CONTEXT7]->handle = set_state_message;
			widgets[CONTEXT8]->state = 1;
			widgets[CONTEXT8]->handle = no_op;
			widgets[DELETE]->handle = delete_state;
			widgets[DELETE]->state = 1;
			break;
		case ADD_EVENT:
			if(ldata.no_of_event == 0){
				add_event(1);
			}
			widgets[15]->handle = set_event_type;
			widgets[16]->handle = set_event_type;
			widgets[17]->handle = set_event_type;
			widgets[18]->handle = set_event_type;
			widgets[19]->handle = set_event_type;
			widgets[20]->handle = set_event_type;
			widgets[DEC_EVIDX]->handle = set_eventidx;
			widgets[INC_EVIDX]->handle = set_eventidx;
			widgets[DELETE]->handle = delete_event;
			widgets[DELETE]->state = 1;
			break;

		case ADD_ENEMY:
			widgets[DEC_EVIDX]->handle = set_enemyidx;
			widgets[INC_EVIDX]->handle = set_enemyidx;
			widgets[DELETE]->state = 1;
			widgets[DELETE]->handle = delete_object;			
			widgets[CONTEXT4]->state = 1;
			widgets[CONTEXT4]->handle = set_motion;	
			widgets[CONTEXT5]->state = 1;
			widgets[CONTEXT5]->handle = set_speed;
			widgets[CONTEXT6]->state = 1;
			widgets[CONTEXT6]->handle = set_health;
			break;
		case ADD_BONUS:
			widgets[DEC_EVIDX]->handle = set_bonusidx;
			widgets[INC_EVIDX]->handle = set_bonusidx;
			widgets[DELETE]->handle = delete_object;
			widgets[DELETE]->state = 1;
			widgets[CONTEXT4]->state = 1;
			widgets[CONTEXT4]->handle = set_bonus_type;
			widgets[CONTEXT5]->state = 1;
			widgets[CONTEXT5]->handle = set_bonus_multi;
			break;
		default:
			break;
	}
}

// SIGNAL THE LAST CONTEXT BUTTON PRESSED FOR SUB CONTEXT
void set_event_type(int i){

	int event_flag = i - 15;
	ldata.events[ldata.ev_idx].type = event_flag;
}

void draw_log(){

	char str[64];
	sprintf(str, "STATE %d:  ENEMIES:%d  BONUS:%d  EVENTS:%d", ldata.index + 1, enemy_count, bonus_count, ldata.no_of_event);

	draw_text(str, CENTER, SCREEN_WIDTH/2, SCREEN_HEIGHT - 35);
}

void display_error(int error){

	int read = 0;
	SDL_Event e;
	while (!read){
				
		while (SDL_PollEvent(&e) != 0){		// check for key event
			
			if(e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONUP){
				return;
			}
		}

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		set_ortho();

		switch (error){

			case 0:
				draw_text("Events and States do not match" ,CENTER, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
				break;
			case 1:
				draw_text("Data file not defined" ,CENTER, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
				break;
			case 2:
				draw_text("Loop file not defined" ,CENTER, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
				break;
			default:
				draw_text("This is strange!" ,CENTER, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
				break;
		}

		unset_ortho();
		draw_grid();
		SDL_GL_SwapWindow(mywindow);
		SDL_Delay(10);
		glClear(GL_COLOR_BUFFER_BIT);
		
	}
}

int validate_level(){

	int count = 1;

	// COUNT NUMBER OF STATE INCREMENTS/DECREMENTS
	// AND COMPARE TO NUMBER OF STATE
	for (int i = 0; i < ldata.no_of_event; ++i){
		if (ldata.events[i].val3 == 1)
			count += 1;
	}

	if (count != ldata.no_of_state){
		printf("Events and States do not validate\n");
		printf("State increments = %d\tNumber of states = %d\n", count, ldata.no_of_state);
		display_error(0);
		return 1;
	}

	// CHECK THAT USER HAS DEFINED A LEVEL FILE
	if (file_names.data[0] == '\0'){
		printf("You should define a data file!\n");
		printf("Lest you loose all your work!\n");
		display_error(1);
		return 1;
	}
	//	TODO check for loop file
	return 0;
}