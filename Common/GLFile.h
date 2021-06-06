#pragma once

namespace GLC {

class CGLFile {
public:
	CGLFile(void);
	virtual ~CGLFile(void);
	
	static CGLArray<CGLString> GetFiles(const CGLString folder);
	static void AppendBackSlash(CGLString& folder);
	static void GetFileName(const CGLString& path, CGLString& name, CGLString& extension);
	static void Copy(const CGLString& from, const CGLString& to, bool Overwrite = true);
	static bool Exists(const CGLString& path);
	static void Delete(const CGLString& path);
};

} // namespace GLC