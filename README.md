# Simulateur_Vente_Palettes
Simulateur de vente en ligne de palettes (indivisibles) en langage C.  Les échanges sont réalisés par des pipes directement entre les processus concernés.


L'objectif est de créer une application qui simule la vente en ligne de palettes de 50 parpaings, qui ne peuvent pas être divisés, mesurant chacun 50 cm de longueur, 20 cm de largeur et 20 cm de hauteur. La simulation prend en compte plusieurs éléments tels que le type d'article (creux ou plein), le serveur web utilisé (sw1 ou sw2), l'acheteur (Antoine ou Françoise) et le transporteur (Jule ou Anne). Ces éléments sont considérés comme des processus potentiellement lancés par le programme principal (main), qui utilise des pipes pour gérer les échanges entre les différents processus impliqués.

Avant de lancer l'exécution de la simulation, il est possible de choisir un scénario de vente dans le code lui-même, de manière simple. Chaque scénario implique un seul serveur web, un seul acheteur, un seul transporteur et un seul type d'article. Les paramètres fixés pour chaque scénario incluent le prix d'une palette, la quantité de mètres carrés de stock disponible et la surface en mètres carrés. Il n'y a aucune saisie utilisateur pendant l'exécution de la simulation, car tout est déterminé à l'avance par le programme.
