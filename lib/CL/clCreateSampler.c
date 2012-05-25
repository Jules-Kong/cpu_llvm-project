#include "pocl_cl.h"
extern CL_API_ENTRY cl_sampler CL_API_CALL
clCreateSampler(cl_context          context,
                cl_bool             normalized_coords, 
                cl_addressing_mode  addressing_mode, 
                cl_filter_mode      filter_mode,
                cl_int *            errcode_ret)
CL_API_SUFFIX__VERSION_1_0
{
  cl_sampler sampler;

  if (context == NULL)
    POCL_ERROR(CL_INVALID_CONTEXT);
  
  sampler = (cl_sampler) malloc(sizeof(struct _cl_sampler));
  if (sampler == NULL)
    POCL_ERROR(CL_OUT_OF_HOST_MEMORY);
  
  if( normalized_coords == CL_TRUE )
    POCL_ABORT_UNIMPLEMENTED();
  
  if( addressing_mode != CL_ADDRESS_CLAMP_TO_EDGE )
    POCL_ABORT_UNIMPLEMENTED();
  
  if( filter_mode != CL_FILTER_NEAREST ) 
    POCL_ABORT_UNIMPLEMENTED();
  
  sampler->normalized_coords = normalized_coords;
  sampler->addressing_mode = addressing_mode;
  sampler->filter_mode = filter_mode;
  
  return sampler;
}
