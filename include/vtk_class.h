#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <bim_distributed_vector.h>
#include <quad_operators_3d.h>
#include <tmesh_3d.h>

// Data containers
typedef std::vector<uint8_t> ByteArray;

class VTKWriter
{
public:

  VTKWriter (std::string basename_ = "default", int rank_ = 0)
    : basename (basename_.empty() ? "default" : basename_),
      rank (rank_), offsetCounter (0)
  {

    /*if (basename_.empty()) {
      std::cerr << "Warning: Basename is empty. Setting basename to 'default'.\n";
    }*/
    snprintf (filename, sizeof (filename), "%s_%4.4d.vtu", basename.c_str(), rank);
  }


  void setBaseName (std::string basename_, int rank_);

  void writeFieldVtuBinary (tmesh_3d& tmsh,
                            const distributed_vector& field,
                            std::string &fieldname_);
  void writeFieldVtuBinary_local (tmesh_3d& tmsh,
                                  const distributed_vector& field,
                                  std::string &fieldname_, double* l_cr, double* r_cr);

  void createPvtuFile (std::vector<std::string> &fieldNames, std::vector<std::string> &baseNames, int numRanks);

private:
  std::string basename;
  std::string fieldname;
  char filename[255];
  std::ofstream file;
  ByteArray dataContainer;
  int nnodes;
  int nelems;
  std::vector<double> p;
  std::vector<double> f_loc;
  std::vector<int> t;
  int offsetCounter;
  int rank;

  void preparingData (const distributed_vector& field, tmesh_3d& tmsh);

  void preparingData_local (const distributed_vector& field, tmesh_3d& tmsh, double* l_cr, double* r_cr);

  void setFieldName (std::string &fieldname_);

  void initializeFile();

  void writePieceHeader();

  void writeGrid();

  void writeDataPoints();

  void writeDataArray (const std::string& type, const std::string& name, int numComponents,
                       const std::vector<double>& data, int numEntries);

  void writeConnectivity();

  void writeOffsets();

  void writeCellTypes();


  void appendData (const ByteArray& rawData);

  void finalizeFile();

  bool fileExists (std::string& name);
};
