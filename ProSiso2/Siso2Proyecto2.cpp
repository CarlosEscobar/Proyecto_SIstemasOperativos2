#include <windows.h>
#include <atlbase.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <iostream>

using namespace std;

#define MENU_REGRESAR			101
#define MENU_ARCHIVO_CREAR		102
#define MENU_CARPETA_CREAR		103
#define CLICK_FILE				200
#define CLICK_FOLDER			201
#define CLICK_MOVE				202
#define CLICK_COPY				203
#define CLICK_DELETE			204
#define CLICK_HARDLINK			205
#define CLICK_SOFTLINK			206
#define CLICK_JUNCTION			207

#define STARTING_X				20
#define STARTING_Y				20	
#define ROW_HEIGHT				50
#define ROW_SEPARATOR			10
#define COL_WIDTH_NO			50
#define COL_WIDTH_ICON			50
#define COL_WIDTH_NAME			150
#define COL_WIDTH_CREATED_DATE  150
#define STARTING_Y_2			70

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
int getClickedRowNumber(HWND hWnd);
void ShowErrorMessage(LPCSTR);
void LoadImages();
void AddMenus(HWND);
void LoadCurrentDirectory(HWND);
void DoRegresar(HWND);
void DoNavigate(HWND,string);
void DoOpenFile(string);
void DoCreate(HWND, string, int);
void DoMove(HWND, string, int, string);
void DoCopy(HWND, string, int, string);
void DoDelete(HWND,string,int);
void DoCreateLink(HWND, string, int, int);
void RegisterAddModalClass(HINSTANCE);
void DisplayAddModal(HWND,int);
void RegisterDirectoryModalClass(HINSTANCE);
void DisplayDirectoryModal(HWND,string,int,int);

HWND    rootWindow;
HMENU	hMenu;
wchar_t currentDirectory_1[128];
char    currentDirectory_2[128];
int     currentDirectoryPos;
HBITMAP folderIconImage, fileIconImage, 
		moveIconImage, copyIconImage, trashIconImage, 
		hardLinkIconImage, softLinkIconImage, junctionIconImage,
		softLinkFolderIconImage, softLinkFileIconImage, hardLinkFileIconImage,
		audioFileIcon, videoFileIcon, xmlFileIcon, wordFileIcon, excelFileIcon, pptFileIcon;
string  inMemoryCurrentItems[8192];
int		inMemoryCurrentItemTypes[8192];
int		rowCounter;
HWND    addModalInput;
int     addModalType;

