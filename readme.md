
# Polymorphic C

Generally when people say "polymorphic code" they
mean code mapped as w+x in memory where one half of
the program is actively _writing_ the second half,
which may sometimes go full circle and be the first half.

This experiment is different; I want to make a program
which modifies itself such that it remembers user data
on subsequent runs without depending on external resources
(files, registries, network calls).

# Strategy

The current setup is pretty simple:

 - print some data[1]
 - increment data[1]
 - read the executing binary into memory
 - read the original file path
 - read the original path permissions
 - find the offset of data[1] in the in-memory copy
 - modify the in-memory copy to update data[1]
 - delete (unlink) the original path
 - write the in-memory copy to the original path
 - assign the original path permissions to the new file
 - free memory (optional)

# Testing

```bash
make

./main # prints data = 5060708 0 5060708
./main # prints data = 5060708 1 5060708
./main # prints data = 5060708 2 5060708
./main # prints data = 5060708 3 5060708

# etc.. incrementing data[1] each run

```

