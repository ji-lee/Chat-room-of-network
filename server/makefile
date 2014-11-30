FINAL:=server
OBJECT:=server.o link.o
OPTIONS:= -Wall -rdynamic -g -lpthread

$(FINAL):$(OBJECT)
	gcc $^ -o $@ $(OPTIONS)

%.o:%.c
	gcc -c $< -o $@ $(OPTIONS)
		
clean:
	rm -rvf $(OBJECT) $(FINAL)
