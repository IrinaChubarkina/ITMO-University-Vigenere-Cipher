#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define OFFER_HELP "See -h for more information\n"

char vigenereTable[26][26];
int keyLenght;

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

void codeText(const char* source, const char* key, FILE *destination) {

  int symbolCount = strlen(source);
  int keyPosition = 0;

  for (int i = 0; i <= symbolCount; ++i)
  {
    if (isalpha(source[i]))
    {
      char symbol = toupper(source[i]); 

      int symbolIndex = symbol - 65;
      int keyIndex = key[keyPosition] - 65;

      char encoded = vigenereTable[symbolIndex][keyIndex];
      fputc(encoded, destination);
    
      keyPosition = (keyPosition < keyLenght -1) 
                    ? keyPosition + 1
                    : 0; 
    }
    else {
      fputc(source[i], destination);
    }
  } 
}

void decodeText(const char* source, const char* key, FILE *destination) {

  int symbolCount = strlen(source);
  int keyPosition = 0;

  for (int i = 0; i <= symbolCount; ++i)
  {
    if (isalpha(source[i]))
    {
      char symbol = toupper(source[i]); 
      int keyIndex = key[keyPosition] - 65;

      int columnNumber = 0;
      while(vigenereTable[keyIndex][columnNumber] != symbol){
          columnNumber++;
      }
      char decoded = vigenereTable[0][columnNumber];
      fputc(decoded, destination);
  
      keyPosition = (keyPosition < keyLenght -1) 
                    ? keyPosition + 1
                    : 0; 
    }
    else {
      fputc(source[i], destination);
    }
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
        destination = openFileOrThrow(argv[5], "w");
        
        break;

      default:
        fprintf(stderr, "Too many arguments!\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
  }
    
  char *textToEncrypt = (char*)malloc(sizeof(char));
  int symbolCount = 0;
  char symbol;

  while (1) 
  {
      symbol = fgetc(source);

      if (symbol == EOF)
      {
        break;
      }

      textToEncrypt[symbolCount] = symbol;
      symbolCount++;
      textToEncrypt = (char*)realloc(textToEncrypt, sizeof(char)*(symbolCount + 1));
  } 
  textToEncrypt[symbolCount + 1] = 0;
  fclose(source);

  fillVigenerTable(vigenereTable);  
  keyLenght = strlen(key);  

  for (int i = 0; i < keyLenght; ++i)
  {
    key[i] = toupper(key[i]);
  }

  if (code){
    codeText(textToEncrypt, key, destination);
  } 
  else { 
    decodeText(textToEncrypt, key, destination);
  }
  
  fputc('\n', destination);

  free(textToEncrypt);
  fclose(destination);
    
  return 0;
}