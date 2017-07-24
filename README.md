# lcs - lowest common subnet

Find the smallest subnet that contains 2 given IPv4 addresses.

## options

```
Usage: lcs [OPTION]... <address1> <address2>
Find the lowest common subnet of two ip addresses.

  -c    print subnet in cidr format   (ex: 10.0.0.0/24)   (default)
  -m    print subnet with long mask   (ex: 10.0.0.0/255.255.255.0)
  -C    print just the cidr number    (ex: 24)
  -M    print just the subnet mask    (ex: 255.255.255.0)
  -v    print everything above
  -h    display this help and exit
  -V    output version information and exit
```

## examples

```
$ lcs 10.0.0.1 10.0.4.1
10.0.0.0/21

$ lcs -m 192.168.0.1 192.168.0.255
192.168.0.0/255.255.255.0

$ lcs -v 12.5.0.0 12.17.0.0
host 1: 12.5.0.0
host 2: 12.17.0.0
subnet: 12.0.0.0/11
mask:   255.224.0.0
```
