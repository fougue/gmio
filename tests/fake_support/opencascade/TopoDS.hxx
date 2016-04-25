#ifndef _TopoDS_HeaderFile
#define _TopoDS_HeaderFile

#include <TopoDS_Face.hxx>

class TopoDS 
{
public:
    static const TopoDS_Face& Face(const TopoDS_Shape&)
    {
        static const TopoDS_Face face;
        return face;
    }
};

#endif // _TopoDS_HeaderFile