wchar_t currentDirectory_1_modal[128];
char    currentDirectory_2_modal[128];
int     currentDirectoryPos_modal;
string  inMemoryCurrentItems_modal[8192];
int     rowCounter_modal;
string  itemName_modal;
int     itemType_modal;
int     operationType_modal;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW wc = { 0 };

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassW(&wc))
		return -1;

	RegisterAddModalClass(hInst);
	RegisterDirectoryModalClass(hInst);

	currentDirectory_1[0] = currentDirectory_2[0] = 'C';
	currentDirectory_1[1] = currentDirectory_2[1] = ':';
	currentDirectory_1[2] = currentDirectory_2[2] = '\\';
	currentDirectory_1[3] = currentDirectory_2[3] = 'P';
	currentDirectory_1[4] = currentDirectory_2[4] = 'r';
	currentDirectory_1[5] = currentDirectory_2[5] = 'o';
	currentDirectory_1[6] = currentDirectory_2[6] = 'S';
	currentDirectory_1[7] = currentDirectory_2[7] = 'i';
	currentDirectory_1[8] = currentDirectory_2[8] = 's';
	currentDirectory_1[9] = currentDirectory_2[9] = 'o';
	currentDirectory_1[10] = currentDirectory_2[10] = '2';
	currentDirectoryPos = 11;
	rootWindow = CreateWindowW(L"myWindowClass", currentDirectory_1, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 20, 20, 780, 800, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int rowNumber;
	switch (msg)
	{
		case WM_COMMAND:
			switch (wp)
			{
				case MENU_REGRESAR:
					DoRegresar(hWnd);
					break;
				case MENU_ARCHIVO_CREAR:
					DisplayAddModal(hWnd,2);
					break;
				case MENU_CARPETA_CREAR:
					DisplayAddModal(hWnd,1);
					break;
				case CLICK_FILE:
					rowNumber = getClickedRowNumber(hWnd);
					if (rowNumber != -1) DoOpenFile(inMemoryCurrentItems[rowNumber]);
					break;
				case CLICK_FOLDER:
					rowNumber = getClickedRowNumber(hWnd);
					if(rowNumber != -1) DoNavigate(hWnd, inMemoryCurrentItems[rowNumber]);
					break;
				case CLICK_MOVE:
					rowNumber = getClickedRowNumber(hWnd);
					if (rowNumber != -1) {
						DisplayDirectoryModal(hWnd, inMemoryCurrentItems[rowNumber], inMemoryCurrentItemTypes[rowNumber], 1);
					}
					break;
				case CLICK_COPY:
					rowNumber = getClickedRowNumber(hWnd);
					if (rowNumber != -1) {
						DisplayDirectoryModal(hWnd, inMemoryCurrentItems[rowNumber], inMemoryCurrentItemTypes[rowNumber], 2);
					}
					break;
				case CLICK_DELETE:
					rowNumber = getClickedRowNumber(hWnd);
					if (rowNumber != -1) {
						DoDelete(hWnd,inMemoryCurrentItems[rowNumber],inMemoryCurrentItemTypes[rowNumber]);
					}
					break;
				case CLICK_HARDLINK:
					rowNumber = getClickedRowNumber(hWnd);
					if (rowNumber != -1) {
						DoCreateLink(hWnd, inMemoryCurrentItems[rowNumber],0, 0);
					}
					break;
				case CLICK_SOFTLINK:
					rowNumber = getClickedRowNumber(hWnd);
					if (rowNumber != -1) {
						DoCreateLink(hWnd, inMemoryCurrentItems[rowNumber], inMemoryCurrentItemTypes[rowNumber], 1);
					}
					break;
				case CLICK_JUNCTION:
					rowNumber = getClickedRowNumber(hWnd);
					if (rowNumber != -1) {
						DoCreateLink(hWnd, inMemoryCurrentItems[rowNumber], inMemoryCurrentItemTypes[rowNumber], 2);
					}
					break;
			}
			break;
		case WM_CREATE:
			LoadImages();
			AddMenus(hWnd);
			LoadCurrentDirectory(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

/***********************
*      Utilities       *
***********************/
int getClickedRowNumber(HWND hWnd)
{
	POINT p;
	if (GetCursorPos(&p))
	{
		if (ScreenToClient(hWnd, &p))
		{
			return (p.y - STARTING_Y) / ROW_HEIGHT;
		}
	}
	return -1;
}

void ShowErrorMessage(LPCWSTR message)
{
	MessageBoxW(NULL, message, L"ERROR MESSAGE", MB_OK);
}

BOOL CALLBACK DestoryChildCallback(HWND hWnd, LPARAM lParam)
{
	if (hWnd != NULL) DestroyWindow(hWnd);
	return TRUE;
}

void RefreshAfterOperation(HWND hWnd)
{
	EnumChildWindows(hWnd, DestoryChildCallback, NULL);
	LoadCurrentDirectory(hWnd);
}

/***********************
*    Initialization    *
***********************/
void LoadImages()
{
	folderIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\folderIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	fileIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\fileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	moveIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\moveIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	copyIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\copyIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	trashIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\trashIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	hardLinkIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\hardLinkIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	softLinkIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\softLinkIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	junctionIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\junctionIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);

	softLinkFolderIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\softLinkFolderIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	softLinkFileIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\softLinkFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	hardLinkFileIconImage = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\hardLinkFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);

	audioFileIcon = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\audioFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	videoFileIcon = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\videoFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	xmlFileIcon = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\xmlFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	wordFileIcon = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\wordFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	excelFileIcon = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\excelFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);
	pptFileIcon = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\Carlos Escobar\\Desktop\\Icons\\bmp\\pptFileIcon.bmp", IMAGE_BITMAP, ROW_HEIGHT, ROW_HEIGHT, LR_LOADFROMFILE);


}

