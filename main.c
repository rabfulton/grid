#include "main.h"
void init_game(void);
void handle_events(void);
void update_node(void);
void update_spring(void);
void update_tiles(void);
void displace(float x, float z, float strength);
void add_force(vector3 f, int i, int row);
void init_grenade(float speed);
void init_torpedo(float unused);
int update_grenade(grenade*);
int update_torpedo(torpedo*);
void collisions(void);
void kill_entity(int idx);
void init_particles();
void set_particles(vector3 pos);
void update_particles(particles * parts, float time);
void create_entity(vector3 pos, int type, int multiply);
void create_entity3d(object *obj);
void init_level(char *str);
void init_player();
void mod_height();
void collect_item(int);
void collect_cards(int number, int type);
void delete_active_tile(int i);

//sint setup_tiles();

void load_gamestate();
void update_sinez();
void update_linear(int i);
void kill_entity3d(int idx);
void play_music();

void load_music();

// TODO 
// lookup tables for sine cos rand
extern SDL_Window* mywindow;
//extern vector3 colours;
int	SCREEN_WIDTH = 480;
int	SCREEN_HEIGHT = 800;
node nodes[GRIDL][GRIDW];
spring springv[GRIDL - 1][GRIDW];
spring springh[GRIDL][GRIDW - 1];
particles parts[4];
particles* next_parts = NULL;
Sound sound[MAX_SOUNDS];
Music music[8];
//player p0;	// PLAYER INIT VALUES
player p1;	// IN GAME PLAYER
entity_array en;
entity3d_array en3d;
GLuint t_parts;
GLuint t_cards;
GLuint t_score;
GLuint t_score_blur;
level_data ldata;
int start_tile_types[MAX_TILES];
int start_no_of_tile = 0;

vector4 view = {0.0, -0.22, -0.38, 27};
//float view.y = -0.22;
//float view.x = 0.0;
//float view.z = -0.38;
//float view.w = 27;
float model_size = 0.3;
int mousex;
int mousey;
int c = 0;
int quit = 0;
int frametime = 0;
int level = 1;
int editor_mode = 0;
int music_index = 0;
int gridmod = 0;
int lives = 3;
int message_flag = 0;
char message[64] = "";
int flicker = 0;


int setup_tiles(){

	float t = 0.05;
	vector3 pos[p1.no_of_tiles];
	int spin[p1.no_of_tiles];
	int tile_no = 1;
	
	for (int i = 0; i < p1.no_of_tiles; ++i){
		pos[i].x = p1.position.x;
		pos[i].y = p1.position.y + i * 0.002;
		pos[i].z = -1;
		spin[i] = 150;
	//	mod_height();
	}
	glViewport(SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glLoadIdentity();
	glTranslatef(view.x, view.y, view.z);
	glRotatef(view.w, 1, 0, 0);	
	play_sound(WHOOSH, 0);

	while (pos[p1.no_of_tiles - 1].z < p1.position.z){
		
		handle_events();	// todo handle system events
		draw_grid();

		if (quit == 1)
			return 0;

		for (int i = 0; i < tile_no; ++i){
			// update tile positions
			pos[i].z = pos[i].z - (pos[i].z - p1.position.z) * t;
			if (spin[i]  > 0)
				spin[i] -= 3;
			if (fabs(pos[i].z - p1.position.z) < 0.0005)
				pos[i].z = p1.position.z;
		}

		for (int i = 0; i < tile_no; ++i){
			// draw the tiles active so far
			draw_setup_tiles(pos[i], p1.tile_type[i], spin[i]);
		}

		if ((pos[tile_no - 1].z - p1.position.z) > -0.3){
			if (tile_no < p1.no_of_tiles){
				++tile_no;
				play_sound(WHOOSH, 0);
			}
		}
		
		//glClear(GL_DEPTH_BUFFER_BIT);
		SDL_GL_SwapWindow(mywindow);
		SDL_Delay(10);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
	}
	
		
	return 0;
}

int main(int argc, char* args[]){

	if (argc > 1)
		level = atoi(args[1]);
	for (int i = 0; i < MAX_TILES; ++i){
		p1.act.tiles[i].type = 99;						// this is never a valid tile type!
		p1.act.tiles[i].data = NULL;
	}
	init_game();
	
	Uint32 startclock = 0;
	Uint32 accumclock = 0;
	Uint32 oldclock = SDL_GetTicks();
	play_music();
	while (!quit){
		
		handle_events();
		
		while (editor_mode == 1){
			
			handle_events();

			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			level_editor();
			glViewport(SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			
			glLoadIdentity();
			glTranslatef(view.x, view.y, view.z);	
			glRotatef(view.w, 1, 0, 0);	
			
			draw_grid();
			draw_entities();
			draw_stack();
			glClear(GL_DEPTH_BUFFER_BIT);
			draw_models();
			set_ortho();
				draw_score(123456);
			unset_ortho();
			SDL_GL_SwapWindow(mywindow);
			SDL_Delay(10);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
			oldclock = SDL_GetTicks();
			if (quit == 1){
				close_gui();
				free_resources();
				close_sdl();
				return 0;
			}
		}
		
		startclock = SDL_GetTicks();				// Start of frame
		frametime = startclock - oldclock;
		accumclock +=  frametime;					// add frame time to accumulator
		oldclock = startclock;
		

		glLoadIdentity();
		glTranslatef(view.x, view.y, view.z);		// TODO JUDDER
		glRotatef(view.w, 1, 0, 0);					// z = rot clock/ccw

		if (accumclock > 130)						// Avoid stall due to accum too high
			accumclock = 130;

		while (accumclock > 64){					// if > 16ms (60fps)
			update_node();							// TODO HOW DOES VSYNC AFFECT THIS?
			update_spring();
			if (message_flag)
				--message_flag;						// TODO ARE WE RENDER BOUND OR PHYSICS BOUND?
			accumclock -= 64;			
		}
		assert (ldata.events[ldata.ev_idx].handle != 0);
		ldata.events[ldata.ev_idx].handle();		// Game event handler

		if (p1.act.no_active > 0)
			update_tiles();

		for (int i = 0; i < en3d.no_active; ++i)
			en3d.data[i].update(i);					// TODO UPDATE_ENT3D FUNCTION??
		
		collisions();
		mod_height();

		// TODO MAYBE FASTER TO PUT THIS LOOP IN UPDATE
		for (int i = 0; i < 4; ++i){
			if (parts[i].age > 0){
				update_particles(&parts[i], frametime);
				draw_particles(&parts[i]);
				parts[i].age -= 0.015;
			}
		}

		draw_models();
		draw_grid();
		draw_entities();
		draw_stack();
		if (p1.act.no_active > 0)
			draw_active_tiles();

		set_ortho();
			draw_score(93210);
			if (message_flag){
				draw_text(message, CENTER, SCREEN_WIDTH/2, SCREEN_HEIGHT/4);
			}
		unset_ortho();
		
		SDL_GL_SwapWindow(mywindow);
		SDL_Delay(1);
		if (flicker){
			flicker = 0;
			continue;
		}
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);		

	}


	if (editor_mode == 1)
		close_gui();
	free_resources();
	close_sdl();
	return 0;
}

void init_sound(){

	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
	int audio_channels = 2;
	int audio_buffers = 1024;
   
    SDL_Init(SDL_INIT_AUDIO);

	/* open up our audio device. Load the sounds */
     
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
    	printf("Unable to open audio!\n");
    	exit(1);
  	}
  	Mix_AllocateChannels(16);	

	sound[SPLASH].effect = Mix_LoadWAV("./data/splash.ogg");
	sound[WHOOSH].effect = Mix_LoadWAV("./data/whoosh.ogg");
	sound[BOMB].effect = Mix_LoadWAV("./data/bomb.ogg");
	sound[KILL_SOUND].effect = Mix_LoadWAV("./data/kill.ogg");
	sound[LAUNCH].effect = Mix_LoadWAV("./data/torpedo.ogg");

	//load_music();
}

