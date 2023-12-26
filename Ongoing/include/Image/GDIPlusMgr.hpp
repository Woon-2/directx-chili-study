#ifndef __GDIPlusMgr
#define __GDIPlusMgr

#include "App/ChiliWindow.hpp"

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};

#endif  // __GDIPlusMgr