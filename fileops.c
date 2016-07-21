#include "main.h"
// FORMAT
// -----------------------------------------
// ldata.index
// ldata.no_of_state
// ldata.states[0].no_of_object OBJECT DATA
// ldata.states[1].no_of_object OBJECT DATA
// ...
// no_of_event
// ldata.events[0] EVENT DATA
// ...
// model verts int
// model data
// texture data????
// sounds ?????
// player data????

extern level_data ldata;
extern model_data *model;
extern player p0;
//extern void init_player();
//model_data *model;
//extern model_data *model;


void write_leveldata(char s[]){

	char prefix[] = "data/";
	char path[64];
	snprintf(path, 64, "%s%s", prefix, s);
	printf("%s\n", path);

	SDL_RWops* file = SDL_RWFromFile(path, "w+b");

	// WRITE PLAYER INFO
	SDL_RWwrite(file, &start_no_of_tile, sizeof(int), 1);
	SDL_RWwrite(file, &start_tile_types, sizeof(int), MAX_TILES);

	// WRITE LEVEL_DATA: int index; int no_of_state
	//SDL_RWwrite(file, &ldata.index, sizeof(int), 1);
	//SDL_RWwrite(file, &ldata.ev_idx, sizeof(int), 1);
	SDL_RWwrite(file, &ldata.no_of_state, sizeof(int), 1);
	SDL_RWwrite(file, &ldata.gcolour, sizeof(vector3), 1);
	SDL_RWwrite(file, &ldata.lcolour, sizeof(vector3), 1);
	SDL_RWwrite(file, &ldata.hcolour, sizeof(vector3), 1);
	// WRITE STATES AND OBJECTS
	for (int i = 0; i < ldata.no_of_state; ++i){							
		// FOR EACH STATE WRITE no_of_object
		SDL_RWwrite(file, &ldata.states[i].no_of_object, sizeof(int), 1);
		SDL_RWwrite(file, &ldata.states[i].model_size, sizeof(float), 1);
		SDL_RWwrite(file, &ldata.states[i].model, sizeof(char), 16);
		SDL_RWwrite(file, &ldata.states[i].loops, sizeof(char), 16);
		SDL_RWwrite(file, &ldata.states[i].message, sizeof(char), 64);
		// WRITE THE OBJECT DATA
		SDL_RWwrite(file, ldata.states[i].objects, sizeof(object), ldata.states[i].no_of_object);
	}

	// WRITE EVENT DATA
	SDL_RWwrite(file, &ldata.no_of_event, sizeof(int), 1);
	for (int i = 0; i < ldata.no_of_event; ++i){
		SDL_RWwrite(file, &ldata.events[i].type, sizeof(int), 1);
		SDL_RWwrite(file, &ldata.events[i].val1, sizeof(int), 1);
		SDL_RWwrite(file, &ldata.events[i].val2, sizeof(int), 1);
		SDL_RWwrite(file, &ldata.events[i].val3, sizeof(int), 1);
	}
	
	SDL_RWclose(file);
}