void load_music(){

	char path[64];
	if (ldata.states[0].loops[0] != '\0'){

		for (int i = 0; i < 8; ++i){

			snprintf(path, 64, "./data/%s%d.ogg", ldata.states[0].loops, i + 1);
			music[i].music = Mix_LoadMUS(path);
			if (music[i].music == NULL){
				printf("%s not found! using default loop\n", path);
				music[i].music = Mix_LoadMUS("./data/loop2.ogg");
			}
		}
	}
	else{
		printf("Loops file not set!\n");
		music[LOOP1].music = Mix_LoadMUS("./data/loop1.ogg");
		music[LOOP2].music = Mix_LoadMUS("./data/loop2.ogg");
		music[LOOP3].music = Mix_LoadMUS("./data/loop3.ogg");
		music[LOOP4].music = Mix_LoadMUS("./data/loop4.ogg");
		music[LOOP5].music = Mix_LoadMUS("./data/loop5.ogg");
		music[LOOP6].music = Mix_LoadMUS("./data/loop6.ogg");
		music[LOOP7].music = Mix_LoadMUS("./data/loop7.ogg");
		music[LOOP8].music = Mix_LoadMUS("./data/loop8.ogg");
	}
}

void play_sound(int index, int loop){
	
	if (editor_mode == 1)
		return;

	Mix_PlayChannel(index, sound[index].effect, loop);
}

void play_music(){
	
	Mix_PlayMusic(music[music_index].music, 0);	
	Mix_HookMusicFinished(play_music);
}

void free_music(){
	
	Mix_HaltMusic();
	for (int i = 0; i < 8; ++i){
		if (music[i].music != NULL){
			Mix_FreeMusic(music[i].music);
			music[i].music = NULL;
		}
	}
}

void init_grid(){

	gridmod = 0;
	vector3 res;
	for (int i = 0; i < GRIDL; ++i){
		// row means column here!!
		for (int row = 0; row < GRIDW; ++row){
			
			nodes[i][row].pos.x = (row - (GRIDW)/2) * GRID_SPACING;
			nodes[i][row].pos.y = 0;
			nodes[i][row].pos.z = -0.1 - (i - 1) * GRID_SPACING;
			
			nodes[i][row].vel.x = 0;
			nodes[i][row].vel.y = 0;
			nodes[i][row].vel.z = 0;

			nodes[i][row].accel.x = 0;
			nodes[i][row].accel.y = 0;
			nodes[i][row].accel.z = 0;
			
			if (row == 0 || row == GRIDW - 1 || i == 0 || i == GRIDL - 1){
				nodes[i][row].imass = 0;
		if (row == 0 || row == GRIDW - 1)
				nodes[i][row].pos.y = 0;//sinf(i*0.126)/25;
			}
			else
				nodes[i][row].imass = NODE_MASS;
			nodes[i][row].damping = NODE_DAMPING;

		}
	}

	for (int i = 0; i < GRIDL; ++i){
		for (int row = 0; row < GRIDW; ++row){
			// Horizontal Springs L * W - 1
			if (row != GRIDW - 1){
				springh[i][row].n1.x = i;
				springh[i][row].n1.y = row;
				springh[i][row].n2.x = i;
				springh[i][row].n2.y = row + 1;
				springh[i][row].k = SPRING_STIFFNESS;
				springh[i][row].d = SPRING_DAMPING;
				res = v3_sub(&nodes[i][row].pos, &nodes[i][row + 1].pos);
				springh[i][row].len = 0.018 * v3_magnitude(&res);
			}
 
			// Vertical Springs L - 1 * W
			if (i != GRIDL - 1){
				springv[i][row].n1.x = i;
				springv[i][row].n1.y = row;
				springv[i][row].n2.x = i + 1;
				springv[i][row].n2.y = row;
				springv[i][row].k = SPRING_STIFFNESS;
				springv[i][row].d = SPRING_DAMPING;
				res = v3_sub(&nodes[i][row].pos, &nodes[i + 1][row].pos);
				springv[i][row].len = 0.018 * v3_magnitude(&res);
			}
		}
	}
}

