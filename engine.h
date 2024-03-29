#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
#define OPTIONS 6
#define OPTIONS2 2
#define INFOS 4
#define RESTITUTION 0.80 //0.80
#define white {255,255,255}
#define CIFRE 4
#define MOUSEFRAMES 10
#define COLLISIONCOOLDOWN 3
#define AIRRESISTANCE 1.2

#define MINDIMENSION 6.0
#define MAXDIMENSION 600.0
#define MINMASS 1.0 //0.1
#define MAXMASS 200.0 //20

//l'asse y e' invertito di default e di conseguenza la rotazione e' in senso orario

double cotan(double x) {
	return (cos(x) / sin(x));
}

class Coordinatesclass{
public:
	float x, y;
};

//cerchi
class Circle{
public:
	float radius, mass, speedx, speedy, speedr, x, y, y1, K, Xprec, Yprec, speedxF, speedyF, collisionframe;
	bool selected, drag;
	Coordinatesclass coordinates[CIRCLESIDES];
	void C(float X, float Y, float rad, float w) {
		x = X;
		y = Y;
		radius = rad;
		mass = w;
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
	void changemass(float w) { mass = w; };
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
		K = mass * (pow(speedyF,2) + pow(speedxF, 2)) / 2;
	};
};

//tasti su schermo
class Tasto {
public:
	SDL_Rect button;
	string tag;
	bool exclusive;
	bool selected;
};

class Slider
{
public:
	SDL_Rect sl, bar, number, tag;
	float vMin, vMax;

	bool selected;
	float s, value;

	SDL_Texture* numberT;
	SDL_Texture* tagT;
	
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
	int UISIZE, textwidthX, fontSize;

	//rendering
	SDL_Window* Window;
	SDL_Renderer* Renderer;

	//cose
	vector <Circle> circles;
	SDL_Rect Floor;
	SDL_Rect walls[3];
	vector <Tasto> ui;
	vector <Tasto> uishapes;
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
	string infotags[INFOS]{"Massa", "Velocita' - x", "Velocita' - y", "Energia cinetica"};
	string uitags[OPTIONS]{ "sliders", "new", "gravity", "walls", "air", "trash" };
	string slidertags[2]{ "Diametro","Densita'" };

	//input
	bool lclick, rclick;
	int mousex, mousey;
	int mousexdata[MOUSEFRAMES];
	int mouseydata[MOUSEFRAMES];

	Game() {}
	~Game() {}


