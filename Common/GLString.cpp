#include "stdafx.h"
#include "GLString.h"
#include <string.h>
#include <stdlib.h> // for _itow

namespace GLC {

namespace {
	const CGLString ms_wsEmpty = CGLString((wchar_t*)L"");
}

CGLString::CGLString() :
		m_wsBuf(0),
		m_nLen(0)
{
	setBuffer(ms_wsEmpty.m_wsBuf);
}

CGLString::CGLString(const wchar_t* string) :
		m_wsBuf(0),
		m_nLen(0)
{
	setBuffer(string);
}

CGLString::CGLString(const CGLString& string) :
		m_wsBuf(0),
		m_nLen(0)
{
	setBuffer(string);
}

CGLString::~CGLString(void) {
	free();
}

void CGLString::free() {
	delete[] m_wsBuf;
	m_wsBuf = 0;
	m_nLen = 0;
}

void CGLString::allocBuf(unsigned int len) {
	assert(m_wsBuf == 0);
	m_wsBuf = new wchar_t[len + 1];
	if (m_wsBuf == 0) {
		throw CGLException::OutOfMemoryException;
	}
}

void CGLString::setBuffer(const wchar_t* string) {
	free();
	unsigned int nLen = (unsigned int)wcslen(string);
	allocBuf(nLen);
	assert(wcsncpy(m_wsBuf, string, nLen) == m_wsBuf);
	m_wsBuf[nLen] = L'\0';
	m_nLen = nLen;
}

wchar_t CGLString::operator[](const unsigned int Index) const {
	assert(Index < m_nLen);
	return m_wsBuf[Index];
}

CGLString CGLString::operator+(const CGLString& string) {
	CGLString wsRes(CGLString::Empty());
	wchar_t* wszMerged = getMerged(*this, string);
	wsRes.free();
	wsRes.m_wsBuf = wszMerged;
	wsRes.m_nLen = m_nLen + string.m_nLen;
	return wsRes;
}

CGLString& CGLString::operator+=(const CGLString& string) {
	wchar_t* wszMerged = getMerged(*this, string);
	unsigned int nLen = m_nLen + string.m_nLen;
	free();
	m_wsBuf = wszMerged;
	m_nLen = nLen;
	return *this;
}

CGLString& CGLString::operator+=(const wchar_t symbol) {
	wchar_t* wszMerged = getMerged(*this, symbol);
	unsigned int nLen = m_nLen + 1;
	free();
	m_wsBuf = wszMerged;
	m_nLen = nLen;
	return *this;
}

wchar_t* CGLString::getMerged(const CGLString& wsLeft, const CGLString& wsRight) {
	wchar_t* wszRes = new wchar_t[wsLeft.m_nLen + wsRight.m_nLen + 1];
	if (wszRes == 0) {
		throw CGLException::OutOfMemoryException;
	}
	memcpy(wszRes, wsLeft.m_wsBuf, sizeof(wchar_t) * wsLeft.m_nLen);
	memcpy(wszRes + wsLeft.m_nLen, wsRight.m_wsBuf, sizeof(wchar_t) * wsRight.m_nLen);
	wszRes[wsLeft.m_nLen + wsRight.m_nLen] = L'\0';
	return wszRes;
}

wchar_t* CGLString::getMerged(const CGLString& wsLeft, const wchar_t symbol) {
	wchar_t* wszRes = new wchar_t[wsLeft.m_nLen + 2];
	if (wszRes == 0) {
		throw CGLException::OutOfMemoryException;
	}
	memcpy(wszRes, wsLeft.m_wsBuf, sizeof(wchar_t) * wsLeft.m_nLen);
	wszRes[wsLeft.m_nLen] = symbol;
	wszRes[wsLeft.m_nLen + 1] = L'\0';
	return wszRes;
}

const CGLString& CGLString::Empty() {
	return ms_wsEmpty;
}

CGLString CGLString::Replace(const wchar_t wchFind, const wchar_t wchReplace) const {
	CGLString wsRes(*this);
	for (unsigned int i = 0; i < wsRes.m_nLen; i++) {
		if (wsRes.m_wsBuf[i] == wchFind) {
			wsRes.m_wsBuf[i] = wchReplace;
		}
	}
	return wsRes;
}

CGLString CGLString::Substring(const unsigned int nStart, const unsigned int nLen) const {
	assert(nStart >= 0 && nStart < m_nLen);
	unsigned int len = nLen;
	if (len == -1) {
		len = m_nLen - nStart;
	}
	assert(len > 0 && len <= m_nLen - nStart);
	wchar_t wchOld = m_wsBuf[nStart + len];
	m_wsBuf[nStart + len] = L'\0';
	try {
		CGLString wsRes(&(m_wsBuf[nStart]));
		m_wsBuf[nStart + len] = wchOld;
		return wsRes;
	} catch (...) {
		m_wsBuf[nStart + len] = wchOld;
		throw;
	}
}

unsigned int CGLString::LastIndexOf(const wchar_t wchFind) const {
	for (unsigned int i = m_nLen - 1; i >= 0; i--) {
		if (m_wsBuf[i] == wchFind) {
			return i;
		}
	}
	return -1;
}

CGLString CGLString::FromInteger(int nValue, int nFromBase/* = 10*/) {
	wchar_t wszBuf[35];
	assert(nFromBase >= 2 && nFromBase <= 36);
	_itow(nValue, wszBuf, nFromBase);
	return wszBuf;
}

CGLString CGLString::FromAnsiString(const char* szAnsiString) {
	int nLen = (int)strlen(szAnsiString);
	wchar_t* wszBuf = new wchar_t[nLen + 1];
	try {
		int nRes = MultiByteToWideChar(CP_ACP, 0, szAnsiString, nLen, wszBuf, nLen);
		if (nRes != 0) {
			wszBuf[nLen] = L'\0';
			CGLString wsRes(wszBuf);
			delete[] wszBuf;
			return wsRes;
		} else {
			throw CGLSystemException::FromLastError();
		}
	} catch (...) {
		delete[] wszBuf;
		throw;
	}
}

} // namespace GLC