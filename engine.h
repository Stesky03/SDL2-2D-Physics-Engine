#include "SDL.h"
#include "SDL_ttf.h"
#include <iostream> 
#include <math.h>
#include <vector>
#include <string>
using std::vector;
using std::string;

#define PROPORTION 60
#define GRAVITY (9.81 * PROPORTION / 60)
#define SQ2 1.4142
#define CIRCLESIDES 64 //64
#define OPTIONS 3
#define OPTIONS2 2
#define INFOS 4
#define FLOOR 0.80 //0.80
#define white {255,255,255}
#define CIFRE 4
#define MOUSEFRAMES 10
#define COLLISIONCOOLDOWN 3

#define MINDIMENSION 6.0
#define MAXDIMENSION 600.0
#define MINWEIGHT 1.0 //0.1
#define MAXWEIGHT 200.0 //20

//l'asse y � invertito di default e di conseguenza la rotazione � in senso orario
//la gpu viene utilizzata solo per le texture

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
	float weight, speedx, speedy, speedr, x, y, y1, r, lato, K, Xprec, Yprec, speedxF, speedyF;
	bool selected, drag;
	Coordinatesclass coordinates[4];
	void S(float X, float Y, float s, float w) {
		x = X;
		y = Y;
		lato = s;
		weight = w;
		create();

		speedx = 0;
		speedy = 0;
		speedr = 0;
		Xprec = Yprec = 0;
		r = 0;
		selected = drag = false;
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
		x += speedx/60;
		y += speedy/60;
		r += speedr/60;
		speedxF = (x - Xprec);
		speedyF = (Yprec - y);
		Xprec = x;
		Yprec = y;
		while (r >= 90)
			r -= 90;
		calcK();
		create();
	};
	void calcK() {
		K = weight * (pow(speedyF, 2) + pow(speedxF, 2)) / 2;
	};
};

//cerchi
class Circle{
public:
	float radius, weight, speedx, speedy, speedr, x, y, y1, K, Xprec, Yprec, speedxF, speedyF, collisionframe;
	bool selected, drag;
	Coordinatesclass coordinates[CIRCLESIDES];
	void C(float X, float Y, float rad, float w) {
		x = X;
		y = Y;
		radius = rad;
		weight = w;
		create();

		speedx = 0;
		speedy = 0;
		speedr = 0;
		Xprec = Yprec = 0;
		collisionframe = 0;
		selected = drag = false;
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
		x += speedx/60;
		y += speedy/60;
		speedxF = (x - Xprec);
		speedyF = (Yprec - y);
		Xprec = x;
		Yprec = y;
		calcK();
		create();
	};
	void calcK() {
		K = weight * (pow(speedyF,2) + pow(speedxF, 2)) / 2;
	};
};

//tasti su schermo
class UI {
public:
	SDL_Rect select[OPTIONS];
	bool selected[OPTIONS];

