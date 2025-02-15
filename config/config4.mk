ENABLE_OMP = 1   # 0: disable, 1: enable. Use openmp for parallelization.
ENABLE_BVH = 1   # 0: disable, 1: enable. Use BVH for spatial acceleration.
ENABLE_SIMD = 1  # 0: disable, 1: enable. Use SIMD for vectorized operations.
ENABLE_MPOOL = 1 # 0: disable, 1: enable. Use memory pool for memory management.
ENABLE_PRAY = 0  # 0: disable, 1: enable. Use PRAY for ray tracing.

EXTRA_FLAGS = -DVERSION="4.0" -DENABLE_OMP -fopenmp -DENABLE_BVH -DENABLE_SIMD -DENABLE_MPOOL