void read_leveldata(char  s[]){

	char prefix[] = "data/";
	char path[64];
	snprintf(path, 64, "%s%s", prefix, s);
	printf("%s\n", path);
	
	
	SDL_RWops* file = SDL_RWFromFile(path, "r+b");

	// FILE DOES NOT EXIST 
	if (file == NULL){ 
		printf("ERROR: Unable to open Level! SDL Error: %s\n", SDL_GetError());
		return;
	}
	
	if (ldata.states != NULL){
		free_resources();
	}

	// READ PLAYER INFO
	SDL_RWread(file, &start_no_of_tile, sizeof(int), 1);
	SDL_RWread(file, &start_tile_types, sizeof(int), MAX_TILES);

	// READ LEVEL_DATA: int index; int no_of_state
	//SDL_RWread(file, &ldata.index, sizeof(int), 1);
	//SDL_RWread(file, &ldata.ev_idx, sizeof(int), 1);
	SDL_RWread(file, &ldata.no_of_state, sizeof(int), 1);
	SDL_RWread(file, &ldata.gcolour, sizeof(vector3), 1);
	SDL_RWread(file, &ldata.lcolour, sizeof(vector3), 1);
	SDL_RWread(file, &ldata.hcolour, sizeof(vector3), 1);
	// READ STATES AND OBJECT DATA
	// ALLOCATE MEMORY FOR STATES
	ldata.states = malloc(sizeof(state) * ldata.no_of_state);

	for (int i = 0; i < ldata.no_of_state; ++i){
		SDL_RWread(file, &ldata.states[i].no_of_object, sizeof(int), 1);
		// ALLOCATE MEMORY FOR OBJECTS
		ldata.states[i].objects = malloc(sizeof(object) * ldata.states[i].no_of_object);
		SDL_RWread(file, &ldata.states[i].model_size, sizeof(float), 1);
		SDL_RWread(file, &ldata.states[i].model, sizeof(char), 16);
		SDL_RWread(file, &ldata.states[i].loops, sizeof(char), 16);
		SDL_RWread(file, &ldata.states[i].message, sizeof(char), 64);
		// READ THE OBJECT DATA
		SDL_RWread(file, ldata.states[i].objects, sizeof(object), ldata.states[i].no_of_object);
	}

	// READ EVENT DATA
	SDL_RWread(file, &ldata.no_of_event, sizeof(int), 1);
	// ALLOCATE MEMORY FOR EVENTS
	ldata.events = malloc(sizeof(event) * ldata.no_of_event);

	for (int i = 0; i < ldata.no_of_event; ++i){
		// READ EVENT VARIABLES
		SDL_RWread(file, &ldata.events[i].type, sizeof(int), 1);
		SDL_RWread(file, &ldata.events[i].val1, sizeof(int), 1);
		SDL_RWread(file, &ldata.events[i].val2, sizeof(int), 1);
		SDL_RWread(file, &ldata.events[i].val3, sizeof(int), 1);
		// SET EVENT HANDLER FUNCTION POINTERS
		switch (ldata.events[i].type){
			case ALL_CLEAR:
				ldata.events[i].handle = e_all_clear;
				break;
			case ALL_DEAD:
				ldata.events[i].handle = e_all_dead;
				break;
			case KILLED_ENTITIES:
				ldata.events[i].handle = e_numkilled;
				break;
			case SWITCH_MUSIC:
				ldata.events[i].handle = e_alt_music;
				break;
			case END_LEVEL:
				ldata.events[i].handle = e_end_level;
				break;
			case E_TIMER:
				ldata.events[i].handle = e_timer;
				break;
			case GRID_MOD:
				ldata.events[i].handle = e_grid_mod;
				break;
			default:
				break;
		}
	}
	
	SDL_RWclose(file);
	
	ldata.index = 0;
	ldata.ev_idx = 0;

}

void write_model(char s[]){

	SDL_RWops* file = SDL_RWFromFile(s, "w+b");
	SDL_RWwrite(file, &no_of_verts, sizeof(int), 1);
	SDL_RWwrite(file, &model, sizeof(model_data), no_of_verts);
	SDL_RWclose(file);
}

void read_model(char s[]){

	char prefix[] = "data/";
	char path[64];
	snprintf(path, 64, "%s%s", prefix, s);
	printf("%s\n", path);

	SDL_RWops* file = SDL_RWFromFile(path, "r+b");
	
	// READ MODEL DATA
	if (file == NULL){
		printf("model file not found\n");
		return;
	}
	
	SDL_RWread(file, &no_of_verts, sizeof(int), 1);

	// if (model != NULL){
	// 	free(model);
	// 	model = NULL;
	// }
	model = malloc(no_of_verts * sizeof(model_data));
	assert (model != 0);
	stride = sizeof(model_data);
	SDL_RWread(file, model, sizeof(model_data), no_of_verts);
	SDL_RWclose(file);

	load_model();
	free(model);
	model = NULL;
}

void read_tracks(char s[]){
	//TODO LOAD TRACKS FROM MEMORY BUFFER
}

void write_tracks(char s[]){
	
	//SDL_RWops* file = SDL_RWFromFile(s, "r+b");
	// GET SIZE OF TRACK
	// MALLOC
	// READ
	// WRITE HEADER
	// WRITE TRACK
	// LOOP
}

void print_leveldata(){

	printf("---------------\n");
	printf("index = %d\n", ldata.index);
	printf("no_of_state = %d\n", ldata.no_of_state);
	printf("no_of_event = %d\n", ldata.no_of_event);
	printf("size of ldata %d\n", (int)sizeof(ldata));
	for (int i = 0; i < ldata.no_of_state; ++i){
		printf("no_of_object in S%d = %d\n", i, ldata.states[i].no_of_object);
	}

}