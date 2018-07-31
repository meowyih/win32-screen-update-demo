# Sample code shows how to refresh screen using memory buffer directly in win32

Win32 provides HDC/GDI+ to let developer to draw the Window. However, the performance is not acceptable if you are developing a game with 30fps or 60fps. In that case, draw and update on the memory directly is a requirement. I wrote this simple demo shows how to do it.

## Create a standard win32 project using Visual studio

Of course, you have to start with a win32 project.

## Remove window title bar and add timer 

Modify the code from:

``` c++
HWND hWnd = CreateWindowW(
	szWindowClass, 
	szTitle, 
	WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, 0, 
	CW_USEDEFAULT, 0, 
	nullptr, nullptr, 
	hInstance, nullptr);
```

to:

``` c++
// create a window without title bar
HWND hWnd = CreateWindowW(
	szWindowClass, szTitle, 
	WS_BORDER | WS_MAXIMIZE,
	CW_USEDEFAULT, 0, 
	CW_USEDEFAULT, 0, 
	nullptr, nullptr, 
	hInstance, nullptr);

// set a timer for every 100 millisecond
SetTimer(hWnd, 100, 100, NULL);
```

2. Remove menu

Modify the code from:

``` c++
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    ...
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ENGINE3D);
    ...
}
```

to:

``` c++
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    ...
    wcex.lpszMenuName   = NULL;
    ...
}
```

## Add timer and paint procedure

Modify the code from

``` c++
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
...
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ...
            EndPaint(hWnd, &ps);
        }
        break;
...
}
```

to

``` c++
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
...
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            OnPaint(HDC hdc); // paint the window
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_TIMER:
		if (wParam == 100 )
		{
			InvalidateRect(hWnd, NULL, FALSE); // update whole window
		}
		break;
...
}
```

## Implement OnPaint(HDC hdc) function mentioned in previous section

``` c++
VOID  OnPaint(HDC hdc)
{
	// get window's width, height and bytes per pixel
	BITMAP structBitmapHeader;
	memset(&structBitmapHeader, 0, sizeof(BITMAP));

	HGDIOBJ hBitmap = GetCurrentObject(hdc, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(BITMAP), &structBitmapHeader);

	LONG width = structBitmapHeader.bmWidth;
	LONG height = structBitmapHeader.bmHeight;
	WORD bytesPerPixel = structBitmapHeader.bmBitsPixel / 8;

	// allocate pixel buffer for drawing
	//
	// NOTE: this is BAD practice, programmer should
	//       allocate buffer only once if the window size
	//       didn't change.
	LONG bufferSize = width * height * bytesPerPixel;
	BYTE* buffer = new BYTE[bufferSize];

	// directly draw on the buffer
	for (int i = 0; i < bufferSize; i = i + bytesPerPixel)
	{
		buffer[i] = 0;     // RED
		buffer[i + 1] = 0; // GREEN
		buffer[i + 2] = 0; // BLUE
	}

	// update the screen with buffer
	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biBitCount = structBitmapHeader.bmBitsPixel;
	bih.biClrImportant = 0;
	bih.biClrUsed = 0;
	bih.biCompression = BI_RGB;
	bih.biWidth = structBitmapHeader.bmWidth;
	bih.biHeight = structBitmapHeader.bmHeight;
	bih.biPlanes = 1;
	bih.biSizeImage = bufferSize;

	BITMAPINFO bi;
	bi.bmiHeader = bih;
	SetDIBitsToDevice(
		hdc, 0, 0, 
		width, height, 
		0, 0, 0, height,
		buffer, &bi, DIB_RGB_COLORS);

	// release buffer
	delete[] buffer;
}
```
