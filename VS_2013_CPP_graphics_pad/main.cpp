// include the basic windows header file and an "extensions" header
#include <Windows.h>
#include <windowsx.h>

// for function declarations of Direct3D basic and extended functions
#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10.h>

// include the Direct3D library files
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x86/d3dx11.lib")
#pragma comment (lib, "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x86/d3dx10.lib")

// the pointer to the interface for the swap chain (that is, the frame buffer)
IDXGISwapChain *g_swap_chain_ptr;

// the pointer to our Direct3D device interface, which is our program's representation of our video adapter and mostly handles video memory
ID3D11Device *g_dev_ptr;

// the pointer to our Direct3d device context, which manages the GPU and the rendering pipeline
ID3D11DeviceContext *g_dev_context_ptr;

// a pointer to the back buffer in the swap chain; this is the render target (as opposed to the front buffer)
ID3D11RenderTargetView *g_render_target_ptr;

// a pointer to the vertex buffer
ID3D11Buffer *g_vertex_buffer_ptr;

// a pointer to the input layout of the vertex buffer
ID3D11InputLayout *g_input_layout_ptr;

// these are self-explanatory
//#define SCREEN_WIDTH    1152
//#define SCREEN_HEIGHT   864
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

// shader objects are global so that they will persist throughout the program
ID3D11VertexShader *g_vertex_shader_ptr;
ID3D11PixelShader *g_pixel_shader_ptr;

