#include "Core.h"
#include "render.h"
#include "cl_user.h"
#include <ctime>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;


constexpr unsigned int Width = 1280;
constexpr unsigned int Height = 800;

vec_t k = (vec_t)Width / Height;
vec_t FOV = 60;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void BSP_init(bspmap_t* tree) {
	system("cls");
	std::cout << ".bsp filename path is: " << std::endl;
	std::string filename;
	
	fs::path current_path = fs::current_path();

	for (const auto& entry : fs::directory_iterator(current_path)) {
		if (fs::is_regular_file(entry.path()) && (entry.path().extension() == ".bsp")) {
			std::cout << entry.path().filename().string() << '\n';
		}
	}

	std::cout << std::endl << ">>> ";
	std::getline(std::cin, filename);


	LoadBSPtree(filename, tree);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	bspmap_t tree;


	WNDCLASSEX wcex;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;

	/* register window class */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		"GLSample",
		"OpenGL Sample",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		Width,
		Height,
		NULL,
		NULL,
		hInstance,
		NULL);

	/* enable OpenGL for the window */


	EnableOpenGL(hwnd, &hDC, &hRC);
	while (!tree.dmodels)
		BSP_init(&tree);
	LoadTexturesToGPU();

	ShowWindow(hwnd, nCmdShow);

	glLoadIdentity();
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	vec_t d0 = GetFrustumDist(FOV);
	glFrustum(-k,  k, -1, 1, d0, 50000);
	
	/* program main loop */
	size_t start = clock();
	size_t end = clock();

	size_t deltatime;
	size_t TimeError = 0;
	float dt;

	entity p({ -50, -50, -50 }, { M_PI / 2, 0, 0 });

	size_t fps = 0;
	size_t truefps = 0;
	char title[256];

	while (!bQuit)
	{
		deltatime = end - start;
		MovePlayerLook(&p, hwnd, Width, Height);
		if (deltatime) {
			vec_t vel = p.GetVel().length();
			vec3_t pos = p.GetPos();
			sprintf_s(title, "FPS: %4i, pos: (%i, %i, %i), vel: %i\n", truefps, (int)pos[0], (int)pos[1], (int)pos[2], (int)vel);
			SetWindowText(hwnd, title);
			start = clock();
			dt = deltatime * 0.001;
			TimeError += deltatime;
			Movement(&p, dt, &tree);
		}

		nodenum = tree.hullheadnum[0];
		while (nodenum >= 0) {
			bool t = tree.dplanes[tree.dnodes[nodenum].planenum].DistToPlane(p.GetPos()) < 0;
			nodenum = tree.dnodes[nodenum].children[t];
		}

		//printf("Nodenum %i\n", -(nodenum + 1));

		if (TimeError >= 1000){
			truefps = fps;
			TimeError %= 1000;
			fps = 0;
		}



		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/* OpenGL animation code goes here */

			glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glPushMatrix();
				vec3_t angle = p.GetAngle();
				vec3_t pos = p.GetPos();
				MoveCamera(angle, pos);
					
				if (0) {
					for (int i = 0; i < tree.numfaces; i++) {
						DrawPolygon(&tree.dfaces[i], 1, &tree, true);
					}
				}
				else {
					processed = 0;
					PaintersAlgorithm(tree.hullheadnum[0], &tree, pos, GetLookVector(angle));
				}
				
			
			glPopMatrix();
			

			SwapBuffers(hDC);

			
			fps++;
		}
		end = clock();
	}

	/* shutdown OpenGL */
	DisableOpenGL(hwnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow(hwnd);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		return 0;

	case WM_SETCURSOR:
		ShowCursor(FALSE);

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}