void AddMenus(HWND hWnd)
{
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, MENU_REGRESAR, "Regresar");
	AppendMenu(hMenu, MF_STRING, MENU_ARCHIVO_CREAR, "Crear Archivo");
	AppendMenu(hMenu, MF_STRING, MENU_CARPETA_CREAR, "Crear Carpeta");
	SetMenu(hWnd, hMenu);
}

/***********************
*       Rendering      *
***********************/
void RenderRow(HWND hWnd, UINT type, CHAR* itemName, FILETIME creationDate)
{
	wchar_t rowCounterStr[32];
	_itow_s(rowCounter, rowCounterStr, 10);
	CreateWindowW(L"Static", rowCounterStr, WS_VISIBLE | WS_CHILD, STARTING_X, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_NO, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	
	if (type == 1) {
		if (itemName[0] == 's' &&
			itemName[1] == 'o' &&
			itemName[2] == 'f' &&
			itemName[3] == 't' &&
			itemName[4] == 'l' &&
			itemName[5] == 'i' &&
			itemName[6] == 'n' &&
			itemName[7] == 'k' &&
			itemName[8] == '.') {
				HWND button = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_FOLDER, NULL, NULL);
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)softLinkFolderIconImage);
		} else if (itemName[0] == 'j' &&
				   itemName[1] == 'u' &&
				   itemName[2] == 'n' &&
				   itemName[3] == 'c' &&
				   itemName[4] == 't' &&
				   itemName[5] == 'i' &&
				   itemName[6] == 'o' &&
				   itemName[7] == 'n' &&
				   itemName[8] == '.') {
			HWND button = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_FOLDER, NULL, NULL);
			SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)junctionIconImage);
		}
		else {
			HWND button = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_FOLDER, NULL, NULL);
			SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)folderIconImage);
		}
	} else if (type == 2) {
		if (itemName[0] == 's' &&
			itemName[1] == 'o' &&
			itemName[2] == 'f' &&
			itemName[3] == 't' &&
			itemName[4] == 'l' &&
			itemName[5] == 'i' &&
			itemName[6] == 'n' &&
			itemName[7] == 'k' &&
			itemName[8] == '.') {
			HWND button = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_FILE, NULL, NULL);
			SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)softLinkFileIconImage);
		}else if (itemName[0] == 'h' &&
				  itemName[1] == 'a' &&
				  itemName[2] == 'r' &&
				  itemName[3] == 'd' &&
				  itemName[4] == 'l' &&
				  itemName[5] == 'i' &&
				  itemName[6] == 'n' &&
				  itemName[7] == 'k' &&
				  itemName[8] == '.') {
			HWND button = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_FILE, NULL, NULL);
			SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hardLinkFileIconImage);
		}else {
			HWND button = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_FILE, NULL, NULL);
			
			std::string strItemName(itemName);
			if (strItemName.substr(strItemName.size() - 3) == "mp3") {
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)audioFileIcon);
			}
			else if (strItemName.substr(strItemName.size() - 3) == "mp4") {
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)videoFileIcon);
			}
			else if (strItemName.substr(strItemName.size() - 3) == "xls" ||
				strItemName.substr(strItemName.size() - 4) == "xlsx") {
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)excelFileIcon);
			}
			else if (strItemName.substr(strItemName.size() - 3) == "doc" ||
				strItemName.substr(strItemName.size() - 4) == "docx") {
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)wordFileIcon);
			}
			else if (strItemName.substr(strItemName.size() - 3) == "ppt" ||
				strItemName.substr(strItemName.size() - 4) == "pptx") {
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)pptFileIcon);
			}
			else if (strItemName.substr(strItemName.size() - 3) == "xml") {
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)xmlFileIcon);
			}
			else {
				SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)fileIconImage);
			}
		}
	}

	WCHAR wItemName[80];
	MultiByteToWideChar(CP_ACP, 0, itemName, -1, wItemName, 80);
	CreateWindowW(L"Static", wItemName, WS_VISIBLE | WS_CHILD, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_NAME, ROW_HEIGHT, hWnd, NULL, NULL, NULL);

	SYSTEMTIME stUTC, stLocal;
	LPTSTR lpszString = new TCHAR[100];
	WCHAR creationDateStr[30];
	FileTimeToSystemTime(&creationDate, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	StringCchPrintf(lpszString, 30, TEXT("%02d/%02d/%d  %02d:%02d"), stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute);
	MultiByteToWideChar(CP_ACP, 0, lpszString, -1, creationDateStr, 80);
	CreateWindowW(L"Static", creationDateStr, WS_VISIBLE | WS_CHILD, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_CREATED_DATE, ROW_HEIGHT, hWnd, NULL, NULL, NULL);

	HWND moveButton = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_MOVE, NULL, NULL);
	SendMessageW(moveButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)moveIconImage);

	HWND copyButton = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_COPY, NULL, NULL);
	SendMessageW(copyButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)copyIconImage);

	HWND deleteButton = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_DELETE, NULL, NULL);
	SendMessageW(deleteButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)trashIconImage);

	if (type == 1) {
		CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	} else if (type == 2) {
		HWND hardLinkButton = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_HARDLINK, NULL, NULL);
		SendMessageW(hardLinkButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hardLinkIconImage);
	}

	HWND softLinkButton = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_SOFTLINK, NULL, NULL);
	SendMessageW(softLinkButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)softLinkIconImage);

	if (type == 1) {
		HWND junctionButton = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON + 7, ROW_HEIGHT, hWnd, (HMENU)CLICK_JUNCTION, NULL, NULL);
		SendMessageW(junctionButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)junctionIconImage);
	}else if (type == 2) {
		CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y + (rowCounter * ROW_HEIGHT), COL_WIDTH_ICON + 7, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	}

	string currentItemName(itemName);
	inMemoryCurrentItems[rowCounter] = currentItemName;
	inMemoryCurrentItemTypes[rowCounter] = type;
	rowCounter++;
}

