#ifndef H_ENTITIESMANAGERPROC_H
#define H_ENTITIESMANAGERPROC_H

int imageModel;
int imageObject;

BOOL InitTreeViewImageLists(HWND hwndTV, int NUM_BITMAPS, int CX_BITMAP, int CY_BITMAP) 
{ 
    HIMAGELIST himl;  // handle to image list 
    HBITMAP hbmp;     // handle to bitmap 

    // Create the image list. 
    if ((himl = ImageList_Create(CX_BITMAP, CY_BITMAP, 
        FALSE, NUM_BITMAPS, 0)) == NULL) 
        return FALSE; 

    // Add the open file, closed file, and document bitmaps. 
    hbmp = LoadBitmap(Engine.GetInstance(), MAKEINTRESOURCE(IDB_PARENT)); 
    imageModel = ImageList_Add(himl, hbmp, (HBITMAP) NULL); 
    DeleteObject(hbmp); 

    hbmp = LoadBitmap(Engine.GetInstance(), MAKEINTRESOURCE(IDB_CHILD)); 
    imageObject = ImageList_Add(himl, hbmp, (HBITMAP) NULL); 
    DeleteObject(hbmp); 


    // Fail if not all of the images were added. 
    if (ImageList_GetImageCount(himl) < NUM_BITMAPS) 
        return FALSE; 

    // Associate the image list with the tree-view control. 
    TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL); 

    return TRUE; 
} 


HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel)
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    static HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
    static HTREEITEM hPrevRootItem = NULL; 
    static HTREEITEM hPrevLev2Item = NULL; 
    HTREEITEM hti; 
 
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
    // Set the text of the item. 
    tvi.cchTextMax = lstrlen(lpszItem); 
    tvi.pszText = (LPTSTR)lpszItem; 
 
    // Assume the item is not a parent item, so give it a 
    // document image. 
    tvi.iImage = imageModel; 
    tvi.iSelectedImage = imageModel; 
 
    // Save the heading level in the item's application-defined 
    // data area. 
    tvi.lParam = (LPARAM) nLevel; 
 
    tvins.item = tvi; 
    tvins.hInsertAfter = hPrev; 
 
    // Set the parent item based on the specified level. 
    if (nLevel == 1) 
        tvins.hParent = TVI_ROOT; 
    else if (nLevel == 2) 
        tvins.hParent = hPrevRootItem; 
    else 
        tvins.hParent = hPrevLev2Item; 
 
    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
 
    // Save the handle to the item. 
    if (nLevel == 1) 
        hPrevRootItem = hPrev; 
    else if (nLevel == 2) 
        hPrevLev2Item = hPrev; 
 
    // The new item is a child item. Give the parent item a 
    // closed folder bitmap to indicate it now has child items. 
    if (nLevel > 1) { 
        hti = TreeView_GetParent(hwndTV, hPrev); 
        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
        tvi.hItem = hti; 
        tvi.iImage = imageObject; 
        tvi.iSelectedImage = imageObject; 
        TreeView_SetItem(hwndTV, &tvi); 
    } 
 
    return hPrev; 
} 



