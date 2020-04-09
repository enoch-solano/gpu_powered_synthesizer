
add_library(kernel
    "kernel.cu"
	"kernel.h"
	
	)
target_compile_options(kernel PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-gencode arch=compute_53,code=sm_53>)
# ${CUDA_TOOLKIT_ROOT_DIR}/lib64
target_link_libraries(kernel ${CUDA_LIBRARIES} cuda cudart)