void init_game(){
	
	srand(time(NULL));
	init_sound();

	if (init_sdl() != 0){
		printf("initialisation error\n");
		close_sdl();
		exit (1);
	}

	t_score = load_texture("data/score.png", 8);	// THIS IS ACTUALLY 1BIT INDEXED?
	t_score_blur = load_texture("data/scoreblur.png", 8);
	t_parts = load_texture("data/part2.png", 32);
	t_cards = load_texture("data/cards4.png", 32);
	active_font = hud_font;
	char str[16];
	sprintf(str, "level%u.dat", level);
	init_level(str);
}

void init_player(){

	lives = 3;
	p1.no_of_tiles = start_no_of_tile;

	for (int i = 0; i < start_no_of_tile; ++i){

		p1.tile_type[i] = start_tile_types[i];

		switch (start_tile_types[i]){

			case GRENADE:
				p1.activate[i] = init_grenade;
				break;
			case TORPEDO:
				p1.activate[i] = init_torpedo;
				break;
			//case HOOK:
				//p1.activate[i] = init_hook;
			default:
				p1.activate[i] = init_torpedo;
				break;
		}
	} 
	// Reset all active tiles.
	for (int i = 0; i < MAX_TILES; ++i){
		p1.act.tiles[i].type = 99;						// this is never a valid tile type!
		if (p1.act.tiles[i].data != NULL){
	//		free(p1.act.tiles[i].data);		// this somehow causes duplicate free()
			p1.act.tiles[i].data = NULL;
		}
	}

	p1.position.x = nodes[2][GRIDW/2].pos.x;			// center of grid
	p1.position.y = nodes[2][GRIDW/2].pos.y + 0.002; 	// above the grid surface
	p1.position.z = nodes[2][GRIDW/2].pos.z;			// in front of the camera
	p1.act.no_active = 0;
}

void init_level(char *str){

	init_grid();
	init_particles();
	//char str[16];
	//sprintf(str, "level%u.dat", level);
	read_leveldata(str);
	load_music();

	view.x = 0.0;
	view.y = -0.22;
	view.z = -0.38;
	view.w = 27;

	init_player();

	grid_colours(ldata.gcolour.x, ldata.gcolour.y, ldata.gcolour.z);
	vector3 col;
	col.x = ldata.lcolour.x; 
	col.y = ldata.lcolour.y; 
	col.z = ldata.lcolour.z;
	set_lights(col);
	col.x = ldata.hcolour.x; 
	col.y = ldata.hcolour.y; 
	col.z = ldata.hcolour.z;
	set_hud_colour(col);

	setup_tiles();
	load_gamestate();
	return;
}

void load_gamestate(){

	
	int idx = ldata.index;
	printf("loading gamestate state idx = %i\n", idx);
	if (ldata.states[idx].model[0] != '\0')
		read_model(ldata.states[idx].model);

	model_size = ldata.states[idx].model_size;

	for (int i = 0; i < ldata.states[idx].no_of_object; ++i){

		if (ldata.states[idx].objects[i].type < MODELS){
			create_entity(ldata.states[idx].objects[i].pos, ldata.states[idx].objects[i].type, ldata.states[idx].objects[i].health);
		}
		else{
			create_entity3d(&ldata.states[idx].objects[i]);
		}
	}

	if (ldata.states[idx].message[0] != '\0'){
		message_flag = MESSAGE_TIME;
		strncpy(message, ldata.states[ldata.index].message, 63);
	}
}

void free_resources(){

	printf("freeing resources\n");
	// FREES LEVEL DATA BEFORE PROGRAM EXIT OR LOADING A NEW LEVEL
	for (int i = 0; i < ldata.no_of_state; ++i){
		free(ldata.states[i].objects);
		ldata.states[i].objects = NULL;	
	}

	free (ldata.states);
	ldata.states = NULL;
	free (ldata.events);
	ldata.events = NULL;

	free_music();
}

void e_all_dead(){
	// If all dead increment state_index AND STATE POINTER/EVENT POINTER
	

	if (p1.act.no_active == 0 && en3d.no_active == 0){
		printf("all_dead\nev_idx = %d\n", ldata.ev_idx);

		if (ldata.events[ldata.ev_idx].val3 == 1){
			ldata.index += 1;
			load_gamestate();
		}
		if (ldata.events[ldata.ev_idx].val3 == -1){
			ldata.index -= 1;
			load_gamestate();
		}

		ldata.ev_idx += 1;
		if (ldata.ev_idx == ldata.no_of_event){
			ldata.ev_idx -= 1;
			ldata.events[ldata.ev_idx].handle = e_end_level;
		}
	}
}

void e_end_level(){
	
	// TODO if end level free resources and load next level
	if (p1.act.no_active == 0 && en3d.no_active == 0){
		printf("end_level\nev_idx = %d\n", ldata.ev_idx);
		//free_resources();
		char str[16];
		sprintf(str, "level%u.dat", level);
		init_level(str);
	}
}

void e_alt_music(){

	// 0 = call back 1 = now 2 = crossfade
	
	printf("music changed\nev_idx = %d\n", ldata.ev_idx);

	music_index = ldata.events[ldata.ev_idx].val1;

	if(ldata.events[ldata.ev_idx].val2 == 1){
		play_music();
	}
	else if(ldata.events[ldata.ev_idx].val2 == 2){
		Mix_FadeOutMusic(2500);
	}
	if (ldata.events[ldata.ev_idx].val3 == 1){
		ldata.index += 1;
		load_gamestate();
	}
	if (ldata.events[ldata.ev_idx].val3 == -1){
		ldata.index -= 1;
		load_gamestate();
	}
	++ldata.ev_idx;
	if (ldata.ev_idx == ldata.no_of_event){
		ldata.ev_idx -= 1;
		ldata.events[ldata.ev_idx].handle = e_end_level;
	}
}

