# Makefile 
# author: Tian Xia
# org: Infolab@tamu
# date: Aug 19, 2015

CC=g++

all : twitterClient

libcurl : 
		cd libtwitcurl && make install 
		cd ..
	

twitterClient : libcurl twitterClient.cpp
	$(CC) twitterClient.cpp -I./include/ -L /usr/local/lib/ -ltwitcurl -o twitterClient -std=c++11



clean : 
		rm twitterClient
		cd ../libtwitcurl && sudo make clean
		cd ../twitterClient

