@echo off
cd bin
dfu-util.exe -d 0x0483:0xDF11 -a 0 -s 0x08000000:leave -D ../firmware.bin
pause