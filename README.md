# mscTools

mscTools is a quick hack at a tool for torturing mass storage. It uses random block access and random block sizes to copy data between one location (file or block device) and another location.
* This is not a production tool, use at your own risk
* There is no built in help. Read main.c to figure it out until I get around to cleaning up argument parsing.

### Build
This project currently only works in Linux environments, and requires GLib headers / libs
```sh
$ git clone [git-repo-url] mscTools
$ cd mscTools
$ make
```
Output binary can be found in the build directory. Superuser permissions may be required for some operations

### Modes
Several modes are planned for the tool. Below is a description of what is currently implemented
* Random Block Copy (rbc) - rbc will copy data from one location (file or block device) to another location, in the worst possible way. It will perform all reads/writes in a random order, with a random size, with fsync's between each operation. This will copy up to the input files length to the output file.

### Example
Below is an example of generating, writing, and reading back a 32MB file to a jump drive (/dev/sdg). This uses the random block copy (rbc) mode.
```sh
# Create random block
$ dd if=/dev/urandom of=/tmp/random_block ibs=512 count=65536
# Copy data from random_block to /dev/sdg using rbc (random block copy)
$ ./mscTools rbc /tmp/random_block /dev/sdg
# (Disconnect / reconnect jump drive to prevent OS caching)
# Copy data back out
$ ./mscTools rbc /dev/sdg /tmp/read_back
# Verify data
$ md5sum /tmp/random_block
$ md5sum /tmp/read_back
```


### Development
All source is written in C.

License
----

Simplified BSD
