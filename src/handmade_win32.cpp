#include <cstdio>
#include <windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nShowCmd) {
  MessageBox(NULL, "X", "Y", MB_OK);
  return (0);
}

// int main() {
//   printf("hello world\n");
//   return (0);
// }
