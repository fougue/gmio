extern "C" {
#include "../commons/bench_tools.h"
}

#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlAPI_Reader.hxx>
#include <TopoDS_Shape.hxx>

static void occ_RWStl_ReadBinary(const char* filepath)
{
  /*Handle_StlMesh_Mesh stlMesh = */RWStl::ReadBinary(OSD_Path(filepath));
}

static void occ_StlAPI_Reader(const char* filepath)
{
  StlAPI_Reader reader;
  TopoDS_Shape shape;
  reader.Read(shape, filepath);
}

int main(int argc, char** argv)
{
  if (argc > 1)
    benchmark(&occ_RWStl_ReadBinary, "RWStl::ReadBinary()", argc - 1, argv + 1);

//  startTick = std::clock();
//  std::cout << "Read with StlAPI_Reader::Read() ..." << std::endl;
//  for (int iarg = 1; iarg < argc; ++iarg) {
//    StlAPI_Reader reader;
//    TopoDS_Shape shape;
//    reader.Read(shape, argv[iarg]);
//  }
//  std::cout << "  StlAPI_Reader::Read() read time: " << elapsed_secs(startTick) << "s" << std::endl;

  return 0;
}
