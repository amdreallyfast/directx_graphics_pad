// include the basic windows header file
#include <Windows.h>

// ??for what??
#include <windowsx.h>

// this is the main message handler for the program
LRESULT CALLBACK my_window_proc(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param)
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
   return DefWindowProc(handle_window, message, w_param, l_param);
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
   HWND handle_window;

   // this struct will hold information for the window class
   // Note: WNDCLASS is depreciated.  It was superseded by WNDCLASSEX, so use that instead.
   WNDCLASSEX wc;
   ZeroMemory(&wc, sizeof(WNDCLASSEX));

   // fill the struct with the needed information
   wc.cbSize = sizeof(WNDCLASSEX);           // tell the struct how big it is (??why doesn't it know how big itself is??)
   wc.style = CS_HREDRAW | CS_VREDRAW;       // redraw on vertical and horizontal resizing
   wc.lpfnWndProc = my_window_proc;          // this is the function that the window class calls when it gets a message from windows (keystroke, mouse movement, etc.)
   wc.hInstance = h_instance;                // the handle to our application instance
   wc.hCursor = LoadCursor(NULL, IDC_ARROW); // don't give it an application handle that stores a pointer graphic (we don't have one anyway), and give it the default moust pointer
   wc.hbrBackground = (HBRUSH)COLOR_WINDOW;  // give the window class's background the default window color
   wc.lpszClassName = L"WindowClass1";       // give the window class a name (when creating a window with this window class, you must specify this string exactly)

   // register the window class so that Windows can use it to create windows
   RegisterClassEx(&wc);

   // calculate the size of the window based on how big we want the client (the space within the window borders) to be
   // Note: This "adjust window rectangle" function conveniently calculates window dimensions given information about
   // the dimensions of the size and origin of the client area, the window style, and whether the window has menus or 
   // not (FALSE in this case).  The last item is some information about extended window styles for "Extended Window".
   // We are not using any such styles, so we provide NULL.
   RECT window_rectangle = { 0, 0, 500, 400 };
   AdjustWindowRectEx(&window_rectangle, WS_OVERLAPPEDWINDOW, FALSE, NULL);

      // create the window on the screen
   handle_window = CreateWindowEx(
      NULL,                               // do not use any extra style options provided by "Extended Window"
      L"WindowClass1",                    // name of the window class that this window will be an instance of (must match an existing window class name)
      L"Our First Windowed Program",      // displayed on the window's title bar
      WS_OVERLAPPEDWINDOW,                // this option specifies several standard features and make a basic window style (check out the #define for more details)
      300,                                // x-position of the window relative to the desktop
      300,                                // y-position of the window relative to the desktop
      window_rectangle.right - window_rectangle.left,    // width of the window (origin at left, increases left to right)
      window_rectangle.bottom - window_rectangle.top,    // height of the window (origin at top, increases top to bottom)
      NULL,                               // we have no parent window, so do not provide a handle to a window
      NULL,                               // we are not using menus, so do not provide a handle to a menu
      h_instance,                         // application handle determined by Windows when the program starts; identifies this window with our application
      NULL);                              // used with multiple windows, which we are not using

   // display the window on the screen
   ShowWindow(handle_window, n_show_cmd);

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


