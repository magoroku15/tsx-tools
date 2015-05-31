all: rtm-goto rtm rtm-thread

rtm-goto: rtm-goto.c
	cc -I ../include rtm-goto.c -o rtm-goto
rtm: rtm.c
	cc -I ../include rtm.c -o rtm -mrtm
rtm-thread: rtm-thread.c
	cc -I ../include rtm-thread.c -o rtm-thread -mrtm -pthread
