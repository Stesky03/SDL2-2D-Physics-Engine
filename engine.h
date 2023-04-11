#include "SDL.h"
#include "SDL_ttf.h"
#include <iostream> 
#include <math.h>
#include <vector>
using std::vector;

#define GRAVITY 0.16344416666 //9.81 / 60(fps)
#define SQ2 1.4142
#define CIRCLESIDES 64 //64
#define OPTIONS 4
#define OPTIONS2 2
#define UISIZE 50
#define FLOOR 0.80 //0.80

//l'asse y è invertito di default e di conseguenza la rotazione è in senso orario
//queste funzioni NON usano la gpu per il rendering

double cotan(double x) {
	return (cos(x) / sin(x));
}

class Coordinatesclass{
public:
	float x, y;
};
//quadrati
class Square {
public:
	float weight, speedx, speedy, speedr, x, y, y1, r, lato;
	bool selected;
	Coordinatesclass coordinates[4];
	void S(float X, float Y, float s) {
		x = X;
		y = Y;
		lato = s;
		create();

		weight = 1;
		speedx = 0;
		speedy = 0;
		speedr = 0;
		r = 0;
		selected = false;
	};
	void create() {
		float r1 = 45 + r;
		for (int i = 0; i < 4; i++) {
			coordinates[i].x = x + (lato * SQ2 / 2 * cos(r1 * M_PI / 180));
			coordinates[i].y = y + (lato * SQ2 / 2 * sin(r1 * M_PI / 180));
			r1 += 90;
		}
	};
	void resize(float s) { lato = s; };
	void changeweight(float w) { weight = w; };
	void changespeed(float X, float Y, float R) { speedx += X; speedy += Y; speedr += R; };
	void move(float X, float Y, float R) { x += X; y += Y; r += R; };
	void update() {
		x += speedx;
		y += speedy;
		r += speedr;
		while (r >= 90)
			r -= 90;
		create();
	};
};

//cerchi
class Circle{
public:
	float radius, weight, speedx, speedy, speedr, x, y, y1;
	bool selected;
	Coordinatesclass coordinates[CIRCLESIDES];
	void C(float X, float Y, float rad) {
		x = X;
		y = Y;
		radius = rad;
		create();

		weight = 1;
		speedx = 0;
		speedy = 0;
		speedr = 0;
		selected = false;
	};

	void create() {
		double r1 = 0;
		for (int i = 0; i < CIRCLESIDES/4; i++) {
			coordinates[i].x = x + (radius * cos(r1 * M_PI / 180));
			coordinates[i + CIRCLESIDES / 4].x = x + (radius * cos((r1+90) * M_PI / 180));
			coordinates[i + CIRCLESIDES / 2].x = x + (radius * cos((r1 + 180) * M_PI / 180));
			coordinates[i + CIRCLESIDES / 4 * 3].x = x + (radius * cos((r1 + 270) * M_PI / 180));

			coordinates[i].y = y + (radius * sin(r1 * M_PI / 180));
			coordinates[i + CIRCLESIDES / 4].y = y + (radius * sin((r1 + 90) * M_PI / 180));
			coordinates[i + CIRCLESIDES / 2].y = y + (radius * sin((r1 + 180) * M_PI / 180));
			coordinates[i + CIRCLESIDES / 4 * 3].y = y + (radius * sin((r1 + 270) * M_PI / 180));

			r1 = 360 * i / CIRCLESIDES;
		}
	};

	void resize(float rad) { radius = rad; };
	void changeweight(float w) { weight = w; };
	void changespeed(float X, float Y, float R) { speedx += X; speedy += Y; speedr += R; };
	void move(float X, float Y) { x += X; y += Y;};
	void update() {
		x += speedx;
		y += speedy;
		create();
	};
};

//tasti su schermo
class UI {
public:
	SDL_Rect select[OPTIONS];
	bool selected[OPTIONS];

	void start(int windowx, int windowy){
		for (int i = 0; i < OPTIONS; i++) {
			select[i].h = select[i].w = UISIZE;
			select[i].y = UISIZE*0.5;
			if (i == 0) {
				select[i].x = windowx - UISIZE*1.5;
			}
			else
				select[i].x = select[i - 1].x - UISIZE*1.3;
		}
	};
	void selection(int j){
		if (selected[j]) {
			selected[j] = false;
			return;
		}
		for (int i = 0; i < OPTIONS; i++) 
			selected[i] = false;
		if (j >= 0 && j < OPTIONS)
			selected[j] = true;
	};
};

class UIshapes {
public:
	SDL_Rect select[OPTIONS2];
	bool selected[OPTIONS2];