void e_all_clear(){


	if (en.no_active == 0 && p1.act.no_active == 0 && en3d.no_active == 0){
		printf("all_clear\nev_idx = %d\n", ldata.ev_idx);

		if (ldata.events[ldata.ev_idx].val3 == 1){
			ldata.index += 1;
			load_gamestate();
		}
		if (ldata.events[ldata.ev_idx].val3 == -1){
			ldata.index -= 1;
			load_gamestate();
		}

		++ldata.ev_idx;
		if (ldata.ev_idx == ldata.no_of_event){
			ldata.ev_idx -= 1;
			ldata.events[ldata.ev_idx].handle = e_end_level;
		}
	}
}

void e_numkilled(){

	static int last = 0;
	static int num = 0;

	if (last - en3d.no_active > 0){
		++num;
	}

	if (num == ldata.events[ldata.ev_idx].val1){
		
		if (ldata.events[ldata.ev_idx].val3 == 1){
			ldata.index += 1;
			load_gamestate();
		}
		if (ldata.events[ldata.ev_idx].val3 == -1){
			ldata.index -= 1;
			load_gamestate();
		}
		last = 0;
		num = 0;
		printf("e killed %d\nev_idx = %d\n", ldata.events[ldata.ev_idx].val1, ldata.ev_idx);
		++ldata.ev_idx;
		if (ldata.ev_idx == ldata.no_of_event){
			ldata.ev_idx -= 1;
			ldata.events[ldata.ev_idx].handle = e_end_level;
		}
		return;
	}

	last = en3d.no_active;
}

void e_timer(){
	
	static unsigned int t = 0;
	t += frametime;
	if (t > ldata.events[ldata.ev_idx].val1 * 1000){
		printf("timer\nev_idx = %d\n", ldata.ev_idx);

		if (ldata.events[ldata.ev_idx].val3 == 1){
			ldata.index += 1;
			load_gamestate();
		}
		if (ldata.events[ldata.ev_idx].val3 == -1){
			ldata.index -= 1;
			load_gamestate();
		}

		t = 0;
		++ldata.ev_idx;
		if (ldata.ev_idx == ldata.no_of_event){
			ldata.ev_idx -= 1;
			ldata.events[ldata.ev_idx].handle = e_end_level;
		}
	}
}

void e_grid_mod(){

	printf("grid_mod\nev_idx = %d\n", ldata.ev_idx);

	gridmod = ldata.events[ldata.ev_idx].val1;

	if (ldata.events[ldata.ev_idx].val3 == 1){
			ldata.index += 1;
			load_gamestate();
		}
	if (ldata.events[ldata.ev_idx].val3 == -1){
		ldata.index -= 1;
		load_gamestate();
	}
	++ldata.ev_idx;
	if (ldata.ev_idx == ldata.no_of_event){
		ldata.ev_idx -= 1;
		ldata.events[ldata.ev_idx].handle = e_end_level;
	}
}

void mod_height(){

	p1.position.y = nodes[2][(int)(GRIDC + GRID_SPACING_INV * p1.position.x)].pos.y + MODHEIGHT;

	for (int i = 0; i < en.no_active; ++i){
		int x = (int)(GRIDC + GRID_SPACING_INV * en.data[i].pos.x);
		int z = -(int)(GRID_SPACING_INV * en.data[i].pos.z);
		en.data[i].pos.y = nodes[z][x].pos.y;
	}
}

void create_entity(vector3 pos, int type, int multiply){

	if (en.no_active == MAX_ENTITIES)
		return;

	en.data[en.no_active].type = type;
	en.data[en.no_active].multiply = multiply;
	en.data[en.no_active].pos.x = pos.x;
	en.data[en.no_active].pos.y = pos.y;
	en.data[en.no_active].pos.z = pos.z;
	++en.no_active;
}

void create_entity3d(object *obj){

	if (en3d.no_active == MAX_3DENTITIES)
		return;

	en3d.data[en3d.no_active].type = obj->type;
	printf("e3d type = %d", obj->type);
	switch (obj->type){
		case SINE_Z:
			en3d.data[en3d.no_active].update = update_sinez;
			en3d.data[en3d.no_active].vel.y = 0.1;
			break;
		case LINEAR:
			en3d.data[en3d.no_active].update = update_linear;
			en3d.data[en3d.no_active].vel.y = 0.0;
			break;
		default:
			break;
	}
	en3d.data[en3d.no_active].pos.x = obj->pos.x;
	en3d.data[en3d.no_active].pos.y = obj->pos.y;
	en3d.data[en3d.no_active].pos.z = obj->pos.z;
	en3d.data[en3d.no_active].ang.x = 0;
	en3d.data[en3d.no_active].ang.y = 0;
	en3d.data[en3d.no_active].ang.z = 0;
	en3d.data[en3d.no_active].vel.x = 0;			// use as time in Asin(wt)
	//en3d.data[en3d.no_active].vel.y = 0.1;	// use as Amplitude in Asin(wt)
	en3d.data[en3d.no_active].vel.z = obj->speed * 0.008;	// dolphin speed
	

	printf("%d created at %.2f, %.2f, %.2f\n", en3d.no_active, en3d.data[en3d.no_active].pos.x, 
		en3d.data[en3d.no_active].pos.y,
		en3d.data[en3d.no_active].pos.z);
	++en3d.no_active;
}

