#pragma once

namespace GLC {

template <typename T>
class CGLArray2D {
public:
	CGLArray2D(int nRows, int nColumns);
	virtual ~CGLArray2D();

	int GetWidth() const { return m_nColumns; }
	int GetHeight() const { return m_nRows; }

	inline T& Value(int nRow, int nColumn);
	inline const T& Value(int nRow, int nColumn) const;

protected:

private:
	T** m_nBuf;
	int m_nRows;
	int m_nColumns;

	CGLArray2D(const CGLArray2D&);
	CGLArray2D& operator=(const CGLArray2D&);

	void deleteBuf();
	void createBuf(int nRows, int nColumns);
};

template <typename T>
CGLArray2D<T>::CGLArray2D<T>(int nRows, int nColumns) :
		m_nBuf(0)
{
	assert(nRows > 0);
	assert(nColumns > 0);

	createBuf(nRows, nColumns);

	assert(m_nBuf != 0);
	assert(m_nRows > 0);
	assert(m_nColumns > 0);
}

template <typename T>
CGLArray2D<T>::~CGLArray2D<T>() {
	deleteBuf();
}

template <typename T>
T& CGLArray2D<T>::Value(int nRow, int nColumn) {
	assert(nRow >= 0 && nRow < m_nRows);
	assert(nColumn >= 0 && nColumn < m_nColumns);
	return m_nBuf[nRow][nColumn];
}

template <typename T>
const T& CGLArray2D<T>::Value(int nRow, int nColumn) const {
	assert(nRow >= 0 && nRow < m_nRows);
	assert(nColumn >= 0 && nColumn < m_nColumns);
	return m_nBuf[nRow][nColumn];
}

template <typename T>
void CGLArray2D<T>::createBuf(int nRows, int nColumns) {
	assert(m_nBuf == 0);
	m_nBuf = new T*[nRows];
	if (m_nBuf == 0) throw CGLException::OutOfMemoryException;
	m_nRows = nRows;
	int i;
	for (i = 0; i < nRows; i++) {
		m_nBuf[i] = 0;
	}
	try {
		for (i = 0; i < nRows; i++) {
			m_nBuf[i] = new T[nColumns];
			if (m_nBuf[i] == 0) throw CGLException::OutOfMemoryException;
		}
		m_nColumns = nColumns;
	} catch (...) {
		deleteBuf();
		throw;
	}
}

template <typename T>
void CGLArray2D<T>::deleteBuf() {
	assert(m_nBuf != 0);
	assert(m_nRows > 0);
	for (int i = 0; i < m_nRows; i++) {
		delete[] m_nBuf[i];
	}
	delete[] m_nBuf;
	m_nBuf = 0;
	m_nRows = 0;
	m_nColumns = 0;
}

} // namespace GLC