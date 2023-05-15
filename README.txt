SDL2-2D-Physics-Engine

SDL2-2D-Physics-Engine è un programma open source, scritto in C++ utilizzando la libreria SDL2, rilasciato sotto una licenza MIT.

Questo programma è un motore fisico per simulare la dinamica di cerchi rigidi bidimensionali.

UTILIZZO
Dopo l'accensione del programma selezionare uno dei 6 bottoni nell'angolo in alto a destra per selezionare una modalità/operazione.

I tasti e le loro funzioni sono elencati in seguito da destra a sinistra:
	1)Sliders: Dopo aver selezionato una figura si possono cambiare i valori degli sliders per modificare le proprietà della figura:
		1) diametro (0.1->10 metri):
		2) peso specifico (0.1->20)
	Sotto sono elencate alcune informazioni sulla figura in tempo reale:
		1) peso (Kg)
		2) velocità sull'asse x (m/s)
		3) velocità sull'asse y (m/s)
		4) energia cinetica (J)
	
	2)New: Dopo aver premuto questo bottone compariranno gli stessi sliders del bottone precedente per modificare le proprietà della figura prima che venga creata, in seguito cliccare qualsiasi punto dello schermo per far comparire una figura.
	
	3)Gravity: Cliccare questo tasto per disattivare/attivare la forza di gravità.
	
	4)Walls: Cliccare questo tasto per disattivare/attivare i bordi (laterali e superiore) dello schermo.
	
	5)Air: Cliccare questo tasto per disattivare/attivare la resistenza dell'aria
	
	5)Trash (operazione): Cliccare questo tasto per cancellare tutte le figure che sono state create
	
Per selezionare un cerchio bisogna cliccarlo (qualsiasi sia la modalità selezionata), tenendo premuto il tasto sinistro del mouse su una figura si può spostare, rilasciando il tasto la figura conserverà la velocità che aveva nel momento del rilascio.

Gli urti tra gli oggetti sono elastici, gli urti con il pavimento hanno come costante di restituzione la variabile RESTITUTION.

ISTRUZIONI PER LA COMPILAZIONE:
compilare il file engine.cpp, dando accesso al compilatore alle cartelle contenenti tutti i file di SDL (SDL.h; ...) e di SDL_ttf (SDL_ttf.h, ...) e al file engine.h, inoltre linkare le librerie SDL2.lib; SDL2main.lib; SDL2_ttf.lib;
