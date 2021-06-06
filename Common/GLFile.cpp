#include "StdAfx.h"
#include "GLFile.h"

namespace GLC {

CGLFile::CGLFile(void) {
}

CGLFile::~CGLFile(void) {
}

CGLStrings CGLFile::GetFiles(const CGLString folder) {
	CGLStrings res;
	CGLString wsFindPath = folder;
	AppendBackSlash(wsFindPath);
	CGLString wsPath = wsFindPath;
	wsFindPath += L'*';
	WIN32_FIND_DATA findData;
	HANDLE hSearch = FindFirstFileW(wsFindPath, &findData);
	if (hSearch == INVALID_HANDLE_VALUE) {
		throw CGLSystemException::FromLastError();
	}
	try {
		bool bDoFind = true;
		do {
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				CGLString wsFile = findData.cFileName;
				res.Add(wsPath + wsFile);
			}
			BOOL bRes = FindNextFileW(hSearch, &findData);
			if (bRes == 0) {
				DWORD nFindRes = GetLastError();
				if (nFindRes == ERROR_NO_MORE_FILES) {
					bDoFind = false;
				} else {
					throw CGLSystemException::FromLastError();
				}
			}
		} while (bDoFind);
		DWORD nRes = FindClose(hSearch);
		hSearch = 0;
		if (nRes == 0) {
			throw CGLSystemException::FromLastError();
		}
		return res;
	} catch (...) {
		if (hSearch != 0) {
			FindClose(hSearch);
			hSearch = 0;
		}
		throw;
	}
}

void CGLFile::AppendBackSlash(CGLString& folder) {
	wchar_t wchLastChar = folder[folder.Length() - 1];
	if (wchLastChar == L'\\' || wchLastChar == L'/') {
		return;
	}
	folder += L'\\';
}

void CGLFile::GetFileName(const CGLString& path, CGLString& name, CGLString& extension) {
	CGLString wsFilename = path.Replace(L'/', L'\\');
	unsigned int nLastSlash = wsFilename.LastIndexOf(L'\\');
	if (nLastSlash >= 0) {
		if (nLastSlash == wsFilename.Length()) {
			name = CGLString::Empty();
			extension = CGLString::Empty();
			return;
		}
		wsFilename = wsFilename.Substring(nLastSlash + 1);
	}
	unsigned int nLastDot = wsFilename.LastIndexOf(L'.');
	if (nLastDot > 0 && nLastDot < wsFilename.Length() - 1) {
		name = wsFilename.Substring(0, nLastDot);
		extension = wsFilename.Substring(nLastDot + 1);
	} else {
		name = wsFilename;
		extension = CGLString::Empty();
	}
}

void CGLFile::Copy(const CGLString& from, const CGLString& to, bool Overwrite) {
	CopyFile(from, to, Overwrite ? FALSE : TRUE);
}

bool CGLFile::Exists(const CGLString& path) {
	WIN32_FIND_DATA findData;
	HANDLE file = FindFirstFileW(path, &findData);
	if (file == INVALID_HANDLE_VALUE) {
		return false;
	}
	FindClose(file);
	return true;
}

void CGLFile::Delete(const CGLString& path) {
	BOOL bRes = DeleteFile(path);
	if (!bRes) {
		throw CGLSystemException::FromLastError();
	}
}

} // namespace GLC