.SUFFIXES:
.SUFFIXES: .c .o
CLNT = client
SRVR = server
CFLAGS = -g -Wall

SRVR_OBJ = server.o nfs_xdr.o nfs_svc.o
CLNT_OBJ = client.o nfs_xdr.o nfs_clnt.o

.c.o:; gcc -c -o $@ $(CFLAGS) $<

default: $(CLNT) $(SRVR)

$(CLNT): $(CLNT_OBJ)
	gcc -o $(CLNT) $(CLNT_OBJ)

$(SRVR): $(SRVR_OBJ)
	gcc -o $(SRVR) $(SRVR_OBJ)

clean:
	rm -f *.o $(CLNT) $(SRVR)

