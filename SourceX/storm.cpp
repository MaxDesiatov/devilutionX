#include "../3rdParty/Radon/Radon/include/Radon.hpp"
#include "pch.h"

DWORD nLastError = 0;

std::string getIniPath()
{
	char path[280];
	int len = GetModuleFileName(ghInst, path, 260);
	path[len - 1] = '/';
	strcat(path, "diablo.ini");

	return path;
}

radon::File ini(getIniPath());

// BOOL STORMAPI SFileCloseArchive(HANDLE hArchive)
// {
// 	UNIMPLEMENTED();
// }

// BOOL STORMAPI SFileCloseFile(HANDLE hFile)
// {
// 	UNIMPLEMENTED();
// }

BOOL STORMAPI SFileDdaBeginEx(HANDLE directsound, DWORD flags, DWORD mask, unsigned __int32 lDistanceToMove,
    signed __int32 volume, signed int a6, int a7)
{
	DUMMY(); // Todo track when the sound can be released, see sfx_stop()
	int bytestoread;
	int nrread;
	void *SFXbuffer;
	SFXbuffer = directsound;

	bytestoread = (int)SFileGetFileSize(directsound, 0);
	SFXbuffer = DiabloAllocPtr(bytestoread);
	SFileReadFile(directsound, (char *)SFXbuffer, bytestoread, (LPDWORD)&nrread, 0);

	SDL_RWops *rw = SDL_RWFromMem(SFXbuffer, bytestoread);
	Mix_Chunk *SoundFX = Mix_LoadWAV_RW(rw, 1);

	Mix_PlayChannel(-1, SoundFX, 0);

	return 1;
}

BOOL STORMAPI SFileDdaDestroy()
{
	DUMMY();
	return 0;
}

BOOL STORMAPI SFileDdaEnd(HANDLE directsound)
{
	DUMMY();
	return 0;
}

BOOL STORMAPI SFileDdaGetPos(HANDLE directsound, int a2, int a3)
{
	DUMMY_ONCE();
	return 0;
}

BOOL STORMAPI SFileDdaInitialize(HANDLE directsound)
{
	DUMMY();
	return 0;
}

BOOL STORMAPI SFileDdaSetVolume(HANDLE directsound, signed int bigvolume, signed int volume)
{
	DUMMY();
	return 0;
}

BOOL STORMAPI SFileGetFileArchive(HANDLE hFile, HANDLE archive)
{
	UNIMPLEMENTED();
}

// LONG STORMAPI SFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
// {
// 	UNIMPLEMENTED();
// }

// BOOL STORMAPI SFileOpenArchive(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE
// *phMpq)
// {
// 	UNIMPLEMENTED();
// }

BOOL STORMAPI SFileOpenFile(const char *filename, HANDLE *phFile)
{
	//eprintf("%s: %s\n", __FUNCTION__, filename);

	BOOL result;

	result = patch_rt_mpq && SFileOpenFileEx((HANDLE)patch_rt_mpq, filename, 0, phFile);
	if (!result) {
		result = SFileOpenFileEx((HANDLE)diabdat_mpq, filename, 0, phFile);
	}

	if (!result || !*phFile) {
		eprintf("%s: Not found: %s\n", __FUNCTION__, filename);
	}
	return result;
}

// BOOL STORMAPI SFileOpenFileEx(HANDLE hMpq, const char *szFileName, DWORD dwSearchScope, HANDLE
// *phFile)
// {
// 	UNIMPLEMENTED();
// }

// BOOL STORMAPI SFileReadFile(HANDLE hFile, void *buffer, DWORD nNumberOfBytesToRead, DWORD *read,
// LONG lpDistanceToMoveHigh)
// {
// 	UNIMPLEMENTED();
// }

