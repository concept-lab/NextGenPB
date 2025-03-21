# NextGenPB 
-----------  
Copyright (C) 2021-2025 Vincenzo Di Florio

Copyright (C) 2019-2025 Carlo de Falco

Copyright (C) 2020-2021 Martina Politi

This software is distributed under the terms
the terms of the GNU/GPL licence v3

# Overview
----------

NextGenPB is linearized Poisson-Boltzmann solver on Octree mesh.

It solves the linearized Poisson-Boltzmann equation (PBE):

$$
-\mathrm{div} \left( \varepsilon_0 \varepsilon_r \nabla \varphi \right) + \kappa^2 \varphi = \rho^f
$$

on a rectangular domain.

---

# Table of Contents
-------------------

1. [Installation](#installation)  
   1.1 [Dependencies](#dependencies)  
   1.2 [For the Impatient](#impatient)  
   1.3 [Installation on macOS](#macos)  
   1.4 [Installation on Rocky Linux and Ubuntu](#linux)   
2. [Usage](#usage)  
   2.1 [Post-processing of Output Files (Octave)](#octave)   
3. [Projects](#projects)

---
# Installation
--------------

## Dependencies
The following dependencies are required to build and run the program:  

### Core Libraries

- **`lis`**
- **`p4est`**
- **`bim++`**  
- **`NanoShaper`**  

### Post-Processing Tools

- **Paraview**  
- **GNU Octave**

## For the Impatient
### Very Impatient
Precompiled Apptainer images with OpenMPI versions 4 or 5 are available in the release.
To run the Apptainer image, install Apptainer (or Singularity) and execute the following command:

```bash
mpirun -np <number_of_processors> apptainer exec --bind /path/to/files/:/App \
  /path/to/sif/NextGenPB_ompi4.sif ngpb --potfile /App/options.pot --pqrfile /App/file.pqr
```
### Less impatient
If you have root permissions, you can build the Docker image. This approach allows customization of library versions and compiler flags (e.g., CFLAGS="-O3 -mtune=native -march=native").
To build the Docker image:
```bash
sudo docker build -f Dockerfile -t <name_of_image>:latest .
```
To run the Docker container:
```bash
sudo docker run -v "$(pwd)":/App <name_of_image>:latest ngpb --potfile /App/options.pot --pqrfile /App/file.pqr
```

## Installation on macOS
### Prerequisites

Use MacPorts (or Homebrew) to install the required packages:

```bash
sudo port install openmpi cmake onetbb boost cgal5 nlohmann-json jansson octave
sudo port install mumps +openmpi -mpich
sudo port install lis +openmpi -mpich
sudo port install p4est +openmpi -mpich
```

### Installing Additional Dependencies
**`NanoShaper`**
Clone and build the NanoShaper library:
```bash
git clone https://gitlab.iit.it/SDecherchi/nanoshaper.git
cd nanoshaper
cp CMakeLists_so.txt CMakeLists.txt
cd build_lib
cmake .. -DCGAL_DIR=/opt/local/ -DCMAKE_BUILD_TYPE=Release
make
```

**`octave_file_io`**
Clone and build the octave_file_io library:
```bash
git clone https://github.com/carlodefalco/octave_file_io.git
cd octave_file_io
./autogen.sh
mkdir build
cd build
../configure CXX=mpicxx --prefix=/opt/octave_file_io/1.0.91 --with-octave-home=/opt/local/bin 'LDFLAGS=-Wl,-rpath -Wl,/opt/local/lib/libgcc -Wl,-rpath -Wl,/opt/local/lib/gcc13 -ld_classic'
make
sudo make install
```

**`bim++`**
Clone and build the bim++ library:
```bash
git clone https://github.com/carlodefalco/bimpp.git
cd bimpp
git checkout nextgenPB
./autogen.sh
mkdir build
cd build
../configure --prefix=/opt/bimpp --disable-option-checking \
LDFLAGS="-L/opt/local/lib -Wl,-rpath,/opt/local/lib/libgcc -Wl,-rpath,/opt/local/lib/gcc14" \
CPPFLAGS="-I/opt/local/include/ -I/opt/local/include/gcc14 -DOMPI_SKIP_MPICXX -DHAVE_OCTAVE_44 -DBIM_TIMING" \
 --with-blas-lapack="-lopenblas"  \
 --with-octave_file_io-home=/opt/octave_file_io/1.0.91 \
 --with-octave-home=/opt/local/bin \
 --with-p4est-home=/opt/local \
 --with-lis-home=/opt/local \
 --with-mumps-home=/opt/local \
 F77=mpif90 CXX=mpicxx MPICC=mpicc CC=mpicc \
CXXFLAGS="-std=c++17 -O3 -mtune=native -march=native" \
--with-mumps-extra-libs="-L/opt/local/lib -lptscotch -lscotch -lmpi -Wl,-flat_namespace -Wl,-commons,use_dylibs \
-L/opt/local/lib/openmpi-mp -lmpi_usempif08 -lmpi_usempi_ignore_tkr -lmpi_mpifh -lopenblas -L/opt/local/lib/gcc14 -lgfortran"

make
sudo make install
```

### Compiling of ngpb
To compile NextGenPB, create a `local_settings.mk` file to add or modify the compilation options specified in the `Makefile`.
Prototypes of `local_settings.mk` for different distributions are available in the `local_setting `directory. Copy the appropriate file to the `src` directory:
```bash
cp local_setting/local_settings_ubuntu.mk src/local_settings.mk
```
Then, run:  

```bash
make clean all
```
This process generates an executable named `ngpb` in the `src` directory. Optionally, add the executable path to your `.zshrc` file:
```bash
export PATH=/path/to/exec:$PATH
```

## Installation on Rocky Linux and Ubuntu

Follow the instructions in the Dockerfile (for Rocky Linux) and Dockerfile_ubuntu (for Ubuntu). If you are running NGPB on the same machine where the libraries are installed, ensure that you compile everything with the following flags:
```bash
export CXXFLAGS="-O3 -mtune=native -march=native"
export CFLAGS="-O3 -mtune=native -march=native"
export FCFLAGS="-O3 -mtune=native -march=native"
```


# Usage
-------

To run the code, you need an options file (e.g. `data/options.pot`)  and a PQR file (e.g. `data/1CCM.pqr`).
To run the code serially, use the following command:

```bash
/path/to/exec/ngpb --potfile options.pot --pqrfile pqrfile.pqr
```
 
To run in parallel with MPI:

```bash
mpirun -np <number of processes> /path/to/exec/ngpb --potfile options.pot --pqrfile pqrfile.pqr
```

Where `<number of processes>` is the desired number of processors.


The .pqr files must not contain the optional `CHAIN_ID` feature, as the code cannot read that column. For example, .pqr files created with `PDB2PQR` are guaranteed to conform to the correct format:

```
pdb2pqr --ff=charmm --whitespace 4ake.pdb 4ake.pqr
```

without the option `--keep-chain`. If the .pqr file is generated using an online tool, ensure the `add/keep chain IDs` in the PQR file option is unchecked.

## Post-processing of Output Files (Octave)

If you are using the option `map_type = oct`, you can use the `GNU Octave` script called `export.m`, distributed with bimpp (located in the `script/m` folder), for post-processing. When using the default names for the files, the following command will create a .vtu file that can be opened using Paraview:

```
export_tmesh_data ("potential_map_%d_%4.4d", {"potential_map_%d_%4.4d", "eps_map_%d_%4.4d"}, { "phi", "eps"}, { }, {}, 'output_name', 0, processors_number)
```

Here, `output_name` can be replaced with any desired name for the .vtu file, and processors_number should be replaced by the number of processors used for the simulation.

The user can also choose to convert only selected outputs based on their needs.


# Projects
----------

Below is an (unordered) list of improvements/fixes/new features we expect to implement in the near future:

	1.	Nonlinear ionic density

	2.	Allow more flexible formats for *.pqr files (particularly with or without the ChainID column) or the use of *.pdb files
