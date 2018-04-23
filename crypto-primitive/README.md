# primitive

## Solve

The ADD ROTATE XOR are three basic operations for many modern block ciphers. And it's possible to build any sbox using ADD and ROTATE only. For example, operations `(0,2),(1,7),(0,255),(1,1)` gives a construction for transposition of adjacent elements `254` and `255`. However, for arbitrary pair `a,b` the direct construction with ADD ROTATE can take too much steps while we have a limit of 2000. But with XOR operation, we can change any pair `a,b` to `0,1` with steps in proportion to element's bit number. Since we can swap any pair now, it would be trivial to build a proper cipher. 

More details of my construction can be found in `solve.py`
