#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define OFFER_HELP "See -h for more information\n"

char vigenereTable[52][52];
int keyLenght;

void help(const char fileName[]) { //информация
    printf("\nUsage:\n");
    printf("\n%s [options] [source file [destination file]]\n", fileName);
    printf("\nOptions:\n\n");
    printf("-h\tshow this help message and exit\n");
    printf("-k KEY\t encryption key\n");
    printf("-c\tcode\n");
    printf("-d\tdecode\n");

    printf("\nATTENTION!\nKey and operation type(code or decode) are required options\n");
  }

bool is_valid( const char key[] ) //проверка ключа на валидность (только буквы)
{
    char c;

    while ( (c = *key) && (isalpha(c)) ) 
      ++key;

    return *key == '\0'; 
}

FILE *open_file_or_throw(const char *fileName, const char *mode) {
    FILE *file;
    file = fopen(fileName, mode);
    if ( file == NULL) {
        fprintf(stderr, "Cannot read file - %s\n", fileName); //если не удалось открыть файл - пишем в стандартный поток ошибок
        exit(EXIT_FAILURE); 
    }
    return file;
} 

void fill_vigener_table(char vigenereTable[52][52]) {
  
  char alphabet[52];

  for (int i = 0; i < 26; ++i) //алфавит - заполняем заглавными буквами (диапазон 65 - 90)
  {
    alphabet[i] = i + 65;
  }
  for (int i = 26; i < 52; ++i) //алфавит - заполняем  буквами (диапазон 97 - 122)
  {
    alphabet[i] = i + 97 - 26;
  }
  for (int i = 0; i < 52; ++i)
  {
    printf("%c", alphabet[i]);
    printf("\n");
  }

  int shift = 0;

  for (int i = 0; i < 52; ++i)
  {
    for (int k = 0; k < 52; ++k)
    {
      char result = alphabet[k] + shift;
      if (result <= 90)
      {
        vigenereTable[i][k] = result;
      } else if (result > 90 && result < 97)
      {
        vigenereTable[i][k] = result + 6;
      } else if (result > 122)
      {
        vigenereTable[i][k] = result - 52 - 6;
      }

      // vigenereTable[i][k] = result <= 90 //если вышли за пределы нашего диапазона - начинаем с начала диапазона
      //           ? result
      //           : result - 26;
      unsigned char c = vigenereTable[i][k];
      printf("%u ", c);
    }
    printf("\n");
    shift++;
  }
}

void code_text(const char* source, const char* key, FILE *destination) {

  int symbolCount = strlen(source);
  int keyPosition = 0;

  for (int i = 0; i <= symbolCount; ++i)
  {
    if (isalpha(source[i])) //если буква, кодируем ее и отправляем в destination файл
    {
      char symbol = toupper(source[i]); 

      int symbolIndex = symbol - 65; //индексы в таблице Виженера
      int keyIndex = key[keyPosition] - 65;

      char encoded = vigenereTable[symbolIndex][keyIndex];
      fputc(encoded, destination);
    
      keyPosition = keyPosition < keyLenght -1 //если мы дошли до конца ключа, вернуться к началу
                    ? keyPosition + 1
                    : 0; 
    }
    else {
      fputc(source[i], destination); //НЕ буквы просто записываем, их нет в таблице
    }
  } 
}

void decode_text(const char* source, const char* key, FILE *destination) {

  int symbolCount = strlen(source);
  int keyPosition = 0;

  for (int i = 0; i <= symbolCount; ++i)
  {
    if (isalpha(source[i]))
    {
      char symbol = toupper(source[i]); 
      int keyIndex = key[keyPosition] - 65;

      int columnNumber = 0; //пройдемся по всем столбцам строки, на которой буква из ключа
      while(vigenereTable[keyIndex][columnNumber] != symbol){
          columnNumber++;
      }
      char decoded = vigenereTable[0][columnNumber]; //когда найдем совпадение, запишем значение из 0 строки, т.к. там не было сдвига
      fputc(decoded, destination);
  
      keyPosition = keyPosition < keyLenght -1
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

  char *opts = "hk:cd"; //k - с обязательным параметром
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
        if  (!is_valid(optarg)) {
            fprintf(stderr, "\"%s\" - invalid KEY!\n%s", optarg, OFFER_HELP); 
            exit(EXIT_FAILURE);
        }
        key =  optarg; 
        keySet = 1; //если ключ есть и он валидный, true
        break;

      case 'c':
        code = 1;
        break;

      case 'd':
        decode = 1;
        break;
    }
  }

  if( !(code ^ decode) ) { //если оба 0 или оба 1 - неверно
    fprintf(stderr, "You should set one of -c/-d options\n%s", OFFER_HELP); 
    exit(EXIT_FAILURE);
  }

  if( !keySet ) { //если не установили ключ
    fprintf(stderr, "-k is required option\n%s", OFFER_HELP); 
    exit(EXIT_FAILURE);
  }

  FILE *source;
  FILE *destination;

  switch(argc){ //здесь устанавливаем source и destination в зависимости от количества аргументов

      case 4:
        source = stdin;
        destination = stdout;

        break;

      case 5:
        source = open_file_or_throw(argv[4], "r");
        destination = stdout;

        break;

      case 6:
        source = open_file_or_throw(argv[4], "r");
        destination = open_file_or_throw(argv[5], "w");
        
        break;

      default:
        fprintf(stderr, "Too many arguments!\n%s", OFFER_HELP); 
        exit(EXIT_FAILURE);
  }
    
  char *textToEncrypt = (char*)malloc(sizeof(char)); //сначала один байт
  int symbolCount = 0; //и нет символов
  char symbol;

  while (1) 
  {
      symbol = fgetc(source);

      if (symbol == EOF) //если встретили конец выйти СРАЗУ - НЕ ЗАПИСЫВАТЬ
      {
        break;
      }

      textToEncrypt[symbolCount] = symbol;
      symbolCount++; //количество символов в строке увеличилось
      textToEncrypt = (char*)realloc(textToEncrypt, sizeof(char)*(symbolCount + 1)); //добавляем для следующего символа
  } 
  textToEncrypt[symbolCount + 1] = 0; //чтобы была корректная строка
  fclose(source);

  fill_vigener_table(vigenereTable);  
  keyLenght = strlen(key);  

  for (int i = 0; i < keyLenght; ++i)
  {
    key[i] = toupper(key[i]); //все к аппер - ведь в таблице все заглавные
  }

  if (code){
    code_text(textToEncrypt, key, destination);
  } 
  else { 
    decode_text(textToEncrypt, key, destination);
  }
  
  fputc('\n', destination); //чтобы красиво

  free(textToEncrypt); //освободим память
  fclose(destination);
    
  return 0;
}