#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "avl.h"

int main(int argc, char* argv[]) 
{
     // Configurer la locale pour gérer les caractères accentués
     setlocale(LC_ALL, "");  
    // Vérifier les arguments passés sur la ligne de commande
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file.csv> <output_file.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }

     
    // Fichiers CSV d'entrée et de sortie
    const char* input_file = argv[1];
    const char* output_file = argv[2];

    // Ouvrir le fichier d'entrée
    FILE* pfile = fopen(input_file, "r");
    if (!pfile) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return EXIT_FAILURE;
    }

    printf("Ouverture du fichier d'entrée avec succès : %s\n", input_file);

    // Construction de l'arbre AVL à partir du fichier CSV d'entrée
    AVLNoeud* racine = construireAVL_fichier(pfile);
    fclose(pfile);  // Fermer le fichier d'entrée après utilisation

    if (!racine) {
        fprintf(stderr, "Erreur : l'arbre AVL est vide ou la construction a échoué.\n");
        return EXIT_FAILURE;
    }

    // Ouvrir le fichier de sortie
    FILE* output = fopen(output_file, "w");
    if (!output) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        return EXIT_FAILURE;
    }

    // Sauvegarde de l'arbre dans le fichier de sortie
    enregistrerAVL(racine, output);
    fclose(output);  // Fermer le fichier de sortie
    printf("Arbre AVL sauvegardé avec succès dans : %s\n", output_file);

    return EXIT_SUCCESS;
}

