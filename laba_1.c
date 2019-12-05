#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

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

void writeWordToResultFile(FILE *wordFile, const char *wordFileName, FILE *resultFile) {
    //int wordSize = getFileSize(wordFile, wordFileName);
    //char *str = (char*)malloc(wordSize + 1);

    //if (wordSize == 0)
    //{/
        return;
    //}  

    //wordFile = fopen(wordFileName, "r");
    //fread(str, sizeof(char), wordSize, wordFile);
    //str[wordSize] = 0;

    //fputs(str, resultFile);
    //wordFile = fopen(wordFileName, "w");
    //free(str);
} 

int main (int argc, char *argv[])
{
  if(argc == 1) { 
        fprintf(stderr, "Missing required options\n%s", OFFER_HELP); 
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
    }
  }
  if( !(code ^ decode) ) { 
    fprintf(stderr, "You should set one of -c/-d options\n%s", OFFER_HELP); 
    exit(EXIT_FAILURE);
  }
  printf("%d %d\n", code, decode);
  if( !kSet ) { 
    fprintf(stderr, "-k is required option\n%s", OFFER_HELP); 
    exit(EXIT_FAILURE);
  }

  FILE *source;
  FILE *destination;


  switch(argc){
      case 3:
        source = stdin;
        destination = stdout;

        break;

      case 4:
        source = openFileOrThrow(argv[4], "r");
        destination = stdout;

        break;

      case 5:
        source = openFileOrThrow(argv[4], "r");
        source = openFileOrThrow(argv[5], "w");
        
        break;

      default:
        fprintf(stderr, "Too many arguments!\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
    }

    FILE *fileForOneWord;
    fileForOneWord = openFileOrThrow(ONE_WORD_FILE_NAME, "w");

    FILE *result;
    result = openFileOrThrow("result.txt", "w");

    char ch;

    for (size_t i = 0; i < sourceSize; i++)
    {
        ch=fgetc(source);

        if(isalpha(ch) ) {
            fputc(tolower(ch), fileForOneWord);
        } 
        else {
            writeWordToResultFile(fileForOneWord, ONE_WORD_FILE_NAME, result);
            fputc(ch, result);     
        }
    }
    writeWordToResultFile(fileForOneWord, ONE_WORD_FILE_NAME, result);

    fclose(result);
    result = openFileOrThrow("result.txt", "r");

    printf("result:\n");

    for (size_t i = 0; i < resultFileSize; i++)
    {
        ch=fgetc(result);
        fputc(ch, stdout);
    }

    fclose(fileForOneWord);
    fclose(source);
    fclose(result);

    
    FILE *fileForOneWord;
    fileForOneWord = openFileOrThrow(ONE_WORD_FILE_NAME, "w");

    FILE *result;
    result = openFileOrThrow("result.txt", "w");

    int ch;
       while((ch=getc(stdin)) != EOF) {
            if(isalpha(ch) ) {
                fputc(tolower(ch), fileForOneWord);
            } 
            else {
                writeWordToResultFile(fileForOneWord, ONE_WORD_FILE_NAME, result);
                fputc(ch, result);     
            }
        }
        writeWordToResultFile(fileForOneWord, ONE_WORD_FILE_NAME, result);

        result = openFileOrThrow("result.txt", "r");

    fclose(fileForOneWord);
    fclose(result);

   return 0;

  
    return 0;
}