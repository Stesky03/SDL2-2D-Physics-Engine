SDL2-2D-Physics-Engine

SDL2-2D-Physics-Engine è un programma open source, scritto in C++ utilizzando la libreria SDL2, rilasciato sotto una licenza MIT.

Questo programma è un motore fisico per simulare la dinamica di corpi rigidi bidimensionali, cerchi e quadrati(WIP).

UTILIZZO
Dopo l'accensione del programma selezionare uno dei 6 bottoni nell'angolo in alto a destra per selezionare una modalità/operazione.

I tasti e le loro funzioni sono elencati in seguito da destra a sinistra:
	1)Settings: (WIP)
	
	2)Sliders: Dopo aver selezionato una figura si possono cambiare i valori degli sliders per modificare le proprietà della figura:
		1) dimensioni (0.1->10 metri):
			-diametro nel caso del cerchio
			-lato nel caso del quadrato
		2) peso specifico (0.1->20)
	Sotto sono elencate alcune informazioni sulla figura in tempo reale:
		1) peso (Kg)
		2) velocità sull'asse x (m/s)
		3) velocità sull'asse y (m/s)
		4) energia cinetica (J)
	
	3)New: Dopo aver premuto questo bottone compariranno due nuovi bottoni nella linea successiva:
		1)cerchio
		2)quadrato
	Dopo aver selezionato una figura compariranno gli stessi sliders del bottone precedente per modificare le proprietà della figura prima che venga creata in seguito cliccare qualsiasi punto dello schermo per far comparire una figura.
	
	4)Gravity: Cliccare questo tasto per annullare la forza di gravità, cliccarlo nuovamente per attivarla.
	
	5)Walls: Cliccare questo tasto per far comparire dei bordi ai lati dello schermo contro i quali gli oggetti potranno rimbalzare, cliccarlo nuovamente per rimuoverli.
	
	6)Trash (operazione): Cliccare questo tasto per cancellare tutte le figure che sono state create
	
Per selezionare un oggetto bisogna cliccarlo (qualsiasi sia la modalità selezionata), tenendo premuto il tasto sinistro del mouse su una figura si può spostare, rilasciando il tasto la figura conserverà la velocità che aveva nel momento del rilascio.

Gli urti tra gli oggetti sono elastici, gli urti con il pavimento hanno come costante di restituzione la variabile RESTITUTION.

WIP:
-modalità Settings, impostazioni del programma:
	-slider RESTITUTION (attualmente non è una variabile ma una macro)
	-slider PROPORTION (moltiplicatore delle dimensioni di tutte le figure)
	
-collisioni, urti e drag and drop dei quadrati
	(i quadrati quando presi dovrebbero agire come un pendolo che dovrebbe avere il cursore del mouse come punto di sospensione e il centro come massa)
	(realizzare urti e collisioni tra quadrati è decisamente un cock moment)
