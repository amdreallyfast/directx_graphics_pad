// include the basic windows header file
#include <Windows.h>

// ??for what??
#include <windowsx.h>

// this is the main message handler for the program
LRESULT CALLBACK my_window_proc(HWND h_wind, UINT message, WPARAM w_param, LPARAM l_param)
{
   // sort through and find what code to run for the message given
   switch (message)
   {
   case WM_DESTROY:
   {
      // this message is read when the window is closed (that is, its deconstructor has been called)
      PostQuitMessage(0);
      return 0;
      break;
   }
   default:
      break;
   }

   // handle any messages that the switch statement didn't
   //??what does this do??
   return DefWindowProc(h_wind, message, w_param, l_param);
}

// this is the entry point for any Windows program
// Note: It is a form of main(...), but made for Windows applications (that is, non-console applications).
// Note: I had to change the project properties->linker->system->subsystem from "Console" to "Windows" to get it to compile correctly.
int WINAPI WinMain(
   HINSTANCE h_instance,
   HINSTANCE h_prev_instance,
   LPSTR lp_cmd_line,
   int n_show_cmd)
{
   // this will be the handle for the window (hence the acronym)
   HWND h_wind;

   // this struct will hold information for the window class
   WNDCLASSEX wc;
   ZeroMemory(&wc, sizeof(WNDCLASSEX));

   // fill the struct with the needed information
   wc.cbSize = sizeof(WNDCLASSEX);
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = my_window_proc;
   wc.hInstance = h_instance;
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
   wc.lpszClassName = L"WindowClass1";

   // register the window class
   RegisterClassEx(&wc);

   // create the window on the screen
   h_wind = CreateWindowEx(
      NULL,                               // ??
      L"WindowClass1",                    // // name of the window class
      L"Our First Windowed Program",      // title of the window
      WS_OVERLAPPEDWINDOW,                // window style
      300,                                // x-position of the window
      300,                                // y-position of the window
      500,                                // width of the window
      400,                                // height of the window
      NULL,                               // we have no parent window
      NULL,                               // we are not using menus
      h_instance,                         // application handle determined by Windows when the program starts; identifies this window with out application
      NULL);                              // used with multiple windows, which we are not using

   // display the window on the screen
   ShowWindow(h_wind, n_show_cmd);

   // enter the main loop

   // this struct holds Windows event messages
   MSG event_message;

   // wait for the next message in the queue, then store the result the message variable
   while (GetMessage(&event_message, NULL, 0, 0))
   {
      // translate keystroke messages into the right format
      TranslateMessage(&event_message);

      // send the message to the WindowProc function
      DispatchMessage(&event_message);
   }

   // return this part (??which part??) of the WM_QUIT message to Windows
   return 0;
}


