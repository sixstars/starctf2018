## idea
`scanf("%256s",buffer)` can read no more than 256 bytes, and set the next byte to "\x00", which means it can change 257 bytes!   

in sub_400D92, we send the payload with the length of 256, exactly change the rbp of main.   

	char *sub_400D92()
	{
	  char s; // [rsp+0h] [rbp-100h]
	
	  _isoc99_scanf("%256s", &s);
	  return strdup(&s);
	}

when it return to main, the variables has been disrupted. Using a meticulously constructed payload, we can control the scanf's first argv, and set it to "%s" or whatever you like, we can overflow the stack and then use `ret to csu` to get the shell.
