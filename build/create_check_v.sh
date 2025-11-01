#!/bin/bash
readelf -aW bootrom.elf | grep ':.*0fef' | grep -v SECTION | awk '{ print $2,$8 }' | sed -e 's/\([0-9a-f]*\) \(.*\)/always \@(suska_68k30l.PC) if (suska_68k30l.PC == 32h\1) \$display(\"PC now \2\");/' | sed -e "s|32h|32'h|" >| sun3-bootrom_check.v
