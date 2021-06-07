#include "StdAfx.h"
#include "ImgSqueeze.h"
#include <math.h>
#include "tiff\tiff.h"

using namespace GLC;

namespace {
	const int ms_nGrayLevelCount = 256;
	const int ms_nMaximumsMinDistance = 100; // In gray levels
	const int ms_nMaximumNeighborhood = 20; // gray levels to left and to right from maximum
	const double ms_dGrayPercentageAroundMaximumsSure = 90.0; // how much pixels must have gray levels around two maximums for surely black-and-white images
	const double ms_dGrayPercentageAroundMaximumsAppr = 70.0; // how much pixels must have gray levels around two maximums for images that are possibly black-and-white
	const double ms_dBlackEdgePercentage = 5.0; // How much black pixels in a normalized edge picture must exist for non-sure images

	struct TGrayMinMax {
		BYTE Minimum;
		BYTE Maximum;
	};
}

CImgSqueeze::CImgSqueeze(void) {
}

CImgSqueeze::~CImgSqueeze(void) {
}

void CImgSqueeze::SqueezeImage(const GLC::CGLString SourceFile, const GLC::CGLString TargetDir) {
	CxImage img;
	CGLString wsOutputFilename;
	loadImage(img, SourceFile);
	WORD nBpp = img.GetBpp();
	EConvertTo fmt = convertTo(img);
	bool bIsBW = false;
	int nThreshold = -1;
	if (fmt == CT_TIFF1 && canBinarize(img, bIsBW, nThreshold)) {
		if (!bIsBW) {
			img.GrayScale();
			binarize(img, nThreshold);
		}
		img.Threshold((BYTE)nThreshold);
		wsOutputFilename = getOutputFilename(SourceFile, TargetDir, fmt);
		img.SetCodecOption(COMPRESSION_CCITTFAX4);
		img.SetXDPI(96);
		img.SetYDPI(96);
		img.Save(wsOutputFilename, CXIMAGE_FORMAT_TIF);
		return;
	} else if (img.GetType() != CXIMAGE_FORMAT_TIF && img.GetType() != CXIMAGE_FORMAT_JPG) {
		// Just save the image as JPEG
		if (IsImageGray(img)) {
			img.GrayScale();
		}
		wsOutputFilename = getOutputFilename(SourceFile, TargetDir, CT_JPEG24); // BPP is irrelevant, we need to get the jpg extention
		img.Save(wsOutputFilename, CXIMAGE_FORMAT_JPG);
		return;
	} else {
		// Just copy the image file
		EConvertTo format = CT_JPEG24;
		if (img.GetType() == CXIMAGE_FORMAT_TIF) {
			format = CT_TIFF1;
		}
		wsOutputFilename = getOutputFilename(SourceFile, TargetDir, format); // BPP is irrelevant, we need to get the jpg extention
		CGLFile::Copy(SourceFile, wsOutputFilename, true);
	}
}

void CImgSqueeze::loadImage(CxImage& img, const GLC::CGLString& file) {
	SetLastError(0);
	img.Load(file);
	if (img.GetType() == CXIMAGE_FORMAT_UNKNOWN) {
		if (img.GetLastError() == 0
				|| (img.GetLastError() != 0 && strlen(img.GetLastError()) == 0)) {
			if (GetLastError() == 0) {
				throw CGLException(L"Cannot read image due to an unspecified error");
			} else {
				throw CGLSystemException::FromLastError();
			}
		} else {
			throw CGLException(CGLString::FromAnsiString(img.GetLastError()));
		}
	}
}

CGLString CImgSqueeze::getOutputFilename(const CGLString& wsInputFilename,
		const CGLString& wsOutputFolder, const EConvertTo format) {
	CGLString wsFilename;
	CGLString wsExtension;
	GLC::CGLFile::GetFileName(wsInputFilename, wsFilename, wsExtension);
	switch (format) {
	case CT_TIFF1:
		wsExtension = L"tif";
		break;
	case CT_JPEG24:
	case CT_JPEG8:
		wsExtension = L"jpg";
		break;
	default:
		assert(false);
	}
	CGLString wsOutputFilename = wsOutputFolder;
	GLC::CGLFile::AppendBackSlash(wsOutputFilename);
	return wsOutputFilename + wsFilename + L"." + wsExtension;
}

