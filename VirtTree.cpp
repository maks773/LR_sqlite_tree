//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VirtTree.h"
#include "sqlite3.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <string.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VirtualTrees"
#pragma link "VirtualTrees"
#pragma resource "*.dfm"
TForm2 *Form2;

//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{
	VirtualStringTree1->NodeDataSize = sizeof(OriginTable);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender)
{
	sqlite3* Database;
	const char *errmsg;
	sqlite3_stmt *pStatement;
	VirtualStringTree1->Clear();

	int res = sqlite3_open16(L"MediaHistory", &Database);
	if (res != 0) {
		  MessageBox(GetActiveWindow(), L"Не удалось открыть файл", L"Ошибка", MB_ICONERROR); exit(1);
	}

	const wchar_t *sql = L"SELECT id, origin, last_updated_time_s FROM origin;";
	int result = sqlite3_prepare16_v2(Database, sql, -1, &pStatement, NULL);

	if(result != SQLITE_OK)
	  {
		   errmsg = sqlite3_errmsg(Database);
		   MessageBox(GetActiveWindow(), (wchar_t*)errmsg, L"Ошибка", MB_ICONERROR); exit(2);
	  }

	VirtualStringTree1->BeginUpdate();
	while(true)
		{
		   result = sqlite3_step(pStatement);
		   if (result == SQLITE_DONE) break; else if (result != SQLITE_ROW) {
			  MessageBox(GetActiveWindow(), L"Не удалось выполнить запрос", L"Ошибка", MB_ICONERROR); exit(3);
		   }

		   PVirtualNode entryNode = VirtualStringTree1->AddChild(VirtualStringTree1->RootNode);
		   OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(entryNode);
		   table->ID = sqlite3_column_int(pStatement, 0);
		   wchar_t *str = (wchar_t*)sqlite3_column_text16(pStatement, 1);
		   table->origin = UnicodeString(str);
		   wchar_t *str1 = (wchar_t*)sqlite3_column_text16(pStatement, 2);							  sqlite3_column_text16(pStatement, 2);
		   table->last_update_time = UnicodeString(str1);
		}

    VirtualStringTree1->EndUpdate();
	sqlite3_finalize(pStatement);
	sqlite3_close(Database);
	Button3->Enabled = true;
	Button2->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::VirtualStringTree1GetText(TBaseVirtualTree *Sender, PVirtualNode Node,
          TColumnIndex Column, TVSTTextType TextType, UnicodeString &CellText)

{
   if (Node == NULL) return;
   OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(Node);
   switch(Column)
   {
	   case 0:
	   {
			CellText = UnicodeString(table->ID); break;
       }
	   case 1:
	   {
			CellText = table->origin; break;
       }
   }
}
//---------------------------------------------------------------------------


void __fastcall TForm2::VirtualStringTree1AddToSelection(TBaseVirtualTree *Sender,
          PVirtualNode Node)
{
	  if (Node == NULL) return;
	  OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(Node);
	  Label2->Caption = table->last_update_time;
	  if (VirtualStringTree1->FocusedNode) Button2->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm2::Button2Click(TObject *Sender)
{
	sqlite3* Database;
	const char *errmsg;
	sqlite3_stmt *pStatement;
	PVirtualNode selNode = VirtualStringTree1->FocusedNode;
	OriginTable *table = (OriginTable*)selNode->GetData();

	int res = sqlite3_open16(L"MediaHistory", &Database);
	if (res != 0) {
		  MessageBox(GetActiveWindow(), L"Не удалось открыть файл", L"Ошибка", MB_ICONERROR); exit(1);
	}

	wchar_t sql[] = L"DELETE FROM origin WHERE id='";
	wchar_t *a = new wchar_t[sizeof(table->ID)];
	_itow(table->ID,a,10);  wchar_t b[] = L";";  wchar_t c[] = L"'";
	wcscat(sql, a); wcscat(sql, c); wcscat(sql, b);

	int result = sqlite3_prepare16_v2(Database, sql, -1, &pStatement, NULL);
	if(result != SQLITE_OK)
	  {
		   errmsg = sqlite3_errmsg(Database);
		   MessageBox(GetActiveWindow(), (wchar_t*)errmsg, L"Ошибка", MB_ICONERROR); exit(2);
	  }

	result = sqlite3_step(pStatement);
	if(result != SQLITE_DONE)
	{
		   errmsg = sqlite3_errmsg(Database);
		   MessageBox(GetActiveWindow(), (wchar_t*)errmsg, L"Ошибка", MB_ICONERROR); exit(3);
	}

	sqlite3_finalize(pStatement);
	sqlite3_close(Database);
	delete[] a;
	Label2->Caption = "";
	Button2->Enabled = false;
	VirtualStringTree1->DeleteNode(VirtualStringTree1->FocusedNode);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button3Click(TObject *Sender)
{
	sqlite3* Database;
	const char *errmsg;
	sqlite3_stmt *pStatement;

	int res = sqlite3_open16(L"MediaHistory", &Database);
	if (res != 0) {
		  MessageBox(GetActiveWindow(), L"Не удалось открыть файл", L"Ошибка", MB_ICONERROR); exit(1);
	}

	wchar_t *sql = L"DELETE FROM origin";

	int result = sqlite3_prepare16_v2(Database, sql, -1, &pStatement, NULL);

	if(result != SQLITE_OK)
	  {
		   errmsg = sqlite3_errmsg(Database);
		   MessageBox(GetActiveWindow(), (wchar_t*)errmsg, L"Ошибка", MB_ICONERROR); exit(2);
	  }

	result = sqlite3_step(pStatement);

	if(result != SQLITE_DONE)
	{
		   errmsg = sqlite3_errmsg(Database);
		   MessageBox(GetActiveWindow(), (wchar_t*)errmsg, L"Ошибка", MB_ICONERROR); exit(3);
	}

	sqlite3_finalize(pStatement);
	sqlite3_close(Database);

	Label2->Caption = "";
	Button3->Enabled = false;
	VirtualStringTree1->Clear();
}
//---------------------------------------------------------------------------

