#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include <conio.h>
#include <string>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")

#define WINDOW_TITLE "Bridge"
#define PI 3.141592654f

// Mode
int currentMode = 0;

// Screen
const float screenWidth = 800, screenHeight = 600;

// Camera
float orthoLeft = -1.0f, orthoRight = 1.0f, orthoBottom = -1.0f, orthoTop = 1.0f, orthoZNear = 0, orthoZFar = 10.0f;
float prespFovy = 60;
float prespLeft = -1.0f, prespRight = 1.0f, prespBottom = -1.0f, prespTop = 1.0f, prespZNear = 0, prespZFar = 10.0f;
float lastCursorPosX, lastCursorPosY;
float cameraPosX = 0, cameraPosY = 0, cameraPosZ = 0, cameraRotX = 0, cameraRotY = 0;
POINT cursorPos;

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CHAR:
		switch (currentMode) {
		case 0: // Ortho Camera
			switch (wParam) {
			case 0x41: // A L-
				orthoLeft--;
				break;
			case 0x61: // a L+
				orthoLeft++;
				break;

			case 0x44: // D R+
				orthoRight++;
				break;
			case 0x64: // d R-
				orthoRight--;
				break;

			case 0x57: // W T+
				orthoTop++;
				break;
			case 0x77: // w T-
				orthoTop--;
				break;

			case 0x53: // S B-
				orthoBottom--;
				break;
			case 0x73: // s B+
				orthoBottom++;
				break;

			case 0x51: // Q N+
				orthoZNear++;
				break;
			case 0x71: // q N-
				if (orthoZNear > 0) {
					orthoZNear--;
				}
				break;

			case 0x45: // E F+
				orthoZFar++;
				break;
			case 0x65: // e F-
				if (orthoZFar > 0) {
					orthoZFar--;
				}
				break;
			}
			break;
		case 1: // Presp Camera
			switch (wParam) {
			case 0x5A: // FOVY-
				prespFovy--;
				break;

			case 0x43: // FOVY+
				prespFovy++;
				break;
			}
			break;
		}

		break;

	case WM_KEYDOWN:
		float xrotrad, yrotrad;

		// Switch current mode
		switch (wParam) {
		case 0x31: // Camera Mode
			currentMode = 0;
			break;
		case 0x32:
			currentMode = 1;
			break;

			// Eye movement
		case 0x57: // Ww
			yrotrad = (cameraRotY / 180 * PI);
			xrotrad = (cameraRotX / 180 * PI);
			cameraPosX += float(sin(yrotrad)) * 0.05;
			cameraPosZ -= float(cos(yrotrad)) * 0.05;
			cameraPosY -= float(sin(xrotrad)) * 0.05;
			break;
		case 0x53: // Ss
			yrotrad = (cameraRotY / 180 * PI);
			xrotrad = (cameraRotX / 180 * PI);
			cameraPosX -= float(sin(yrotrad)) * 0.05;
			cameraPosZ += float(cos(yrotrad)) * 0.05;
			cameraPosY += float(sin(xrotrad)) * 0.05;
			break;
		case 0x41: // Aa
			yrotrad = (cameraRotY / 180 * PI);
			cameraPosX -= float(cos(yrotrad)) * 0.05;
			cameraPosZ -= float(sin(yrotrad)) * 0.05;
			break;
		case 0x44: // Dd
			yrotrad = (cameraRotY / 180 * PI);
			cameraPosX += float(cos(yrotrad)) * 0.05;
			cameraPosZ += float(sin(yrotrad)) * 0.05;
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------
/*
	Basic Shape
*/

void drawSphere() {
	GLUquadricObj* var = gluNewQuadric();
	gluQuadricDrawStyle(var, GLU_LINE);
	gluSphere(var, 0.5f, 16, 16);
	gluDeleteQuadric(var);
}

void drawCylinder() {
	GLUquadricObj* var = gluNewQuadric();
	gluQuadricDrawStyle(var, GLU_LINE);
	gluCylinder(var, 0, 0.5f, 0.5f, 16, 16);
	gluDeleteQuadric(var);
}

void drawPyramid() {
	float points[5][3] = {
		{ 0.5f, 0, 0.5f }, // A [0][]
		{ -0.5f, 0, 0.5f }, // B [1][]
		{ -0.5f, 0, -0.5f }, // C [2][]
		{ 0.5f, 0, -0.5f }, // D [3][]
		{ 0, 0.5f, 0 }, // E [4][]
	};

	glBegin(GL_QUADS);
	{
		// Bottom (ABCD)
		glVertex3f(points[0][0], points[0][1], points[0][2]);
		glVertex3f(points[1][0], points[1][1], points[1][2]);
		glVertex3f(points[2][0], points[2][1], points[2][2]);
		glVertex3f(points[3][0], points[3][1], points[3][2]);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	{
		// Left (BCE)
		glVertex3f(points[1][0], points[1][1], points[1][2]);
		glVertex3f(points[2][0], points[2][1], points[2][2]);
		glVertex3f(points[4][0], points[4][1], points[4][2]);
		
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	{
		// Back (CDE)
		glVertex3f(points[2][0], points[2][1], points[2][2]);
		glVertex3f(points[3][0], points[3][1], points[3][2]);
		glVertex3f(points[4][0], points[4][1], points[4][2]);
		
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	{
		// Right (DAE)
		glVertex3f(points[3][0], points[3][1], points[3][2]);
		glVertex3f(points[0][0], points[0][1], points[0][2]);
		glVertex3f(points[4][0], points[4][1], points[4][2]);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	{
		// Front (ABE)
		glVertex3f(points[0][0], points[0][1], points[0][2]);
		glVertex3f(points[1][0], points[1][1], points[1][2]);
		glVertex3f(points[4][0], points[4][1], points[4][2]);
	}
	glEnd();
}

void drawCube() {
	float points[8][3] = {
		{ 0.5f, 0.5f, 0.5f }, // A [0][]
		{ 0.5f, -0.5f, 0.5f }, // B [1][]
		{ -0.5f, -0.5f, 0.5f }, // C [2][]
		{ -0.5f, 0.5f, 0.5f }, // D [3][]
		{ 0.5f, 0.5f, -0.5f }, // E [4][]
		{ 0.5f, -0.5f, -0.5f }, // F [5][]
		{ -0.5f, -0.5f, -0.5f }, // G [6][]
		{ -0.5f, 0.5f, -0.5f }, // H [7][]
	};
	
	glBegin(GL_QUADS);
	{
		// Front (ABCD)
		glVertex3f(points[0][0], points[0][1], points[0][2]);
		glVertex3f(points[1][0], points[1][1], points[1][2]);
		glVertex3f(points[2][0], points[2][1], points[2][2]);
		glVertex3f(points[3][0], points[3][1], points[3][2]);
	
		// Back (EFGH)
		glVertex3f(points[4][0], points[4][1], points[4][2]);
		glVertex3f(points[5][0], points[5][1], points[5][2]);
		glVertex3f(points[6][0], points[6][1], points[6][2]);
		glVertex3f(points[7][0], points[7][1], points[7][2]);
	
		// Left (CDHg)
		glVertex3f(points[2][0], points[2][1], points[2][2]);
		glVertex3f(points[3][0], points[3][1], points[3][2]);
		glVertex3f(points[7][0], points[7][1], points[7][2]);
		glVertex3f(points[6][0], points[6][1], points[6][2]);
	
		// Right (ABFE)
		glVertex3f(points[0][0], points[0][1], points[0][2]);
		glVertex3f(points[1][0], points[1][1], points[1][2]);
		glVertex3f(points[5][0], points[5][1], points[5][2]);
		glVertex3f(points[4][0], points[4][1], points[4][2]);
	
		// Top (ADHE)
		glVertex3f(points[0][0], points[0][1], points[0][2]);
		glVertex3f(points[3][0], points[3][1], points[3][2]);
		glVertex3f(points[7][0], points[7][1], points[7][2]);
		glVertex3f(points[4][0], points[4][1], points[4][2]);
	
		// Bottom (BCGF)
		glVertex3f(points[1][0], points[1][1], points[1][2]);
		glVertex3f(points[2][0], points[2][1], points[2][2]);
		glVertex3f(points[6][0], points[6][1], points[6][2]);
		glVertex3f(points[5][0], points[5][1], points[5][2]);
	}
	glEnd();
}

//--------------------------------------------------------------------
/*
	Construction
*/



//--------------------------------------------------------------------

void display() {
	glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	drawPyramid();
	glPopMatrix();
}

//--------------------------------------------------------------------

void setCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	switch (currentMode) {
	case 0:
		glOrtho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoZNear, orthoZFar);
		break;
	case 1:
		gluPerspective(prespFovy, screenWidth / screenHeight, -1, 1);
		//glFrustum(prespLeft, prespRight, prespBottom, prespTop, prespZNear, prespZFar);


		if (GetCursorPos(&cursorPos)) {
			int diffCursorPosX = cursorPos.x - lastCursorPosX; //check the difference between the current x and the last x position
			int diffCursorPosY = cursorPos.y - lastCursorPosY; //check the difference between the current y and the last y position
			lastCursorPosX = cursorPos.x; //set lastCursorPosX to the current x position
			lastCursorPosY = cursorPos.y; //set lastCursorPosY to the current y position
			cameraRotX += (float)diffCursorPosY; //set the cameraRotX to cameraRotX with the addition of the difference in the y position
			cameraRotY += (float)diffCursorPosX;    //set the cameraRotX to cameraRotY with the addition of the difference in the x position
			if (cameraRotX > 180) cameraRotX -= 180;
			if (cameraRotY > 180) cameraRotY -= 180;
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(cameraRotX, 1.0, 0.0, 0.0);  //rotate our camera on the x-axis (left and right)
		glRotatef(cameraRotY, 0.0, 1.0, 0.0);  //rotate our camera on the y-axis (up and down)
		glTranslated(-cameraPosX, -cameraPosY, -cameraPosZ); //translate the screen to the position of our camera


		break;
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance,
	_In_ LPSTR cmdLine, _In_ int nCmdShow) // FIXED
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	cursorPos.x = screenWidth / 2;
	cursorPos.y = screenHeight / 2;
	lastCursorPosX = cursorPos.x;
	lastCursorPosY = cursorPos.y;
	ShowCursor(false);
	SetCursorPos(cursorPos.x, cursorPos.y);

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		setCamera();
		display();

		SwapBuffers(hdc);
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------
