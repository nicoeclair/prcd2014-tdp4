#####
#
#####

TETRAHORI = ../../spd/tetrahori
NFF2LANCEUR = ../../spd/nff2lanceur

RM = /bin/rm -f

TETRAS = tetrah1.rriff.gz tetrah2.rriff.gz tetrah3.rriff.gz tetrah4.rriff.gz tetrah5.rriff.gz

# niveau de decomposition des pyramides => nbr facettes = 5 * 4^FACTEUR
FACTOR = 8


all : montetra.scn

clean :
	$(RM) $(TETRAS)

montetra.scn : entete.scn $(TETRAS)
	cp entete.scn montetra.scn
	echo '$$tetrah1.rriff' >> montetra.scn
	echo '$$tetrah2.rriff' >> montetra.scn
	echo '$$tetrah3.rriff' >> montetra.scn
	echo '$$tetrah4.rriff' >> montetra.scn
	echo '$$tetrah5.rriff' >> montetra.scn

tetrah1.rriff.gz :
	$(TETRAHORI) $(FACTOR)   4  1  1   12 > tetrah1.nff
	$(NFF2LANCEUR) tetrah1.nff tetrah1.rriff _1
	$(RM) tetrah1.nff
	gzip -9 tetrah1.rriff

tetrah2.rriff.gz :
	$(TETRAHORI) $(FACTOR)   1 12  1   8 > tetrah2.nff
	$(NFF2LANCEUR) tetrah2.nff tetrah2.rriff _2
	$(RM) tetrah2.nff
	gzip -9 tetrah2.rriff

tetrah3.rriff.gz :
	$(TETRAHORI) $(FACTOR)  15 15  1   4 > tetrah3.nff
	$(NFF2LANCEUR) tetrah3.nff tetrah3.rriff _3
	$(RM) tetrah3.nff
	gzip -9 tetrah3.rriff

tetrah4.rriff.gz :
	$(TETRAHORI) $(FACTOR)   1  6 14   6 > tetrah4.nff
	$(NFF2LANCEUR) tetrah4.nff tetrah4.rriff _4
	$(RM) tetrah4.nff
	gzip -9 tetrah4.rriff

tetrah5.rriff.gz :
	$(TETRAHORI) $(FACTOR)  12  4 14   6 > tetrah5.nff
	$(NFF2LANCEUR) tetrah5.nff tetrah5.rriff _5
	$(RM) tetrah5.nff
	gzip -9 tetrah5.rriff
