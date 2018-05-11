## Preface

Really terrible for this challenge, **all guys submitted writeup** used the unintended solution.

## Main

The challenge is inspired by `nonamestill` from `Code Blue CTF 2017`.
Thanks [binja](https://github.com/binja) for sharing the [source code](http://binja.github.io/2017/11/13/Thoughts-on-CODE-BLUE-CTF-write-ups/#nonamestill).

In fact, there should be only one urlparse challenge in design.
However, when I write the exploit for urlparse, I think an info leak is needed.
So I comment the check for small size to allow `fgets` read nothing, thus avoid terminate null bytes.
However, 0 size not only cause info leak, but can also lead to heap mess.
That's why I can forgive myself because I use the same unintended solution to pwn `nonamestill`.
And then, I decided to think about a revenge, urlparse2 is born.

Unfortunately, most of you guys used another unintended solution, which haven't been fixed in urlparse2. Regretful for not checking the captured payload carefully.

## Unintended

When created url, the program `fgets` the input to a global buffer, decode it, and copy to heap.
Normally, assume input size `n`, the string length is at most `n - 1`.
However, if the input is ended with `%`, it will eat the terminate null byte and use the residual chars in global buf which would cause the decoded string longer than original string.
So, `n` bytes is copied to heap without terminate null byte, thus, info leak and control flow hijacking.

Another key point is how to arrange heap layout when `\0` is not allowed with strncpy.
In urlparse1, we can easily decode `%00` to generate `\0`.
However, in urlparse2, all chars after `\0` is cleared after decoding.

0ops told me a great bypass for `\0` in urlparse2, thanks.
After leaking libc address, they first point the linked list to libc to leak program base address.
Then they point the linked list to the global buffer, construct the fake chunk in global buffer.
Input string starts with `\0`, the decode will not change the input.
So arbitrary chars is allowed now.

## Intended

After encoding url with specific size, we can leave a `%` at the end of the url.
It gives us a chance to modify the size of next chunk while decoding url.
However, most of time, you can only change the size of next chunk to `0`, which is invalid.
Only when the next chunk size is `0x100xx`, you can change the size to `0x10000`.
So, in fact, it's supposed to be an `off-by-one-null-byte` vulnerability.

For bypassing `\0`, as we can leave a null byte at the end of the input url, we can free the previous input and input again with smaller size to achieve the goal.

For more detail, view the code.
