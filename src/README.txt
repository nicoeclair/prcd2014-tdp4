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

#####################
# Fichier de config #
#####################
Pour correctement écrire un fichier de configuration ("config"), suivre cet en tête, ligne par ligne :

fake
repartition
vol_de_travail
nb_task 


avec
fake==1   <=>   tâches factices
fake==0   <=>   calcul de l'image
repartition==1   <=>   restes chinois
repartition==0   <=>   répartition linéaire
vol_de_travail==1   <=>   on fait du vol de travail
nb_task  <=> le nombre de tâche total

Puis, ligne par ligne, écrire le nombre de tâches (factices donc) que vous voulez pour une longueur spécifique, puis la longueur en microsecondes.

Exemple de fichier :
1
0
1
16
6 500
10 100

Ce fichier signifie que l'on calculera des tâches factices, avec une répartition linéaire, en faisant du vol de travail. On aura 16 tâches au total. 6 tâches dureront 500µs et 10 dureront 100µs.