void update_sinez(int i){
	
	float time = frametime * 0.01;
	float new_y, new_z;
	
	en3d.data[i].vel.x += 0.01;	// time
	new_y = -1 * en3d.data[i].vel.y * cosf(2 * PI * en3d.data[i].vel.x);
	new_z = -1 * en3d.data[i].vel.y * cosf(2 * PI * en3d.data[i].vel.x - 1);
	en3d.data[i].ang.z = new_z * 100;
	if ((en3d.data[i].pos.y < 0) && new_y > 0){	// UP
		displace(en3d.data[i].pos.x, en3d.data[i].pos.z + 0.14, 0.02);
	}
	if ((en3d.data[i].pos.y > 0) && (new_y < 0)){	// DOWN
		displace(en3d.data[i].pos.x, en3d.data[i].pos.z + 0.14, -0.02);
	}
	
	en3d.data[i].pos.z += (en3d.data[i].vel.z) * time;
	en3d.data[i].pos.y = new_y;
	// reset dolphin
	if (en3d.data[i].pos.z > 0.3){
		en3d.data[i].pos.z = -(GRIDL - 1) * GRID_SPACING - 0.3; // test case reset dolphin
		en3d.data[i].vel.x = 0;
	}
}

void update_linear(int i){
	
	float time = frametime / 100.0;

 	displace(en3d.data[i].pos.x, en3d.data[i].pos.z, -0.01);

	en3d.data[i].pos.z += (en3d.data[i].vel.z) * time;
	// reset dolphin
	if (en3d.data[i].pos.z > 0.3){
		en3d.data[i].pos.z = -(GRIDL - 1) * GRID_SPACING - 0.3; // test case reset dolphin
		en3d.data[i].vel.x = 0;
	}
}

void update_node(){

	int row;
	static float x = 0;
	float yoffset;
	float yoffset2;
	x += 1;
	
	for (int i = 1; i < GRIDL - 1; ++i){
		yoffset = gridmod ? sinf((x+i) * 0.126) * 0.0325 * gridmod : 0;
		yoffset2 = gridmod ? cosf((x+i) * 0.126) * 0.0325 * gridmod : 0;
		nodes[i][0].pos.y = yoffset2;
		nodes[i][GRIDW-1].pos.y = yoffset2;
		for (row = 1; row < GRIDW - 1; ++row){

			nodes[i][row].vel.x += nodes[i][row].accel.x;
			nodes[i][row].vel.y += nodes[i][row].accel.y;
			nodes[i][row].vel.z += nodes[i][row].accel.z;
			
			nodes[i][row].pos.x += nodes[i][row].vel.x;
			nodes[i][row].pos.y += nodes[i][row].vel.y;
			//nodes[i][row].pos.y = yoffset;
			nodes[i][row].pos.z += nodes[i][row].vel.z;

			nodes[i][row].accel.x = 0;
			nodes[i][row].accel.y = yoffset/100;//0;
			nodes[i][row].accel.z = 0;
			
			v3_scaler_multiply(&nodes[i][row].vel, nodes[i][row].damping);
		}
	}
}

void update_spring(){

	vector3 x, dv, force, force2;
	float len;

	for (int i = 0; i < GRIDL; ++i){
		for (int row = 0; row < GRIDW; ++row){
			if (row != GRIDW - 1){

				// TODO BENCHMARK
				// vector3 *end1 = nodes[i][row].pos;
				// vector3 *end2 = nodes[i][row+1].pos;
				// SHOULD HAVE USED __inline INSTEAD
				// inlined x = v3_sub(&nodes[i][row].pos, &nodes[i][row + 1].pos);
				x.x = nodes[i][row].pos.x - nodes[i][row + 1].pos.x;
				x.y = nodes[i][row].pos.y - nodes[i][row + 1].pos.y;
				x.z = nodes[i][row].pos.z - nodes[i][row + 1].pos.z;
				// inlined len = v3_magnitude(&x);
				len = sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
				// inlined v3_scaler_divide(&x, len);
				x.x /= len;
				x.y /= len;
				x.z /= len;
				// inlined v3_scaler_multiply(&x, len - springh[i][row].len);
				len = len - springh[i][row].len;
				x.x *= len;
				x.y *= len;
				x.z *= len;
				// inlined dv = v3_sub(&nodes[i][row].vel, &nodes[i][row + 1].vel);
				dv.x = nodes[i][row].vel.x - nodes[i][row + 1].vel.x;
				dv.y = nodes[i][row].vel.y - nodes[i][row + 1].vel.y;
				dv.z = nodes[i][row].vel.z - nodes[i][row + 1].vel.z;
				// inlined v3_scaler_multiply(&dv, springh[i][row].d);
				dv.x *= springh[i][row].d;
				dv.y *= springh[i][row].d;
				dv.z *= springh[i][row].d;
				// inlined v3_scaler_multiply(&x, springh[i][row].k);
				x.x *= springh[i][row].k;
				x.y *= springh[i][row].k;
				x.z *= springh[i][row].k;
				// inlined force = v3_sub(&x, &dv);
				force.x = x.x - dv.x;
				force.y = x.y - dv.y;
				force.z = x.z - dv.z;
				// added reverse vector
				force2.x = -force.x;
				force2.y = -force.y;
				force2.z = -force.z;
				// inlined add_force(force, i, row + 1);
					// inlined v3_scaler_multiply(&f, nodes[i][row+1].imass);
					force.x *= nodes[i][row+1].imass;
					force.y *= nodes[i][row+1].imass;
					force.z *= nodes[i][row+1].imass;
					// inlined nodes[i][row+1].accel = v3_add(&nodes[i][row+1].accel, &f);
					nodes[i][row+1].accel.x += force.x;
					nodes[i][row+1].accel.y += force.y;
					nodes[i][row+1].accel.z += force.z;
				// inlined add_force(force2, i, row);
					// inlined v3_scaler_multiply(&f, nodes[i][row].imass);
					force2.x *= nodes[i][row].imass;
					force2.y *= nodes[i][row].imass;
					force2.z *= nodes[i][row].imass;
					// inlined nodes[i][row].accel = v3_add(&nodes[i][row].accel, &f);
					nodes[i][row].accel.x +=  force2.x;
					nodes[i][row].accel.y +=  force2.y;
					nodes[i][row].accel.z +=  force2.z;
			}
			if (i != GRIDL - 1){
				//x = v3_sub(&nodes[i][row].pos, &nodes[i + 1][row].pos);
				x.x = nodes[i][row].pos.x - nodes[i + 1][row].pos.x;
				x.y = nodes[i][row].pos.y - nodes[i + 1][row].pos.y;
				x.z = nodes[i][row].pos.z - nodes[i + 1][row].pos.z;
				//len = v3_magnitude(&x);
				len = sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
				//v3_scaler_divide(&x, len);
				x.x /= len;
				x.y /= len;
				x.z /= len;
				//v3_scaler_multiply(&x, len - springv[i][row].len);
				len = len - springv[i][row].len;
				x.x *= len;
				x.y *= len;
				x.z *= len;
				//dv = v3_sub(&nodes[i][row].vel, &nodes[i + 1][row].vel);
				dv.x = nodes[i][row].vel.x - nodes[i + 1][row].vel.x;
				dv.y = nodes[i][row].vel.y - nodes[i + 1][row].vel.y;
				dv.z = nodes[i][row].vel.z - nodes[i + 1][row].vel.z;
				//v3_scaler_multiply(&dv, springv[i][row].d);
				dv.x *= springv[i][row].d;
				dv.y *= springv[i][row].d;
				dv.z *= springv[i][row].d;
				//v3_scaler_multiply(&x, springv[i][row].k);
				x.x *= springv[i][row].k;
				x.y *= springv[i][row].k;
				x.z *= springv[i][row].k;
				//force = v3_sub(&x, &dv);
				force.x = x.x - dv.x;
				force.y = x.y - dv.y;
				force.z = x.z - dv.z;
				// added/moved reverse vector
				force2.x = -force.x;
				force2.y = -force.y;
				force2.z = -force.z;
				//add_force(force, i + 1, row);
					// inlined v3_scaler_multiply(&f, nodes[i+1][row].imass);
					force.x *= nodes[i+1][row].imass;
					force.y *= nodes[i+1][row].imass;
					force.z *= nodes[i+1][row].imass;
					// inlined nodes[i][row+1].accel = v3_add(&nodes[i][row+1].accel, &f);
					nodes[i+1][row].accel.x += force.x;
					nodes[i+1][row].accel.y += force.y;
					nodes[i+1][row].accel.z += force.z;
				//v3_reverse(&force);
				//add_force(force, i, row);
					// inlined v3_scaler_multiply(&f, nodes[i][row].imass);
					force2.x *= nodes[i][row].imass;
					force2.y *= nodes[i][row].imass;
					force2.z *= nodes[i][row].imass;
					// inlined nodes[i][row].accel = v3_add(&nodes[i][row].accel, &f);
					nodes[i][row].accel.x +=  force2.x;
					nodes[i][row].accel.y +=  force2.y;
					nodes[i][row].accel.z +=  force2.z;
			}
		} 
	}
}

