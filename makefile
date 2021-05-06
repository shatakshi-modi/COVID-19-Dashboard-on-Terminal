
# gcc is the compiler
CC=gcc


# directory where object files will be created
ODIR=obj


# create string with obj/<object filename> in our case obj/hellomake.o and obj/hellofunc.o  (obj folder will be created under current i.e. source directory)
_OBJ = ssltest.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
#The flow in which the targets are going to be called
all: connect ssltest.out main.out run
#create executable file of ssltest.c
ssltest.out: $(ODIR)/ssltest.o
	$(CC) -Wall -o $@ $< -L/usr/lib -lssl -lcrypto -ljson-c
#create executable file of main.c
main.out: $(ODIR)/main.o
	$(CC) -Wall -o $@ $< -L/usr/lib -lssl -lcrypto -ljson-c
#create object file of both
$(ODIR)/%.o: %.c 
	$(CC) -c -Wall -o $@ $< -L/usr/lib -lssl -lcrypto -ljson-c
#the installations that need to be performed in order to run the program
connect:
	sudo apt-get install libssl-dev
	openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
	sudo apt install libjson-c-dev
#used to execute the code 
#execute make COUNTRY=*country name*
#Eg.:make COUNTRY=India
run:
	./main.out $(COUNTRY)
	

	
.PHONY: clean

# removes the .out files that are created
clean: 
	
	rm -f main.out
	rm -f ssltest.out
	








	