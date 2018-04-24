## idea
the heap challenge is boring, so I write a new method for malloc and free. =，=  
you can find the `chunk` and `bins_ptr`(a simple version of `main_arena`) struct at [https://github.com/sixstars/starctf2018/tree/master/pwn-young_heap](https://github.com/sixstars/starctf2018/tree/master/pwn-young_heap)

    chunk struct
    |--------------------|
    | Last Chunk Size    |  last bit 0:free, 1:use
    |--------------------|  last two bit: 1:mmaped chunk
    | Chunk Size         |
    |--------------------|
    | Content or Next ptr|
    |                    |
    |--------------------|

there is no bug in libheap.so (for now), but in program, when you edit chunk, you can overwrite the next chunk's `last chunk size` after you feed the full of the chunk you using. Finally you can use `off by one` bug to change the top chunk size.  

At initiation, program call mmap to allocate two memory chunks, one for top chunk, one for `bins_ptr`. The distance between two memory chunks is a const value (but different from different machine). Therefore, you can change the top chunk size to a big value to overwrite `bins_ptr`, then you can malloc a fake chunk at got table, control the execution flow.  