void add_force(vector3 f, int i, int row){
	
	v3_scaler_multiply(&f, nodes[i][row].imass);
	nodes[i][row].accel = v3_add(&nodes[i][row].accel, &f);
}

void displace(float x, float z, float strength){
	
	x = x * GRID_SPACING_INV;
	x += GRIDW/2;
	z = -z * GRID_SPACING_INV;

	if (z > GRIDL || x > GRIDW){		// DEBUG CODE
		return;
	}
	if (z < 0 || x < 0){				// DEBUG CODE
		return;
	}

	nodes[(int)z][(int)x].accel.y += strength * 2;
}

void init_grenade(float speed){

	play_sound(LAUNCH, 0);
	grenade * new;
	new = (grenade *)malloc (sizeof(grenade));

	assert(new != 0);
	
	new->spin = 5;
	
	new->position.x = p1.position.x;
	new->position.y = p1.position.y + 0.001 * p1.no_of_tiles;	// TODO want this?
	new->position.z = p1.position.z;

	new->velocity.x = 0;
	new->velocity.y = speed/2;//0.03;//0.001*speed;
	new->velocity.z = -speed*2;//-0.06;

	new->acceleration.x = 0;
	new->acceleration.y = 0.001;
	new->acceleration.z = 0;
	p1.act.tiles[p1.act.no_active].type = GRENADE;
	p1.act.tiles[p1.act.no_active].data = new;
	p1.act.no_active++;
	p1.no_of_tiles--;
}

int update_grenade(grenade *g){		// return int to indicate if grenade is still live or dead
									// TODO caller will then free the card and reorder stack as appropriate

	float time = frametime / 100.0;
	g->position.y += g->velocity.y * time;
	g->position.z += g->velocity.z * time;
	g->spin += 8;
	g->velocity.y += g->acceleration.y * time; 
	g->acceleration.y -= GRAVITY * time;
 
	if (g->position.y < 0){
		
		flicker = 1;
		displace(g->position.x, g->position.z, 0.05);
		play_sound(BOMB, 0);
		float front, back, left, right;
		// Collisions for grenades go here
		back = g->position.z - GRID_SPACING*8;
		front = back + GRID_SPACING*16;
		left = g->position.x - GRID_SPACING*5;
		right = left + GRID_SPACING*10;

		for (int e = 0; e < en.no_active; ++e){	
			
			if(en.data[e].pos.z > back && en.data[e].pos.z < front){
				if(en.data[e].pos.x > left && en.data[e].pos.x < right){
					collect_item(e);
				}
			}
		}
		for (int e = 0; e < en3d.no_active; ++e){
			if(en3d.data[e].pos.z > back && en3d.data[e].pos.z < front){
				if(en3d.data[e].pos.x > left && en3d.data[e].pos.x < right){
					kill_entity3d(e);
				}
			}
		}

		return 1;
	}

	return 0;	
}

void init_torpedo(float unused){

	play_sound(LAUNCH, 0);
	torpedo * new;
	new = (torpedo *)malloc (sizeof(torpedo));

	assert(new != 0);

	new->position.x = p1.position.x;
	new->position.y = p1.position.y + 0.001 * p1.no_of_tiles;
	new->position.z = p1.position.z;

	new->velocity.x = 0;
	new->velocity.y = 0;
	new->velocity.z = -0.06;

	p1.act.tiles[p1.act.no_active].type = TORPEDO;
	p1.act.tiles[p1.act.no_active].data = new;
	p1.act.no_active++;
	p1.no_of_tiles--;
}