void LoadCurrentDirectory(HWND hWnd)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	rowCounter = 0;

	StringCchLength(currentDirectory_2, MAX_PATH, &length_of_arg);
	if (length_of_arg > (MAX_PATH - 3))
	{
		ShowErrorMessage(L"Directory path is too long.");
		return;
	}

	StringCchCopy(szDir, MAX_PATH, currentDirectory_2);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		ShowErrorMessage(L"ERROR in FindFirstFile");
		return;
	}

	CreateWindowW(L"Static", L"No.", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X, STARTING_Y, COL_WIDTH_NO, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Type", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO, STARTING_Y, COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Name", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON, STARTING_Y, COL_WIDTH_NAME, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Created Date", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME, STARTING_Y, COL_WIDTH_CREATED_DATE, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Move", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE, STARTING_Y, COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Copy", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON, STARTING_Y, COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Delete", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y, COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Create Hard Link", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y, COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Create Soft Link", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y, COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Create Junction", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME + COL_WIDTH_CREATED_DATE + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON + COL_WIDTH_ICON, STARTING_Y, COL_WIDTH_ICON + 7, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	rowCounter++;

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (ffd.cFileName[0] != '.') {
				RenderRow(hWnd, 1, ffd.cFileName, ffd.ftCreationTime);
			}
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			RenderRow(hWnd, 2, ffd.cFileName, ffd.ftCreationTime);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		ShowErrorMessage(L"ERROR in GetLastError");
		return;
	}
	FindClose(hFind);
}

/***********************
*      Navigation      *
***********************/
void DoRegresar(HWND hWnd)
{
	if (currentDirectoryPos == 11) return;

	while (currentDirectory_1[currentDirectoryPos] != '\\')
	{
		currentDirectory_1[currentDirectoryPos] = currentDirectory_2[currentDirectoryPos] = '\0';
		currentDirectoryPos--;
	}
	currentDirectory_1[currentDirectoryPos] = currentDirectory_2[currentDirectoryPos] = '\0';

	SetWindowTextW(hWnd,currentDirectory_1);
	RefreshAfterOperation(hWnd);
}

