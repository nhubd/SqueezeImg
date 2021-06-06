#include "StdAfx.h"
#include "GLException.h"

namespace GLC {

const CGLOutOfMemoryException CGLException::OutOfMemoryException = CGLOutOfMemoryException(L"Out of memory");

CGLException::CGLException(const int Number, const CGLString& Message) :
		m_nNumber(Number),
		m_wsMessage(Message)
{
}

CGLException::CGLException(const CGLString& Message) :
		m_nNumber(0),
		m_wsMessage(Message)
{
}

CGLException::~CGLException(void) {
}

CGLSystemException CGLSystemException::FromLastError() {
	void* lpMsgBuf;
	DWORD dw = GetLastError(); 

	DWORD nRes = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, 0);

	try {
		CGLString sMsg = CGLString::Empty();
		if (nRes != 0) {
			sMsg = ((wchar_t*)lpMsgBuf);
		} else {
			sMsg = CGLString(L"System error: ") + CGLString::FromInteger(dw);
		}
		CGLSystemException exc(dw, sMsg);
		LocalFree(lpMsgBuf);
		return exc;
	} catch (...) {
		LocalFree(lpMsgBuf);
		throw;
	}
}

} // namespace GLC