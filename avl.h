#ifndef AVL_H_INCLUDED
#define AVL_H_INCLUDED

#include <stdio.h>

typedef struct AVLNoeud {
    int id_station;            // Identifiant de la station
    long capacite;              // Capacit� de la station
    long consommation;          // Consommation totale d'une station
    struct AVLNoeud* fils_gauche; // Fils gauche
    struct AVLNoeud* fils_droit;  // Fils droit
    int equilibre;              // Facteur d'�quilibre (hauteur sous-arbre droit - gauche)
} AVLNoeud;

// Fonction pour cr�er un nouveau noeud
AVLNoeud* creerArbre(int id_stattion, long capacite, long consommation);

// Fonction pour calculer la hauteur d'un noeud
int equilibre(AVLNoeud* noeud);

// Fonction pour obtenir le maximum de deux nombres
int calculermax(int a, int b);

// Rotation droite
AVLNoeud* rotationdroite(AVLNoeud* y);

// Rotation gauche
AVLNoeud* rotationgauche(AVLNoeud* x);

// Calcul du facteur d'�quilibre d'un noeud
int calculerequilibre(AVLNoeud* noeud);

// Fonction pour ins�rer une station dans un AVL
AVLNoeud* insertionAVL(AVLNoeud* noeud, int id_stattion, long capacite, long consommation);

// Fonction pour sauvegarder l'arbre AVL apr�s mise � jour dans le fichier CSV de sortie final
void enregistrerAVL(AVLNoeud* racine, FILE *file);

// Fonction pour construire l'arbre AVL � partir du fichier CSV d'entr�e
AVLNoeud* construireAVL_fichier(FILE *file);

// Fonction pour afficher l'arbre en ordre croissant (parcours infixe)
void afficherInfixe(AVLNoeud* racine);

#endif // AVL_H_INCLUDED