int update_torpedo(torpedo *t){

	float time = frametime / 100.0;
	t->position.z += t->velocity.z * time;
	if (-t->position.z > GRIDL * GRID_SPACING){		
		return 1;
	}
	//grid_stain(t->position.x, t->position.z, 0);
 	displace(t->position.x, t->position.z, -0.01);
 	//displace(t->position.x/GRID_SPACING, -z + GRID_SPACING*2, -0.001);
 	//displace(t->position.x/GRID_SPACING, -z - GRID_SPACING*2, -0.001);

	return 0;
}

void update_tiles(){

	// for each active tile pointer update according to its type
	// FREE MEMORY OF NON ACTIVE TILES
	int status = 0;
	
	for (int i = 0; i < p1.act.no_active; ++i){
		switch (p1.act.tiles[i].type){

			case GRENADE:
				status = update_grenade(p1.act.tiles[i].data);
				break;
			case TORPEDO:
				status = update_torpedo(p1.act.tiles[i].data);
				break;
			default:
				break;
		}
		
		if (status == 1){
			delete_active_tile(i);
			status = 0;
		}
	}
}

void delete_active_tile(int i){

	assert (p1.act.tiles[i].data != NULL);
	free (p1.act.tiles[i].data);
	p1.act.tiles[i].data = NULL;
	if (i != p1.act.no_active - 1){	// if not last tile, move last tile into its place
		//memmove(&p1.act.tiles[i], &p1.act.tiles[p1.act.no_active - 1], sizeof(tile));
		p1.act.tiles[i] = p1.act.tiles[p1.act.no_active - 1];	// TODO this was ok???
		--i; // Current i is now different
	}
	--p1.act.no_active;
}

void collisions(){

	float left, right, front, back;
	int flag = 0;
	// ACTIVE TILES -- ENTITIES
	if (p1.act.no_active != 0){

		for (int i = 0; i < p1.act.no_active; ++i){
			// CHECK AGAINST 2D ENTITIES
			for (int e = 0; e < en.no_active; ++e){

				switch (p1.act.tiles[i].type){
					case TORPEDO:
						// (x >= min && x < max) can be transformed into (unsigned)(x-min) < (max-min)
						left = ((torpedo*)p1.act.tiles[i].data)->position.z - GRID_SPACING;
						right = left + 2 * GRID_SPACING;
						if(en.data[e].pos.z > left && en.data[e].pos.z < right){
							if(en.data[e].pos.x > ((torpedo*)p1.act.tiles[i].data)->position.x - GRID_SPACING*3){
								if(en.data[e].pos.x < ((torpedo*)p1.act.tiles[i].data)->position.x + GRID_SPACING*3){
									collect_item(e);
								} 
							}
						}
						break;
					default:
						break;
				}
			}
			// CHEACK AGAINST 3D ENEMIES
			for (int e = 0; e < en3d.no_active; ++e){
				switch (p1.act.tiles[i].type){
					case TORPEDO:
						back = ((torpedo*)p1.act.tiles[i].data)->position.z - GRID_SPACING;
						front = ((torpedo*)p1.act.tiles[i].data)->position.z + GRID_SPACING;
						if (en3d.data[e].pos.y < 0)	// dolphin under water
							break;
						if (en3d.data[e].pos.x > ((torpedo*)p1.act.tiles[i].data)->position.x - GRID_SPACING * 2){
							if(en3d.data[e].pos.x < ((torpedo*)p1.act.tiles[i].data)->position.x + GRID_SPACING * 2){
								if((en3d.data[e].pos.z > back) && (en3d.data[e].pos.z < front)){
									kill_entity3d(e);
									delete_active_tile(i);
									--i;
								}
							}
						}
						break;
					default:
						break;	
				}
			}
		}
	}
}

void collect_item(int idx){

	play_sound(SPLASH, 0);
	message_flag = MESSAGE_TIME;

	switch (en.data[idx].type){
		case TORPEDO:
			sprintf(message, "Missile X%i", en.data[idx].multiply);
			collect_cards(en.data[idx].multiply, TORPEDO);
			break;
		case GRENADE:
			sprintf(message, "Grenade X%i", en.data[idx].multiply);
			collect_cards(en.data[idx].multiply, GRENADE);
			break;
		case LIFE:
			sprintf(message, "Life X%i", en.data[idx].multiply);
			lives += en.data[idx].multiply;
			break;
		default:
			break;
	}
	memmove(&en.data[idx], &en.data[en.no_active-1], sizeof(entity));
	--en.no_active;	
}

void collect_cards(int number, int type){
	
	for (int x = 0; x < number; ++x){
		if (p1.no_of_tiles < MAX_TILES){

			p1.tile_type[p1.no_of_tiles] = type;

			switch(type){

				case GRENADE:
					p1.activate[p1.no_of_tiles] = init_grenade;
					break;
				case TORPEDO:
					p1.activate[p1.no_of_tiles] = init_torpedo;
					break;
				default:
					break;
			}

			++p1.no_of_tiles;
		}
	}
	printf("Added %i cards\n", number);
}

void kill_entity(int idx){
	// delete entity and sort array decrement no_active
	play_sound(KILL_SOUND, 0);

	memmove(&en.data[idx], &en.data[en.no_active-1], sizeof(entity));
	--en.no_active;	
}

void kill_entity3d(int idx){
	// delete entity and sort array decrement no_active
	flicker = 1;
	set_particles(en3d.data[idx].pos);
	grid_stain(en3d.data[idx].pos.x, en3d.data[idx].pos.z, 0);

	if (en3d.data[idx].health == 0){
		play_sound(KILL_SOUND, 0);
		memmove(&en3d.data[idx], &en3d.data[en3d.no_active-1], sizeof(entity3d));
		en3d.no_active -= 1;		
	}
	else{
		--en3d.data[idx].health;
	}
}