// vertices for our first triangle
struct MY_VERTEX
{
   //D3DXVECTOR3 position;    // position
   float x, y, z;    // position
   D3DXCOLOR color;  // color
};
MY_VERTEX g_my_vertices_arr[] =
{
   //{ D3DXVECTOR3(+0.0f, +0.5f, +0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
   //{ D3DXVECTOR3(+0.45f, -0.5f, +0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
   //{ D3DXVECTOR3(-0.45f, -0.5f, +0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
   { +0.0f, +0.5f, +0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
   { +0.45f, -0.5f, +0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
   { -0.45f, -0.5f, +0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
};


// initialize the Direct3D pipeline
void init_pipeline(void)
{
   // load and compile the two shaders
   ID3D10Blob *vertex_shader_blob_ptr;
   ID3D10Blob *pixel_shader_blob_ptr;
   ID3D10Blob *VS, *PS;
   D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &vertex_shader_blob_ptr, 0, 0);
   D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &pixel_shader_blob_ptr, 0, 0);
   //D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &vertex_shader_blob_ptr, 0, 0);
   //D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &pixel_shader_blob_ptr, 0, 0);

   // encapsulate bother shader blobs into their own shader objects
   g_dev_ptr->CreateVertexShader(vertex_shader_blob_ptr->GetBufferPointer(), vertex_shader_blob_ptr->GetBufferSize(), NULL, &g_vertex_shader_ptr);
   g_dev_ptr->CreatePixelShader(pixel_shader_blob_ptr->GetBufferPointer(), pixel_shader_blob_ptr->GetBufferSize(), NULL, &g_pixel_shader_ptr);

   // register the shader objects with the graphical context
   g_dev_context_ptr->VSSetShader(g_vertex_shader_ptr, 0, 0);
   g_dev_context_ptr->PSSetShader(g_pixel_shader_ptr, 0, 0);


   // now for the actual vertex data

   // describe how the data is layed out in the vertex buffer
   // - Create input element descriptions for each unique item in my custom vertex.  In this case, I have a position, and 3 floats later I have a color.
   // - Create an input layout object and assign it to the global input layout pointer.
   // - Register the input layout object with the context so that it can make sense of the data in the vertex buffer.
   D3D11_INPUT_ELEMENT_DESC input_elem_desc_arr[] =
   {
      // the position will use 3 32bit floats
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

      // the color will use 4 32bit floats
      { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA },
   };
   g_dev_ptr->CreateInputLayout(
      input_elem_desc_arr,
      sizeof(input_elem_desc_arr) / sizeof(D3D11_INPUT_ELEMENT_DESC),
      vertex_shader_blob_ptr->GetBufferPointer(),
      vertex_shader_blob_ptr->GetBufferSize(),
      &g_input_layout_ptr);
   g_dev_context_ptr->IASetInputLayout(g_input_layout_ptr);
}

// initialize ??what are we initializing? "graphics" is not very descriptive
void init_graphics(void)
{
   // create a buffer description structure that describes the data to be held (we want vertex data), then tell the video adapter handling object to create such this buffer in GPU memory
   D3D11_BUFFER_DESC vertex_buffer_desc;
   ZeroMemory(&vertex_buffer_desc, sizeof(vertex_buffer_desc));
   vertex_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;             // an optimization lable; basically says that CPU is only allowed to write to this buffer (not read from), GPU only allowed to read from this buffer (not write to)
   vertex_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // apparently the "usage" field needs clarification; states that CPU is only allowed write permission (I don't understand MSDN's article on it)
   vertex_buffer_desc.ByteWidth = sizeof(MY_VERTEX) * 3;       // "times 3" because we are sending three vertices
   vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // label this buffer as being a buffer for vertex data

   // create the buffer with the description that was just filled out and set the global vertex buffer pointer to point at it
   // Note: Don't provide initializing information.
   g_dev_ptr->CreateBuffer(&vertex_buffer_desc, NULL, &g_vertex_buffer_ptr);

   // now that the buffer is created, fill out the buffer with its initial data
   // Note: Direct3D may be working with the buffer in the background, so we are not allowed direct access to it.  The global vertex buffer pointer is a pointer to an object, not the buffer's memory
   // address, so we cannot access the memory directly from the vertex buffer pointer.  We can, however, map the buffer to the context, which will wait for the GPU to finish accessing the buffer,
   // then block the GPU from accessing it and allow the CPU to access it.  Then we send the data to the video memory using the CPU, and then we unmap the buffer so that the CPU is blocked and the
   // GPU is unblocked.
   D3D11_MAPPED_SUBRESOURCE mapped_subresource;
   g_dev_context_ptr->Map(g_vertex_buffer_ptr, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapped_subresource);
   size_t x = sizeof(g_my_vertices_arr);
   memcpy(mapped_subresource.pData, g_my_vertices_arr, sizeof(g_my_vertices_arr));
   g_dev_context_ptr->Unmap(g_vertex_buffer_ptr, NULL);
}

// sets up and initializes Direct3D
void init_d3d(HWND window_handle)
{
   HRESULT r = 0;

   // create a struct to hold information about the swap chain
   DXGI_SWAP_CHAIN_DESC swap_chain_desc;
   ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

   // fill the swap chain description structure
   swap_chain_desc.BufferCount = 1;                                  // one back buffer and one front buffer (??is there only one buffer total??) (we could specify more, but we will just start with 1)
   swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   // use 32-bit color (8 red bits, 8 green bits, 8 blue bits, 8 alpha bits)
   swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
   swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
   swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // render the target output to the back buffer of the swap chain (that is, not the currently displayed one)
   swap_chain_desc.OutputWindow = window_handle;                     // the handle of the window that Direct3D will draw in
   swap_chain_desc.SampleDesc.Count = 4;                             // how many multisamples Direct3D will perform when blending pixels (that is, anti-aliasing)
   swap_chain_desc.Windowed = TRUE;                                  // windowed mode (that is, not full-screen mode) because our window is not full screen
   swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // allow full-screen/windowed switch when user inputs Alt-Enter 

   // create a device, device context, and swap chain using the information in the swap chain structure
   r = D3D11CreateDeviceAndSwapChain(
      NULL,                         // adapter; tell Direct3D to use the default graphics adapter
      D3D_DRIVER_TYPE_HARDWARE,     // driver type; tell Direct3D to use the graphics adapter's hardware for rendering (there are other options, but we will use this one)
      NULL,                         // handle to software module; the driver type is not "use software", so use NULL
      NULL,                         // flags; we are using the defaults, so specify NULL
      NULL,                         // feature level; we don't know anything about my particular video card other than that it is DirectX 11 compliant, so use NULL
      NULL,                         // number of feature levels; we don't have any feature levels, so use NULL
      D3D11_SDK_VERSION,            // DirectX SDK version; used for backwards compatability so that Direct3D knows how to adapt varying DirectX 11 versions (our version) to our program
      &swap_chain_desc,             // pointer to the swap chain description structure
      &g_swap_chain_ptr,                // a pointer to a pointer to the swap chain object
      &g_dev_ptr,                       // a pointer to a pointer to the Direct3D device object
      NULL,                         // a pointer to a feature level variable; we are not using special features, so use NULL
      &g_dev_context_ptr                // a pointer to a pointer to the device context object
      );

   // initialize the render target
   // This process is how we make a render target object make it point to the swap chain's back buffer.
   // - Create a pointer to a 2D texture object.  This is an intermediate object that basically carries the back buffer's pointer until the render target is created.
   // - Create a 2D texture object from the back buffer in the swap chain, and get its address.
   // - Create a "render target view" object, and put the pointer into the global render target pointer.
   // - Release the 2D texture object since it's address carrying purpose is finished.
   // - Bind the our list of render targets (in this case, of length 1) to the "output-merger" stage of the pipeline.
   ID3D11Texture2D *back_buffer_ptr;
   r = g_swap_chain_ptr->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);
   r = g_dev_ptr->CreateRenderTargetView(back_buffer_ptr, NULL, &g_render_target_ptr);
   r = back_buffer_ptr->Release();
   g_dev_context_ptr->OMSetRenderTargets(1, &g_render_target_ptr, NULL);

   // set the viewpoint
   // Normalize the viewport from "0 -> width/height" to "(-1,-1) to (1,1) with (0,0) at the center.
   // Also, only use one viewport.
   D3D11_VIEWPORT viewport;
   ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
   viewport.TopLeftX = 0;
   viewport.TopLeftY = 0;
   viewport.Width = SCREEN_WIDTH;
   viewport.Height = SCREEN_HEIGHT;
   g_dev_context_ptr->RSSetViewports(1, &viewport);

   init_pipeline();
   init_graphics();
}

