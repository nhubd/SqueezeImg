#pragma once

namespace GLC {

template <typename T>
class CGLArray {
public:
	CGLArray();
	CGLArray(const unsigned int);
	CGLArray(const CGLArray<T>&);
	virtual ~CGLArray();

	unsigned int Length() const { return m_Buf.Length(); }
	T& Add(const T&);
	void Remove(const unsigned int Index);
	T& operator[](const unsigned int Index);
	const T& operator[](const unsigned int Index) const;
	CGLArray<T>& operator=(const CGLArray<T>&);

protected:

private:
	CGLSimpleArray<T*> m_Buf;

	void free();
	void alloc(unsigned int quantity);
	void grow(unsigned int quantity);
};

//////////////////////////////////////////////////////////////////////////
// Public members

template <typename T>
CGLArray<T>::CGLArray() :
		m_Buf()
{
}

template <typename T>
CGLArray<T>::CGLArray(unsigned int size) :
		m_Buf(size)
{
}

template <typename T>
CGLArray<T>::CGLArray(const CGLArray<T>& Array) :
		m_Buf(Array.m_Buf.BufLength())
{
	try {
		for (unsigned int i = 0; i < Array.Length(); i++) {
			T* val = new T(Array[i]);
			if (val == 0) {
				throw CGLException::OutOfMemoryException;
			}
			m_Buf.Add(val);
		}
	} catch (...) {
		free();
		throw;
	}
}

template <typename T>
CGLArray<T>::~CGLArray() {
	free();
}

template <typename T>
T& CGLArray<T>::Add(const T& Value) {
	T* newValue = new T(Value);
	if (newValue == 0) {
		throw CGLException::OutOfMemoryException;
	}
	try {
		m_Buf.Add(newValue);
		return *newValue;
	} catch (...) {
		delete newValue;
		throw;
	}
}

template <typename T>
void CGLArray<T>::Remove(const unsigned int Index) {
	assert(Index < m_Buf.Length());
	T* val = m_Buf[Index];
	delete val;
	m_Buf.Remove(Index);
}

template <typename T>
T& CGLArray<T>::operator[](const unsigned int Index) {
	assert(Index < m_Buf.Length());
	return *(m_Buf[Index]);
}

template <typename T>
const T& CGLArray<T>::operator[](const unsigned int Index) const {
	assert(Index < m_Buf.Length());
	return *(m_Buf[Index]);
}

template <typename T>
CGLArray<T>& CGLArray<T>::operator=(const CGLArray<T>& Array) {
	CGLSimpleArray<T*> newBuf;
	try {
		for (unsigned int i = 0; i < Array.Length(); i++) {
			T* val = new T(Array[i]);
			if (val == 0) {
				throw CGLException::OutOfMemoryException;
			}
			newBuf.Add(val);
		}
	} catch (...) {
		for (unsigned int j = 0; j < newBuf.Length(); j++) {
			delete newBuf[j];
		}
		throw;
	}
	free();
	m_Buf = newBuf;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Private members

template <typename T>
void CGLArray<T>::free() {
	for (unsigned int i = 0; i < m_Buf.Length(); i++) {
		delete m_Buf[i];
	}
	m_Buf.Empty();
}

} // namespace GLC