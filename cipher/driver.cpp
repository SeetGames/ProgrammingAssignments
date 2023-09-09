#include <iostream>
#include <cstdint>
#include <string>
#include <cstring>   // for C-style strlen and strcmp
#include <ctime>     // for C-style random number generation
#include <cstdlib>   // for C-style random number generation
#include "cipher.h"

namespace {
  void encode_decode(char const *);
  void test0(); void test1(); void test2(); void test3();
  void test4(); void test5(); void test6(); void test7();
}

int main (int argc, char **argv) {
  constexpr int max_tests{8};
  void (*pTests[max_tests])() = {test0, test1, test2, test3, test4, test5, test6, test7};
  
  if (argc > 1) {
    int test = std::stoi(std::string(argv[1]));
    test = test > 0 ? test : -test;
    if (test < max_tests) {
      pTests[test]();
    }
  }
}

namespace {
void encode_decode(char const *plaintext) {
  int32_t buff_size = strlen(plaintext);
  char *encryptedtext = new char [buff_size]{}; // more then needed, so some space will be wasted
  char *decodedtext   = new char [buff_size+1]{};
  int32_t num_bits_used; 
  
  encode(plaintext, encryptedtext, &num_bits_used);
  print_bits(encryptedtext, 0, num_bits_used);
  decode(encryptedtext, buff_size, decodedtext);
  std::cout << decodedtext << '\n';

  delete [] decodedtext;
  delete [] encryptedtext;
}

void test0() { encode_decode("a"); }
void test1() { encode_decode("ab"); }
void test2() { encode_decode("cccc"); }
void test3() { encode_decode("xaxa"); }
void test4() { encode_decode("abcdefghijklmnopqrstuvwxyz"); }
void test5() { encode_decode("kdjasfhkdslfhksdjhfkldsfhlskdfjh"); }
void test6() { encode_decode("aardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvarkaardvark"); }

void test7() {
  int32_t buff_size{100'000};
  char *plaintext     = new char [buff_size+1]{};
  char *encryptedtext = new char [buff_size+1]{};
  char *decodedtext    = new char [buff_size+1]{};

  // randomly generate plaintext
  srand(time(NULL));
  for (int32_t i{}; i < buff_size; ++i) {
    plaintext[i] = 'a' + rand()%26;
  }
  plaintext[buff_size] = 0;

  int32_t num_bits_used; 
  encode(plaintext, encryptedtext, &num_bits_used);
  decode(encryptedtext, buff_size, decodedtext);
  if (strcmp(plaintext, decodedtext) == 0) {
    std::cout << "All good\n";
  }

  delete [] decodedtext;
  delete [] encryptedtext;
  delete [] plaintext;
}
} // end namespace 
