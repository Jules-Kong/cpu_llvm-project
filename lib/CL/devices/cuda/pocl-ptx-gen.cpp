/* pocl-ptx-gen.c - PTX code generation functions

   Copyright (c) 2016 James Price / University of Bristol

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include "config.h"

#include "pocl-ptx-gen.h"

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

int pocl_ptx_gen(char *bc_filename, char *ptx_filename)
{
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer =
    llvm::MemoryBuffer::getFile(bc_filename);
  if (!buffer)
    return 1;

  // Load bitcode
  llvm::ErrorOr<std::unique_ptr<llvm::Module>> module =
    parseBitcodeFile(buffer->get()->getMemBufferRef(),
    llvm::getGlobalContext());
  if (!module)
    return 1;

  llvm::LLVMContext& context = llvm::getGlobalContext();

  // Add nvvm.annotations metadata to mark kernel entry points
  llvm::NamedMDNode *md_kernels = (*module)->getNamedMetadata("opencl.kernels");
  if (md_kernels)
  {
    llvm::NamedMDNode *nvvm_annotations =
      (*module)->getOrInsertNamedMetadata("nvvm.annotations");
    for (auto k = md_kernels->op_begin(); k != md_kernels->op_end(); k++)
    {
      llvm::ConstantAsMetadata *cam =
        llvm::dyn_cast<llvm::ConstantAsMetadata>((*k)->getOperand(0).get());
      if (!cam)
        continue;

      llvm::Function *function =
        llvm::dyn_cast<llvm::Function>(cam->getValue());

      llvm::Constant *one =
        llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(context), 1);
      llvm::Metadata *md_f = llvm::ValueAsMetadata::get(function);
      llvm::Metadata *md_n = llvm::MDString::get(context, "kernel");
      llvm::Metadata *md_1 = llvm::ConstantAsMetadata::get(one);

      llvm::ArrayRef<llvm::Metadata*> md({md_f, md_n, md_1});
      nvvm_annotations->addOperand(llvm::MDNode::get(context, md));
    }
  }

  //(*module)->dump();

  // TODO: support 32-bit?
  llvm::StringRef triple = "nvptx64-nvidia-cuda";

  // Get NVPTX target
  std::string error;
  const llvm::Target *target = llvm::TargetRegistry::lookupTarget(
    triple, error);
  if (!target)
    return 1;

  // TODO: set options
  llvm::TargetOptions options;

  // TODO: CPU and features?
  std::unique_ptr<llvm::TargetMachine> machine(
      target->createTargetMachine(triple, "", "", options));

  llvm::legacy::PassManager passes;

  // Add pass to emit PTX
  std::error_code ec;
  llvm::raw_fd_ostream *ptx =
    new llvm::raw_fd_ostream(ptx_filename, ec, llvm::sys::fs::F_Text);
  if (machine->addPassesToEmitFile(passes, *ptx,
                                   llvm::TargetMachine::CGFT_AssemblyFile))
    return 1;

  // Run passes
  passes.run(**module);

  return 0;
}