	void start(int windowx, int windowy) {
		for (int i = 0; i < OPTIONS2; i++) {
			select[i].h = select[i].w = UISIZE;
			select[i].y = UISIZE * 1.8;
			if (i == 0) {
				select[i].x = windowx - UISIZE * 1.5;
			}
			else
				select[i].x = select[i - 1].x - UISIZE * 1.3;
		}
	};
	void selection(int j) {
		if (selected[j]) {
			selected[j] = false;
			return;
		}
		for (int i = 0; i < OPTIONS2; i++)
			selected[i] = false;
		if (j >= 0 && j < OPTIONS2)
			selected[j] = true;
	};
};

class Slider
{
public:
	SDL_Rect sl, bar;
	int vMin, vMax;

	bool selected;
	int s;
	
	void start() {
		sl.h = UISIZE;
		sl.w = UISIZE / 2;
		bar.h = UISIZE / 4;
		bar.w = UISIZE * 4.9;
	};
};

class Game
{
public:

	//funzionamento
	bool Running;
	int windowx, windowy;

	//rendering
	SDL_Window* Window;
	SDL_Renderer* Renderer;

	//cose
	vector <Square> squares;
	vector <Circle> circles;
	SDL_Rect Floor;
	UI ui;
	UIshapes uishapes;
	vector <Slider> sliders;

	//input
	bool lclick, rclick;
	int mousex, mousey, mousey1;

	Game() {}
	~Game() {}


	bool init(const char* title, int xpos, int ypos, int width, int height, int flags)
	{
		//prepara SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
		{
			Window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
			if (Window != 0)
			{
				Renderer = SDL_CreateRenderer(Window, -1, 0);
				if (Renderer != 0)
				{
					SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
					SDL_GetRendererOutputSize(Renderer, &windowx, &windowy);
				}
				else
				{
					return false;
				}
			}
			else
			{
				std::cout << "window init fail\n";
				return false;
			}
		}
		else
		{
			std::cout << "SDL init fail\n";
			return false;
		}
		if(TTF_Init() < 0){
			std::cout << "TTF init fail\n";
			return false;
		}
		std::cout << "init success\n";
		Running = true;
		init2();
		return true;
	};
	void init2() {
		//prepara il resto
		ui.start(windowx, windowy);
		uishapes.start(windowx, windowy);
		startsliders();
		Floor.h = 20;
		Floor.w = windowx;
		Floor.x = 0;
		Floor.y = windowy - Floor.h;
	};

	void startsliders() {
		Slider t;
		for (int i = 0; i < 2; i++) {
			sliders.push_back(t);
			sliders[i].start();
		}
		for (int i = 0; i < sliders.size(); i++) {
			sliders[i].bar.y = sliders[i].sl.y = UISIZE * 4 + UISIZE * 2.2 * i;
			sliders[i].bar.x = sliders[i].sl.x = windowx - UISIZE * 0.5 - sliders[i].bar.w;
		}
	};

