# Fly By Wire
Progetto realizzato da Akinola Akinjolagbe Samuel Folabi

L'obiettivo del progetto è emulare un’architettura, anche se enormemente minimale e difforme, di un sistema Fly-By-Wire. Al fine di contestualizzare il progetto e alcuni termini, si precisa che si è utilizzato come riferimento l’architettura del Boeing 777

Il Progetto è stato programmato su  **Ubuntu 18.04.5 LTS**.

Altri dettagli riguardanti il progetto sono visibili nel documento PDF "relazione"

## Istruzioni per la compilazione:
Il progetto contiene un makefile quindid è possibile compilarlo con

$ make all

Ed eseguirlo con

$ make run
che utilizza l'argomento di default che è possibile definire allinterno del makefile

oppure

$ make run ARGS= "..."
sostutuendo ai puntini il percorso del file in input 

oppure

$ ./bin/pri /home/%user%/labso/R18.txt