void DoNavigate(HWND hWnd, string toFolder)
{
	currentDirectory_1[currentDirectoryPos] = currentDirectory_2[currentDirectoryPos] = '\\';
	currentDirectoryPos++;

	for (int i = 0; i < toFolder.length(); i++) {
		currentDirectory_1[currentDirectoryPos] = currentDirectory_2[currentDirectoryPos] = toFolder[i];
		currentDirectoryPos++;
	}

	SetWindowTextW(hWnd, currentDirectory_1);
	RefreshAfterOperation(hWnd);
}

/***********************
*      Operations      *
***********************/
void DoOpenFile(string fileName) {
	string itemPath = " ";
	for (int i = 0; i < currentDirectoryPos; i++) {
		itemPath += currentDirectory_2[i];
	}
	itemPath += '\\';
	itemPath += fileName;
	LPSTR lpStrItemPath = const_cast<char*>(itemPath.c_str());

	string applicationPath = "";
	if (fileName.substr(fileName.size() - 3) == "mp3" ||
		fileName.substr(fileName.size() - 3) == "mp4") {
		applicationPath = "C:\\Program Files (x86)\\Windows Media Player\\wmplayer.exe";
	} 
	else if (fileName.substr(fileName.size() - 3) == "xls" ||
			 fileName.substr(fileName.size() - 4) == "xlsx") {
		applicationPath = "C:\\Program Files\\Microsoft Office\\root\\Office16\\EXCEL.exe";
	}
	else if (fileName.substr(fileName.size() - 3) == "doc" ||
			 fileName.substr(fileName.size() - 4) == "docx") {
		applicationPath = "C:\\Program Files\\Microsoft Office\\root\\Office16\\WINWORD.exe";
	}
	else if (fileName.substr(fileName.size() - 3) == "ppt" ||
			 fileName.substr(fileName.size() - 4) == "pptx") {
		applicationPath = "C:\\Program Files\\Microsoft Office\\root\\Office16\\POWERPNT.exe";
	}
	else if (fileName.substr(fileName.size() - 3) == "xml" ||
			 fileName.substr(fileName.size() - 3) == "txt") {
		applicationPath = "C:\\Windows\\notepad.exe";
	}

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	CreateProcessA(applicationPath.c_str(),lpStrItemPath,NULL,NULL,FALSE,INHERIT_CALLER_PRIORITY,NULL,NULL,&si,&pi);
}