	void render() {
		//ripulisce lo schermo e disegna
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);
		rendershapes();
		renderui();
		SDL_RenderPresent(Renderer);
	};

	void renderui() {//devo riscrivere questa funzione per non far cambiar colore al renderer più di una volta       (dividere in renderuiselected() e renderuiunselected())
		renderuiunselected();
		renderuiselected();
	};

	void renderuiunselected() {
		for (int i = 0; i < OPTIONS; i++)
			if (!ui.selected[i])
				SDL_RenderFillRect(Renderer, &ui.select[i]);
		if (ui.selected[1])
			for (int i = 0; i < OPTIONS2; i++)
				if (!uishapes.selected[i])
					SDL_RenderFillRect(Renderer, &uishapes.select[i]);
		if (ui.selected[0] || ui.selected[1])
			for (int i = 0; i < sliders.size(); i++) {
				SDL_RenderFillRect(Renderer, &sliders[i].bar);
				if(!sliders[i].selected)
					SDL_RenderFillRect(Renderer, &sliders[i].sl);
			}
	};

	void renderuiselected() {
		SDL_SetRenderDrawColor(Renderer, 150, 150, 150, 255);
		for (int i = 0; i < OPTIONS; i++)
			if (ui.selected[i])
				SDL_RenderFillRect(Renderer, &ui.select[i]);
		if (ui.selected[1])
			for (int i = 0; i < OPTIONS2; i++)
				if (uishapes.selected[i])
					SDL_RenderFillRect(Renderer, &uishapes.select[i]);
		if (ui.selected[0] || ui.selected[1])
			for (int i = 0; i < sliders.size(); i++)
				if (sliders[i].selected)
					SDL_RenderFillRect(Renderer, &sliders[i].sl);
	};

	void rendershapes() {
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(Renderer, &Floor);
		rendersquares();
		//rendersquaresdebug();
		rendercircles();
	};
	
	void rendersquares() {
		for (int i = 0; i < squares.size(); i++)
			for (int j = 0; j < 4; j++) 
				SDL_RenderDrawLineF(Renderer, squares[i].coordinates[j].x, squares[i].coordinates[j].y, squares[i].coordinates[(j + 1) % 4].x, squares[i].coordinates[(j + 1) % 4].y);
	};
	void rendersquaresdebug() {
		for (int i = 0; i < squares.size(); i++) {
			SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
			SDL_RenderDrawLineF(Renderer, squares[i].coordinates[0].x, squares[i].coordinates[0].y, squares[i].coordinates[1].x, squares[i].coordinates[1].y);
			SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
			SDL_RenderDrawLineF(Renderer, squares[i].coordinates[1].x, squares[i].coordinates[1].y, squares[i].coordinates[2].x, squares[i].coordinates[2].y);
			SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 255);
			SDL_RenderDrawLineF(Renderer, squares[i].coordinates[2].x, squares[i].coordinates[2].y, squares[i].coordinates[3].x, squares[i].coordinates[3].y);
			SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
			SDL_RenderDrawLineF(Renderer, squares[i].coordinates[3].x, squares[i].coordinates[3].y, squares[i].coordinates[0].x, squares[i].coordinates[0].y);
			squares[i].move(0, 0, 0.1);
		}

	}

	void rendercircles() {
		for (int i = 0; i < circles.size(); i++)
			for (int j = 0; j < CIRCLESIDES; j++)
				SDL_RenderDrawLineF(Renderer, circles[i].coordinates[j].x, circles[i].coordinates[j].y, circles[i].coordinates[(j + 1) % CIRCLESIDES].x, circles[i].coordinates[(j + 1) % CIRCLESIDES].y);
	};

	void update() {
		getMouseState();
		gravity();
		checkBorders();
		updateshapes();
		clearcheck();
		sliderupdate();
	};

	void getMouseState() {
		SDL_GetMouseState(&mousex, &mousey);
		mousey1 = mousey - windowy;
	};

	void gravity() {
		for (int i = 0; i < squares.size(); i++) {
			if (squares[i].coordinates[0].y < Floor.y - 2 || squares[i].speedy <= 0)//questo if() serve a non far rimbalzare oggetti che sono fermi a terra
				squares[i].changespeed(0, GRAVITY, 0);
		}
		for (int i = 0; i < circles.size(); i++) {
			if (circles[i].coordinates[CIRCLESIDES / 4].y < Floor.y - 2 || circles[i].speedy <= 0)
				circles[i].changespeed(0, GRAVITY, 0);
		}

	};

	void checkBorders() {
		for (int i = 0; i < squares.size(); i++) {//l'angolo [0] è sempre il più basso
			if (squares[i].coordinates[0].y > Floor.y - 1 && squares[i].speedy >= 0) {
				squares[i].move(0, (Floor.y - 1) - squares[i].coordinates[0].y, 0);
				squares[i].changespeed(0, -squares[i].speedy * FLOOR * 2, 0);
			}
		}
		for (int i = 0; i < circles.size(); i++) {//(CIRCLESIDES / 4) è sempre il punto più in basso del cerchio
			if (circles[i].coordinates[CIRCLESIDES / 4].y > Floor.y - 1 && circles[i].speedy >= 0) {
				circles[i].move(0, (Floor.y - 1) - circles[i].coordinates[CIRCLESIDES / 4].y);
				circles[i].changespeed(0, -circles[i].speedy * FLOOR * 2, 0);
			}
		}

	};

	void updateshapes() {
		updatesquares();
		updatecircles();
	};

	void updatesquares() {
		for (int i = 0; i < squares.size(); i++)
			squares[i].update();
	};

	void updatecircles() {
		for (int i = 0; i < circles.size(); i++)
			circles[i].update();
	};

	void clearcheck() {
		if (ui.selected[3]) {
			squares.clear();
			circles.clear();
			ui.selection(-1);
			uishapes.selection(-1);
		}
	};

	void sliderupdate() {
		for (int i = 0; i < sliders.size(); i++) {
			if (lclick && sliders[i].selected) {
				if (mousex + sliders[i].s >= sliders[i].bar.x - sliders[i].sl.w / 2 && mousex + sliders[i].s <= sliders[i].bar.x + sliders[i].bar.w - sliders[i].sl.w / 2)
					sliders[i].sl.x = mousex + sliders[i].s;
				else if (mousex + sliders[i].s < sliders[i].bar.x)
					sliders[i].sl.x = sliders[i].bar.x - sliders[i].sl.w / 2;
				else
					sliders[i].sl.x = sliders[i].bar.x + sliders[i].bar.w - sliders[i].sl.w / 2;
			}
			else
				sliders[i].selected = false;
		}
	};
	
	void handleEvents() {
		//registra i tasti cliccati
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					Running = false;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					lclick = true;
					LClickChecks();
					break;
				case SDL_BUTTON_RIGHT:
					rclick = true;
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					lclick = false;
					break;
				case SDL_BUTTON_RIGHT:
					rclick = false;
					break;
				}
				break;
			case SDL_QUIT:
				Running = false;
				break;
			default:
				break;
			}
		}
	};

	void LClickChecks() {
		if (sel() && slidercheck() && ui.selected[1])
			newshape();
		else if (ui.selected[0]) {
			selectshape();
		}
		else if (ui.selected[0] || ui.selected[1]) {
			slidercheck();
		}
	};
	bool sel() {
		if (mousey >= ui.select[0].y && mousey <= ui.select[0].y + UISIZE)
			for (int i = 0; i < OPTIONS; i++)
				if (mousex >= ui.select[i].x && mousex <= ui.select[i].x + UISIZE) {
					ui.selection(i);
					return false;
				}
		if (ui.selected[1]) {
			if (mousey >= uishapes.select[0].y && mousey <= uishapes.select[0].y + UISIZE)
				for (int i = 0; i < OPTIONS2; i++)
					if (mousex >= uishapes.select[i].x && mousex <= uishapes.select[i].x + UISIZE) {
						uishapes.selection(i);
						return false;
					}
		}
		return true;
	};

	void newshape() {
		if (uishapes.selected[0]) {
			Square t;
			t.S(mousex, mousey, 300);//va cambiato
			squares.push_back(t);
		}
		else if (uishapes.selected[1]) {
			Circle t;
			t.C(mousex, mousey, 100);//va cambiato
			circles.push_back(t);
		}
	};

	void selectshape() {
		int t;
		for (int i = 0; i < squares.size(); i++) {
			t = 0;
			if (squares[i].r == 0 && abs(mousex - squares[i].x) < squares[i].lato / 2 && abs(mousey - squares[i].y) < squares[i].lato / 2) {
				t = 2;
			}
			else {
				if (mousey <= squares[i].coordinates[1].y) {
					if (mousex >= squares[i].coordinates[1].x + (abs(mousey - squares[i].coordinates[1].y) * cotan(squares[i].r * M_PI / 180)))
						t++;
				}
				else if (mousey >= squares[i].coordinates[1].y) {
					if (mousex >= squares[i].coordinates[1].x + (abs(mousey - squares[i].coordinates[1].y) * tan(squares[i].r * M_PI / 180)))
						t++;
				}
				if (mousey <= squares[i].coordinates[3].y) {
					if (mousex <= squares[i].coordinates[3].x - (abs(mousey - squares[i].coordinates[3].y) * tan(squares[i].r * M_PI / 180)))
						t++;
				}
				else if (mousey >= squares[i].coordinates[3].y) {
					if (mousex <= squares[i].coordinates[3].x - (abs(mousey - squares[i].coordinates[3].y) * cotan(squares[i].r * M_PI / 180)))
						t++;
				}
			}
			if (t == 2) {
				ui.selected[2] = true;
				for (int j = 0; j < squares.size(); j++) {
					squares[j].selected = false;
				}
				for (int j = 0; j < circles.size(); j++) {
					circles[j].selected = false;
				}
				squares[i].selected = true;
				return;
			}
			else { ui.selected[2] = false; }
		}
		for (int i = 0; i < circles.size(); i++) {
			if (sqrt(pow(mousex - circles[i].x, 2) + pow(mousey - circles[i].y, 2)) < circles[i].radius) {
				for (int j = 0; j < squares.size(); j++) {
					squares[j].selected = false;
				}
				for (int j = 0; j < circles.size(); j++) {
					circles[j].selected = false;
				}
				circles[i].selected = true;
				return;
			}
		}
	};

	bool slidercheck() {
		for (int i = 0; i < sliders.size(); i++) {
			if (mousex >= sliders[i].sl.x && mousex <= sliders[i].sl.x + sliders[i].sl.w && mousey >= sliders[i].sl.y && mousey <= sliders[i].sl.y + sliders[i].sl.h) {
				sliders[i].selected = true;
				sliders[i].s = sliders[i].sl.x - mousex;
				return false;
			}
		}
		return true;
	}

	void clean() {
		//chiude tutto
		std::cout << "cleaning\n";
		SDL_DestroyWindow(Window);
		SDL_DestroyRenderer(Renderer);
		TTF_Quit();
		SDL_Quit();
	};

	bool running() { return Running; }

};