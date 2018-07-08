#include <windows.h>
#include <commctrl.h> //per SBARS_SIZEGRIP e STATUSCLASSNAME

#define IDM_OPEN		100
#define IDM_EXIT		101
#define IDM_ABOUT		102
#define IDC_STATUSBAR	103
#define IDM_ARRANGE		104
#define IDM_CASCADE		105
#define IDM_TILE		106
#define IDM_CLOSEALL	107
#define IDM_CLOSE		108
#define ID_CLIENTWND	109
#define IDM_FIRSTCHILD	5000


const char frameClass[] = "myFrameClass";
const char clientClass[] = "mdiclient"; //funziona solo con "mdiclient"
const char childClass[] = "myChildClass";

HWND hWndStatusBar;
HWND hWndClient;



//STEP 5 Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	switch (msg){
		case WM_CREATE:
			//STEP 6: Create Status Bar
			hWndStatusBar = CreateWindowEx(0,STATUSCLASSNAME,NULL,
            							   WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
										   0, 0, 0, 0,
										   hwnd,
										   (HMENU)IDC_STATUSBAR,
										   (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			
			if(!hWndStatusBar){
                MessageBox(NULL, "Failed To Create The Status Bar", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }
            // Establish the number of partitions or 'parts' the status bar will 
			// have, their actual dimensions will be set in the parent window's 
			// WM_SIZE handler.
			RECT rc;
			GetClientRect(hwnd, &rc);
			int nHalf = rc.right / 2;
			int nParts[4] = { nHalf, nHalf + nHalf / 3, nHalf + nHalf * 2 / 3, -1 };
			SendMessage(hWndStatusBar, SB_SETPARTS, 4, (LPARAM)&nParts);
			
			//crea la finestra CLIENT
			CLIENTCREATESTRUCT ccs;
			// Assign the 'Window' menu.
			ccs.hWindowMenu = GetSubMenu(GetMenu(hwnd), 1); // uno perchè così la finestra appare nel secondo menu (partendo da zero) ovvero nel menu 'Windows'
			ccs.idFirstChild = IDM_FIRSTCHILD;
			// Create the client window. (quella che contiene i child!!)
			hWndClient = CreateWindowEx(WS_EX_CLIENTEDGE, clientClass,//FUNZIONA SOLO CON "mdiclient"
										NULL,
										WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
										0, 0, 0, 0,
										hwnd,(HMENU)ID_CLIENTWND/*GetMenu(hwnd)*/,GetModuleHandle(NULL),&ccs);
			if(hWndClient == NULL) {
				MessageBox(NULL, "Client Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
				return 0;
			}
        break;
        case WM_SIZE:
        	//resize the parts
			GetClientRect(hwnd, &rc);
			nHalf = rc.right / 2;
			nParts[0] = nHalf; nParts[1] = nHalf + nHalf / 3; nParts[2] = nHalf + nHalf * 2 / 3; nParts[3] = -1;
			SendMessage(hWndStatusBar, SB_SETPARTS, 4, (LPARAM)&nParts);
			//resize the statusbar
        	SendMessage(GetDlgItem(hwnd, IDC_STATUSBAR), WM_SIZE,0,0);
        break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDM_EXIT) DestroyWindow(hwnd);
			if(LOWORD(wParam) == IDM_ABOUT) MessageBox(hwnd,"ABOUT premuto","ciao",MB_OK);
			if(LOWORD(wParam) == IDM_OPEN){
				//apre il file...
				
				//TODO
				
				//crea la finestra CHILD
				HWND hwndChild = CreateWindowEx(WS_EX_MDICHILD,               //extended styles
                      childClass,            //control 'class' name
                      "MDI child",              //control caption
                      WS_CLIPCHILDREN,                      //control style 
                      0,                      //position: left
                      0,                       //position: top
                      400,                     //width
                      300,                    //height
                      hWndClient,                      //parent window handle
                      //control's ID
                      MAKEINTRESOURCE(IDM_FIRSTCHILD),
                      GetModuleHandle(NULL),                        //application instance
                      0);                           //user defined info
                      
                if(hwndChild == NULL){
                	MessageBox(hwnd,"Impossibile creare la finestra Child","Errore",MB_ICONERROR);
				}
			}
		break;
		default:
			//return DefWindowProc(hwnd,msg,wParam,lParam);
			return DefFrameProc(hwnd, hWndClient, msg, wParam, lParam); //per tenere conto delle finestre figlie
	}
	return 0;
}

LRESULT CALLBACK ChildWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg) {		
		case WM_CREATE :
//			LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;
//			LPMDICREATESTRUCT lpMDICreateStruct = (LPMDICREATESTRUCT)lpCreateStruct->lpCreateParams;
//			FrameParam = lpMDICreateStruct->lParam;
		break;	
		case WM_COMMAND:
			
		break;	
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefMDIChildProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;
	
	
	
	//STEP 1: Registering Frame Class Window
	wc.cbSize 			=	sizeof(WNDCLASSEX);
	wc.style 			=	CS_HREDRAW | CS_VREDRAW;;
	wc.lpfnWndProc 		=	WndProc;
	wc.cbClsExtra		=	0;
	wc.cbWndExtra		=	0;
	wc.hInstance		=	hInstance;
	wc.hIcon			=	LoadIcon(NULL,IDI_APPLICATION);
	wc.hCursor			=	LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground	=	(HBRUSH)(COLOR_APPWORKSPACE+1);//(COLOR_WINDOW);
	wc.hIconSm			=	LoadIcon(NULL,IDI_APPLICATION);
	wc.lpszMenuName		=	NULL;
	wc.lpszClassName	=	frameClass;
	
	if(!RegisterClassEx(&wc)){
		MessageBox(NULL,"Frame Window Registration Failed!","Error!",MB_ICONEXCLAMATION);
		return 0;
	}
	
	//la classe CLIENT non la devo registrare?
	
	//STEP 1.2: Registering Child Class Window
	wc.lpfnWndProc 		=	(WNDPROC)ChildWndProc;
	wc.hIcon			=	(HICON)LoadImage(0,IDI_APPLICATION,IMAGE_ICON,0,0,LR_SHARED);
	wc.hIconSm			=	LoadImage(hInstance,childClass,IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	wc.hCursor			=	(HCURSOR)LoadImage(0,IDC_ARROW,IMAGE_CURSOR,0,0,LR_SHARED);
	wc.hbrBackground	=	(HBRUSH)(COLOR_WINDOW+1);//(COLOR_WINDOW);
	wc.lpszMenuName		=	NULL;
	wc.lpszClassName	=	childClass;
	
	if(!RegisterClassEx(&wc)){
		MessageBox(NULL,"Child Window Registration Failed!","Error!",MB_ICONEXCLAMATION);
		return 0;
	}
	
	
	
	//STEP 2: Creating the Window
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
						  frameClass,
						  "046 - Visualizer",
						  WS_VISIBLE|WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT,CW_USEDEFAULT,600,400,
						  NULL,NULL,hInstance,NULL);
						  
	if(hwnd == NULL){
		MessageBox(NULL,"Frame Window Creation Failed!","Error!",MB_ICONEXCLAMATION);
		return 0;
	}
	
	ShowWindow(hwnd,nCmdShow);
	UpdateWindow(hwnd);
	
	
	
	//STEP 3: Create Menu
	
	HMENU hMenu = CreateMenu(); //crea un menu vuoto
	
	HMENU hSubMenu; //variabile usata per aggiungere sottomenu ti tipo POPUP
	
	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu,MF_STRING,IDM_OPEN,"&Open");
	AppendMenu(hSubMenu,MF_STRING,IDM_CLOSE,"&Close");
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hSubMenu,MF_STRING,IDM_EXIT,"&Exit");
	AppendMenu(hMenu,MF_STRING|MF_POPUP,(UINT)hSubMenu,"&File"); //aggiunge il sottomenu al menu
	
	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu,MF_STRING,IDM_ARRANGE,"Arrange");
	AppendMenu(hSubMenu,MF_STRING,IDM_CASCADE,"Cascade");
	AppendMenu(hSubMenu,MF_STRING,IDM_TILE,"Tile");
	AppendMenu(hSubMenu,MF_STRING,IDM_CLOSEALL,"Close All");
	AppendMenu(hMenu,MF_STRING|MF_POPUP,(UINT)hSubMenu,"&Windows");
	
	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu,MF_STRING,IDM_ABOUT,"&About");
	AppendMenu(hMenu,MF_STRING|MF_POPUP,(UINT)hSubMenu,"&Help");
	
	if(hMenu == NULL){
		MessageBox(NULL,"Menu Creation Failed!","Error!",MB_ICONEXCLAMATION);
		return 0;
	}
	
	SetMenu(hwnd,hMenu); //setta il menu sulla finestra
	
	
	
	//STEP 4: Message Loop
	while(GetMessage(&msg,NULL,0,0) > 0){
		if(hWndClient && TranslateMDISysAccel(hWndClient,&msg)) continue; //process the predefined MDI specific accelerator keys
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