CImgSqueeze::EConvertTo CImgSqueeze::convertTo(CxImage& img) {
	assert(img.GetType() != CXIMAGE_FORMAT_UNKNOWN);
	if (IsGrayScale(img)) {
		return CT_TIFF1;
	}
	if (IsImageGray(img)) {
		return CT_TIFF1;
	}
	return CT_JPEG24;
}

bool CImgSqueeze::canBinarize(CxImage& img, bool& bIsBW, int& nThreshold) {
	assert(img.GetType() != CXIMAGE_FORMAT_UNKNOWN);
	bIsBW = false;
	nThreshold = 0;
	long histogram[ms_nGrayLevelCount];
	long nMaximum = img.Histogram(0, 0, 0, histogram);
	assert(nMaximum > 0);
	int nMax1Idx = -1; // Index of first gray level maximum
	long nMax1Value = -1; // Value of the first gray level maximum
	int nMax2Idx = -1; // Index of second gray level maximum
	long nMax2Value = -1; // Value of the second gray level maximum
	int nIdx;
	// Find first maximum
	for (nIdx = 0; nIdx < ms_nGrayLevelCount; nIdx++) {
		if (nMax1Idx < 0 || (histogram[nIdx] > nMax1Value)) {
			nMax1Idx = nIdx;
			nMax1Value = histogram[nIdx];
		}
	}
	// Find second maximum
	for (nIdx = 0; nIdx < ms_nGrayLevelCount; nIdx++) {
		bool bCanBe2ndMax = (nMax1Idx < 0 || abs(nIdx - nMax1Idx) >= ms_nMaximumsMinDistance);
		if (!bCanBe2ndMax) {
			continue;
		}
		if (nMax2Idx < 0 || (histogram[nIdx] > nMax2Value)) {
			nMax2Idx = nIdx;
			nMax2Value = histogram[nIdx];
		}
	}
	assert(nMax1Idx >= 0 && nMax2Idx >= 0);
	nThreshold = (nMax1Idx + nMax2Idx) / 2; // TODO: calculate threshold from other data if necessary
	long nPixelsTotal = img.GetWidth() * img.GetHeight();
	if (histogram[nMax1Idx] + histogram[nMax2Idx] == nPixelsTotal) {
		// This is a binary image
		bIsBW = true;
		return true;
	}
	// Calculate the number of pixels colored with levels around 1st and 2nd maximums
	long nPixels1 = 0;
	long nPixels2 = 0;
	for (nIdx = 0; nIdx < ms_nGrayLevelCount; nIdx++) {
		if (nIdx >= nMax1Idx - ms_nMaximumNeighborhood
				&& nIdx <= nMax1Idx + ms_nMaximumNeighborhood) {
			nPixels1 += histogram[nIdx];
		}
		if (nIdx >= nMax2Idx - ms_nMaximumNeighborhood
				&& nIdx <= nMax2Idx + ms_nMaximumNeighborhood) {
			nPixels2 += histogram[nIdx];
		}
	}
	double dPercentage = double(nPixels1 + nPixels2) / double(nPixelsTotal) * 100.0;

	if (dPercentage >= ms_dGrayPercentageAroundMaximumsSure) {
		return true;
	} else if (dPercentage < ms_dGrayPercentageAroundMaximumsAppr) {
		return false;
	}
	CxImage edge;
	edge.Copy(img, true, false, false);
	edge.GrayScale();
	edge.Edge();
	autoLevels(edge);
	double dBlackPercentage = getHistogramPercentage(edge, 0, 128);
	if (dBlackPercentage >= ms_dBlackEdgePercentage) {
		return true;
	} else {
		return false;
	}
}

void CImgSqueeze::binarize(CxImage& img, BYTE nThreshold) {
	CGLArray2D<BYTE> arrImg(img.GetHeight(), img.GetWidth());
	copyImageToArray(img, arrImg);
	binarize(arrImg, nThreshold);
	copyArrayToImage(arrImg, img);
}

