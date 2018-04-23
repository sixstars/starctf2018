# stackoverflow

## Idea

The original idea is, even for a trivial stack overflow, if you cannot input some special chars like `\x00`, the exploit would be much harder.


## Solve

Since chars below `\x20` are not allowed, it's quite natural to consider partial overwrite. For the given libc we have one_gadget with printable low-bits but we need to leak the libc_base first. After partial the stored rbp in stack, we can add an offset towards the local variables. And the message `OVERFLOW!` and `no overflow` can be used as side channel to leak some value in stack byte by byte. Partial write of rbp require the knowledge of lowest byte of current stack address but you can just guess and have around 1/16 chance to win.

(Sadly at least one unintended solution existed during competition)
