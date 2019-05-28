gcc -o amvc *.c;
echo -e "\033[90;3mgcc -o amvc *.c\033[0m";
cp amvc ../tests;
echo -e "\033[90;3mcp amvc ../tests\033[0m";
cp amvc ../exe;
echo -e "\033[90;3mcp amvc ../exe\033[0m";
cp ../amvTools/amv.h ../amvSelf;
echo -e "\033[90;3mcp ../amvTools/amv.h ../amvSelf\033[0m";
cp ../amvTools/amv.c ../amvSelf;
echo -e "\033[90;3mcp ../amvTools/amv.c ../amvSelf\033[0m";
echo -e "\033[95;3mupdate done\033[0m";
echo -e "\033[95mdon't forget to change content of 'dir' in main.c\033[0m"