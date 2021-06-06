//////////////////////////////////////////////////////////////////////////
// Exception.h
// The base class for all exceptions
//////////////////////////////////////////////////////////////////////////

#pragma once

namespace GLC {

class CGLOutOfMemoryException;

class CGLException {
public:
	CGLException(const int Number, const CGLString& Message);
	CGLException(const CGLString& Message);
	virtual ~CGLException(void);

	int GetNumber() const {
		return m_nNumber;
	}

	const CGLString& GetMessage() const {
		return m_wsMessage;
	}

	static const CGLOutOfMemoryException OutOfMemoryException;

protected:
	int m_nNumber;
	GLC::CGLString m_wsMessage;

private:
	//static COutOfMemoryException ms_outOfMemoryExc;
};

class CGLOutOfMemoryException : public CGLException {
public:
	CGLOutOfMemoryException(const CGLString Message) :
			CGLException(Message) {
	}
};

class CGLBadParameterException : public CGLException {
public:
	CGLBadParameterException(const CGLString Message) :
			CGLException(Message) {
	}
};

class CGLSystemException : public CGLException {
public:
	CGLSystemException(const int Number, const CGLString& Message) :
			CGLException(Number, Message)
	{
	}

	static CGLSystemException FromLastError();
};

} // namespace GLC