TARGETS=player ringmaster

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

player: player.cpp
	g++ -g -o $@ $<

ringmaster: ringmaster.cpp
	g++ -g -o $@ $<

clean:
	rm -rf player ringmaster *.o *~