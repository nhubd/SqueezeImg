#pragma once

#include <memory.h>

namespace GLC {

template <typename T>
class CGLSimpleArray {
public:
	CGLSimpleArray();
	CGLSimpleArray(unsigned int MaxSize);
	CGLSimpleArray(const CGLSimpleArray<T>&);
	virtual ~CGLSimpleArray(void);

	unsigned int Length() const { return m_nLen; }
	unsigned int BufLength() const { return m_nBufLen; }
	void Add(const T);
	void Remove(const unsigned int Index);
	void Empty();
	T& operator[](const unsigned int Index);
	const T& operator[](const unsigned int Index) const;
	CGLSimpleArray<T>& operator=(const CGLSimpleArray<T>&);

	static const unsigned int MIN_LEN = 10;

protected:

private:
	T* m_pBuf;
	unsigned int m_nLen; // Current quantity of elements in the buffer
	unsigned int m_nBufLen; // Maximum quantity of elements in the buffer

	void free();
	T* alloc(unsigned int quantity);
	void grow(unsigned int quantity);
};

//////////////////////////////////////////////////////////////////////////
// Public members

template <typename T>
CGLSimpleArray<T>::CGLSimpleArray() :
		m_nBufLen(MIN_LEN),
		m_pBuf(0),
		m_nLen(0)
{
	m_pBuf = alloc(m_nBufLen);
}

template <typename T>
CGLSimpleArray<T>::CGLSimpleArray(unsigned int size) :
		m_nBufLen(MIN_LEN),
		m_pBuf(0),
		m_nLen(0)
{
	if (m_nBufLen < MIN_LEN) {
		m_nBufLen = MIN_LEN;
	}
	m_pBuf = alloc(m_nBufLen);
}

template <typename T>
CGLSimpleArray<T>::CGLSimpleArray(const CGLSimpleArray<T>& Array) :
		m_nLen(Array.m_nLen),
		m_nBufLen(Array.m_nLen),
		m_pBuf(0)
{
	m_pBuf = alloc(m_nBufLen);
	memcpy(m_pBuf, Array.m_pBuf, m_nLen * sizeof(T));
}

template <typename T>
CGLSimpleArray<T>::~CGLSimpleArray() {
	free();
}

template <typename T>
void CGLSimpleArray<T>::Add(const T Value) {
	if (m_nLen >= m_nBufLen) {
		grow(m_nBufLen * 2);
	}
	m_nLen++;
	m_pBuf[m_nLen - 1] = Value;
}

template <typename T>
void CGLSimpleArray<T>::Remove(const unsigned int Index) {
	assert(Index < m_nLen);
	m_nLen--;
	if (Index == m_nLen) {
		return;
	}
	byte* pTarAddr = (byte*)m_pBuf + Index * sizeof(T);
	byte* pSrcAddr = pTarAddr + sizeof(T);
	unsigned int nCnt = (m_nLen - Index) * sizeof(T);
	memmove(pTarAddr, pSrcAddr, nCnt);
}

template <typename T>
void CGLSimpleArray<T>::Empty() {
	m_nLen = 0;
}

template <typename T>
T& CGLSimpleArray<T>::operator[](const unsigned int Index) {
	assert(Index < m_nLen);
	return m_pBuf[Index];
}

template <typename T>
const T& CGLSimpleArray<T>::operator[](const unsigned int Index) const {
	assert(Index < m_nLen);
	return m_pBuf[Index];
}

template <typename T>
CGLSimpleArray<T>& CGLSimpleArray<T>::operator=(const CGLSimpleArray<T>& Array) {
	if (Array.m_pBuf == m_pBuf) {
		return *this;
	}
	free();
	m_nBufLen = Array.m_nBufLen;
	m_nLen = Array.m_nLen;
	m_pBuf = alloc(m_nBufLen);
	memcpy(m_pBuf, Array.m_pBuf, m_nLen * sizeof(T));
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Private members

template <typename T>
void CGLSimpleArray<T>::free() {
	assert(m_nLen <= m_nBufLen);
	delete[] (char*)m_pBuf;
	m_nLen = 0;
	m_nBufLen = 0;
}

template <typename T>
T* CGLSimpleArray<T>::alloc(unsigned int quantity) {
	T* pBuf = reinterpret_cast<T*>(new char[quantity * sizeof(T)]);
	if (pBuf == 0) {
		throw CGLException::OutOfMemoryException;
	}
	return pBuf;
}

template <typename T>
void CGLSimpleArray<T>::grow(unsigned int quantity) {
	assert(quantity > m_nBufLen);
	T* pNewBuf = alloc(quantity);
	memcpy(pNewBuf, m_pBuf, m_nLen * sizeof(T));
	delete[] (char*)m_pBuf;
	m_pBuf = pNewBuf;
	m_nBufLen = quantity;
}

} // namespace GLC