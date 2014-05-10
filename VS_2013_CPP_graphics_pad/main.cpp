// include the basic windows header file
#include <Windows.h>

// this is the entry point for any Windows program
// Note: It is a form of main(...), but made for Windows applications (that is, non-console applications).
int WINAPI WinMain(
   HINSTANCE h_instance, 
   HINSTANCE h_prev_instance, 
   LPSTR lp_cmd_line, 
   int n_show_cmd)
{
   MessageBox(
      NULL,
      L"hello world",
      L"just another 'hello world' program!",
      MB_ICONEXCLAMATION | MB_OK);


   return 0;
}


