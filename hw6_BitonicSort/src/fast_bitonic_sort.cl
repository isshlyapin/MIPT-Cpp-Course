//---------------------------------------------------------
//                Fast Bitonic Sort Kernel
//---------------------------------------------------------

#ifndef TYPE
#define TYPE int
#endif

#ifndef LSZ
#define LSZ 256
#endif

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

__kernel void local_bitonic_sort(__global TYPE* array, int sz, int cnt, int k) {
  const int gid = get_global_id(0);
  const int lid = get_local_id(0);
  
  __local TYPE larray[LSZ];

  larray[lid] = array[gid];
  barrier(CLK_LOCAL_MEM_FENCE);

  // true == up; false == down
  bool dir = ((gid / cnt) % 2 == 0);

  for (int i = k; i >= 2; i /= 2) {
    int j = i/2;
    // protection against repeated comparisons
    if ((gid % i) < j) {
      // true if dir == up   and left > right or
      //         dir == down and left < right 
      if (dir == (larray[lid] > larray[lid + j])) {
        local_swap(&larray[lid], &larray[lid + j]);
      }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  array[gid] = larray[lid];
  barrier(CLK_LOCAL_MEM_FENCE);  
}

__kernel void global_bitonic_sort(__global TYPE* array, int sz, int cnt, int k) {
  const int gid = get_global_id(0);

  // true == up; false == down
  bool dir = ((gid / cnt) % 2 == 0);

  int j = k/2;

  // protection against repeated comparisons
  if ((gid % k) < j) {
    // true if dir == up   and left > right or
    //         dir == down and left < right 
    if (dir == (array[gid] > array[gid + j])) {
      global_swap(&array[gid], &array[gid + j]);
    }
  }
}