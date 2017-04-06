#ifndef _BRepBuilderAPI_CellFilter_HeaderFile
#define _BRepBuilderAPI_CellFilter_HeaderFile

class BRepBuilderAPI_VertexInspector;
class gp_XYZ;

class BRepBuilderAPI_CellFilter
{
public:
    BRepBuilderAPI_CellFilter(double /*cellSize*/) {}

    void Inspect(
            const gp_XYZ& /*min_pnt*/,
            const gp_XYZ& /*max_pnt*/,
            BRepBuilderAPI_VertexInspector& /*inspector*/)
    {}

    void Add(int /*target*/, const gp_XYZ& /*pnt*/) {}
};

#endif