	bool init(const char* title, int xpos, int ypos, int width, int height, int flags)//prepara SDL
	{
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

	void init2() {//prepara il resto
		startui();
		frame = 0;
		for (int i = 0; i < MOUSEFRAMES; i++) {
			mousexdata[i] = 0;
			mouseydata[i] = 0;
		}
	};

	void startui() {//inizializza tutti gli elementi statici
		UISIZE = windowx / 38;
		fontSize = UISIZE / 10 * 4;
		gFont = TTF_OpenFont("assets/FreeSans.ttf", fontSize);
		startbuttons();
		startsliders();
		startinfo();
		uitextures();
		startfloorandwalls();
	};

	void startbuttons() {//inizializza i bottoni (in alto a dx)
		Tasto t;
		t.button.x = windowx - UISIZE * 1.5;;
		t.button.y = UISIZE * 0.5;
		t.button.w = t.button.h = UISIZE;
		t.selected = false;
		t.exclusive = true;
		for (int i = 0; i < OPTIONS; i++) {
			if (i > 0)
				t.button.x -= UISIZE * 1.3;
			t.tag = uitags[i];
			if (t.tag == "walls" || t.tag == "gravity" || t.tag == "air")
				t.exclusive = false;
			ui.push_back(t);
		}
		t.button.y = UISIZE * 1.8;
		t.button.x = windowx - UISIZE * 1.5;;
		for (int i = 0; i < OPTIONS; i++) {
			if (i > 0)
				t.button.x -= UISIZE * 1.3;
			uishapes.push_back(t);
		}
	};

	void startsliders() {//inizializza gli sliders per cambiare dimensioni e densità delle figure
		Slider t;

		for (int i = 0; i < 2; i++) {
			sliders.push_back(t);
			sliders[i].start(UISIZE);
		}
		sliders[0].vMin = MINDIMENSION;
		sliders[0].vMax = MAXDIMENSION;
		sliders[1].vMin = MINMASS;
		sliders[1].vMax = MAXMASS;
		for (int i = 0; i < sliders.size(); i++) {
			sliders[i].value = sliders[i].vMin;
			sliders[i].bar.y = sliders[i].sl.y = UISIZE * 3.3 + UISIZE * 2.5 * i;
			sliders[i].number.y = sliders[i].sl.y + sliders[i].sl.h;
			sliders[i].bar.x = windowx - UISIZE * 0.5 - sliders[i].bar.w;
			sliders[i].sl.x = sliders[i].bar.x - sliders[i].sl.w / 2;
			sliders[i].number.x = sliders[i].sl.x;
			sliders[i].tag.x = sliders[i].bar.x + UISIZE / 8;

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

			for (int j = 0; j < tempstring.size(); j++)
				tempcha[j] = slidertags[i][j];
			textSurface = TTF_RenderText_Solid(gFont, tempcha, white);
			sliders[i].tagT = SDL_CreateTextureFromSurface(Renderer, textSurface);
			SDL_QueryTexture(sliders[i].tagT, NULL, NULL, &sliders[i].tag.w, &sliders[i].tag.h);
			sliders[i].tag.y = sliders[i].bar.y - sliders[i].tag.h - UISIZE / 8;
		}
		tempsurface = SDL_LoadBMP("assets/tri1.bmp");
		slidertriangle[0] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/tri2.bmp");
		slidertriangle[1] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
	};

	void startinfo() {//inizializza tutte le scritte
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

	void uitextures() {//inizializza tutte le texture (bottoni e cursori degli sliders)
		tempsurface = SDL_LoadBMP("assets/sliders.bmp");
		ui1[0] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/new.bmp");
		ui1[1] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/gravity.bmp");
		ui1[2] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/walls.bmp");
		ui1[3] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/air.bmp");
		ui1[4] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		tempsurface = SDL_LoadBMP("assets/trash.bmp");
		ui1[5] = SDL_CreateTextureFromSurface(Renderer, tempsurface);
		SDL_FreeSurface(tempsurface);
	};

	void startfloorandwalls() {//inizializza pavimento, muri(walls[0] & walls[2]) e soffitto(walls[1])
		Floor.h = 20;
		Floor.w = windowx;
		Floor.x = 0;
		Floor.y = windowy - Floor.h;

		walls[0].y = walls[1].y = walls[2].y = 0;
		walls[0].x = windowx - 5;
		walls[1].x = walls[2].x = 0;
		walls[0].w = walls[1].h = walls[2].w = 5;
		walls[0].h = walls[2].h = windowy;
		walls[1].w = windowx;
	};

	void render() {//ripulisce lo schermo e disegna il frame corrente
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);
		rendershapes();
		renderui();
		SDL_RenderPresent(Renderer);
	};

	void renderui() {//far cambiare colore al renderer richiede un'enormita' di tempo, percio' il rendering e' diviso per colore
		renderuiunselected();
		renderuiselected();
		rendertextures();
	};

	void renderuiunselected() {//rendering bottoni non cliccati
		SDL_SetRenderDrawColor(Renderer, 150, 150, 150, 255);
		for (int i = 0; i < OPTIONS; i++)
			if (!ui[i].selected)
				SDL_RenderFillRect(Renderer, &ui[i].button);
		if (ui[0].selected || ui[1].selected)
			for (int i = 0; i < sliders.size(); i++) {
				SDL_RenderFillRect(Renderer, &sliders[i].bar);
			}
	};

	void renderuiselected() {//rendering bottoni cliccati
		SDL_SetRenderDrawColor(Renderer, 80, 80, 80, 255);
		for (int i = 0; i < OPTIONS; i++)
			if (ui[i].selected)
				SDL_RenderFillRect(Renderer, &ui[i].button);
	};

	void rendertextures() {//rendering texture
		for (int i = 0; i < OPTIONS; i++)
			SDL_RenderCopy(Renderer, ui1[i], NULL, &ui[i].button);
		if (ui[0].selected || ui[1].selected)
			for (int i = 0; i < sliders.size(); i++) {
				if(sliders[i].selected)
					SDL_RenderCopy(Renderer, slidertriangle[1], NULL, &sliders[i].sl);
				else
					SDL_RenderCopy(Renderer, slidertriangle[0], NULL, &sliders[i].sl);
				SDL_RenderCopy(Renderer, sliders[i].numberT, NULL, &sliders[i].number);
				SDL_RenderCopy(Renderer, sliders[i].tagT, NULL, &sliders[i].tag);
			}
		if (ui[0].selected) {
			for (int i = 0; i < INFOS; i++) {
				SDL_QueryTexture(infofields[i].Texture[1], NULL, NULL, &textwidthX, NULL);
					infofields[i].field[1].w = textwidthX;
				SDL_RenderCopy(Renderer, infofields[i].Texture[0], NULL, &infofields[i].field[0]);
				SDL_RenderCopy(Renderer, infofields[i].Texture[1], NULL, &infofields[i].field[1]);
			}
		}
	};

	void rendershapes() {//rendering di muri e figure
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(Renderer, &Floor);
		renderwalls();
		rendercircles();
	};
	
	void renderwalls() {//rendering di muri
		for (int i = 0; i < OPTIONS; i++) {
			if(ui[i].selected)
				if(ui[i].tag=="walls")
					SDL_RenderFillRects(Renderer, walls, 3);
		}
	};

	void rendercircles() {//rendering di cerchi
		for (int i = 0; i < circles.size(); i++)
			for (int j = 0; j < CIRCLESIDES; j++)
				SDL_RenderDrawLineF(Renderer, circles[i].coordinates[j].x, circles[i].coordinates[j].y, circles[i].coordinates[(j + 1) % CIRCLESIDES].x, circles[i].coordinates[(j + 1) % CIRCLESIDES].y);
	};

	void update() {//da qui viene chiamata tutta la logica
		getMouseState();
		gravity();
		checkBorders();
		updateCircles();
		clearCheck();
		sliderUpdate();
		info();
		dragShapes();
		checkCollisions();
		airResistance();
		frame++;
	};

	void getMouseState() {//prende coordinate x e y del mouse
		int t;
		for (int i = MOUSEFRAMES - 1; i > 0; i--) {
			mousexdata[i] = mousexdata[i - 1];
			mouseydata[i] = mouseydata[i - 1];
		}
		mousexdata[0] = mousex;
		mouseydata[0] = mousey;
		SDL_GetMouseState(&mousex, &mousey);
	};

	void gravity() {//applica la gravità se attiva
		for (int i = 0; i < OPTIONS; i++) {
			if (ui[i].tag == "gravity")
				if (ui[i].selected)
					return;
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

	void checkBorders() {//controllo collisioni figure - muri
		bool t;
		for (int i = 0; i < OPTIONS; i++)
			if (ui[i].selected && ui[i].tag == "walls") {
				t = true;
				break;
			}
			else
				t = false;

		for (int i = 0; i < circles.size(); i++) {//(CIRCLESIDES / 4) è sempre il punto pi� in basso del cerchio
			if (circles[i].coordinates[CIRCLESIDES / 4].y > Floor.y - 1 && circles[i].speedy >= 0) {
				if (circles[i].coordinates[CIRCLESIDES / 4].y > Floor.y + 600) {
					circles.erase(circles.begin() + i);
					break;
				}
				circles[i].move(0, (Floor.y - 1) - circles[i].coordinates[CIRCLESIDES / 4].y);
				circles[i].changespeed(0, -circles[i].speedy * RESTITUTION * 2, 0);
			}
			if (t) {
				if (circles[i].coordinates[CIRCLESIDES * 3 / 4].y < walls[1].h + 1 && circles[i].speedy <= 0) {
					if (circles[i].coordinates[CIRCLESIDES * 3 / 4].y < walls[1].h - 600) {
						circles.erase(circles.begin() + i);
						break;
					}
					circles[i].move(0, (walls[1].h + 1) - circles[i].coordinates[CIRCLESIDES * 3 / 4].y);
					circles[i].changespeed(0, -circles[i].speedy * RESTITUTION * 2, 0);
				}
				if (circles[i].coordinates[0].x > walls[0].x - 1 && circles[i].speedx >= 0) {
					if (circles[i].coordinates[0].x > walls[0].x + 600) {
						circles.erase(circles.begin() + i);
						break;
					}
					circles[i].move((walls[0].x - 1) - circles[i].coordinates[0].x, 0);
					circles[i].changespeed(-circles[i].speedx * RESTITUTION * 2, 0, 0);
				}
				if (circles[i].coordinates[CIRCLESIDES / 2].x < walls[2].w + 1 && circles[i].speedx <= 0) {
					if (circles[i].coordinates[CIRCLESIDES / 2].x < walls[2].w - 600) {
						circles.erase(circles.begin() + i);
						break;
					}
					circles[i].move((walls[2].w + 1) - circles[i].coordinates[CIRCLESIDES / 2].x, 0);
					circles[i].changespeed(-circles[i].speedx * RESTITUTION * 2, 0, 0);
				}
			}
		}


	};

	void updateCircles() {
		for (int i = 0; i < circles.size(); i++)
			circles[i].update();
	};

	void clearCheck() {//tasto cestino: se viene cliccato cancella tutto
		for(int i=0;i<OPTIONS;i++)
			if (ui[i].tag == "trash" && ui[i].selected) {
				circles.clear();
				selection(-1,0);
			}
	};

	void sliderUpdate() {//aggiorna posizione e valore dello slider selezionato e spostato
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
				sliders[i].number.x = sliders[i].sl.x + sliders[i].sl.w/2;
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
				SDL_QueryTexture(sliders[i].numberT, NULL, NULL, &sliders[i].number.w, &sliders[i].number.h);
				sliders[i].number.x -= sliders[i].number.w / 2;
			}
			else
				sliders[i].selected = false;
		}
	};

	void info() {//cambiamento dei valori controllati dagli slider
		if (ui[0].selected) {
			for (int i = 0; i < circles.size(); i++) {
				if (circles[i].selected) {
					if (sliders[0].selected) {
						circles[i].changemass(circles[i].mass / pow(circles[i].radius/PROPORTION, 3) * pow(sliders[0].value/2/PROPORTION, 3));
						circles[i].resize(sliders[0].value / 2);
					}
					else if (sliders[1].selected) {
						circles[i].changemass(sliders[1].value * 100 * pow(circles[i].radius / PROPORTION, 3) * 4 / 3 * M_PI);
					}
					for (int j = 0; j < 4; j++)
						produceInfoText(i, j);
				}
			}
		}
	};

	void produceInfoText(int i, int j) {//aggiorna le scritte 
		if (j == 0) {
			tempstring = std::to_string(circles[i].mass);
			tempstring += " kg";
		}
		if (j == 1) {
			tempstring = std::to_string(circles[i].speedxF);
			tempstring += " m/s";
		}
		if (j == 2) {
			tempstring = std::to_string(circles[i].speedyF);
			tempstring += " m/s";
		}
		if (j == 3) {
			tempstring = std::to_string(circles[i].K);
			tempstring += " J";
		}
		bool intero = false;
		for (int k = 0; k < tempstring.size(); k++) {
			if (tempstring[k] == '.') {
				tempstring.erase(k + 3, 4);
				break;
			}
		}
		for (int j = 0; j < tempstring.size(); j++)
			tempcha[j] = tempstring[j];
		tempcha[tempstring.size()] = '\0';
		textSurface = TTF_RenderText_Solid(gFont, tempcha, white);
		infofields[j].Texture[1] = SDL_CreateTextureFromSurface(Renderer, textSurface);
	};

	void dragShapes() {//drag and drop
		int x, y;
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
				circles[i].changespeed((mousex - x)*12, (mousey - y)*12, 0);
				circles[i].drag = false;
			}
			else {
				circles[i].speedx = circles[i].speedy = 0;
				circles[i].move(mousex - mousexdata[0], mousey - mouseydata[0]);
			}

		}
	};

	void checkCollisions() {
		checkCC();
	};

	void checkCC() {//collisioni e urti cerchio - cerchio
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
						m1 = circles[i].mass;

						v2x = circles[j].speedx;
						v2y = circles[j].speedy;
						m2 = circles[j].mass;

						// distanza tra i cerchi
						dx = x2 - x1;
						dy = y2 - y1;
						d = sqrt(dx * dx + dy * dy);

						// unit vector
						ux = dx / d;
						uy = dy / d;

						// componenti della velocita' lungo l'asse di collisione
						v1c = v1x * ux + v1y * uy;
						v2c = v2x * ux + v2y * uy;

						// componenti della velocita' perpendicolari all'asse di collisione
						v1p = v1x * uy - v1y * ux;
						v2p = v2x * uy - v2y * ux;

						// conservazione della quantita' di moto lungo l'asse di collisione
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

	void airResistance() { //applicazione resistenza dell'aria
		for (int i = 0; i < ui.size(); i++) {
			if (ui[i].tag == "air")
				if (ui[i].selected)
					return;
		}
		float v, vx, vy;
		float area;
		float m;
		float D;
		float angle;
		float Fx, Fy;
		int t;
		for (int i = 0; i < circles.size(); i++) {
			vx = circles[i].speedx;
			vy = circles[i].speedy;
			if (vx == 0 && vy == 0)
				continue;
			if (vx == 0) {
				t = 1;
				v = vy;
			}
			else if (vy == 0) {
				t = 2;
				v = vx;
			}
			else{
				v = sqrt(pow(vx, 2) + pow(vy, 2));
				t = 0;
			}
			area = pow(circles[i].radius/PROPORTION, 2) * M_PI;
			m = circles[i].mass;
			D = 0.47 * pow(v, 2) * area * AIRRESISTANCE / 60;
			switch (t) {
			case 0:
				angle = atan(vy / vx);
				Fx = D * cos(angle);
				Fy = D * sin(angle);
				if (vx > 0)
					Fx = -abs(Fx);
				else
					Fx = abs(Fx);
				if (vy > 0)
					Fy = -abs(Fy);
				else
					Fy = abs(Fy);

				if (abs(Fx / m) > abs(circles[i].speedx)) {
					circles[i].changespeed(-circles[i].speedx, 0, 0);
					Fx = 0;
				}
				if (abs(Fy / m) > abs(circles[i].speedy)) {
					circles[i].changespeed(0,-circles[i].speedy, 0);
					Fy = 0;
				}
				circles[i].changespeed(Fx / m, Fy / m, 0);
				break;
			case 1:
				if (vy < 0)
					circles[i].changespeed(0, D / m, 0);
				else
					circles[i].changespeed(0, -D / m, 0);
					
				break;
			case 2:
				if (vx < 0)
					circles[i].changespeed(D / m, 0, 0);
				else
					circles[i].changespeed(-D / m, 0, 0);
				break;
			}
		}
	};

	
	void handleEvents() {//registra i tasti cliccati
		SDL_Event event;
		while (SDL_PollEvent(&event))
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

	void LClickChecks() {//se viene cliccato il tasto sinistro del mouse vengono chiamate le seguenti funzioni
		if (!selectui())
			return;
		if (selectshape())
			return;
		if (ui[1].selected && slidercheck())
			newshape();
		else {
			if (ui[1].selected || ui[0].selected) {
				slidercheck();
			}
		}
	};

	bool selectui() {//se il mouse viene cliccato quando è sopra un bottone chiama la selezione del bottone
		if (mousey >= ui[0].button.y && mousey <= ui[0].button.y + UISIZE)
			for (int i = 0; i < OPTIONS; i++)
				if (mousex >= ui[i].button.x && mousex <= ui[i].button.x + UISIZE) {
					selection(0,i);
					return false;
				}
		return true;
	};

	void selection(int a, int b) {//selezione del bottone
		if (a == -1) {
			for (int i = 0; i < OPTIONS; i++)
				ui[i].selected = false;
		}

		if (a == 0) {
			if (ui[b].selected) { // se è selezionato si deseleziona
				ui[b].selected = false;
				return;
			}
			if (!ui[b].exclusive) {// se è standalone si seleziona
				ui[b].selected = true;
				return;
			}
			for (int i = 0; i < OPTIONS; i++) { //se è selezionato e non è standalone si deseleziona
				if (ui[i].exclusive) {
					ui[i].selected = false;
				}
			}//si seleziona quello giusto
			ui[b].selected = true;
			return;
		}
	};

	void newshape() {//aggiunge una figura e seleziona quella appena creata
		Circle t;	
		t.C(mousex, mousey, sliders[0].value / 2, sliders[1].value * 100 * pow((sliders[0].value/2) / PROPORTION, 3) * 4 / 3 * M_PI);
		for (int i = 0; i < circles.size(); i++)
			circles[i].selected = false;
		t.selected = true;
		circles.push_back(t);
	};

	bool selectshape() {//seleziona la figura su cui è il cursore se viene cliccato il tasto del mouse
		int t;
		for (int i = 0; i < circles.size(); i++) {
			if (sqrt(pow(mousex - circles[i].x, 2) + pow(mousey - circles[i].y, 2)) < circles[i].radius) {
				for (int j = 0; j < circles.size(); j++) {
					circles[j].selected = false;
				}
				circles[i].selected = true;
				circles[i].drag = true;
				return 1;
			}
		}
		return 0;
	};

	bool slidercheck() {//se il mouse viene cliccato quando il cursore è sopra uno slider, questo viene selezionato
		for (int i = 0; i < sliders.size(); i++) {
			if (mousex >= sliders[i].sl.x && mousex <= sliders[i].sl.x + sliders[i].sl.w && mousey >= sliders[i].sl.y && mousey <= sliders[i].sl.y + sliders[i].sl.h) {
				sliders[i].selected = true;
				sliders[i].s = sliders[i].sl.x - mousex;
				return false;
			}
		}
		return true;
	}

	void clean() {//chiude tutto
		std::cout << "cleaning\n";
		SDL_DestroyWindow(Window);
		SDL_DestroyRenderer(Renderer);
		TTF_Quit();
		SDL_Quit();
	};

	bool running() { return Running; }

};