BOOL __stdcall SBmpLoadImage(const char *pszFileName, PALETTEENTRY *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *dwHeight, DWORD *pdwBpp)
{
	HANDLE hFile;
	size_t size;
	PCXHeader pcxhdr;
	BYTE paldata[256][3];
	BYTE *dataPtr, *fileBuffer;
	BYTE byte;

	if (pdwWidth)
		*pdwWidth = 0;
	if (dwHeight)
		*dwHeight = 0;
	if (pdwBpp)
		*pdwBpp = 0;

	if (!pszFileName || !*pszFileName) {
		return false;
	}

	if (pBuffer && !dwBuffersize) {
		return false;
	}

	if (!pPalette && !pBuffer && !pdwWidth && !dwHeight) {
		return false;
	}

	if (!SFileOpenFile(pszFileName, &hFile)) {
		return false;
	}

	while (strchr(pszFileName, 92))
		pszFileName = strchr(pszFileName, 92) + 1;

	while (strchr(pszFileName + 1, 46))
		pszFileName = strchr(pszFileName, 46);

	// omit all types except PCX
	if (!pszFileName || _strcmpi(pszFileName, ".pcx")) {
		return false;
	}

	if (!SFileReadFile(hFile, &pcxhdr, 128, 0, 0)) {
		SFileCloseFile(hFile);
		return false;
	}

	int width = pcxhdr.xmax - pcxhdr.xmin + 1;
	int height = pcxhdr.ymax - pcxhdr.ymin + 1;

	if (pdwWidth)
		*pdwWidth = width;
	if (dwHeight)
		*dwHeight = height;
	if (pdwBpp)
		*pdwBpp = pcxhdr.bitsPerPixel;

	if (!pBuffer) {
		SFileSetFilePointer(hFile, 0, 0, 2);
		fileBuffer = NULL;
	} else {
		size = SFileGetFileSize(hFile, 0) - SFileSetFilePointer(hFile, 0, 0, 1);
		fileBuffer = (BYTE *)malloc(size);
	}

	if (fileBuffer) {
		SFileReadFile(hFile, fileBuffer, size, 0, 0);
		dataPtr = fileBuffer;

		for (int j = 0; j < height; j++) {
			for (int x = 0; x < width; dataPtr++) {
				byte = *dataPtr;
				if (byte < 0xC0) {
					*pBuffer = byte;
					*pBuffer++;
					x++;
					continue;
				}
				dataPtr++;

				for (int i = 0; i < (byte & 0x3F); i++) {
					*pBuffer = *dataPtr;
					*pBuffer++;
					x++;
				}
			}
		}

		free(fileBuffer);
	}

	if (pPalette && pcxhdr.bitsPerPixel == 8) {
		SFileSetFilePointer(hFile, -768, 0, 1);
		SFileReadFile(hFile, paldata, 768, 0, 0);
		for (int i = 0; i < 256; i++) {
			pPalette[i].peRed = paldata[i][0];
			pPalette[i].peGreen = paldata[i][1];
			pPalette[i].peBlue = paldata[i][2];
			pPalette[i].peFlags = 0;
		}
	}

	SFileCloseFile(hFile);

	return true;
}

// int __stdcall SFileSetFilePointer(HANDLE, int, HANDLE, int)
// {
// 	UNIMPLEMENTED();
// }

HWND STORMAPI SDrawGetFrameWindow(HWND *sdraw_framewindow)
{
	DUMMY();
	return NULL;
}

// BOOL STORMAPI SDrawManualInitialize(HWND hWnd, LPDIRECTDRAW ddInterface, LPDIRECTDRAWSURFACE
// primarySurface, LPDIRECTDRAWSURFACE surface2, LPDIRECTDRAWSURFACE surface3, LPDIRECTDRAWSURFACE
// backSurface, LPDIRECTDRAWPALETTE ddPalette, HPALETTE hPalette)
//{
//	UNIMPLEMENTED();
//}

void *STORMAPI SMemAlloc(unsigned int amount, char *logfilename, int logline, char defaultValue)
{
	// fprintf(stderr, "%s: %d (%s:%d)\n", __FUNCTION__, amount, logfilename, logline);
	assert(amount != -1);
	return malloc(amount);
}

BOOL STORMAPI SMemFree(void *location, char *logfilename, int logline, char defaultValue)
{
	// fprintf(stderr, "%s: (%s:%d)\n", __FUNCTION__, logfilename, logline);
	assert(location);
	free(location);
	return TRUE;
}

void *STORMAPI SMemReAlloc(void *location, unsigned int amount, char *logfilename, int logline, char defaultValue)
{
	UNIMPLEMENTED();
}

bool getIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, int *dataSize = NULL)
{
	radon::Section *section = ini.getSection(sectionName);
	if (!section)
		return false;

	radon::Key *key = section->getKey(keyName);
	if (!key)
		return false;

	std::string value = key->getStringValue();
	if (dataSize)
		*dataSize = value.length();

	if (string)
		strncpy(string, value.c_str(), stringSize);

	return true;
}

