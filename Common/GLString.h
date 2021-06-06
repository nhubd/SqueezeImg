#pragma once

namespace GLC {

class CGLString {
public:
	CGLString();
	CGLString(const wchar_t* string);
	CGLString(const CGLString& string);
	virtual ~CGLString(void);

	const wchar_t* GetBuffer() const { return m_wsBuf; }
	operator const wchar_t*() const { return m_wsBuf; }
	CGLString& operator=(const CGLString& string) {
		if (m_wsBuf != string.m_wsBuf) {
			setBuffer(string.m_wsBuf);
		}
		return *this;
	}
	CGLString operator+(const CGLString& string);
	CGLString& operator+=(const CGLString& string);
	CGLString& operator+=(const wchar_t symbol);
	unsigned int Length() const { return m_nLen; }
	CGLString Replace(const wchar_t wchFind, const wchar_t wchReplace) const;
	CGLString Substring(const unsigned int nStart, const unsigned int nLen = -1) const;
	unsigned int LastIndexOf(const wchar_t wchFind) const;
	wchar_t operator[](const unsigned int Index) const;

	static const CGLString& Empty();
	static CGLString FromInteger(int nValue, int nFromBase = 10);
	static CGLString FromAnsiString(const char* szAnsiString);

private:
	wchar_t* m_wsBuf;
	unsigned int m_nLen;

	void free();
	void allocBuf(const unsigned int len);
	void setBuffer(const wchar_t* string);
	wchar_t* getMerged(const CGLString& wsLeft, const CGLString& wsRight);
	wchar_t* getMerged(const CGLString& wsLeft, const wchar_t symbol);
};

} // namespace GLC