### Introduction: ####
 
Seguir este tutorial  http://mpitutorial.com/tutorials/running-an-mpi-cluster-within-a-lan/
para montar las máquinas virtuales. 

Para compilar y ejecutar con MPI
En cada máquina (tanto la maestra como los esclavos)
1. Necesitas g++ (en ubuntu "sudo apt-get install g++")
1. Hay que tener libcurl instalado (en ubuntu "apt-get install libcurl4-gnutls-dev")
1. Hay que tener instalado subversion ("sudo apt-get install subversion")
1. haces sudo make. Importante hacerlo en todas las máquinas, para que se instalen las correspondientes librerías en el directorio del sistema, y se enlacen adecuadamente.

Ahora que en todas las máquinas están las librerías instaladas, y el código y todo lo necesario debe estar en la carpeta compartida con NFS con todos los nodos.
Hacemos en la máquina maestra
mpiexec -n num_procesos -f machinefile ./twitterClient palabra_buscar num_usuarios (usuarioInicial)


Los resultados estan en el archivo datos.csv que se crea al ejecutar el programa
