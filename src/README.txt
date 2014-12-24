###############
# Compilation #
###############
make

#############
# Execution #
#############
make exec np=a FILE=b
np le nombre de processus. Par défaut 4.
FILE le fichier de description de la scène (sans le ".scn" à la fin). Par défaut ../scn/exemple/test
np et FILE optionnels.

#########
# Stats #
#########
./test.sh
affiche les temps passés à calculer par les processus.

