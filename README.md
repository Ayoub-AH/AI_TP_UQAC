# AI_TP_UQAC

WESTWORLD WITH MESSAGING

---------------------------------------------
# Membres de l'équipe
---------------------------------------------
PELM12599502		Marion PELLICER
GUIA10519600		Agathe GUILLEMOT
ALHA09119400		Ayoub AL HADDAN


---------------------------------------------
# Travaux effectués
---------------------------------------------
  --> Ajouts des fonctionnalités demandées à savoir : Bob et un saoulard peuvent entrer en interaction et engager un combat si nécessaire. Les diagrammes d'états correspondants à ces nouveaux comportement sont fournis dans le dossier "docs".
  --> Transformation du code en programme concurrent, chaque agent évolue sur son propre thread et un verrou est présent pour gérer le système de messages. Autrement dit, deux agents ne peuvent pas envoyer de messages en même temps, de même toute lecture n'est pas admise si une autre est déjà en cours ou si un message est en cours d'envoi.
  --> Une interface graphique représentant l'évolution des personnages dans leur univers à été implémenté. On y retrouve les principaux lieux de notre simulation.
  --> Nous avons compris la consigne "spécifier l'état de départ d'un agent" dans le sens "afficher, donner à voir l'état de départ d'un agent" (d'où les pictogrammes marquant les changements d'états) et non dans le sens "choisir l'état de départ". Cette fonction n'a donc pas été implémentée ; pour modifier l'état initial d'un agent, il faut modifier directement le code.


---------------------------------------------
# Problèmes non résolus
---------------------------------------------
  --> 