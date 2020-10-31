#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define READ  0
#define WRITE 1

int haysimbolopipe(char comando[]){
  if(strchr(comando, '|')!=NULL){
     return 1;
   }
   return 0;
}

int haysimbolodireccion(char comando[]){
  if(strchr(comando, '>')!=NULL){
     return 1;
   }
   return 0;
}

void separador(char comando[], char *comandoseparado[], char simbolo[]){
  char * puntero;
  int i = 0;
  puntero = strtok(comando,simbolo);
  while(i<2){
   comandoseparado[i] = puntero;
   puntero = strtok(NULL,simbolo);
   i++;
   }
}

//wordCounter cuentas las palabras en una cadena, conciderandolas por espacio
int wordCounter(char cadena[]){
    //Contar palabras de la cadena
    int i=0, cont=0;
    int longitud = strlen(cadena);

    while(i<longitud){
        while(cadena[i]==' '){
        i=i+1;
        }
        cont=cont+1;
        while(cadena[i]!=' '){
        i=i+1;
        }
   }
   //printf("Longitud del arreglo cont+1 = %d\n", cont+1);
   return cont;
}

//arrayChunks recibe una cadena y un arreglo de la longitud de pablabras de la cadena
//se llena el arreglo con las palabras de la cadena
void arrayChunks(char cadena[],char *cadenaVals[], int wc){
   char delimitador[] = " ";
   char *token = strtok(cadena, delimitador);
    int j=0;
    while (j<wc){
        cadenaVals[j]=token;
        //printf("Encontramos un token: %s\n", token);
        token = strtok(NULL, delimitador);
        j++;
    }
    cadenaVals[wc]=NULL;
}

void execute(char command[]){
   int size = strlen(command);
   if (haysimbolopipe(command)==0 && haysimbolodireccion(command)==0)
   {
      int lon= wordCounter(command);
      char *vals[lon+1];
      arrayChunks(command,vals,lon);
      execvp(vals[0],vals);
   }
   else if (haysimbolopipe(command)==1 && haysimbolodireccion(command)==0)
   {
      char *comsPipe[2];
      char pip[]="|";
      separador(command,comsPipe,pip);

      int lonL= wordCounter(comsPipe[0]);
      int lonR= wordCounter(comsPipe[1]);
      char *Lvals[lonL+1];
      char *Rvals[lonR+1];
      arrayChunks(comsPipe[0],Lvals,lonL);
      arrayChunks(comsPipe[1],Rvals,lonR);

      pid_t pid;
      int fd[2];

      if (pipe(fd)==-1){
         perror("Creating pipe");
         exit(EXIT_FAILURE);
      }

      pid = fork();
      if (pid == -1){

          perror("fork() failed xd");
          exit(EXIT_FAILURE);

      } else if (pid == 0){

          //// Close the pipe write descriptor.
          close(fd[WRITE]);
          // Redirect STDIN to read from the pipe.
          dup2(fd[READ], STDIN_FILENO);
          // Execute
          execvp(Rvals[0],Rvals);

      } else {

          // Close the pipe read descriptor.
          close(fd[READ]);
          // Redirect STDOUT to write to the pipe.
          dup2(fd[WRITE], STDOUT_FILENO);
          // Execute
          execvp(Lvals[0],Lvals);
      }
   }
}

int main(){
        while(1){
                char command[100];
                printf("-> ");
                scanf("%99[^\n]",command);
                if (!strcmp("exit",command)){
                        break;
                }

                pid_t pid = fork();
                if(!pid){
                        execute(command);
                        break;
                }else{
                        wait(NULL);
                }

        }
        return 0;
}
