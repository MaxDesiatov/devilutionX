#pragma once

BOOL EndDialog(HWND hDlg, INT_PTR nResult);
BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString);
BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
DWORD FormatMessage(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, char *lpBuffer, DWORD nSize, va_list *Arguments);
int DialogBoxParam(HINSTANCE hInstance, int msgId, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
int MAKEINTRESOURCE(int i);
