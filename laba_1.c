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
    char c;

    while ( (c = *key) && (isalpha(c)) ) 
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
    return file;
} 

void fillVigenerTable(char vigenereTable[26][26]) {
  
  char alphabet[26];

  for (int i = 0; i < 26; ++i)
  {
    alphabet[i] = i + 65;
  }

  int shift = 0;

  for (int i = 0; i < 26; ++i)
  {
    for (int k = 0; k < 26; ++k)
    {
      char result = alphabet[k] + shift;

      vigenereTable[i][k] = result <= 90 
                ? result
                : result - 26;
    }
    shift++;
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

  char *key;

  while((opt = getopt(argc, argv, opts)) != -1) {
    switch(opt){

      case 'h':
        help(argv[0]);
        exit(EXIT_SUCCESS);

      case 'k':
        if  (!isValid(optarg)) {
            fprintf(stderr, "\"%s\" - invalid KEY!\n%s", optarg, OFFER_HELP); 
            exit(EXIT_FAILURE);
        }
        key =  optarg;
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
        destination = openFileOrThrow(argv[5], "wb");
        
        break;

      default:
        fprintf(stderr, "Too many arguments!\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
    }
    
    char *tempString = (char*)malloc(sizeof(char));
    int symbolCount = 0;
    char symbol;

    while (1) 
    {
        symbol = fgetc(source);

        if (symbol == EOF)
        {
          break;
        }

        tempString[symbolCount] = symbol;
        symbolCount++;
        tempString = (char*)realloc(tempString, sizeof(char)*(symbolCount + 1));
    } 
    tempString[symbolCount+1] = 0;

  char vigenereTable[26][26];
  fillVigenerTable(vigenereTable);
  
  int keyLenght = strlen(key);  
  for (int i = 0; i < keyLenght; ++i)
  {
    key[i] = toupper(key[i]);
  }
  printf("%s\n", key);

  int keyPosition = 0;

  for (int i = 0; i <= symbolCount; ++i)
  {
    if (isalpha(tempString[i]))
    {
      char symbol = toupper(tempString[i]); 

      int symbolIndex = symbol - 65;
      int keyIndex = key[keyPosition] - 65;

      char encoded = vigenereTable[symbolIndex][keyIndex];

      char c = fputc(encoded, destination);
      if (c == EOF) {
          fprintf(stderr, "Cannot write symbol to destination file\n"); 
          exit(EXIT_FAILURE);
      }
      keyPosition = (keyPosition < keyLenght -1) 
              ? keyPosition +1
              : 0; 
    }
    else {

      char c = fputc(tempString[i], destination);
      if (c == EOF) {
          fprintf(stderr, "Cannot write symbol to destination file\n"); 
          exit(EXIT_FAILURE);
        }
    }
  } 

  // for (int i = 0; i <= symbolCount; ++i)
  // {
  //   if (isalpha(tempString[i]))
  //   {
  //     char symbol = toupper(tempString[i]); 

  //     int keyIndex = key[keyPosition] - 65;

  //     int k = 0;
  //     while(vigenereTable[keyIndex][k] != symbol){
  //         k++;
  //     }
  //     char iNeed = vigenereTable[0][k];

  //     char c = fputc(iNeed, destination);
  //     if (c == EOF) {
  //         fprintf(stderr, "Cannot write symbol to destination file\n"); 
  //         exit(EXIT_FAILURE);
  //     }
  //     keyPosition = (keyPosition < keyLenght -1) 
  //             ? keyPosition +1
  //             : 0; 
  //   }
  //   else {

  //     char c = fputc(tempString[i], destination);
  //     if (c == EOF) {
  //         fprintf(stderr, "Cannot write symbol to destination file\n"); 
  //         exit(EXIT_FAILURE);
  //       }
  //   }
  // } 
  fputc('\n', destination);

  free(tempString);
  fclose(source);
  fclose(destination);
    
  return 0;
}