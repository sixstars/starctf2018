# babystack 2018

by `tytssn`

## Idea 
- Make a baby stack overflow task in 2018
- Keep it fun

## Vulnerability
- Simple stack overflow

## Mitigations
- NX/RELRO/CANARY are enabled
- Only disable PIE
- But we have thread !
    - <http://www.openwall.com/lists/oss-security/2018/02/27/5> 
    - Thread stack address and thread fs register value is close

## Exploit
- Just like in the 90s
- *Smashing the Stack for Fun and Profit* by Aleph One
- Details in `solve.py`
