
all: main main.exe copy_to_testing

main: main.c
	gcc -g -o main main.c

main.exe: main.c
	x86_64-w64-mingw32-gcc -g -o main.exe main.c

copy_to_testing: main.exe
	# TODO add your own testing directories or something idk
	#[ -e /guest/ ] && cp main.exe /guest/main.exe
	hostname | grep -q azure-angel && rclone copy --update --verbose main.exe blog-jmcateer-pw:blog.jmcateer.pw/f/

