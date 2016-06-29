#ifndef _BRepBuilderAPI_CellFilter_HeaderFile
#define _BRepBuilderAPI_CellFilter_HeaderFile

class BRepBuilderAPI_VertexInspector;
class gp_Pnt;

class BRepBuilderAPI_CellFilter
{
public:
    BRepBuilderAPI_CellFilter(double /*cellSize*/) {}

    void Inspect(
            const gp_Pnt& /*min_pnt*/,
            const gp_Pnt& /*max_pnt*/,
            BRepBuilderAPI_VertexInspector& /*inspector*/)
    {}

    void Add(int /*target*/, const gp_Pnt& /*pnt*/) {}
};

#endif
