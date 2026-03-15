//---------------------------------------------------------
//                Fast Bitonic Sort Kernel
//---------------------------------------------------------

// #ifndef TYPE
// #define TYPE int
// #endif

// #ifndef LSZ
// #define LSZ 256
// #endif

void global_swap(__global TYPE* left, __global TYPE* right) {
    TYPE tmp = *left;
    *left = *right;
    *right = tmp;
}

void local_swap(__local TYPE* left, __local TYPE* right) {
    TYPE tmp = *left;
    *left = *right;
    *right = tmp;
}

__kernel void local_bitonic_sort(__global TYPE* array, int sz, int merge_size, int sort_size) {
  const int gid = get_global_id(0);
  const int lid = get_local_id(0);
  
  __local TYPE larray[LSZ];

  larray[lid] = array[gid];
  barrier(CLK_LOCAL_MEM_FENCE);

  // true == up; false == down
  const bool dir = ((gid / merge_size) % 2 == 0);

  for (;sort_size >= 2; sort_size >>= 1) {
    const int stride = sort_size / 2;
    // protection against repeated comparisons
    if ((gid % sort_size) < stride) {
      // true if dir == up   and left > right or
      //         dir == down and left < right 
      if (dir == (larray[lid] > larray[lid + stride])) {
        local_swap(&larray[lid], &larray[lid + stride]);
      }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  array[gid] = larray[lid];
  barrier(CLK_LOCAL_MEM_FENCE);  
}

__kernel void global_bitonic_sort(__global TYPE* array, int sz, int merge_size, int sort_size) {
  const int gid = get_global_id(0);

  // true == up; false == down
  bool dir = ((gid / merge_size) % 2 == 0);

  int stride = sort_size / 2;

  // protection against repeated comparisons
  if ((gid % sort_size) < stride) {
    // true if dir == up   and left > right or
    //         dir == down and left < right 
    if (dir == (array[gid] > array[gid + stride])) {
      global_swap(&array[gid], &array[gid + stride]);
    }
  }
}