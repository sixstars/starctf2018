## Explain

The code is really a mess, I don't have time to rearrange it.
And I'm not familiar to Android development as most of time, I only do reversing work on Android.
If you'd like to read it, keep calm.

I'm sorry that the app is not well tested on different phone which cause most of you guys failed to run it.
My test phone is really old, `Honor 4 Play G621-TL00` with Android `4.4.4` and kernel `3.10.28-gc883740`.

## Motivation

I'd like to reproduce some protections I'v ever met.
But it's really a hard work to realize it.
I need to protect the app from both static and dynamic analysis to keep it more fair for all the competitors.
So I made some tradeoff.

## Key point

1. I hide the library load operation for `b.so` in `JNI_Onload` of `a.so`.
Though you can guess that `b.so` must have been loaded in this challenge.

1. `a.so` ptrace itself when loaded to prevent debugger. Thanks AAA for remindering me that ptrace self in main thread / parent process won't work when SELinux turned on.

1. `a.so` bind `stringFromJNI` to supply checksum for `a.so`, which will be called by `b.so`.

1. `b.so` checked app name in `.init` function to prevent reuse the `so` in another app.

1. `b.so` decrypt the `JNI_Onload` and key function in `.init`.

1. `b.so` decrypt the key function again in `JNI_Onload`.

1. `b.so` assert the checksum for `a.so` and `b.so` is the same to prevent binary patch.

1. `b.so` bind the key `check` method, and the strings for binding are encrypted to prevent string search.

1. `b.so` checked the trace id to prevent debugging.

1. `b.so` called `stringFromJNI` to get the final key from `a.so`.

1. Most of keys are related to the checksum to make the patched binary unable to decrypt.

1. There is an snag in the checksum function to assert interval between some operations is less than 2s which prevent you guys to modify some value with debugger manually.
   If interval is too long, the checksum would be wrong and the decryption would fail.
   And I guess, on most Android ARM emulator, the interval would be too long.

1. Indeed, the checksum is crc32 but removed a `reverse_bits` operation before function return.
   I don't remember where I download the script for crc32 calculation and modification.
   I'm sorry that half of you guys realized the checksum function because of the failure on running app.
   And I'd like to pay great respect to the guys who inferred the checksum by the first four bytes of encrypted `JNI_Onload`. It's really amazing!
   By the way, maybe you can bypass the crash point with debugger to get the checksum dynamically.
   Or even, maybe you can write an binary to call the function directly.

I only remember the above key points...

PS: There is something interesting if you dig into the code. You can consider why I use `Runtime.getRuntime().load(LIB_B, Thread.currentThread().getContextClassLoader())` instead of calling `System.load(LIB_B)` directly in `JNI_Onload` of `liba.so`.
