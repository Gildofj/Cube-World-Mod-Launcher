#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <string_view>

bool FileExists(std::string_view fileName);
int Bail(int result);
int main(int argc, char** argv);

#endif // MAIN_H_INCLUDED
