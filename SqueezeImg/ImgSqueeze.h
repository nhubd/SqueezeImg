#pragma once

#include "ximage.h"

using GLC::CGLArray2D;

class CImgSqueeze {
public:
	CImgSqueeze(void);
	virtual ~CImgSqueeze(void);
	
	void SqueezeImage(const GLC::CGLString SourceFile, const GLC::CGLString TargetDir);
	bool IsGrayScale(CxImage& img);
	bool IsImageGray(CxImage& img);

protected:

private:
	enum EConvertTo {
		CT_TIFF1,
		CT_JPEG8,
		CT_JPEG24
	};

	void loadImage(CxImage& img, const GLC::CGLString& file);
	EConvertTo convertTo(CxImage& img);
	bool canBinarize(CxImage& img, bool& bIsBW, int& nThreshold);
	GLC::CGLString getOutputFilename(const GLC::CGLString& wsInputFilename,
			const GLC::CGLString& wsOutputFolder, const EConvertTo format);
	void binarize(CxImage& img, BYTE nThreshold);
	void binarize(CGLArray2D<BYTE>& img, BYTE nThreshold);
	void copyImageToArray(const CxImage& img, CGLArray2D<BYTE>& array);
	void copyArrayToImage(const CGLArray2D<BYTE>& array, CxImage& img);
	void autoLevels(CxImage& img);
	double getHistogramPercentage(CxImage& img, BYTE nFrom, BYTE nTo);
};