	void start(int windowx, int windowy, int UISIZE){
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

	void start(int windowx, int windowy, int UISIZE) {
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
	SDL_Rect sl, bar, number;
	float vMin, vMax;

	bool selected;
	float s, value;

	SDL_Texture* numberT;
	
	void start(int UISIZE) {
		sl.h = UISIZE;
		sl.w = UISIZE / 2;
		bar.h = UISIZE / 4;
		bar.w = UISIZE * 4.9;
		number.h = UISIZE / 2;
		number.w = UISIZE / 2;
	};
};

class Info {
public:
	SDL_Rect field[2];
	SDL_Texture* Texture[2];
	char content[30];
	bool modified;


	void I(int w, int h, string c1) {
		field[0].h = field[1].h = h;
		field[0].w = field[1].w = w;
		for (int i = 0; i < c1.size(); i++) {
			content[i] = c1[i];
		}
		content[c1.size()] = ':';
		content[c1.size() + 1] = '\0';
	};
};

class Game
{
public:

	//funzionamento
	bool Running;
	int windowx, windowy, frame;
	int UISIZE, textwidthX;

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
	vector <Info> infofields;

	SDL_Surface* tempsurface;
	SDL_Texture* slidertriangle[2];
	SDL_Texture* ui1[OPTIONS];
	SDL_Texture* ui2[OPTIONS2];
	TTF_Font* gFont;
	SDL_Surface* textSurface;
	string tempstring;
	char tempcha[20];
	string infotags[INFOS]{"mass", "speed - x", "speed - y", "kinetic energy"};

	//input
	bool lclick, rclick;
	int mousex, mousey;
	int mousexdata[MOUSEFRAMES];
	int mouseydata[MOUSEFRAMES];

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
		UISIZE = windowx / 38;
		gFont = TTF_OpenFont("assets/FreeSans.ttf", 28);
		ui.start(windowx, windowy, UISIZE);
		uishapes.start(windowx, windowy, UISIZE);
		startsliders();
		startinfo();
		uitextures();
		Floor.h = 20;
		Floor.w = windowx;
		Floor.x = 0;
		Floor.y = windowy - Floor.h;
		frame = 0;
		for (int i = 0; i < MOUSEFRAMES; i++) {
			mousexdata[i] = 0;
			mouseydata[i] = 0;
		}
	};

	void startsliders() {
		Slider t;

		for (int i = 0; i < 2; i++) {
			sliders.push_back(t);
			sliders[i].start(UISIZE);
		}
		sliders[0].vMin = MINDIMENSION;
		sliders[0].vMax = MAXDIMENSION;
		sliders[1].vMin = MINWEIGHT;
		sliders[1].vMax = MAXWEIGHT;
		for (int i = 0; i < sliders.size(); i++) {
			sliders[i].value = sliders[i].vMin;
			sliders[i].bar.y = sliders[i].sl.y = UISIZE * 4 + UISIZE * 2.2 * i;
			sliders[i].number.y = sliders[i].sl.y + sliders[i].sl.h;
			sliders[i].bar.x = windowx - UISIZE * 0.5 - sliders[i].bar.w;
			sliders[i].sl.x = sliders[i].bar.x - sliders[i].sl.w / 2;
			sliders[i].number.x = sliders[i].sl.x;
			if (i == 1)
				tempstring = std::to_string(float(sliders[i].value) / 10);
			else
				tempstring = std::to_string(sliders[i].value/PROPORTION);
			for (int j = 0; j < 10; j++) {
				tempcha[j] = '\0';
			}
			for (int j = 0; j < tempstring.size(); j++)
				tempcha[j] = tempstring[j];
			for (int j = 0; j < 3; j++) {
				if (tempcha[j] == '.')
					tempcha[j + 2] = '\0';
			}

			textSurface = TTF_RenderText_Solid(gFont, tempcha, white);
			sliders[i].numberT = SDL_CreateTextureFromSurface(Renderer, textSurface);
		}
		tempsurface = SDL_LoadBMP("assets/tri1.bmp");
		slidertriangle[0] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/tri2.bmp");
		slidertriangle[1] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
	};

	void startinfo() {
		Info t;
		for (int i = 0; i < INFOS; i++) {
			infofields.push_back(t);
			infofields[i].I(UISIZE * 2, UISIZE / 2, infotags[i]);
			infofields[i].field[0].x = infofields[i].field[1].x = sliders[0].bar.x;
			infofields[i].field[0].y = UISIZE * 8.4 + UISIZE * 3 * i;
			infofields[i].field[1].y = infofields[i].field[0].y + infofields[i].field[0].h;

			textSurface = TTF_RenderText_Solid(gFont, infofields[i].content, white);
			infofields[i].Texture[0] = SDL_CreateTextureFromSurface(Renderer, textSurface);
			SDL_QueryTexture(infofields[i].Texture[0], NULL, NULL, &textwidthX, NULL);
			infofields[i].field[0].w = textwidthX;
			textSurface = TTF_RenderText_Solid(gFont, "0\0", white);
			infofields[i].Texture[1] = SDL_CreateTextureFromSurface(Renderer, textSurface);
		}
	};

	void uitextures() {
		tempsurface = SDL_LoadBMP("assets/ico1.bmp");
		ui1[0] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/ico2.bmp");
		ui1[1] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/ico4.bmp");
		ui1[2] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/square.bmp");
		ui2[0] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/circle.bmp");
		ui2[1] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		SDL_FreeSurface(tempsurface);
	};

	void render() {
		//ripulisce lo schermo e disegna
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);
		rendershapes();
		renderui();
		SDL_RenderPresent(Renderer);
	};

	void renderui() {//far cambiare colore al renderer richiede un'enormit� di tempo, perci� il rendering � diviso per colore
		renderuiunselected();
		renderuiselected();
		rendertextures();
	};

