This is an open source operating system. It uses docker as it's build environment. I will redo this later but for now it's just build instructions.

Requirements to run:
Docker and QEMU

First run
```docker build buildenv -t mangos-buildenv```
to get your docker build environment set up. Then open your docker like this:

Linux/MacOS:
```docker run --rm -it -v $(pwd):/root/env mangos-buildenv```
Windows(CMD):
```docker run --rm -it -v "%cd%":/root/env mangos-buildenv```
Windows(Powershell):
```docker run --rm -it -v "${pwd}:/root/env" mangos-buildenv```


To actually build it, in your docker instance run
    
```make build-x86_64```

For x86_64(other architectures may come in the future)

Then type `exit` to leave the build environment and run

```qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso```

from the project root folder.

TASKS TO DO
('✔' means done, '.' means in progress)
[✔] - Very basic printf that outputs to serial 
    - (Hint) va_args
    - if the current char is '%' skip it and check the one after it
       - if its 'd' take an integer
       - if its 's' take a string
       - if its 'p' take a pointer
[.] - Make the kernel load its own GDT
[ ] - Implement an IDT
[ ] - Exception handling (i.e. just make an exception handler that prints when errors occur)
[ ] - Find the "MMAP" tag in the `addr` and print out the areas of memory it tells you
[ ] - Make a page allocator (Bitmap, Page list, whichever one you feel like)
    - (Hint) GRUB IS WEIRD:
       - As in, it doesn't mark the kernel as reserved memory. You'll have to do that yourself
    - Using the MMAP go through each entry and if its available add it to your allocator
[ ] - Implement paging using the allocator
    - (Hint) Map available memory
    - (Hint) Map the kernel itself
    - (Hint) Map the framebuffer to some place you choose (or identity map it)
[ ] - Load the new page table
[ ] - REACH GUI MODE