void setIniValue(const char *sectionName, const char *keyName, char *value, int len = 0)
{
	radon::Section *section = ini.getSection(sectionName);
	if (!section) {
		ini.addSection(sectionName);
		section = ini.getSection(sectionName);
	}

	std::string stringValue(value, len ?: strlen(value));

	radon::Key *key = section->getKey(keyName);
	if (!key) {
		section->addKey(radon::Key(keyName, stringValue));
	} else {
		key->setValue(stringValue);
	}

	ini.saveToFile();
}

BOOL STORMAPI SRegLoadData(const char *keyname, const char *valuename, int size, LPBYTE lpData, BYTE flags, LPDWORD lpcbData)
{
	return getIniValue(keyname, valuename, lpData, size, lpcbData);
}

BOOL STORMAPI SRegLoadString(const char *keyname, const char *valuename, BYTE flags, char *buffer, unsigned int buffersize)
{
	return getIniValue(keyname, valuename, buffer, buffersize);
}

BOOL STORMAPI SRegLoadValue(const char *keyname, const char *valuename, BYTE flags, int *value)
{
	char string[10];
	if (getIniValue(keyname, valuename, string, 10)) {
		*value = strtol(string, NULL, 10);
		return TRUE;
	}

	return FALSE;
}

BOOL STORMAPI SRegSaveData(const char *keyname, const char *valuename, int size, BYTE *lpData, DWORD cbData)
{
	setIniValue(keyname, valuename, (char *)lpData, cbData);

	return TRUE;
}

BOOL STORMAPI SRegSaveString(const char *keyname, const char *valuename, BYTE flags, char *string)
{
	setIniValue(keyname, valuename, string);

	return TRUE;
}

BOOL STORMAPI SRegSaveValue(const char *keyname, const char *valuename, BYTE flags, DWORD result)
{
	char str[10];
	sprintf(str, "%d", result);
	setIniValue(keyname, valuename, str);

	return TRUE;
}

// BOOL STORMAPI SVidDestroy()
//{
//	UNIMPLEMENTED();
//}
//
// BOOL STORMAPI SVidInitialize(HANDLE video)
//{
//	UNIMPLEMENTED();
//}
//
// BOOL STORMAPI SVidPlayBegin(char *filename, int arg4, int a3, int a4, int a5, int a6, HANDLE
// *video)
//{
//	UNIMPLEMENTED();
//}
//
// BOOL STORMAPI SVidPlayEnd(HANDLE video)
//{
//	UNIMPLEMENTED();
//}

BOOL STORMAPI SErrDisplayError(DWORD dwErrMsg, const char *logfilename, int logline, const char *message,
    BOOL allowOption, int exitCode)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SErrGetErrorStr(DWORD dwErrCode, char *buffer, unsigned int bufferchars)
{
	DUMMY();
	return FALSE;
}

DWORD STORMAPI SErrGetLastError()
{
	return nLastError;
}

void STORMAPI SErrSetLastError(DWORD dwErrCode)
{
	nLastError = dwErrCode;
}

void STORMAPI SMemCopy(void *dest, const void *source, unsigned int size)
{
	UNIMPLEMENTED();
}

void STORMAPI SMemFill(void *location, unsigned int length, char fillWith)
{
	UNIMPLEMENTED();
}

void STORMAPI SMemZero(void *location, DWORD length)
{
	UNIMPLEMENTED();
}

int STORMAPI SMemCmp(void *location1, void *location2, DWORD size)
{
	UNIMPLEMENTED();
}

int STORMAPI SStrCopy(char *dest, const char *src, int max_length)
{
	strncpy(dest, src, max_length);
	return strlen(dest);
}

int STORMAPI SStrCmp(const char *string1, const char *string2, unsigned int size)
{
	UNIMPLEMENTED();
}

int STORMAPI SStrCmpI(const char *string1, const char *string2, unsigned int size)
{
	UNIMPLEMENTED();
}

void __stdcall SDrawMessageBox(char *Text, char *Title, int Flags)
{
	MessageBox(NULL, Text, Title, Flags);
}

// void __cdecl SDrawDestroy(void)
//{
//	UNIMPLEMENTED();
//}

BOOLEAN __cdecl StormDestroy(void)
{
	DUMMY();
	return TRUE;
}

BOOLEAN __stdcall SFileSetBasePath(char *)
{
	DUMMY();
	return TRUE;
}

void __cdecl SDrawRealizePalette(void)
{
	DUMMY();
}

// bool __cdecl SVidPlayContinue(void)
//{
//	UNIMPLEMENTED();
//}

BOOL __stdcall SFileEnableDirectAccess(BOOL enable)
{
	DUMMY();
	return TRUE;
}
