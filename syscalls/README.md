# Syscalls

Part of learning systems level stuff with C is learning syscalls. So here I
go.

## Raw thoughts
- reading man pages now to enjoy the process of learning syscalls, man
SECTION - 1 user commands, 2 syscalls, 3 lib fn, 4 special files, 5 file
formats, 7 overview & protocols, 8 sysadmin
- when doing open(..., O_CREAT ...), must pass 0644 as a permissions mask to,
else files will be created with garbage permissions.
- apparently you can expose different portions of the posix API by doing
something like `#define _POSIX_C_SOURCE 200809L`
- `strtol` middle argument accepts `NULL`
- `inet_pton` is the more modern and safe approach
- `perror` is a thing
- `man` pages are seriously so goated
- `AF_UNIX` is for local host, `AF_INET` is for IP protocols
- `nc -u -l` netcat udp listen
- `tcpdump -i any udp port [PORT] -x` view packets on the wire at port for udp
- `ethtool -S eth0` network card counters
- `nstat -az` kernel's UDP counters
- `taskset -cp N <PID>` Pin PID to core
- `ps -o pid,psr,comm -p <PID>` show which core proc on
- `/proc/interrupts`: showed every card's interrupts landing on core 0

