#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define OFFER_HELP "See -h for more information\n"
#define ONE_WORD_FILE_NAME "fileForOneWord.txt"

void help(const char fileName[]) {
    printf("\nUsage:\n");
    printf("\n%s [options] [source file [destination file]]\n", fileName);
    printf("\nOptions:\n\n");
    printf("-h\tshow this help message and exit\n");
    printf("-k KEY\t encryption key\n");
    printf("-c\tcode\n");
    printf("-d\tdecode\n");

    printf("\nATTENTION!\nKey and operation type(code or decode) are required options");
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

int getFileSize(FILE *file, const char *fileName){
  if (fseek(file, 0, SEEK_END) == -1) {
      fprintf(stderr, "failed to fseek %s\n", fileName);
      exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  return size;
}

void writeWordToResultFile(FILE *wordFile, const char *wordFileName, FILE *resultFile) {
    
    int wordSize = getFileSize(wordFile, wordFileName);
    char *str = (char*)malloc(wordSize + 1);

    if (wordSize == 0)
    {
        return;
    }  

    // wordFile = fopen(wordFileName, "r");
    fread(str, sizeof(char), wordSize, wordFile);
    str[wordSize] = 0;

    fputs(str, resultFile);
    // wordFile = fopen(wordFileName, "w");
    fseek(wordFile, 0, SEEK_SET);
    free(str);
} 

int main (int argc, char *argv[])
{
  if(argc == 1) { 
        fprintf(stderr, "Missing required options\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
    }
  if(argc > 6) { 
        fprintf(stderr, "Too many arguments!\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
    }
  char *opts = "hk:cd";
  int opt;
  bool code, decode, kSet;

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
        kSet = true;
        break;

      case 'c':
        code = true;
        break;

      case 'd':
        decode = true;
        break;

        // default:
        // break;
    }
  }
  if( !(code ^ decode) ) { 
    fprintf(stderr, "You should set one of -c/-d options\n%s", OFFER_HELP); 
    exit(EXIT_FAILURE);
  }

  if( !kSet ) { 
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
          break;
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