#!/bin/bash
make
echo ">>>>> Loading md1.ko..."
echo
sleep 0.8
insmod md1.ko
echo ">>>>> Loading md2.ko..."
echo
sleep 0.8
insmod md2.ko
echo ">>>>> Modules 1 and 2 was loaded. Let's look at them: "
lsmod | grep md
echo
sleep 1
echo ">>>>> This is the last 15 lines in the system log: "
dmesg | tail -15
echo
read -n 1 -s -r -p "Press any key to continue..."
echo
echo ">>>>> Remove md2..."
rmmod md2
echo
sleep 0.8
echo ">>>>> Loading md3.ko..."
echo
insmod md3.ko
sleep 0.8
echo ">>>>> This is the last 15 lines in the system log: "
dmesg | tail -15
echo
read -n 1 -s -r -p "Press any key to continue..."
echo
echo ">>>>> Remove md3..."
rmmod md3
echo
sleep 0.8
echo ">>>>> Remove md1..."
rmmod md1
echo
sleep 0.8
dmesg | tail -5
make disclean
echo ">>>>> No md modules left:..."
lsmod | grep md