void CImgSqueeze::binarize(CGLArray2D<BYTE>& img, BYTE nThreshold) {
	int nSquareSize = 3;
	int nSquareHalf = nSquareSize / 2;
	CGLSimpleArray<TGrayMinMax> grayRng = CGLSimpleArray<TGrayMinMax>(nSquareSize);
	TGrayMinMax val;
	val.Maximum = 0;
	val.Minimum = 0;
	for (int i = 0; i < nSquareSize; i++) grayRng.Add(val);
	int nY, nY1;
	int nX, nX1;
	int nImgWidth = img.GetWidth();
	int nImgHeight = img.GetHeight();
	CGLArray2D<BYTE> res(nImgHeight, nImgWidth);
	BYTE nCurMinimum;
	BYTE nCurMaximum;
	// Go from left to right
	for (nX = 0; nX < nImgWidth; nX++) {
		// Initialize gray ranges
		for (nY1 = 0; nY1 < nSquareSize; nY1++) {
			if (nY1 - nSquareHalf < 0) {
				grayRng[nY1].Minimum = img.Value(0, nX);
				grayRng[nY1].Maximum = img.Value(0, nX);
			} else {
				grayRng[nY1].Minimum = 255;
				grayRng[nY1].Maximum = 0;
				for (nX1 = nX - nSquareHalf; nX1 <= nX + nSquareHalf; nX1++) {
					if (nX1 < 0 || nX1 >= nImgWidth) continue;
					if (img.Value(nY1 - nSquareHalf, nX1) < grayRng[nY1].Minimum) {
						grayRng[nY1].Minimum = img.Value(nY1 - nSquareHalf, nX1);
					}
					if (img.Value(nY1 - nSquareHalf, nX1) > grayRng[nY1].Maximum) {
						grayRng[nY1].Maximum = img.Value(nY1 - nSquareHalf, nX1);
					}
				}
			}
		}
		int nCurGray = nSquareSize - 1;
		// Go from top to bottom
		for (int nY = 0; nY < nImgHeight; nY++) {
			// Fill next gray range
			grayRng[nCurGray].Minimum = 255;
			grayRng[nCurGray].Maximum = 0;
			nY1 = nY + nSquareHalf;
			if (nY1 < nImgHeight) {
				// Go through horizontal strip
				for (nX1 = nX - nSquareHalf; nX1 <= nX + nSquareHalf; nX1++) {
					if (nX1 < 0 || nX1 >= nImgWidth) continue;
					BYTE nNextVal = img.Value(nY1, nX1);
					if (nNextVal < grayRng[nCurGray].Minimum) {
						grayRng[nCurGray].Minimum = nNextVal;
					} else if (nNextVal > grayRng[nCurGray].Maximum) {
						grayRng[nCurGray].Maximum = nNextVal;
					}
				}
			} else {
				grayRng[nCurGray].Minimum = img.Value(nImgHeight - 1, nX);
				grayRng[nCurGray].Maximum = img.Value(nImgHeight - 1, nX);
			}
			// Find current minimum and maximum
			nCurMinimum = 255;
			nCurMaximum = 0;
			for (int i = 0; i < nSquareSize; i++) {
				if (nCurMaximum < grayRng[i].Maximum) {
					nCurMaximum = grayRng[i].Maximum;
				}
				if (nCurMinimum > grayRng[i].Minimum) {
					nCurMinimum = grayRng[i].Minimum;
				}
			}
			int nThresholdToUse = nThreshold;
			if (nCurMaximum - nCurMinimum > 50) {
				// Use adopted threshold
				nThresholdToUse = (nCurMaximum + nCurMinimum) / 2;
			}
			if (nThresholdToUse < nThreshold) {
				nThresholdToUse = nThreshold;
			}
			if (img.Value(nY, nX) > nThresholdToUse) {
				res.Value(nY, nX) = 255;
			} else {
				res.Value(nY, nX) = 0;
			}
			nCurGray++;
			if (nCurGray >= nSquareSize) {
				nCurGray = 0;
			}
		}
	}
	for (nY = 0; nY < nImgHeight; nY++) {
		for (nX = 0; nX < nImgWidth; nX++) {
			img.Value(nY, nX) = res.Value(nY, nX);
		}
	}
}

