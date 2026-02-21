//---------------------------------------------------------
//                Simple Bitonic Sort Kernel
//---------------------------------------------------------

#ifndef TYPE
#define TYPE int
#endif

void swap(__global TYPE* left, __global TYPE* right) {
    TYPE tmp = *left;
    *left = *right;
    *right = tmp;
}

__kernel void bitonic_sort(__global TYPE* input, uint input_sz) {
  const int gid = get_global_id(0);

  for (int i = 2; i <= input_sz; i *= 2) {
    for (int j = i; j >= 2; j /= 2) {
      int k = j/2;

      // true == up; false == down
      bool dir = ((gid / i) % 2 == 0);

      // protection against repeated comparisons
      if ((gid % i) < (j / 2)) {
        // true if dir == up   and left > right or
        //         dir == down and left < right 
        if (dir == (input[gid] > input[gid + k])) {
          swap(&input[gid], &input[gid + k]);
        }
      }

      barrier(CLK_LOCAL_MEM_FENCE);
    }
  }
}