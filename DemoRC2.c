/* DemoRC2.h - Demo para RC2.

   Universidad Nacional Autonoma de Mexico,
        Dr. Gerardo Vega Hernández
              Abril del 2003.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "RC2.h"
#define TRUE 1
#define FALSE 0
typedef int BOOLEAN;
BOOLEAN borrar = TRUE;
BOOLEAN cifrar = TRUE;
BOOLEAN menu = FALSE;



B8 Key1[8]  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
B8 Ptx1[8]  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff};
B8 Key2[8]  = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
B8 Ptx2[8]  = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
B8 Key3[16] = {0x88,0xbc,0xa9,0x0e,0x90,0x87,0x5a,0x7f,
               0x0f,0x79,0xc3,0x84,0x62,0x7b,0xaf,0xb2};

int TamBuff = 65536;
char ent[128], sal[128];                      /* Nombres de los archivos de E/S */
FILE *fpe, *fps;
B32 hexkey;






main(argc, argv)
int argc;
char *argv[];
{
    FILE *archivo;
    int i, nbytes, T_Salida;
    B8 *ap;
    B16 Ky[64];
    B8 buffer[TamBuff];
    int lenKey, effLenKey;
    lenKey = atoi(argv[4]);
    effLenKey = atoi(argv[5]);
    B8 llave[lenKey];
    char n2[10] = {0};
    unsigned char *llave0 = (unsigned char *) llave;
    for(i=0;(i<lenKey*2)&&(argv[2][i]!='\0');i++){
        sprintf(n2, "%c%c", argv[2][2*i], argv[2][2*i+1]);
        llave0[i] = strtol(n2,(char **)NULL,16);;  
    } 
    
    procesa_argv(argc,argv); 
    ExpandeLlave(Ky,llave,lenKey,effLenKey);

    if (argv[1][1]=='c')
    {

        RC2_Cifra(buffer, nbytes, T_Salida, Ky);
    }
    if (argv[1][1]=='d')
    {
        RC2_Descifra(buffer, nbytes, T_Salida, Ky);
    }



    
    RC2_CifraBloque((B16 *)Ptx1,Ky);
    ap = Ptx1;
    for(i=0;i<8;i++) {
      if(!(i%4)) printf(" ");
      printf("%02x",ap[i]);
    }
    RC2_DescifraBloque((B16 *)Ptx1,Ky);
    ap = Ptx1;
    for(i=0;i<8;i++) {
      if(!(i%4)) printf(" ");
      printf("%02x",ap[i]);
    }
    printf("\n");
    fclose(fpe); fclose(fps);
    


}



void RC2_Cifra(bufferES,T_entrada,T_Salida,K)
B8 bufferES[];
int T_entrada, T_Salida;
B16 K[];
{
    unsigned char *Ptx;
    int j;
    int i, longPad, numBloques;
    while(1){
        j=fread((char *)bufferES,1,TamBuff,fpe);
        T_entrada=strlen(bufferES);
        longPad=8-(T_entrada%8);
        printf("Entro_1 \n");
        if(j!=TamBuff) {    
            if(j==0) break;                       /* Termino de leer, todo bien */
            else if((j>0)&&(i<TamBuff)){ 
                printf("Entro_2 cadena sin  pading %s \n",bufferES);
                /*for(;i<8;i++) ((char *)text)[i] = 0;             /* Relleno */
                for(j=0;j<longPad;j++){ 
                    bufferES[T_entrada+j] = (unsigned char) longPad;
                    printf("Entro_for agrega pading %i \n",j);
                }
            }
            else {               /* Hubo un error en la lectura */
                 fprintf(stderr,"RC2-Error: Error al leer del archivo\n");
                 exit(1);
             }
        }
        printf("Entro_3 \n");
        printf("ent %i y pad %i \n", T_entrada, longPad);
        printf("cadena sin cifrar %s \n", bufferES);
        printf("Entro_long cadena sin cifrar %i \n",strlen(bufferES));
        printf("Entro_char %c \n",(unsigned char) longPad);

        /* Agregamos el padding antes de cifrar. */
        
        T_Salida = T_entrada + longPad;  /* La longitud del buffer por cifrar. */
        printf("Entro_4 T_Salida %i \n",T_Salida);
        numBloques = (T_Salida) >> 3;  /* Núm. de bloques por cifrar. */
        printf("Entro_5 bloques %i \n",numBloques);
        /* ¡A cifrar! */
        for(i=0,Ptx=bufferES;i<numBloques;i++,Ptx+=8){
            RC2_CifraBloque((B16 *)Ptx,K);
            /*printf("Entro_6 %c \n",Ptx);*/

        }
        printf("Entro_6 len bufferES antes de escribir %i \n", strlen(bufferES));
        if(fwrite((char *)bufferES,1,T_Salida,fps)!=T_Salida) {  /* Escribe el bloque cifrado */
            fprintf(stderr,"RC2-Error: Error al escribir al archivo\n");
            exit(1);
        }
        printf("Entro_7 texti cifrado %s \n",bufferES);
        printf("Entro_7 len bufferES despues de if %i \n",strlen(bufferES));
    }
}


