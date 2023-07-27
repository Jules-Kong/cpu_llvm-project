#### 1. Getting Started

Download all the repositories firstly and place them in the same path.

* llvm-ventus : git clone https://github.com/THU-DSP-LAB/llvm-project.git  --- you need reset a996cc21  Remove unused #include "llvm/ADT/Optional.h"
* pocl : git clone https://github.com/Jules-Kong/pocl.git
* ocl-icd : git clone https://github.com/OCL-dev/ocl-icd.git   --- you need reset d7c6dfc FIX ABI issues. (#25)

Move the build-ventus.sh file to the llvm project, and execute "chmod +x build-ventus.sh".
 
#### 2. Build all the programs
Our program is based on LLVM, so the need packages to build ventus are almost the same as what are needed to build LLVM, you can refer to official website for detailed llvm building guidance, we just list most important needed packages here.

ccache
cmake
ninja
clang(optional)
If you see any packages missing information, just install them

The following packages are needed for other repositories:

device-tree-compiler
bsdmainutils
Run ./build-ventus.sh to automatically build all the programs, but we need to run firstly

For developers who want to build Debug version for llvm, export BUILD_TYPE=Debug, since it's set default to be 'Release'
export POCL_DIR=<path-to-pocl-dir>, default folder path will be set to be <llvm-ventus-parentFolder>/pocl
export OCL_ICD_DIR=<path-to-ocl-icd-dir>, default folder path will be set to be <llvm-ventus-parentFolder>/ocl-icd
You can dive into build-ventus.sh file to see the detailed information about build process

#### 3. Bridge icd loader
Run export LD_LIBRARY_PATH=<path_to>/ventus-llvm/install/lib to tell OpenCL application to use your own built libOpenCL.so, also to correctly locate LLVM shared libraries

Run export OCL_ICD_VENDORS=<path_to>/libpocl.so to tell ocl icd loader where the icd driver is.

Finally, run export POCL_DEVICES="basic" to tell pocl driver which device is available.

You will see Ventus GPGPU device is found if your setup is correct.

NOTE: OpenCL host side program should be linked with icd loader -lOpenCL.

```
$ <pocl-install-dir>/bin/poclcc -l

LIST OF DEVICES:
0:
  Vendor:   AuthenticAMD
    Name:   basic-AMD Ryzen 7 5800U with Radeon Graphics
 Version:   OpenCL 2.0 PoCL HSTR: basic-x86_64-unknown-linux-gnu-znver3
```

