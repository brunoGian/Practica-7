#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mmu.h>

#define NUMPROCS 4
#define PAGESIZE 4096
#define PHISICALMEMORY 12*PAGESIZE 
#define TOTFRAMES PHISICALMEMORY/PAGESIZE // TOTFRAMES = 12
#define RESIDENTSETSIZE PHISICALMEMORY/(PAGESIZE*NUMPROCS) // RESIDENTSETSIZE = 3

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;


int getfreeframe();
int getvirtualframe();
int searchvirtualframe();
int getfifo();
int get_lru();

int pagefault(char *vaddress)
{
    int i;
    int frame,vframe; 
    long pag_a_expulsar;
    int fd;
    char buffer[PAGESIZE];
    int pag_del_proceso;

    // A partir de la dirección que provocó el fallo, calculamos la página
    pag_del_proceso=(long) vaddress>>12;


    // Si la página del proceso está en un marco virtual del disco
    if ((ptbr + pag_del_proceso)->presente == 0 && (ptbr+pag_del_proceso)->framenumber == -1)
    {
		// Lee el marco virtual al buffer
        vframe = (ptbr + pag_del_proceso)->framenumber;
        readblock(buffer, vframe);

        // Libera el frame virtual
        systemframetable[vframe].assigned = 0;
    }


    // Cuenta los marcos asignados al proceso
    i=countframesassigned();

    // Si ya ocupó todos sus marcos, expulsa una página
    if(i>=RESIDENTSETSIZE)
    {
		// Buscar una página a expulsar
		pag_a_expulsar = get_lru();
		// Poner el bitde presente en 0 en la tabla de páginas
        (ptbr+pag_a_expulsar)->presente = 0;
        
        // Si la página ya fue modificada, grábala en disco
        if ((ptbr+pag_a_expulsar)->modificado != 0)
        {
			// Escribe el frame de la página en el archivo de respaldo y pon en 0 el bit de modificado
            frame = (ptbr+pag_a_expulsar)->framenumber;
            saveframe(frame);
            
        }
		
        // Busca un frame virtual en memoria secundaria
        vframe = getvirtualframe();
		// Si no hay frames virtuales en memoria secundaria regresa error
        if (vframe == -1)
		{
            return(-1);
        }
        // Copia el frame a memoria secundaria, actualiza la tabla de páginas y libera el marco de la memoria principal
        copyframe(frame, vframe);

        // Actualiza la tabla de paginas
        (ptbr + pag_a_expulsar)->framenumber = vframe;
        (ptbr + pag_a_expulsar)->presente = 0;
        (ptbr + pag_a_expulsar)->modificado = 0;
        // Libera el marco de la memoria virtual
    }

    // Busca un marco físico libre en el sistema
	// Si no hay marcos físicos libres en el sistema regresa error
    {
        return(-1); // Regresar indicando error de memoria insuficiente
    }

    // Si la página estaba en memoria secundaria
    {
        // Cópialo al frame libre encontrado en memoria principal y transfiérelo a la memoria física
    }
   
	// Poner el bit de presente en 1 en la tabla de páginas y el frame 


    return(1); // Regresar todo bien
}

int getvirtualframe(){
    int i;
    // Busca un marco fisico libre en el sistema
    for(i=systemframetablesize+framesbegin; i<2*systemframetablesize+framesbegin; i++)
        if(!systemframetable[i].assigned)
        {
            systemframetable[i].assigned=1;
            break;
        }
    if(i<systemframetablesize+framesbegin)
        systemframetable[i].assigned=1;
    else
        i=-1;
    return(i);
}

int get_lru(){
    unsigned long t = 0xffffffffffffffff;
    int i;
    int p=0;
    for (i=0; i<6; i++)
    {

    }
}