void DoCreate(HWND hWnd, string itemName, int itemType)
{
	string itemPath = "";
	for (int i = 0; i < currentDirectoryPos; i++) {
		itemPath += currentDirectory_2[i];
	}
	itemPath += '\\';
	itemPath += itemName;

	if (itemType == 1) {
		CreateDirectoryA(itemPath.c_str(), NULL);
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully created directory: " + itemPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}
	else if (itemType == 2) {
		itemPath += ".txt";
		HANDLE newFile = CreateFileA(itemPath.c_str(), GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
		CloseHandle(newFile);
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully created file: " + itemPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}
}

void DoMove(HWND hWnd, string itemName, int itemType, string destinationDirectory)
{
	string originItemPath = "";
	for (int i = 0; i < currentDirectoryPos; i++) {
		originItemPath += currentDirectory_2[i];
	}
	originItemPath += '\\';
	originItemPath += itemName;
	string destinationItemPath = destinationDirectory + '\\' + itemName;

	if (itemType == 1) {
		CreateDirectoryA(destinationItemPath.c_str(), NULL);
		RemoveDirectoryA(originItemPath.c_str());
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully moved directory: " + originItemPath + " => " + destinationItemPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}
	else if (itemType == 2) {
		MoveFileA(originItemPath.c_str(), destinationItemPath.c_str());
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully moved file: " + originItemPath + " => " + destinationItemPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}
}

void DoCopy(HWND hWnd, string itemName, int itemType, string destinationDirectory)
{
	string originItemPath = "";
	for (int i = 0; i < currentDirectoryPos; i++) {
		originItemPath += currentDirectory_2[i];
	}
	originItemPath += '\\';
	originItemPath += itemName;
	string destinationItemPath = destinationDirectory + '\\' + itemName;

	if (itemType == 1) {
		CreateDirectoryA(destinationItemPath.c_str(), NULL);
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully copied directory: " + originItemPath + " => " + destinationItemPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	} else if (itemType == 2) {
		CopyFileA(originItemPath.c_str(), destinationItemPath.c_str(), false);
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully copied file: " + originItemPath + " => " + destinationItemPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}
}

void DoDelete(HWND hWnd, string itemName, int itemType)
{
	if (MessageBoxW(hWnd, L"Are you sure you want to delete?", L"Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		string itemPath = "";
		for (int i = 0; i < currentDirectoryPos; i++) {
			itemPath += currentDirectory_2[i];
		}
		itemPath += '\\';
		itemPath += itemName;
		if (itemType == 1) {
			RemoveDirectoryA(itemPath.c_str());
			RefreshAfterOperation(hWnd);
			string successMsg = "Successfully deleted directory: " + itemPath;
			MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
		}
		else if (itemType == 2) {
			DeleteFileA(itemPath.c_str());
			RefreshAfterOperation(hWnd);
			string successMsg = "Successfully deleted file: " + itemPath;
			MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
		}
	}
}

void DoCreateLink(HWND hWnd, string itemName, int itemType, int linkType)
{
	string itemPath = "";
	for (int i = 0; i < currentDirectoryPos; i++) {
		itemPath += currentDirectory_2[i];
	}
	itemPath += '\\';
	string linkPath = itemPath;
	itemPath += itemName;

	if (linkType == 0) {
		linkPath += "hardlink.";
		linkPath += itemName;
		CreateHardLinkA(linkPath.c_str(), itemPath.c_str(), NULL);
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully created hard link: " + linkPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}else if (linkType == 1){
		linkPath += "softlink.";
		linkPath += itemName;
		if (itemType == 1) {
			CreateSymbolicLinkA(linkPath.c_str(), itemPath.c_str(), 0x1);
		}else {
			CreateSymbolicLinkA(linkPath.c_str(), itemPath.c_str(), 0x0);
		}
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully created soft link: " + linkPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}
	else if (linkType == 2) {
		linkPath += "junction.";
		linkPath += itemName;
		CreateDirectoryA(linkPath.c_str(), NULL);
		CreateHardLinkA(linkPath.c_str(), itemPath.c_str(), NULL);
		RefreshAfterOperation(hWnd);
		string successMsg = "Successfully created junction: " + linkPath;
		MessageBox(hWnd, successMsg.c_str(), "Confirmation", MB_OK);
	}
}

/***********************
*      InputModal      *
***********************/
LRESULT CALLBACK AddModalProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_COMMAND:
			switch (wp)
			{
				case 1:
					char newItemName[30];
					GetWindowText(addModalInput, newItemName, 30);
					string addModalInputStr = string(newItemName);
					DestroyWindow(hWnd);
					DoCreate(rootWindow, addModalInputStr, addModalType);
					break;
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

void RegisterAddModalClass(HINSTANCE hInst)
{
	WNDCLASSW modal = { 0 };
	modal.hbrBackground = (HBRUSH)COLOR_WINDOW;
	modal.hCursor = LoadCursor(NULL, IDC_CROSS);
	modal.hInstance = hInst;
	modal.lpszClassName = L"myAddModalClass";
	modal.lpfnWndProc = AddModalProcedure;
	RegisterClassW(&modal);
}

void DisplayAddModal(HWND hWnd, int itemType)
{
	HWND addModal = CreateWindowW(L"myAddModalClass", L"Create", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 400, 400, 275, 180, hWnd, NULL, NULL, NULL);
	if (itemType == 1) {
		SetWindowTextW(addModal, L"Create Directory");
	}
	else if (itemType == 2) {
		SetWindowTextW(addModal, L"Create File");
	}
	addModalType = itemType;

	CreateWindowW(L"Static", L"Name:", WS_VISIBLE | WS_CHILD, 20, 25, 50, 50, addModal, NULL, NULL, NULL);
	addModalInput = CreateWindowW(L"Edit", L"...", WS_VISIBLE | WS_CHILD, 100, 20, 120, 30, addModal, NULL, NULL, NULL);

	CreateWindowW(L"Button", L"Create", WS_VISIBLE | WS_CHILD, 90, 70, 70, 40, addModal, (HMENU)1, NULL, NULL);
}

/***********************
*    DirectoryModal    *
***********************/
int getClickedRowNumber_Modal(HWND hWnd)
{
	POINT p;
	if (GetCursorPos(&p))
	{
		if (ScreenToClient(hWnd, &p))
		{
			return (p.y - STARTING_Y_2) / ROW_HEIGHT;
		}
	}
	return -1;
}

void RenderRow_Modal(HWND hWnd, CHAR* itemName, FILETIME creationDate)
{
	wchar_t rowCounterStr[32];
	_itow_s(rowCounter_modal, rowCounterStr, 10);
	CreateWindowW(L"Static", rowCounterStr, WS_VISIBLE | WS_CHILD, STARTING_X, STARTING_Y_2 + (rowCounter_modal * ROW_HEIGHT), COL_WIDTH_NO, ROW_HEIGHT, hWnd, NULL, NULL, NULL);

	HWND button = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, STARTING_X + COL_WIDTH_NO, STARTING_Y_2 + (rowCounter_modal * ROW_HEIGHT), COL_WIDTH_ICON, ROW_HEIGHT, hWnd, (HMENU)CLICK_FOLDER, NULL, NULL);
	SendMessageW(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)folderIconImage);

	WCHAR wItemName[80];
	MultiByteToWideChar(CP_ACP, 0, itemName, -1, wItemName, 80);
	CreateWindowW(L"Static", wItemName, WS_VISIBLE | WS_CHILD, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON, STARTING_Y_2 + (rowCounter_modal * ROW_HEIGHT), COL_WIDTH_NAME, ROW_HEIGHT, hWnd, NULL, NULL, NULL);

	SYSTEMTIME stUTC, stLocal;
	LPTSTR lpszString = new TCHAR[100];
	WCHAR creationDateStr[30];
	FileTimeToSystemTime(&creationDate, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	StringCchPrintf(lpszString, 30, TEXT("%02d/%02d/%d  %02d:%02d"), stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute);
	MultiByteToWideChar(CP_ACP, 0, lpszString, -1, creationDateStr, 80);
	CreateWindowW(L"Static", creationDateStr, WS_VISIBLE | WS_CHILD, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME, STARTING_Y_2 + (rowCounter_modal * ROW_HEIGHT), COL_WIDTH_CREATED_DATE, ROW_HEIGHT, hWnd, NULL, NULL, NULL);

	string currentItemName(itemName);
	inMemoryCurrentItems_modal[rowCounter_modal] = currentItemName;
	rowCounter_modal++;
}

void LoadCurrentDirectory_Modal(HWND hWnd)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	rowCounter_modal = 0;

	StringCchLength(currentDirectory_2_modal, MAX_PATH, &length_of_arg);
	if (length_of_arg > (MAX_PATH - 3))
	{
		ShowErrorMessage(L"Directory path is too long.");
		return;
	}

	StringCchCopy(szDir, MAX_PATH, currentDirectory_2_modal);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		ShowErrorMessage(L"ERROR in FindFirstFile");
		return;
	}

	CreateWindowW(L"Button", L"Back", WS_VISIBLE | WS_CHILD, 10, 10, 100, 40, hWnd, (HMENU)100, NULL, NULL);
	CreateWindowW(L"Button", L"Select", WS_VISIBLE | WS_CHILD, 200, 10, 100, 40, hWnd, (HMENU)operationType_modal, NULL, NULL);

	CreateWindowW(L"Static", L"No.", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X, STARTING_Y_2, COL_WIDTH_NO, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Type", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO, STARTING_Y_2, COL_WIDTH_ICON, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Name", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON, STARTING_Y_2, COL_WIDTH_NAME, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Created Date", WS_VISIBLE | WS_CHILD | WS_BORDER, STARTING_X + COL_WIDTH_NO + COL_WIDTH_ICON + COL_WIDTH_NAME, STARTING_Y_2, COL_WIDTH_CREATED_DATE, ROW_HEIGHT, hWnd, NULL, NULL, NULL);
	rowCounter_modal++;

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (ffd.cFileName[0] != '.') {
				RenderRow_Modal(hWnd, ffd.cFileName, ffd.ftCreationTime);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		ShowErrorMessage(L"ERROR in GetLastError");
		return;
	}
	FindClose(hFind);
}

void DoRegresar_Modal(HWND hWnd)
{
	if (currentDirectoryPos_modal == 11) return;

	while (currentDirectory_1_modal[currentDirectoryPos_modal] != '\\')
	{
		currentDirectory_1_modal[currentDirectoryPos_modal] = currentDirectory_2_modal[currentDirectoryPos_modal] = '\0';
		currentDirectoryPos_modal--;
	}
	currentDirectory_1_modal[currentDirectoryPos_modal] = currentDirectory_2_modal[currentDirectoryPos_modal] = '\0';

	SetWindowTextW(hWnd, currentDirectory_1_modal);
	EnumChildWindows(hWnd, DestoryChildCallback, NULL);
	LoadCurrentDirectory_Modal(hWnd);
}

void DoNavigate_Modal(HWND hWnd, string toFolder)
{
	currentDirectory_1_modal[currentDirectoryPos_modal] = currentDirectory_2_modal[currentDirectoryPos_modal] = '\\';
	currentDirectoryPos_modal++;

	for (int i = 0; i < toFolder.length(); i++) {
		currentDirectory_1_modal[currentDirectoryPos_modal] = currentDirectory_2_modal[currentDirectoryPos_modal] = toFolder[i];
		currentDirectoryPos_modal++;
	}

	SetWindowTextW(hWnd, currentDirectory_1_modal);
	EnumChildWindows(hWnd, DestoryChildCallback, NULL);
	LoadCurrentDirectory_Modal(hWnd);
}

LRESULT CALLBACK DirectoryModalProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	string s1;
	string destinationDirectory;
	switch (msg)
	{
	case WM_COMMAND:
		switch (wp)
		{
		case 1:
			destinationDirectory = "";
			for (int i = 0; i < currentDirectoryPos_modal; i++)
			{
				destinationDirectory += currentDirectory_2_modal[i];
			}
			DestroyWindow(hWnd);
			DoMove(rootWindow, itemName_modal, itemType_modal, destinationDirectory);
			break;
		case 2:
			destinationDirectory = "";
			for (int i = 0; i<currentDirectoryPos_modal; i++)
			{
				destinationDirectory += currentDirectory_2_modal[i];
			}
			DestroyWindow(hWnd);
			DoCopy(rootWindow, itemName_modal, itemType_modal, destinationDirectory);
			break;
		case 100:
			DoRegresar_Modal(hWnd);
			break;
		case CLICK_FOLDER:
			int rowNumber = getClickedRowNumber_Modal(hWnd);
			if (rowNumber != -1) DoNavigate_Modal(hWnd, inMemoryCurrentItems_modal[rowNumber]);
			break;
		}
		break;
	case WM_CREATE:
		LoadCurrentDirectory_Modal(hWnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

void RegisterDirectoryModalClass(HINSTANCE hInst)
{
	WNDCLASSW directoryModal = { 0 };
	directoryModal.hbrBackground = (HBRUSH)COLOR_WINDOW;
	directoryModal.hCursor = LoadCursor(NULL, IDC_ARROW);
	directoryModal.hInstance = hInst;
	directoryModal.lpszClassName = L"myDirectoryModalClass";
	directoryModal.lpfnWndProc = DirectoryModalProcedure;
	RegisterClassW(&directoryModal);
}

void DisplayDirectoryModal(HWND hWnd, string itemName, int itemType, int operationType)
{
	itemName_modal = itemName;
	itemType_modal = itemType;
	operationType_modal = operationType;
	for (int i = 0; i < currentDirectoryPos; i++) {
		currentDirectory_1_modal[i] = currentDirectory_1[i];
		currentDirectory_2_modal[i] = currentDirectory_2[i];
	}
	currentDirectoryPos_modal = currentDirectoryPos;

	for (int i = currentDirectoryPos_modal; i < 128; i++) {
		currentDirectory_1_modal[i] = currentDirectory_2_modal[i] = '\0';
	}

	CreateWindowW(L"myDirectoryModalClass", currentDirectory_1_modal, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 200, 200, 440, 600, hWnd, NULL, NULL, NULL);
}