CC=gcc -g
CFLAGS=-I.
DEPS = oauth2.h curl_request.h
OBJ = main.o curl_request.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

oauth2: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lcurl

.PHONY: clean

clean:
	rm -f *.o *~ core
