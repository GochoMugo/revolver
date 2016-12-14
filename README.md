# revolver

> 1. **Push** (add) items to the end of an **items file**.<br>
> 2. **Shift** (remove) items from the beginning of an **items file**.<br>
> 3. **Revolve** i.e. Continuously shift items, passing each to a custom command.<br>


## Sample scenario:

##### push:

Push an item, such as URL, to the end of a file, named `urls.txt`:

```bash
$ revolver --file=urls.txt --push http://forfuture.co.ke
```

1. This is basically the same as `echo http://forfuture.co.ke >> urls.txt`

Push multiple URLs:

```bash
$ revolver --file=urls.txt --push https://github.com https://duckduckgo.com https://cmake.org
```

##### shift:

Shift (remove) a URL from the beginning of `urls.txt`:

```bash
$ revolver --file=urls.txt --shift
http://forfuture.co.ke
```

Shift 2 URLs from `urls.txt`:

```bash
$ revolver --file=urls.txt --shift=2
https://github.com
https://duckduckgo.com
```

##### revolve:

Process the URLs from `urls.txt`, passing each URL to a custom command,
e.g. `echo`:

```bash
$ revolver --file=urls.txt --revolve="echo ="
revolver: running command `echo = https://cmake.org'
= https://cmake.org
```


1. `revolver` **waits** for the command to complete processing the URL before
   moving to the next one.
1. While the command is running, you can make `revolver` **skip** the current
   item, by sending one SIGINT signal i.e. Ctrl+C. The item is pushed
   back to the items file.
1. To **stop** the processing, send the SIGINT signal *twice*,
   in quick succession.


## building:

You will require [CMake][cmake]:

```bash
$ mkdir build && cd build
$ cmake ..
$ make install
```


## license:

**The MIT License (MIT)**

Copyright (c) 2016 GochoMugo (www.gmugo.in)


[cmake]:https://cmake.org