void CImgSqueeze::copyImageToArray(const CxImage& img, CGLArray2D<BYTE>& array) {
	assert(img.GetWidth() == array.GetWidth());
	assert(img.GetHeight() == array.GetHeight());
	BYTE nVal = 0;
	for (int i = 0; i < (int)img.GetHeight(); i++) {
		for (int j = 0; j < (int)img.GetWidth(); j++) {
			//nVal = img.GetPixelGray((long)j, (long)i);
			//BYTE& val = array.Value(i, j);
			array.Value(i, j) = img.GetPixelGray((long)j, (long)i);
		}
	}
	//for (int i = 0; i < 30; i++) {
	//	for (int j = 0; j < 30; j++) {
	//		TRACE(L"%d ", array.Value(i, j));
	//	}
	//	TRACE(L"r\n");
	//}
}

void CImgSqueeze::copyArrayToImage(const CGLArray2D<BYTE>& array, CxImage& img) {
	assert(img.GetWidth() == array.GetWidth());
	assert(img.GetHeight() == array.GetHeight());
	for (int i = 0; i < (int)img.GetHeight(); i++) {
		for (int j = 0; j < (int)img.GetWidth(); j++) {
			BYTE gray = array.Value(i, j);
			img.SetPixelColor((long)j, (long)i, RGB(gray, gray, gray));
		}
	}
}

void CImgSqueeze::autoLevels(CxImage& img) {
	BYTE nMin = 255;
	BYTE nMax = 0;
	DWORD nWidth = img.GetWidth();
	DWORD nHeight = img.GetHeight();
	// Calculate max and min levels
	for (DWORD nX = 0; nX < nWidth; nX++) {
		for (DWORD nY = 0; nY < nHeight; nY++) {
			BYTE nVal = img.GetPixelGray(nX, nY);
			if (nVal < nMin) {
				nMin = nVal;
			} else if (nVal > nMax) {
				nMax = nVal;
			}
		}
	}
	// Apply transfomation f(x) = A*x + B, A == dMultiplier, B == dSummand
	assert(nMax > nMin);
	double dMultiplier = 255.0 / double(nMax - nMin);
	double dSummand = -dMultiplier * nMin;
	for (DWORD nX = 0; nX < nWidth; nX++) {
		for (DWORD nY = 0; nY < nHeight; nY++) {
			BYTE nVal = img.GetPixelGray(nX, nY);
			double dNewVal = dMultiplier * nVal + dSummand;
			if (dNewVal < 0) {
				dNewVal = 0;
			} else if (dNewVal > 255) {
				dNewVal = 255;
			}
			BYTE nNewVal = (BYTE)dNewVal;
			img.SetPixelColor(nX, nY, RGB(nNewVal, nNewVal, nNewVal));
		}
	}
}

double CImgSqueeze::getHistogramPercentage(CxImage& img, BYTE nFrom, BYTE nTo) {
	assert(nFrom >= 0 && nFrom < 256);
	assert(nTo >= 0 && nTo < 256);
	long pHistogram[256];
	long nCount = 0;
	img.Histogram(0, 0, 0, &(pHistogram[0]));
	for (BYTE i = nFrom; i <= nTo; i++) {
		nCount += pHistogram[i];
	}
	return (double)nCount / double(img.GetWidth() * img.GetHeight()) * 100.0;
}

bool CImgSqueeze::IsGrayScale(CxImage& img)
{
	RGBQUAD* pPal = img.GetPalette();
	if (img.GetDIB() == 0 || pPal == 0 || img.GetNumColors() == 0) {
		return false;
	}
	for (DWORD i = 0; i < img.GetNumColors(); i++) {
		if (pPal[i].rgbBlue != pPal[i].rgbGreen || pPal[i].rgbGreen != pPal[i].rgbRed) {
			return false;
		}
	}
	return true;
}

bool CImgSqueeze::IsImageGray(CxImage& img) {
	long nX;
	long nY;
	long nWidth = (long)img.GetWidth();
	long nHeight = (long)img.GetHeight();
	for (nY = 0; nY < nHeight; nY++) {
		for (nX = 0; nX < nWidth; nX++) {
			RGBQUAD clr = img.GetPixelColor(nX, nY, false);
			if (clr.rgbBlue != clr.rgbGreen || clr.rgbGreen != clr.rgbRed) {
				return false;
			}
		}
	}
	return true;
}