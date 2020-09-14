#include <stdio.h> /* Input/Output */
#include <stdlib.h>
#include <string.h>

/* En este archivo irá el scheduler */

int main(int argc, char **argv)
{
  if (argc > 4 || argc < 3)
  {
    printf("Cantidad incorrecta de comandos! :(\n");
    printf("Formato: ./edf <file> <output> [<n>]\n");
    return 1;
  }

  if (argc == 4)
  {
    int n_cpu = atoi(argv[3]);
    printf("CANTIDAD DE CPUS: %d\n", n_cpu);
  }
  else
  {
    int n_cpu = 1;
    printf("CANTIDAD DE CPUS: %d\n", n_cpu);
  }

  // TODO: Sera necesario agregar una verificación de que el archivo de input entregado existe? para que no tire segmentation fault en caso que no exista, o podremos asumir que siempre se entregara un nombre archivo de input valido?
  FILE *input_file = fopen(argv[1], "r");
  // TODO: Abrir archivo una vez terminó la ejecución (i.e. mover esta linea al final del programa)
  FILE *output_file = fopen(argv[2], "w");

  char n_lines[255];
  int K;
  fgets(n_lines, sizeof(n_lines), input_file);

  K = atoi(n_lines);

  printf("CANTIDAD DE PROCESOS: %d\n", K);

  char line[256];
  const char s[2] = " ";
  char *token;

   while (fgets(line, sizeof(line), input_file))
   {
     //printf("%s\n", line);
     printf("PROCESO\n");
     token = strtok(line, s);
     while (token != NULL) 
     {
       printf("%s\n", token);
       token = strtok(NULL, s);
     }
   }
  printf("WEEEEEEEEEEEEEE\n");

  fclose(input_file);
  fclose(output_file);
  return 0;
}
