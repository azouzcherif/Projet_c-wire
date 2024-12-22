#include "avl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------------------*/

AVLNoeud* creerArbre(int id_station, long capacite, long consommation) {
    AVLNoeud* noeud = (AVLNoeud*)malloc(sizeof(AVLNoeud));
    if (!noeud) {
        fprintf(stderr, "Erreur : allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    noeud->id_station = id_station;
    noeud->capacite = capacite;
    noeud->consommation = consommation;
    noeud->fils_gauche = noeud->fils_droit = NULL;
    noeud->equilibre = 1;
    return noeud;
}

/*-----------------------------------------------------------------------*/

int hauteur(AVLNoeud* noeud) {
    return noeud ? noeud->equilibre : 0;
}

/*-----------------------------------------------------------------------*/

int calculerMax(int a, int b) {
    return (a > b) ? a : b;
}

/*-----------------------------------------------------------------------*/

AVLNoeud* rotationDroite(AVLNoeud* y) {
    AVLNoeud* x = y->fils_gauche;
    AVLNoeud* T2 = x->fils_droit;

    x->fils_droit = y;
    y->fils_gauche = T2;

    y->equilibre = calculerMax(hauteur(y->fils_gauche), hauteur(y->fils_droit)) + 1;
    x->equilibre = calculerMax(hauteur(x->fils_gauche), hauteur(x->fils_droit)) + 1;

    return x;
}

/*-----------------------------------------------------------------------*/

AVLNoeud* rotationGauche(AVLNoeud* x) {
    AVLNoeud* y = x->fils_droit;
    AVLNoeud* T2 = y->fils_gauche;

    y->fils_gauche = x;
    x->fils_droit = T2;

    x->equilibre = calculerMax(hauteur(x->fils_gauche), hauteur(x->fils_droit)) + 1;
    y->equilibre = calculerMax(hauteur(y->fils_gauche), hauteur(y->fils_droit)) + 1;

    return y;
}

/*-----------------------------------------------------------------------*/

int calculerEquilibre(AVLNoeud* noeud) {
    return noeud ? hauteur(noeud->fils_gauche) - hauteur(noeud->fils_droit) : 0;
}

/*-----------------------------------------------------------------------*/

AVLNoeud* insertionAVL(AVLNoeud* noeud, int id_station, long capacite, long consommation) {
    if (!noeud) {
        return creerArbre(id_station, capacite, consommation);
    }

    if (id_station < noeud->id_station) {
        noeud->fils_gauche = insertionAVL(noeud->fils_gauche, id_station, capacite, consommation);
    } else if (id_station > noeud->id_station) {
        noeud->fils_droit = insertionAVL(noeud->fils_droit, id_station, capacite, consommation);
    } else {
        noeud->consommation += consommation; // Mise à jour de la consommation si la station existe déjà
        return noeud;
    }

    noeud->equilibre = calculerMax(hauteur(noeud->fils_gauche), hauteur(noeud->fils_droit)) + 1;

    int balance = calculerEquilibre(noeud);

    // Cas 1: Rotation droite
    if (balance > 1 && id_station < noeud->fils_gauche->id_station)
        return rotationDroite(noeud);

    // Cas 2: Rotation gauche
    if (balance < -1 && id_station > noeud->fils_droit->id_station)
        return rotationGauche(noeud);

    // Cas 3: Double rotation gauche-droite
    if (balance > 1 && id_station > noeud->fils_gauche->id_station) {
        noeud->fils_gauche = rotationGauche(noeud->fils_gauche);
        return rotationDroite(noeud);
    }

    // Cas 4: Double rotation droite-gauche
    if (balance < -1 && id_station < noeud->fils_droit->id_station) {
        noeud->fils_droit = rotationDroite(noeud->fils_droit);
        return rotationGauche(noeud);
    }

    return noeud;
}

/*-----------------------------------------------------------------------*/

void enregistrerAVL(AVLNoeud* racine, FILE* file) {
    if (racine != NULL) {
        enregistrerAVL(racine->fils_gauche, file); // Traversée à gauche
        fprintf(file, "%d:%ld:%ld\n", racine->id_station, racine->capacite, racine->consommation); // Écriture dans le fichier
        enregistrerAVL(racine->fils_droit, file);  // Traversée à droite
    }
}

/*-----------------------------------------------------------------------*/

AVLNoeud* construireAVL_fichier(FILE* file) {
    AVLNoeud* racine = NULL;
    int id_station;
    long capacite, consommation;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') continue;

        if (sscanf(line, "%d:%ld:%ld", &id_station, &capacite, &consommation) == 3) {
            racine = insertionAVL(racine, id_station, capacite, consommation);
        } else {
            fprintf(stderr, "Erreur : ligne incorrecte - %s", line);
        }
    }

    return racine;
}

/*-----------------------------------------------------------------------*/

void afficherInfixe(AVLNoeud* racine) {
    if (racine != NULL) {
        afficherInfixe(racine->fils_gauche);
        printf("Station ID = %d, Capacité = %ld, Consommation = %ld\n", racine->id_station, racine->capacite, racine->consommation);
        afficherInfixe(racine->fils_droit);
    }
}