// closes Direct3D and releases memory
void clean_d3d(void)
{
   // whenever Direct3D is created, it must be closed down
   // Note: Direct3D is actually incapable of closing down when in fullscreen mode, so force it into windowed mode when closing.

   // force it into windowed mode
   // Note: The second argument is a feature that allows you to select a particular video adapter manually.  Set it to NULL, and it will choose the correct adapter automatically (??how do I know??).
   g_swap_chain_ptr->SetFullscreenState(FALSE, NULL);

   // release global resources
   // Note: Resources for DirectX are created at the Windows level, and if not released, they will remain running/in memory until the next program restart.
   g_vertex_shader_ptr->Release();
   g_pixel_shader_ptr->Release();
   g_vertex_buffer_ptr->Release();
   g_input_layout_ptr->Release();
   g_swap_chain_ptr->Release();
   g_render_target_ptr->Release();
   g_dev_ptr->Release();
   g_dev_context_ptr->Release();
}


// render a single frame
void render_frame(void)
{
   // clear the render target (this should be the back buffer) to a deep blue
   g_dev_context_ptr->ClearRenderTargetView(g_render_target_ptr, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));


   // do 3D render to the render target (this should be the back buffer)

   // select which vertex buffer to display (we only have one, so this is simple)
   UINT stride = sizeof(MY_VERTEX);
   UINT offset = 0;
   g_dev_context_ptr->IASetVertexBuffers(0, 1, &g_vertex_buffer_ptr, &stride, &offset);

   // select the drawing routine for the primitives that will be drawn
   g_dev_context_ptr->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

   // draw the vertex buffer to the render target
   g_dev_context_ptr->Draw(3, 0);


   // switch the back buffer and the front buffer
   g_swap_chain_ptr->Present(0, 0);
}


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
      // no need for "break" because "return" is used
   }
   default:
      break;
   }

   // handle any messages that the switch statement didn't
   // Note: From http://msdn.microsoft.com/en-us/library/windows/desktop/ms633572(v=vs.85).aspx,
   // "Calls the default window procedure to provide default processing for any 
   // window messages that an application does not process. This function ensures 
   // that every message is processed. DefWindowProc is called with the same 
   // parameters received by the window procedure."
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
   wc.hCursor = LoadCursor(NULL, IDC_ARROW); // don't give it an application handle that stores a pointer graphic (we don't have one anyway), and give it the default mouse pointer
   //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;  // give the window class's background the default window color
   wc.lpszClassName = L"WindowClass1";       // give the window class a name (when creating a window with this window class, you must specify this string exactly)

   // register the window class so that Windows can use it to create windows
   RegisterClassEx(&wc);

   // calculate the size of the window based on how big we want the client (the space within the window borders) to be
   // Note: This "adjust window rectangle" function conveniently calculates window dimensions given information about
   // the dimensions of the size and origin of the client area, the window style, and whether the window has menus or 
   // not (FALSE in this case).  The last item is some information about extended window styles for "Extended Window".
   // We are not using any such styles, so we provide NULL.
   RECT window_rectangle = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
   AdjustWindowRectEx(&window_rectangle, WS_OVERLAPPEDWINDOW, FALSE, NULL);

   // create the window on the screen
   handle_window = CreateWindowEx(
      NULL,                               // do not use any extra style options provided by "Extended Window"
      L"WindowClass1",                    // name of the window class that this window will be an instance of (must match an existing window class name)
      L"Our First Direct3D Program",      // displayed on the window's title bar
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

   // set up and initialize Direct3D
   init_d3d(handle_window);


   // enter the main loop

   // this struct holds Windows event messages
   MSG event_message = { 0 };

   // enter the loop and stay here forever
   while (true)
   {
      // check to see if any messages are waiting in the queue, and remove them from the queue if you find them
      if (PeekMessage(&event_message, NULL, 0, 0, PM_REMOVE))
      {
         // translate keystroke messages into the right format, then send them to the "window processing" function for handling
         TranslateMessage(&event_message);
         DispatchMessage(&event_message);

         // check to see if it is time to quite, and if it is, break out of the loop and return from main
         // Note: The window will not send a WM_QUIT message.  It is called when the application (in this
         // case, our code) calls the PostQuitMessage(...) function, which in turn will only be called if
         // the message was WM_DESTROY.
         // Previously, we used the return value of GetMessage(...) as the condition in the while(...) 
         // loop, and that function would return false if it got a WM_QUIT message.  Now the loop 
         // condition is always true, so we need to manually check for WM_QUIT and stop the application
         // if we see it.
         if (WM_QUIT == event_message.message)
         {
            break;
         }
      }

      // render a frame, regardless of whether a message was handled or not
      render_frame();
   }

   clean_d3d();

   // return this part (??which part??) of the WM_QUIT message to Windows
   return event_message.wParam;
}


