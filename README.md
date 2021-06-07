# SqueezeImg
Squeeze Image is a tool intended to detect black-and-white images scanned to gray format, and convert such images to black-and-white. It loops through the input folder, a heuristical algorithm detects such images, then conversion to black-and-white is done using an adaptive thresholding algorithm.

The project is created in Visual Studio 2003. For manipulating images, it uses the [CxImage](https://sourceforge.net/projects/cximage/) library, it is required to build to solution.

Performance of the code was not of interest, therefore, it might require researching if anyone wants to use the code in a production system.