void RC2_Descifra(bufferES,T_entrada,T_Salida,K)
B8 bufferES[];
int T_entrada, T_Salida;
B16 K[];
{
    int i, numBloques,j;
    unsigned char *Ptx;
    while(1){
        if((j=fread((char *)bufferES,1,TamBuff,fpe))!=TamBuff) {
            printf("Entro_1 %s \n", bufferES);
            if(j==0) break;                       /* Termino de leer, todo bien */
            
        }
        T_entrada=strlen(bufferES);
        numBloques = T_entrada >> 3;  /* Núm. de bloques por descifrar. */
        printf("Entro_1 T_entrada %i \n", T_entrada);
        printf("Entro_1 bloques %i \n", numBloques);
        /* ¡A descifrar! */
        for(i=0,Ptx=bufferES;i<numBloques;i++,Ptx+=8){
            RC2_DescifraBloque((B16 *)Ptx,K);
        }
        printf("Entro_2 %s \n", bufferES);
        printf("Entro_2 %i \n", strlen(bufferES));
        T_Salida = T_entrada - (unsigned int) *(Ptx-1);  /* Quitamos padding. */
        printf("Entro_3 T_Salida %i \n", T_Salida);
        printf("Entro_3 T_entrada %i \n", T_entrada);
        printf("Entro_3 Ptx %i \n", (unsigned int) *(Ptx-10));
        if(fwrite((char *)bufferES,1,T_Salida,fps)!=T_Salida) {  /* Escribe el bloque cifrado */
            fprintf(stderr,"RC2-Error: Error al escribir al archivo\n");
            exit(1);
        }
    }
}

/* Rutina de expancion de la llave;
   salida: EKey, la llave expandida.
   entrada: Key, la llave por expander.
            t, número de bytes en la llave.
            t1, número de bits efectivos en la llave.
*/
void ExpandeLlave(EKey,Key,t,t1)
B16 *EKey;
B8 *Key;
int t, t1;
{
    int i, T8=((t1+7)/8);
    int TM=(255 % (1 << (8+t1-8*T8)));
    B8 *Ly = (B8 *) EKey;

    memcpy(Ly,Key,t);   /* Inicializa EKey */
    /* Primer loop, ver RFC2268 p. 3. */
    for(i=t;i<128;i++) Ly[i] = PITABLE[(Ly[i-1]+Ly[i-t])&0xff];
    /* Paso intermedio. */
    Ly[128-T8] = PITABLE[Ly[128-T8]&TM];
    /* Segundo loop. */
    for(i=127-T8;i>=0;i--) Ly[i] = PITABLE[Ly[i+1]^Ly[i+T8]];
}

/* Rutina para cifrar un bloque usando RC2.
   parámetros: R el bloque de 64 bits a cifrar y la llave
   expandida K que habrá de usarse. La salida i.e. el bloque
   cifrado estará en R.
*/
void RC2_CifraBloque(R,K)
B16 R[],K[];
{
    int j=0,cnr,im;
    B8 S[4]={0x01,0x02,0x03,0x05};

    Mix(R,K,j,5);
    Mash(R,K);
    Mix(R,K,j,6);
    Mash(R,K);
    Mix(R,K,j,5);
}

/* Rutina para descifrar un bloque usando RC2.
   parámetros: R el bloque de 64 bits a descifrar y la llave
   expandida K que habrá de usarse. La salida i.e. el bloque
   descifrado estará en R.
*/
void RC2_DescifraBloque(R,K)
B16 R[],K[];
{
    int j=63,cnr,im;
    B8 S[4]={0x01,0x02,0x03,0x05};

    R_Mix(R,K,j,5);
    R_Mash(R,K);
    R_Mix(R,K,j,6);
    R_Mash(R,K);
    R_Mix(R,K,j,5);
}

void procesa_argv(argc, argv)
int argc;
char *argv[];
{
    if(argc!=6) uso();
        strcpy(ent,argv[3]);    strcpy(sal,argv[3]);
    if(argv[1][0]=='-') {
        switch(argv[1][1]) {
        case 'c':
            strcat(sal,".cif");
            break;
        case 'd':
            cifrar = FALSE;
            if(strcmp(".cif",&ent[strlen(argv[3])-4])) strcat(ent,".cif");
            else sal[strlen(argv[3])-4] = 0;
            if(argv[1][1]=='d') borrar = FALSE;
            break;
        default:
            fprintf(stderr,"RC2-Error: %c\n",argv[1][1]);
            exit(1);
        }
    } else uso();    
    

    if((fpe = fopen(ent,"r"))==NULL) {
        fprintf(stderr,"RC2-Error: no puedo abrir %s\n",ent);
        exit(1);
    }
    if(!menu) 
            if((fps = fopen(sal,"w"))==NULL) {
                fprintf(stderr,"RC2-Error: no puedo abrir %s\n",sal);
                exit(1);
            }

}

void uso()
{
    printf("\nUso : $ rc2 [-c | -d] L1 ARCHIVO TAMANIO_LLAVE TAMANIO_EFFECTIVO\n");
    printf("    -c : para cifrar\n");
    printf("    -d : para descifrar sin borrar ARCHIVO.cif\n");
    printf("Ejemplo : ./RC2 -c 0000000000000000 ceros 8 64\n\n");
    exit(0);
}