void init_particles(){
	
	float theta;
	float z;
	float adj = 0.02;
	// http://math.stackexchange.com/questions/44689/how-to-find-a-random-axis-or-unit-vector-in-3d
	for (int j = 0; j < 4; ++j){
		for (int i = 0; i < 3 * MAX_PARTS; ++i){
			theta = ((float)rand()/(float)RAND_MAX) * 2.0 * PI;
			z = ((float)rand()/(float)RAND_MAX) * 2.0 - 1;
			parts[j].velocity[i++] = sqrt(1 - z * z) * cos(theta) * adj;
			parts[j].velocity[i++] = (sqrt(1 - z * z) * sin(theta) * adj * 2);
			parts[j].grav = 1; 
			parts[j].velocity[i] = adj * z;
			parts[j].age = 0;
		}
		if (j < 3)
			parts[j].next = &parts[j+1];
		else
			parts[j].next = &parts[0];
	}
	next_parts = &parts[0];
}

void set_particles(vector3 pos){

	for (int i = 0; i < 3 * MAX_PARTS; ++i){
		next_parts->vertex[i++] = pos.x;
		next_parts->vertex[i++] = pos.y;
		next_parts->vertex[i] = pos.z;
	}
	// for (int i = 1; i < 3 * MAX_PARTS; i+=3){
	// 	next_parts->velocity[i] = next_parts->start_y;
	// }
	next_parts->age = 1.0;
	next_parts = next_parts->next;
}

void update_particles(particles *parts, float time){

	float ft = time / 100;
	for (int i = 0; i < 3 * MAX_PARTS; ++i){
		if (parts->vertex[i+1] > -0.001){
			parts->vertex[i] += parts->velocity[i] * ft;		
			++i;
			//parts->vertex[i] -= GRAVITY * ft;// TODO SHOULD BE VERTEX TO AVOID REINIT
			parts->vertex[i] += parts->velocity[i] * ft - 0.0001 * parts->grav * ft;// - GRAVITY;
			++parts->grav;
			++i;
			parts->vertex[i] += parts->velocity[i] * ft;
		}
		else {
			grid_stain(parts->vertex[i], parts->vertex[i+2], 0);
			i +=2;
		}
	}
}

void handle_events(){

	static float red = 0;
	static float green = 0;
	static float blue = 0;
	static float strength = 0.05;
	static int stain = 0;
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0){
		
		if (e.type == SDL_MOUSEBUTTONUP){
			mousex = e.button.x; 
			mousey = e.button.y;
			gui_select();
			//printf("button x: %d\t y:%d\n", mousex, mousey);
			break;
		}
		if (e.type == SDL_KEYDOWN){
			switch(e.key.keysym.sym){
			// KEYDOWNS
			case SDLK_RCTRL:
				quit = 1;
				break;
			case SDLK_UP:
				view.y += 0.01;	//printf("view.y %f\n", view.y);
				break;
			case SDLK_DOWN:
				view.y -= 0.01;	//printf("view.y %f\n", view.y);
				break;
			case SDLK_LEFT:
				view.x += GRID_SPACING;
				printf("view.x %f\n", view.x);
				p1.position.x -= GRID_SPACING;
				break;
			case SDLK_RIGHT:
				view.x -= GRID_SPACING;
				p1.position.x += GRID_SPACING;
				printf("view.x %f\n", view.x);
				break;
			case SDLK_w:
				view.z += 0.01;	//printf("view.z %f\n", view.z);
				break;
			case SDLK_s:
				view.z -= 0.01;	//printf("view.z %f\n", view.z);
				break;
			case SDLK_a:
				view.w += 6;	//printf("view.w %f\n", view.w);
				break;
			case SDLK_d:
				view.w -= 6;	//printf("view.w %f\n", view.w);
				break;
			case SDLK_i:
				++stain;
				grid_stain(0,0,stain);	printf("stain %i\n", stain);
				break;
			
			case SDLK_m:
				music_index = (music_index + 1)%8;
				break;
			case SDLK_SPACE:
				if (p1.no_of_tiles > 0)
					p1.activate[p1.no_of_tiles-1](strength);
				break;
			case SDLK_p:
				if (strength > 0.09)
					strength = 0.01;
				strength += 0.01;
				printf("strength = %f\n", strength);
				break;
			case SDLK_t:
				if (p1.no_of_tiles > 0)
					init_torpedo(1);
				break;
			case SDLK_g:
				gridmod = (gridmod + 1)%3;
				printf("gridmod %d\n", gridmod);
				break;
			case SDLK_1:
				red += 0.02;
				printf("R: %f\t G: %f\t B: %f\n", red, green, blue);
				grid_colours(red, green, blue);
				break;
			case SDLK_2:
				green += 0.02;
				printf("R: %f\t G: %f\t B: %f\n", red, green, blue);
				grid_colours(red, green, blue);
				break;
			case SDLK_3:
				blue += 0.02;
				printf("R: %f\t G: %f\t B: %f\n", red, green, blue);
				grid_colours(red, green, blue);
				break;
			case SDLK_4:
				red -= 0.02;
				printf("R: %f\t G: %f\t B: %f\n", red, green, blue);
				grid_colours(red, green, blue);
				break;
			case SDLK_5:
				green -= 0.02;
				printf("R: %f\t G: %f\t B: %f\n", red, green, blue);
				grid_colours(red, green, blue);
				break;
			case SDLK_6:
				blue -= 0.02;
				printf("R: %f\t G: %f\t B: %f\n", red, green, blue);
				grid_colours(red, green, blue);
				break;
			case SDLK_l:
				if (editor_mode == 0){
					init_gui();
					editor_mode = 1;
				}
				else {

					run_game();
				}
				break;
			}

		}
		if (e.type == SDL_QUIT)
				quit = 1;
	}	
}

