
CC       = gcc
DEFS     = -DNIDAQ_MAJOR=251
CFLAGS   = -Wall

BINARIES = opendaq \
           aisingle aisleep aiblock aicontinuous aiinfinitely aitrigger aimulti aimulticont ainonblock \
           aodirect aosleep aonoblock aotrigger aochannels aostage \
           aiao aimultiao \
           dioparallel dioserial


all: $(BINARIES)


opendaq : opendaq.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+


# read single scans from several channels
aisingle : aisingle.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# non-continous analog input with a single blocking read call.
aisleep : aisleep.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# non-continous analog input with three non-blocking read calls.
ainonblock : ainonblock.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# continuous analog input with several non-blocking read calls.
aicontinuous : aicontinuous.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# continuous analog input with several blocking read calls
aiblock : aiblock.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# continuous analog input with many non-blocking read calls.
# Use this to test very long runs of analog input and the ST_TC counter.
aiinfinitely : aiinfinitely.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# ai with external trigger
aitrigger : aitrigger.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# multi channel ai
aimulti : aimulti.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# multi channel continuois nonblocking ai
aimulticont : aimulticont.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# multi channel continuous ai with ao
aimultiao : aimultiao.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+ -lm


# direct write to analog output.
aodirect : aodirect.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# blocking analog output.
aosleep : aosleep.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+ -lm

# non-blocking analog output.
aonoblock : aonoblock.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+ -lm

# start analog output with external trigger.
aotrigger : aotrigger.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+ -lm

# blocking analog output first on channel 0 then on channel 1.
aochannels : aochannels.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+ -lm

# waveform staging.
aostage : aostage.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+ -lm


# analog input and output
aiao : aiao.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+ -lm


# digital input/output parallel
dioparallel : dioparallel.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+

# digital input/output serial
dioserial : dioserial.c
	$(CC) $(CFLAGS) $(DEFS) -o $@ $+


clean: 
	rm -f $(BINARIES) *~ *.dat