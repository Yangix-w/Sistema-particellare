# _Progetto per il corso Informatica grafica di Università dei studi di Milano-Bicocca._

**Studente: Shi Le Yang, 
Matricola: 894536**

Sistema particellare fatto con OpenGL modificando il codice usato in classe. 

Il codice è all'interno della cartella ***code***. 

## File modificati: 
main.cpp, Makefile. 

## Nuovi file aggiunti: 
* particle.h
* particle.cpp
* particleSystem.h
* particleSystem.cpp
* particle.vert
* particle.frag
* particleShader.h
* particleShader.cpp

## Altre cartelle
Vengono utilizzate solo le librerie contenuti nella cartella ***base***.

La cartella ***texture*** contiene le texture usate per le particelle. 

La cartella ***gif*** contiene i gif usati nella presentazione. 

## Comandi
### Comandi per compilare e avviare il programma: 
```
make
./main.exe
```
> [!NOTE]
> Compilando particleSystem.cpp uscirà una nota, cercando su internet è un problema di versione, 
non influisce sulla funzionalità del programma.

### Durante l'esecuzione: 
+ wasd: per spostare la posizione dell'emettitore
+ q: per spostare l'emettitore verso l'alto
+ z: per spostare l'emettitore verso il basso
+ f: per attivare o disattivare l'effetto fuoco
+ e: per attivare l'effetto esplosione
+ r: per attivare o disattivare l'effetto pioggia
+ g: per attivere o disattivare l'effetto fumo
+ 1: per diminuire l'intensità di pioggia
+ 2: per aumentare l'intensità di pioggia
+ 4: per diminuire l'intensità di fumo
+ 5: per aumentare l'intensità di fumo