LRESULT CALLBACK EntitiesManagerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	int wmId;
	int wmEvent;
	
	//MSS Child Parameters
	OPENFILENAME openParamsChild = {0}; 
	char file_nameChild[BUFF_MAX] = {0};
	char file_nameChildTitle[BUFF_MAX] = {0};
	char dialogFilterChild[BUFF_MAX] = {0};
	//Inizializza i filtri
	strcat(dialogFilterChild,"FX Child Files");
	int indexFilterChild = (int)strlen(dialogFilterChild) + 1;
	dialogFilterChild[indexFilterChild++] = '*';
	dialogFilterChild[indexFilterChild++] = '.';
	dialogFilterChild[indexFilterChild++] = 'f';
	dialogFilterChild[indexFilterChild++] = 'x';


	switch (message)
	{
		case WM_INITDIALOG:
		{
			hWndEntitiesManager=hWnd;
			RECT windowRectMain;
			GetWindowRect( Engine.GetHWND(),&windowRectMain);
			SetWindowPos(hWnd,HWND_TOP,windowRectMain.right,windowRectMain.top,NULL,NULL,SWP_NOSIZE);
			ShowWindow(hWnd,SW_SHOW);

			//Figli totali
			char bufferTotalChild[10]="";
			sprintf(bufferTotalChild,"%.0f",(float)Scene0.Entity.GetTotalChildren());
			SetDlgItemText(hWnd, TXT_CHILD_TOTAL,bufferTotalChild);
			
			if (Scene0.currentCHILDID == -1)
			{
				EnableWindow(GetDlgItem(hWnd,CHK_ANCHOR_ENABLE),false);
				EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ENABLE),false);
				EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ACTIVE),false);
				EnableWindow(GetDlgItem(hWnd,BUT_CHILD_REMOVE),false);
				CheckDlgButton(hWnd,CHK_ANCHOR_ENABLE, BST_UNCHECKED);
				CheckDlgButton(hWnd,CHK_CHILD_ENABLE, BST_UNCHECKED);
				CheckDlgButton(hWnd,CHK_CHILD_ACTIVE, BST_CHECKED);
				
				EnableWindow(GetDlgItem(hWnd,LAB_POS),false);
				EnableWindow(GetDlgItem(hWnd,LAB_ROT),false);
				EnableWindow(GetDlgItem(hWnd,LAB_SCALE),false);
				EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSX),true);
				EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSY),true);
				EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSZ),true);
				if (Scene0.Entity.GetParent().GetScreenAligned())	//Non permette di ruotare xkè lo fa già da solo rispetto alla vista :)
				{
					EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTX),false);
					EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTY),false);
					EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTZ),false);
				}
				else
				{
					EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTX),true);
					EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTY),true);
					EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTZ),true);
				}
				EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEX),true);
				EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEY),true);
				EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEZ),true);

				char bufferTransformation[50]="";
				//Posizione
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetPosition().x);
				SetDlgItemText(hWnd, TXT_CHILDPOSX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetPosition().y);
				SetDlgItemText(hWnd, TXT_CHILDPOSY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetPosition().z);
				SetDlgItemText(hWnd, TXT_CHILDPOSZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetRotation().x);
				SetDlgItemText(hWnd, TXT_CHILDROTX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetRotation().y);
				SetDlgItemText(hWnd, TXT_CHILDROTY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetRotation().z);
				SetDlgItemText(hWnd, TXT_CHILDROTZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetScale().x);
				SetDlgItemText(hWnd, TXT_CHILDSCALEX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetScale().y);
				SetDlgItemText(hWnd, TXT_CHILDSCALEY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParent().GetScale().z);
				SetDlgItemText(hWnd, TXT_CHILDSCALEZ, bufferTransformation);

				//Ancora
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointPos.x);
				SetDlgItemText(hWnd, TXT_ANCHORPOSX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointPos.y);
				SetDlgItemText(hWnd, TXT_ANCHORPOSY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointPos.z);
				SetDlgItemText(hWnd, TXT_ANCHORPOSZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointRot.x);
				SetDlgItemText(hWnd, TXT_ANCHORROTX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointRot.y);
				SetDlgItemText(hWnd, TXT_ANCHORROTY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointRot.z);
				SetDlgItemText(hWnd, TXT_ANCHORROTZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointScale.x);
				SetDlgItemText(hWnd, TXT_ANCHORSCALEX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointScale.y);
				SetDlgItemText(hWnd, TXT_ANCHORSCALEY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetParentProperties().anchorPointScale.z);
				SetDlgItemText(hWnd, TXT_ANCHORSCALEZ, bufferTransformation);
				

				//SetDlgItemText(hWnd, TXT_CHILD_FILENAME, Scene0.Entity.GetParent().GetFilenameFX());
			}
			if (Scene0.currentCHILDID>=0)
			{
				EnableWindow(GetDlgItem(hWnd,CHK_ANCHOR_ENABLE),true);
				EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ENABLE),true);
				EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ACTIVE),true);
				EnableWindow(GetDlgItem(hWnd,BUT_CHILD_REMOVE),true);
				//Ancora abilitata
				if (Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).enableAnchor)
					CheckDlgButton(hWnd,CHK_ANCHOR_ENABLE, BST_CHECKED);
				else	
					CheckDlgButton(hWnd,CHK_ANCHOR_ENABLE, BST_UNCHECKED);

				//Figlio abilitata
				if (Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).enableChild)
					CheckDlgButton(hWnd,CHK_CHILD_ENABLE, BST_CHECKED);
				else	
					CheckDlgButton(hWnd,CHK_CHILD_ENABLE, BST_UNCHECKED);

				//Figlio attivo
				if (Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).active)
					CheckDlgButton(hWnd,CHK_CHILD_ACTIVE, BST_CHECKED);
				else	
					CheckDlgButton(hWnd,CHK_CHILD_ACTIVE, BST_UNCHECKED);


				char bufferTransformation[50]="";
				//Posizione
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetPosition().x);
				SetDlgItemText(hWnd, TXT_CHILDPOSX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetPosition().y);
				SetDlgItemText(hWnd, TXT_CHILDPOSY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetPosition().z);
				SetDlgItemText(hWnd, TXT_CHILDPOSZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetRotation().x);
				SetDlgItemText(hWnd, TXT_CHILDROTX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetRotation().y);
				SetDlgItemText(hWnd, TXT_CHILDROTY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetRotation().z);
				SetDlgItemText(hWnd, TXT_CHILDROTZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetScale().x);
				SetDlgItemText(hWnd, TXT_CHILDSCALEX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetScale().y);
				SetDlgItemText(hWnd, TXT_CHILDSCALEY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChild(Scene0.currentCHILDID).GetScale().z);
				SetDlgItemText(hWnd, TXT_CHILDSCALEZ, bufferTransformation);

				//Ancora
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointPos.x);
				SetDlgItemText(hWnd, TXT_ANCHORPOSX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointPos.y);
				SetDlgItemText(hWnd, TXT_ANCHORPOSY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointPos.z);
				SetDlgItemText(hWnd, TXT_ANCHORPOSZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointRot.x);
				SetDlgItemText(hWnd, TXT_ANCHORROTX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointRot.y);
				SetDlgItemText(hWnd, TXT_ANCHORROTY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointRot.z);
				SetDlgItemText(hWnd, TXT_ANCHORROTZ, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointScale.x);
				SetDlgItemText(hWnd, TXT_ANCHORSCALEX, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointScale.y);
				SetDlgItemText(hWnd, TXT_ANCHORSCALEY, bufferTransformation);
				sprintf(bufferTransformation,"%.3f",Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointScale.z);
				SetDlgItemText(hWnd, TXT_ANCHORSCALEZ, bufferTransformation);

				//SetDlgItemText(hWnd, TXT_CHILD_FILENAME, Scene0.Entity.GetChild(Scene0.currentCHILDID).GetFilenameFX());
			}



			//*** TREE CONTROL ***
 			hWndTree = GetDlgItem(hWnd, TREE_MANAGER);
			SendMessage(hWndTree, TVM_DELETEITEM, 0, (LPARAM) (HTREEITEM)TVI_ROOT); 
			InitTreeViewImageLists(hWndTree,2,16,16);


			AddItemToTree(hWndTree,"Parent",1);
			if (Scene0.Entity.GetTotalChildren()>0)
			{
				for(unsigned long o = 0; o < Scene0.Entity.GetTotalChildren(); o++)
				{
					char bufferTreeChild[50]="";
					sprintf(bufferTreeChild,"Child%.0f",float(o));
					AddItemToTree(hWndTree,bufferTreeChild,2);
				}
			}
			TreeView_Expand(hWndTree, 0, TVE_EXPAND);
		}
		break;


		case WM_CLOSE:
		{
			// If we close the dialog box
		    SendMessage(hWndToolbar, TB_CHECKBUTTON, MENU_EDIT_ENTITIESMANAGER, MAKELONG(false, 0));
			Scene0.showEntitiesManager=false;
			hWndEntitiesManager=NULL;
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		}
		break;
			



		case WM_NOTIFY:
		{	
		    LPNMHDR lpnm = (LPNMHDR) lParam;
	        switch (lpnm->code)
			{

				case TVN_SELCHANGED:
				{
					TVITEM pitem;
					ZeroMemory(&pitem, sizeof(TVITEM));
					pitem.hItem = TreeView_GetSelection(hWndTree);
					pitem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM |TVIF_STATE|TVIF_HANDLE|TVIF_CHILDREN; 
					pitem.cchTextMax = 50;
					char bufferText[50]="";
					pitem.pszText = bufferText;
					TreeView_GetItem(hWndTree,&pitem);
					char strChild[5]="";
					if (strncmp(pitem.pszText, "Child", 5)==0)
					{
						strncpy(strChild,pitem.pszText+5,2);
						Scene0.currentCHILDID = (int)atoi(strChild);
					}
					else
					{
						Scene0.currentCHILDID = -1;
					}

					//Padre
					if (Scene0.currentCHILDID==-1)
					{
						EnableWindow(GetDlgItem(hWnd,CHK_ANCHOR_ENABLE),false);
						EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ENABLE),false);
						EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ACTIVE),false);
						EnableWindow(GetDlgItem(hWnd,BUT_CHILD_REMOVE),false);
						CheckDlgButton(hWnd,CHK_ANCHOR_ENABLE, BST_UNCHECKED);
						CheckDlgButton(hWnd,CHK_CHILD_ENABLE, BST_UNCHECKED);
						CheckDlgButton(hWnd,CHK_CHILD_ACTIVE, BST_CHECKED);
						EnableWindow(GetDlgItem(hWnd,LAB_POS),false);
						EnableWindow(GetDlgItem(hWnd,LAB_ROT),false);
						EnableWindow(GetDlgItem(hWnd,LAB_SCALE),false);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSX),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSY),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSZ),true);
						if (Scene0.Entity.GetParent().GetScreenAligned())	//Non permette di ruotare xkè lo fa già da solo rispetto alla vista :)
						{
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTX),false);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTY),false);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTZ),false);
						}
						else
						{
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTX),true);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTY),true);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTZ),true);
						}
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEX),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEY),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEZ),true);


						char bufferTransformation[50]="";
						//Posizione
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetPosition().x);
						SetDlgItemText(hWnd, TXT_CHILDPOSX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetPosition().y);
						SetDlgItemText(hWnd, TXT_CHILDPOSY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetPosition().z);
						SetDlgItemText(hWnd, TXT_CHILDPOSZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetRotation().x);
						SetDlgItemText(hWnd, TXT_CHILDROTX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetRotation().y);
						SetDlgItemText(hWnd, TXT_CHILDROTY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetRotation().z);
						SetDlgItemText(hWnd, TXT_CHILDROTZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetScale().x);
						SetDlgItemText(hWnd, TXT_CHILDSCALEX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetScale().y);
						SetDlgItemText(hWnd, TXT_CHILDSCALEY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParent().GetScale().z);
						SetDlgItemText(hWnd, TXT_CHILDSCALEZ, bufferTransformation);

						//Ancora
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointPos.x);
						SetDlgItemText(hWnd, TXT_ANCHORPOSX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointPos.y);
						SetDlgItemText(hWnd, TXT_ANCHORPOSY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointPos.z);
						SetDlgItemText(hWnd, TXT_ANCHORPOSZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointRot.x);
						SetDlgItemText(hWnd, TXT_ANCHORROTX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointRot.y);
						SetDlgItemText(hWnd, TXT_ANCHORROTY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointRot.z);
						SetDlgItemText(hWnd, TXT_ANCHORROTZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointScale.x);
						SetDlgItemText(hWnd, TXT_ANCHORSCALEX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointScale.y);
						SetDlgItemText(hWnd, TXT_ANCHORSCALEY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetParentProperties().anchorPointScale.z);
						SetDlgItemText(hWnd, TXT_ANCHORSCALEZ, bufferTransformation);

						//SetDlgItemText(hWnd, TXT_CHILD_FILENAME, Scene0.Entity.GetParent().GetFilenameFX());
					}
					//Figli
					if (Scene0.currentCHILDID>=0)
					{
						EnableWindow(GetDlgItem(hWnd,CHK_ANCHOR_ENABLE),true);
						EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ENABLE),true);
						EnableWindow(GetDlgItem(hWnd,CHK_CHILD_ACTIVE),true);
						EnableWindow(GetDlgItem(hWnd,BUT_CHILD_REMOVE),true);
						EnableWindow(GetDlgItem(hWnd,LAB_POS),true);
						EnableWindow(GetDlgItem(hWnd,LAB_ROT),true);
						EnableWindow(GetDlgItem(hWnd,LAB_SCALE),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSX),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSY),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORPOSZ),true);
						if (Scene0.Entity.GetChild(Scene0.currentCHILDID).GetScreenAligned())	//Non permette di ruotare xkè lo fa già da solo rispetto alla vista :)
						{
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTX),false);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTY),false);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTZ),false);
						}
						else
						{
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTX),true);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTY),true);
							EnableWindow(GetDlgItem(hWnd,TXT_ANCHORROTZ),true);
						}
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEX),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEY),true);
						EnableWindow(GetDlgItem(hWnd,TXT_ANCHORSCALEZ),true);
						
						//Ancora abilitata
						if (Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).enableAnchor)
							CheckDlgButton(hWnd,CHK_ANCHOR_ENABLE, BST_CHECKED);
						else	
							CheckDlgButton(hWnd,CHK_ANCHOR_ENABLE, BST_UNCHECKED);

						//Figlio abilitata
						if (Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).enableChild)
							CheckDlgButton(hWnd,CHK_CHILD_ENABLE, BST_CHECKED);
						else	
							CheckDlgButton(hWnd,CHK_CHILD_ENABLE, BST_UNCHECKED);

						//Figlio attivo
						if (Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).active)
							CheckDlgButton(hWnd,CHK_CHILD_ACTIVE, BST_CHECKED);
						else	
							CheckDlgButton(hWnd,CHK_CHILD_ACTIVE, BST_UNCHECKED);

						char bufferTransformation[50]="";
						//Posizione
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetPosition().x);
						SetDlgItemText(hWnd, TXT_CHILDPOSX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetPosition().y);
						SetDlgItemText(hWnd, TXT_CHILDPOSY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetPosition().z);
						SetDlgItemText(hWnd, TXT_CHILDPOSZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetRotation().x);
						SetDlgItemText(hWnd, TXT_CHILDROTX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetRotation().y);
						SetDlgItemText(hWnd, TXT_CHILDROTY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetRotation().z);
						SetDlgItemText(hWnd, TXT_CHILDROTZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetScale().x);
						SetDlgItemText(hWnd, TXT_CHILDSCALEX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetScale().y);
						SetDlgItemText(hWnd, TXT_CHILDSCALEY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChild(Scene0.currentCHILDID).GetScale().z);
						SetDlgItemText(hWnd, TXT_CHILDSCALEZ, bufferTransformation);

						//Ancora
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointPos.x);
						SetDlgItemText(hWnd, TXT_ANCHORPOSX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointPos.y);
						SetDlgItemText(hWnd, TXT_ANCHORPOSY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointPos.z);
						SetDlgItemText(hWnd, TXT_ANCHORPOSZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointRot.x);
						SetDlgItemText(hWnd, TXT_ANCHORROTX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointRot.y);
						SetDlgItemText(hWnd, TXT_ANCHORROTY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointRot.z);
						SetDlgItemText(hWnd, TXT_ANCHORROTZ, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointScale.x);
						SetDlgItemText(hWnd, TXT_ANCHORSCALEX, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointScale.y);
						SetDlgItemText(hWnd, TXT_ANCHORSCALEY, bufferTransformation);
						sprintf(bufferTransformation,"%.3f",(float)Scene0.Entity.GetChildProperties(Scene0.currentCHILDID).anchorPointScale.z);
						SetDlgItemText(hWnd, TXT_ANCHORSCALEZ, bufferTransformation);

						//Nome del figlio (se c'è)
						//SetDlgItemText(hWnd, TXT_CHILD_FILENAME, Scene0.Entity.GetChild(Scene0.currentCHILDID).GetFilenameFX());
					}


				}

			}
		}
		break;



		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
	
				case CHK_CHILD_ENABLE:
				{
					CheckDlgButton(hWnd,CHK_ANCHOR_ENABLE, BST_CHECKED);
				}
				break;

				case BUT_CHILD_SET:
				{

					if (Scene0.currentCHILDID==-1)
					{
						STRUCT_DE_ENTITYPARENT newParentProperties;
						LPSTR ParentTransform={0};
						ParentTransform=(LPSTR)malloc(20);					
						//Trasformazioni delle ancore
						GetDlgItemText(hWnd,TXT_ANCHORPOSX,ParentTransform,10);
						newParentProperties.anchorPointPos.x = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORPOSY,ParentTransform,10);
						newParentProperties.anchorPointPos.y = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORPOSZ,ParentTransform,10);
						newParentProperties.anchorPointPos.z = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORROTX,ParentTransform,10);
						newParentProperties.anchorPointRot.x = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORROTY,ParentTransform,10);
						newParentProperties.anchorPointRot.y = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORROTZ,ParentTransform,10);
						newParentProperties.anchorPointRot.z = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORSCALEX,ParentTransform,10);
						newParentProperties.anchorPointScale.x = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORSCALEY,ParentTransform,10);
						newParentProperties.anchorPointScale.y = (float)(atof(ParentTransform));
						GetDlgItemText(hWnd,TXT_ANCHORSCALEZ,ParentTransform,10);
						newParentProperties.anchorPointScale.z = (float)(atof(ParentTransform));
						//Setta le proprietà
						Scene0.Entity.SetParentProperties(newParentProperties);
						free(ParentTransform);
					}

					//Setta il figlo corrente
					if (Scene0.currentCHILDID>=0)
					{
						STRUCT_DE_ENTITYCHILD newChildProperties;

						//Attivo
						if(IsDlgButtonChecked(hWnd, CHK_CHILD_ACTIVE))
							newChildProperties.active = true;
						else
							newChildProperties.active = false;			
						//Figlio
						if(IsDlgButtonChecked(hWnd, CHK_CHILD_ENABLE))
							newChildProperties.enableChild = true;
						else
							newChildProperties.enableChild = false;			
						//Ancora
						if(IsDlgButtonChecked(hWnd, CHK_ANCHOR_ENABLE))
						{
							newChildProperties.enableAnchor = true;
							newChildProperties.anchorAxes.Create("Scene0.Entity.Child[...].anchorAxes");
						}
						else
						{
							newChildProperties.enableAnchor = false;			
						}

						LPSTR ChildTransform={0};
						ChildTransform=(LPSTR)malloc(20);					
						//Trasformazioni delle ancore
						GetDlgItemText(hWnd,TXT_ANCHORPOSX,ChildTransform,10);
						newChildProperties.anchorPointPos.x = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORPOSY,ChildTransform,10);
						newChildProperties.anchorPointPos.y = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORPOSZ,ChildTransform,10);
						newChildProperties.anchorPointPos.z = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORROTX,ChildTransform,10);
						newChildProperties.anchorPointRot.x = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORROTY,ChildTransform,10);
						newChildProperties.anchorPointRot.y = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORROTZ,ChildTransform,10);
						newChildProperties.anchorPointRot.z = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORSCALEX,ChildTransform,10);
						newChildProperties.anchorPointScale.x = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORSCALEY,ChildTransform,10);
						newChildProperties.anchorPointScale.y = (float)(atof(ChildTransform));
						GetDlgItemText(hWnd,TXT_ANCHORSCALEZ,ChildTransform,10);
						newChildProperties.anchorPointScale.z = (float)(atof(ChildTransform));
						
						//Setta le proprietà
						Scene0.Entity.SetChildProperties(Scene0.currentCHILDID, newChildProperties);
						free(ChildTransform);
					}



				}
				break;


				case BUT_CHILD_ADD:
				{
					ZeroMemory(&openParamsChild, sizeof(OPENFILENAME));
					openParamsChild.lStructSize = sizeof(OPENFILENAME);
					openParamsChild.hwndOwner = hWnd; 
					openParamsChild.lpstrFilter = dialogFilterChild;	
					openParamsChild.lpstrFileTitle = file_nameChildTitle;				
					openParamsChild.lpstrFile = file_nameChild;				
					openParamsChild.nMaxFile = BUFF_MAX;
					openParamsChild.nMaxFileTitle = BUFF_MAX;
					openParamsChild.lpstrInitialDir = "Effects"; 
					openParamsChild.lpstrTitle = "Open FX Child file...";
					openParamsChild.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
				
					if(GetOpenFileName(&openParamsChild))
					{
						//Controlla che sia una sotto-directory della root principale del programma
						if (DEUtility::InRoot(openParamsChild.lpstrFile))
						{
							char *openFilenameChild;
							openFilenameChild = (char *)malloc(MAX_PATH);
							memset(openFilenameChild, 0, sizeof(char) * MAX_PATH);
							openFilenameChild = DEUtility::NoRoot(openParamsChild.lpstrFile);
										
							SetDlgItemText(hWnd, TXT_CHILD_FILENAME, openFilenameChild);
							CheckDlgButton(hWnd, CHK_ANCHOR_ENABLE, BST_CHECKED);
							CheckDlgButton(hWnd, CHK_CHILD_ENABLE, BST_CHECKED);
							CheckDlgButton(hWnd, CHK_CHILD_ACTIVE, BST_UNCHECKED);
							//Carica subito il modello del figlio specificato
							Scene0.Entity.AddChild(openFilenameChild, true, true, false, D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3(1.0f,1.0f,1.0f));
							
							Scene0.currentCHILDID = -1;
							SendMessage(hWnd,WM_INITDIALOG, 0, 0);
						}
						else
						{
							TCHAR strMsg[512];
							_tcscpy( strMsg, _T("Current file has got wrong path!\n")
												_T("All files must reside in root directory."));
							MessageBox(NULL, strMsg, "Loading Error", MB_ICONWARNING|MB_OK);
						}
					}

				}
				break;


				case BUT_CHILD_REMOVE:
				{
					if (Scene0.currentCHILDID>=0)
					{
						if (MessageBox(NULL,"Are you sure to remove selected child?","Remove child",MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
						{
							Scene0.Entity.RemoveChild(Scene0.currentCHILDID);
							Scene0.currentCHILDID = -1;
							SendMessage(hWnd,WM_INITDIALOG, 0, 0);
						}
					}
				}
				break;

			}
			break;
		}
		break;

		



	}
    return FALSE;
}


#endif