	void renderuiunselected() {
		SDL_SetRenderDrawColor(Renderer, 140, 140, 140, 255);
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
			}
	};

	void renderuiselected() {
		SDL_SetRenderDrawColor(Renderer, 80, 80, 80, 255);
		for (int i = 0; i < OPTIONS; i++)
			if (ui.selected[i])
				SDL_RenderFillRect(Renderer, &ui.select[i]);
		if (ui.selected[1])
			for (int i = 0; i < OPTIONS2; i++)
				if (uishapes.selected[i])
					SDL_RenderFillRect(Renderer, &uishapes.select[i]);
	};

	void rendertextures() {
		for (int i = 0; i < OPTIONS; i++)
			SDL_RenderCopy(Renderer, ui1[i], NULL, &ui.select[i]);
		for (int i = 0; i < OPTIONS2; i++)
			SDL_RenderCopy(Renderer, ui2[i], NULL, &uishapes.select[i]);
		if (ui.selected[0] || ui.selected[1])
			for (int i = 0; i < sliders.size(); i++) {
				if(sliders[i].selected)
					SDL_RenderCopy(Renderer, slidertriangle[1], NULL, &sliders[i].sl);
				else
					SDL_RenderCopy(Renderer, slidertriangle[0], NULL, &sliders[i].sl);
				SDL_RenderCopy(Renderer, sliders[i].numberT, NULL, &sliders[i].number);
			}
		if (ui.selected[0]) {
			for (int i = 0; i < INFOS; i++) {
				SDL_QueryTexture(infofields[i].Texture[1], NULL, NULL, &textwidthX, NULL);
					infofields[i].field[1].w = textwidthX;
				SDL_RenderCopy(Renderer, infofields[i].Texture[0], NULL, &infofields[i].field[0]);
				SDL_RenderCopy(Renderer, infofields[i].Texture[1], NULL, &infofields[i].field[1]);
			}
		}
	};

	void rendershapes() {
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(Renderer, &Floor);
		rendersquares();
		rendercircles();
	};
	
	void rendersquares() {
		for (int i = 0; i < squares.size(); i++)
			for (int j = 0; j < 4; j++) 
				SDL_RenderDrawLineF(Renderer, squares[i].coordinates[j].x, squares[i].coordinates[j].y, squares[i].coordinates[(j + 1) % 4].x, squares[i].coordinates[(j + 1) % 4].y);
	};

	void rendercircles() {
		for (int i = 0; i < circles.size(); i++)
			for (int j = 0; j < CIRCLESIDES; j++)
				SDL_RenderDrawLineF(Renderer, circles[i].coordinates[j].x, circles[i].coordinates[j].y, circles[i].coordinates[(j + 1) % CIRCLESIDES].x, circles[i].coordinates[(j + 1) % CIRCLESIDES].y);
	};

	void update() {
		getMouseState();
		gravity();
		dragshapes();
		checkcollisions();
		checkBorders();
		updateshapes();
		clearcheck();
		sliderupdate();
		info();
		frame++;
	};

	void getMouseState() {
		int t;
		for (int i = MOUSEFRAMES - 1; i > 0; i--) {
			mousexdata[i] = mousexdata[i - 1];
			mouseydata[i] = mouseydata[i - 1];
		}
		mousexdata[0] = mousex;
		mouseydata[0] = mousey;
		SDL_GetMouseState(&mousex, &mousey);
	};

	void gravity() {
		for (int i = 0; i < squares.size(); i++) {
			if (!(squares[i].coordinates[0].y > Floor.y - 2 && squares[i].speedy >= 0 && squares[i].speedy < 3))//questo if() serve a non far rimbalzare oggetti che sono fermi a terra
				squares[i].changespeed(0, GRAVITY, 0);
			else
				squares[i].speedy = 0;
		}
		for (int i = 0; i < circles.size(); i++) {
			if (!(circles[i].coordinates[CIRCLESIDES / 4].y > Floor.y - 2 && circles[i].speedy >= 0 && circles[i].speedy < 3 || circles[i].drag)) {
				circles[i].changespeed(0, GRAVITY, 0);
			}
			else {
				circles[i].speedy = 0;
			}
		}

	};

	void checkBorders() {
		for (int i = 0; i < squares.size(); i++) {//l'angolo [0] � sempre il pi� basso
			if (squares[i].coordinates[0].y > Floor.y - 1 && squares[i].speedy >= 0) {
				squares[i].move(0, (Floor.y - 1) - squares[i].coordinates[0].y, 0);
				squares[i].changespeed(0, -squares[i].speedy * FLOOR * 2, 0);
			}
		}
		for (int i = 0; i < circles.size(); i++) {//(CIRCLESIDES / 4) � sempre il punto pi� in basso del cerchio
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
		if (ui.selected[2]) {
			squares.clear();
			circles.clear();
			ui.selection(-1);
			uishapes.selection(-1);
		}
	};

	void sliderupdate() {
		for (int i = 0; i < sliders.size(); i++) {
			if (lclick && sliders[i].selected) {
				if (mousex + sliders[i].s >= sliders[i].bar.x - sliders[i].sl.w / 2 && mousex + sliders[i].s <= sliders[i].bar.x + sliders[i].bar.w - sliders[i].sl.w / 2) {
					sliders[i].sl.x = mousex + sliders[i].s;
				}
				else if (mousex + sliders[i].s < sliders[i].bar.x)
					sliders[i].sl.x = sliders[i].bar.x - sliders[i].sl.w / 2;
				else
					sliders[i].sl.x = sliders[i].bar.x + sliders[i].bar.w - sliders[i].sl.w / 2;
				
				sliders[i].value = (sliders[i].sl.x - (sliders[i].bar.x - sliders[i].sl.w / 2)) * ((sliders[i].vMax - sliders[i].vMin) / sliders[i].bar.w) + sliders[i].vMin;
				sliders[i].number.x = sliders[i].sl.x;
				if(i==1)
					tempstring = std::to_string(float(int(sliders[i].value))/10);
				else
					tempstring = std::to_string(sliders[i].value/PROPORTION);
				for (int j = 0; j < 10; j++) {
					tempcha[j] = '\0';
				}
				for (int j = 0; j < tempstring.size(); j++)
					tempcha[j] = tempstring[j];
				for (int j = 0; j < 3; j++) {
					if (tempcha[j] == '.')
						tempcha[j + 2] = '\0';
				}

				textSurface = TTF_RenderText_Solid(gFont, tempcha, white);
				sliders[i].numberT = SDL_CreateTextureFromSurface(Renderer, textSurface);
			}
			else
				sliders[i].selected = false;
		}
	};

	void info() {
		if (ui.selected[0]) {
			for (int i = 0; i < squares.size(); i++) {
				if (squares[i].selected) {
					if (sliders[0].selected) {
						squares[i].changeweight(squares[i].weight / pow(squares[i].lato/PROPORTION, 3) * pow(sliders[0].value/PROPORTION, 3));
						squares[i].resize(sliders[0].value);
					}
					else if (sliders[1].selected) {
						squares[i].changeweight(sliders[1].value / 10 * pow(squares[i].lato / PROPORTION, 3));
					}
					for(int j=0;j<4;j++)
						produceinfotext(i, j, true);
				}
			}
			for (int i = 0; i < circles.size(); i++) {
				if (circles[i].selected) {
					if (sliders[0].selected) {
						circles[i].changeweight(circles[i].weight / pow(circles[i].radius/PROPORTION, 3) * pow(sliders[0].value/2/PROPORTION, 3));
						circles[i].resize(sliders[0].value / 2);
					}
					else if (sliders[1].selected) {
						circles[i].changeweight(sliders[1].value / 10 * pow(circles[i].radius / PROPORTION, 3) * 4 / 3 * M_PI);
					}
					for (int j = 0; j < 4; j++)
						produceinfotext(i, j, false);
				}
			}
		}
	};

	void produceinfotext(int i, int j, bool rect) {
		if (j == 0) {
			if (rect)
				tempstring = std::to_string(squares[i].weight);
			else
				tempstring = std::to_string(circles[i].weight);
			tempstring += " kg";
		}
		if (j == 1) {
			if (rect)
				tempstring = std::to_string(squares[i].speedxF);
			else
				tempstring = std::to_string(circles[i].speedxF);
			tempstring += " m/s";
		}
		if (j == 2) {
			if (rect)
				tempstring = std::to_string(squares[i].speedyF);
			else
				tempstring = std::to_string(circles[i].speedyF);
			tempstring += " m/s";
		}
		if (j == 3) {
			if (rect)
				tempstring = std::to_string(squares[i].K);
			else
				tempstring = std::to_string(circles[i].K);
			tempstring += " J";
		}
		for (int j = 0; j < tempstring.size(); j++)
			tempcha[j] = tempstring[j];
		for (int j = 0; j < tempstring.size(); j++)
			if (tempcha[j] == '.')
				tempcha[j + CIFRE + 1] = '\0';
		textSurface = TTF_RenderText_Solid(gFont, tempcha, white);
		infofields[j].Texture[1] = SDL_CreateTextureFromSurface(Renderer, textSurface);
	};

	void dragshapes() {
		int x, y;
		//squares
		for (int i = 0; i < circles.size(); i++) {
			if (!circles[i].drag)
				continue;
			if (!lclick) {
				x = 0;
				y = 0;
				for (int i = 0; i < MOUSEFRAMES; i++) {
					x += mousexdata[i];
					y += mouseydata[i];
				}
				x /= MOUSEFRAMES;
				y /= MOUSEFRAMES;
				circles[i].changespeed((mousex - x)*4, (mousey - y)*4, 0);
				circles[i].drag = false;
			}
			else {
				circles[i].speedx = circles[i].speedy = 0;
				circles[i].move(mousex - mousexdata[0], mousey - mouseydata[0]);
			}

		}
	};

	void checkcollisions() {
		checkCC();
	};

	void checkCC() {
		float x1, x2;
		float y1, y2;
		float v1x, v2x;
		float v1y, v2y;
		float m1, m2;
		float d, dx, dy;
		float ux, uy;
		float v1c, v2c;
		float v1p, v2p;
		float v1c1, v2c1;
		float overlap;

		for (int i = 0; i < circles.size(); i++) {
			for (int j = i + 1; j < circles.size(); j++) {
				if (circles[i].collisionframe + circles[j].collisionframe + 2 * COLLISIONCOOLDOWN < 2 * frame){
						x1 = circles[i].x;
						y1 = circles[i].y;
						x2 = circles[j].x;
						y2 = circles[j].y;

					if (sqrt(pow(circles[i].y - circles[j].y, 2) + pow(circles[i].x - circles[j].x, 2)) < circles[i].radius + circles[j].radius) {
						
						v1x = circles[i].speedx;
						v1y = circles[i].speedy;
						m1 = circles[i].weight;

						v2x = circles[j].speedx;
						v2y = circles[j].speedy;
						m2 = circles[j].weight;

						// distanza tra i cerchi
						dx = x2 - x1;
						dy = y2 - y1;
						d = sqrt(dx * dx + dy * dy);

						// unit vector
						ux = dx / d;
						uy = dy / d;

						// componenti della velocit� lungo l'asse di collisione
						v1c = v1x * ux + v1y * uy;
						v2c = v2x * ux + v2y * uy;

						// componenti della velocit� perpendicolari all'asse di collisione
						v1p = v1x * uy - v1y * ux;
						v2p = v2x * uy - v2y * ux;

						// conservazione della quantit� di moto lungo l'asse di collisione
						v1c1 = ((m1 - m2) * v1c + 2 * m2 * v2c) / (m1 + m2);
						v2c1 = ((m2 - m1) * v2c + 2 * m1 * v1c) / (m1 + m2);

						circles[i].speedx = v1c1 * ux + v1p * uy;
						circles[i].speedy = v1c1 * uy - v1p * ux;
						circles[j].speedx = v2c1 * ux + v2p * uy;
						circles[j].speedy = v2c1 * uy - v2p * ux;

						circles[i].collisionframe = circles[j].collisionframe = frame;

					}
					overlap = circles[i].radius + circles[j].radius - sqrt(pow(circles[i].y - circles[j].y, 2) + pow(circles[i].x - circles[j].x, 2));
					if (overlap > 0) {
						float adjustx = overlap / 2 * (x1 - x2) / sqrt(pow(circles[i].y - circles[j].y, 2) + pow(circles[i].x - circles[j].x, 2));
						float adjusty = overlap / 2 * (y1 - y2) / sqrt(pow(circles[i].y - circles[j].y, 2) + pow(circles[i].x - circles[j].x, 2));
						circles[i].x += adjustx;
						circles[i].y += adjusty;
						circles[j].x -= adjustx;
						circles[j].y -= adjusty;
					}
				}
			}
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
		else {
			if (ui.selected[0] || ui.selected[1]) {
				slidercheck();
			}
			if (!ui.selected[1]) {
				selectshape();
			}
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
			t.S(mousex, mousey, sliders[0].value, sliders[1].value / 10 * pow(sliders[0].value / PROPORTION, 3));
			squares.push_back(t);
		}
		else if (uishapes.selected[1]) {
			Circle t;	
			t.C(mousex, mousey, sliders[0].value / 2, sliders[1].value / 10 * pow((sliders[0].value/2) / PROPORTION, 3) * 4 / 3 * M_PI);
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
				for (int j = 0; j < squares.size(); j++) {
					squares[j].selected = false;
				}
				for (int j = 0; j < circles.size(); j++) {
					circles[j].selected = false;
				}
				squares[i].selected = true;
				squares[i].drag = true;
				return;
			}
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
				circles[i].drag = true;
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