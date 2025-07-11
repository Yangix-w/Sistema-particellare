Progetto per il corso Informatica grafica
Studente: Shi Le Yang
Matricola: 894536

Sistema particellare fatto con OpenGL modificando il codice usato in classe. 

File modificati: main.cpp, Makefile.

Nuovi file aggiunti: 
1. particle.h
2. particle.cpp
3. particleSystem.h
4. particleSystem.cpp
5. particle.vert
6. particle.frag
7. particleShader.h
8. particleShader.cpp

Vengono utilizzate solo le librerie contenuti nella cartella "base".

Cartella "texture" contiene le texture usate per le particelle. 
Cartella "gif" contiene i gif usati nella presentazione. 

Comandi per compilare e avviare il programma: make, ./main.exe
Compilando particleSystem.cpp uscirà una nota, cercando su internet è un problema di versione, 
non influisce sulla funzionalità del programma. 

Comandi: 
wasd: per spostare la posizione dell'emettitore
q: per spostare l'emettitore verso l'alto
z: per spostare l'emettitore verso il basso
f: per attivare o disattivare l'effetto fuoco
e: per attivare l'effetto esplosione
r: per attivare o disattivare l'effetto pioggia
g: per attivere o disattivare l'effetto fumo
1: per diminuire l'intensità di pioggia
2: per aumentare l'intensità di pioggia
4: per diminuire l'intensità di fumo
5: per aumentare l'intensità di fumo