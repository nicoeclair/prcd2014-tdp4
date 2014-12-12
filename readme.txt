##################################################
# LANCER DE RAYON                                #
#                                                #
# Module MP2, 3me annee ENSERB, 1995-1996        #
##################################################

Contenu de l'arborescence
-------------------------

./readme.txt : le fichier present

./src : sources su programme lanceur

./include/ : repertoire contenant differents fichiers d'en-tete (.h)
             d'interet general

./bin/ : executables (obtenus apres compilation et installation,
         cf. ci-dessous)

./scn/ : Repertoire contenant des exemples de fichier de description
         de scenes

./scn/exemple/ : une scene simple : piece avec spheres et miroirs
./scn/pyramides/ : scenes avec des pyramides recursives

./spd/ : utilitaires de fabrication de pyramides recursives
         traducteur (partiel) du format nff vers le format de lanceur



Compilation et installation
---------------------------
Le Makefile se trouve dans le repertoire ./src/

Pour compiler, faire la commande :
% make all

Pour installer les executables dans le repertoire ./bin/, faire la commande :
% make install
