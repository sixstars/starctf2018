# ssss+ssss2

## Idea

The original idea of this challenge is from [KRACK](https://www.krackattacks.com/). If you are familiar with WPA2 you can see the script imitates the process of WPA2 handshake although the protocol fields are designed (quite arbitrarily) by myself. 

By the way, the `secret` in script is quite short. But due to the KDF used, I think it's impossible to bruteforce it. 

## Solve

Both two service use AES in CTR mode incorrectly, which leads to reuse of xor key. If you get pairs of long plaintext and ciphertext you can decrypt some of the future messages. 

For `ssss`, just store a long string and read it, then you would have enough xor key to recover the flag.

For `ssss2`, the stored string is truncated and read the stored flag is much harder. But with the idea of KRACK, you can reenter the final step of handshake so that the key used will be misaligned by one. You can make two connections and misalign one of them. The key for received message in one connection will be the key for sent message in another connection, so you can keep sending valid messages. After collecting enough xor keys you can recover the flag correctly.
