#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define OFFER_HELP "See -h for more information\n"

void help(const char fileName[]) {
    printf("\nUsage:\n");
    printf("\n%s [options] [source file [destination file]]\n", fileName);
    printf("\nOptions:\n\n");
    printf("-h\tshow this help message and exit\n");
    printf("-k KEY\t encryption key\n");
    printf("-c\tcode\n");
    printf("-d\tdecode\n");

    printf("\nATTENTION!\nKey and operation type(code or decode) are required options\n");
  }

bool isValid( const char key[] )
{
    unsigned char c;

    while ( ( c = *key ) && ( isalpha( c ) ) ) 
      ++key;

    return *key == '\0'; 
}

FILE *openFileOrThrow(const char *fileName, const char *mode) {
    FILE *file;
    file = fopen(fileName, mode);
    if ( file == NULL) {
        fprintf(stderr, "Cannot read file - %s\n", fileName); 
        exit(EXIT_FAILURE);
    }
} 

int main (int argc, char *argv[])
{
  if(argc == 1) { 
        fprintf(stderr, "Missing required options\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
    }

  char *opts = "hk:cd";
  int opt;

  int code = 0;
  int decode = 0;
  int keySet = 0;

  while((opt = getopt(argc, argv, opts)) != -1) {
    switch(opt){
      case 'h':
        help(argv[0]);
        exit(EXIT_SUCCESS);

      case 'k':
        if  (!isValid(optarg)) {
            fprintf(stderr, "\"%s\" - KEY is not valid\n%s", optarg, OFFER_HELP); 
            exit(EXIT_FAILURE);
        }
        keySet = 1;
        break;

      case 'c':
        code = 1;
        break;

      case 'd':
        decode = 1;
        break;
    }
  }

  if( !(code ^ decode) ) { 
    fprintf(stderr, "You should set one of -c/-d options\n%s", OFFER_HELP); 
    exit(EXIT_FAILURE);
  }

  if( !keySet ) { 
    fprintf(stderr, "-k is required option\n%s", OFFER_HELP); 
    exit(EXIT_FAILURE);
  }

  FILE *source;
  FILE *destination;

  switch(argc){

      case 4:
        source = stdin;
        destination = stdout;

        break;

      case 5:
        source = openFileOrThrow(argv[4], "r");
        destination = stdout;

        break;

      case 6:
        source = openFileOrThrow(argv[4], "r");
        source = openFileOrThrow(argv[5], "w");
        
        break;

      default:
        fprintf(stderr, "Too many arguments!\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
    }
    
    char *str = (char*)malloc(sizeof(char));
    int n = 0;
    char ch;

    while (1) 
    {
        ch = fgetc(source);

        if (ch == EOF)
        {
          break;
        }

        str[n] = ch;
        n++;
        str = (char*)realloc(str, sizeof(char)*(n + 1));
    } 
    str[n+1] = 0;

    printf("string - %s\n", str);

fclose(source);
fclose(destination);
  
    return 0;
}