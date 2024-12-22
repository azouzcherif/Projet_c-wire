#!/bin/bash

Aide() {
    echo -e "Aide: $0 <fichier_csv> <type_station> <type_consommateur> [id_centrale] [-h]"
    echo -e "\nParamètres:"
    echo "  <fichier_csv>         Nom du fichier CSV d'entrée dans le répertoire 'input' (obligatoire)."
    echo "  <type_station>        Type de station à analyser : hvb, hva, lv (obligatoire)."
    echo "  <type_consommateur>   Type de consommateur : comp, indiv, all (obligatoire pour lv)."
    echo "  [id_centrale]         Identifiant de la centrale à filtrer (optionnel)."
    echo "  -h                    Afficher cette aide et quitter."
}

# --- Vérifier les  paramètres passés sur la ligne de commande---
if [ "$#" -lt 3 ] || [ "$1" == "-h" ]; then
    Aide
    exit 1
fi

# Déclarer les arguments 
INPUT_FILE="$1"
STATION_TYPE="$2"
CONSUMER_TYPE="$3"
CENTRAL_ID="${4:-}"

# Vérifier le fichier CSV d'entrée
if [ ! -f "input/$INPUT_FILE" ]; then
    echo "Erreur : Le fichier d'entrée 'input/$INPUT_FILE' n'existe pas."
    exit 1
fi

# Créer et gérer les répertoires du projet c-wire
for dir in tmp tests graphs; do
    if [ ! -d "$dir" ]; then
        echo "Création du répertoire '$dir'..."
        mkdir -p "$dir"
    elif [ "$dir" == "tmp" ]; then
        echo "Vidage du contenu du répertoire 'tmp'..."
        rm -rf tmp/*
    fi
done

# Définir les restrictions
if { [[ "$STATION_TYPE" == "hvb" || "$STATION_TYPE" == "hva" ]] && [ "$CONSUMER_TYPE" != "comp" ]; } || \
   { [[ "$STATION_TYPE" == "lv" ]] && ! [[ "$CONSUMER_TYPE" =~ ^(comp|indiv|all)$ ]]; }; then
    echo "Erreur : Les combinaisons station '$STATION_TYPE' et consommateur '$CONSUMER_TYPE' sont invalides."
    Aide
    exit 1
fi

# traiter les données 
echo "Traitement des données..."
START_TIME=$(date +%s.%N)

# Nom des fichiers temporaires
TEMP_FILE_CAPACITY="tmp/${STATION_TYPE}_capacity.csv"
TEMP_FILE_CONSUMPTION="tmp/${STATION_TYPE}_${CONSUMER_TYPE}_consumption.csv"
TEMP_INPUT_MAIN="tmp/${STATION_TYPE}_${CONSUMER_TYPE}_main_input.csv"
FINAL_OUTPUT_FILE="tests/${STATION_TYPE}_${CONSUMER_TYPE}${CENTRAL_ID:+_$CENTRAL_ID}.csv"

# Générer le fichier temporaire de consommation
> "$TEMP_FILE_CONSUMPTION"
case "$STATION_TYPE" in
    "lv")
        case "$CONSUMER_TYPE" in
            "comp")
                awk -F';' '$1 != "-" && $4 != "-" && $5 != "-" && $8 != "" {print $4 "," $8}' "input/$INPUT_FILE" > "$TEMP_FILE_CONSUMPTION"
                ;;
            "indiv")
                awk -F';' '$1 != "-" && $4 != "-" && $6 != "-" && $8 != "" {print $4 "," $8}' "input/$INPUT_FILE" > "$TEMP_FILE_CONSUMPTION"
                ;;
            "all")
                awk -F';' '$1 != "-" && $4 != "-" && $5 != "-" && $8 != "" {print $4 "," $8}' "input/$INPUT_FILE" > "tmp/lv_comp_consumption.csv"
                awk -F';' '$1 != "-" && $4 != "-" && $6 != "-" && $8 != "" {print $4 "," $8}' "input/$INPUT_FILE" > "tmp/lv_indiv_consumption.csv"
                cat "tmp/lv_comp_consumption.csv" "tmp/lv_indiv_consumption.csv" > "$TEMP_FILE_CONSUMPTION"
                ;;
        esac
        ;;
    "hvb")
        awk -F';' '$1 != "-" && $2 != "-" && $5 != "-" && $8 != "" {print $2 "," $8}' "input/$INPUT_FILE" > "$TEMP_FILE_CONSUMPTION"
        ;;
    "hva")
        awk -F';' '$1 != "-" && $3 != "-" && $5 != "-" && $8 != "" {print $3 "," $8}' "input/$INPUT_FILE" > "$TEMP_FILE_CONSUMPTION"
        ;;
    *)
        echo "Erreur : Type de station inconnu ou non pris en charge pour la consommation."
        exit 1
        ;;
esac

# Générer le fichier temporaire de capacité
> "$TEMP_FILE_CAPACITY"
case "$STATION_TYPE" in
    "lv")
        awk -F';' '$1 != "-" && $3 != "-" && $4 != "-" && $7 != "-" {print $4 "," $7}' "input/$INPUT_FILE" > "$TEMP_FILE_CAPACITY"
        ;;
    "hvb")
        awk -F';' '$1 != "-" && $2 != "-" && $3 == "-" && $7 != "-" {print $2 "," $7}' "input/$INPUT_FILE" > "$TEMP_FILE_CAPACITY"
        ;;
    "hva")
        awk -F';' '$1 != "-" && $2 != "-" && $3 != "-" && $4 == "-"&& $7 != "-" {print $3 "," $7}' "input/$INPUT_FILE" > "$TEMP_FILE_CAPACITY"
        ;;
    *)
        echo "Erreur : Type de station inconnu pour la capacité."
        exit 1
        ;;
esac

# Tri des fichiers temporaires de capacité et de consommation
sort -t',' -k1,1 "$TEMP_FILE_CAPACITY" -o "$TEMP_FILE_CAPACITY"
sort -t',' -k1,1 "$TEMP_FILE_CONSUMPTION" -o "$TEMP_FILE_CONSUMPTION"

# Fusionner les fichiers temporaires capacité et consommation
> "$TEMP_INPUT_MAIN"
join -t',' -1 1 -2 1 "$TEMP_FILE_CAPACITY" "$TEMP_FILE_CONSUMPTION" | \
    awk -F',' '{print $1 ":" $2 ":" $3}' >> "$TEMP_INPUT_MAIN"

# Supprimer les en-têtes ou lignes incorrectes
sed -i '/LV Station:Capacity:Load/d' "$TEMP_INPUT_MAIN"

if [ ! -s "$TEMP_INPUT_MAIN" ]; then
    echo "Erreur : Le fichier d'entrée pour le programme main est vide."
    exit 1
fi

# Compiler et exécuter le programme C
cd codeC || { echo "Erreur : Impossible d'accéder au répertoire codeC."; exit 1; }
make
if [ $? -ne 0 ]; then
    echo "Erreur : La compilation a échoué."
    exit 1
fi

./main "../$TEMP_INPUT_MAIN" "../$FINAL_OUTPUT_FILE"
if [ $? -ne 0 ]; then
    echo "Erreur : L'exécution du programme main a échoué."
    exit 1
fi

cd - > /dev/null

# Ajouter l'en-tête au fichier CSV final
if [ -f "$FINAL_OUTPUT_FILE" ]; then
    HEADER="Station ${STATION_TYPE^^}:Capacité:Consommation ($CONSUMER_TYPE)"
    sed -i "1s/^/$HEADER\\n/" "$FINAL_OUTPUT_FILE"
else
    echo "Erreur : Le fichier final n'a pas été généré."
    exit 1
fi

# Afficher la durée d'exécution
END_TIME=$(date +%s.%N)
DURATION=$(echo "$END_TIME - $START_TIME" | bc)
echo "Durée d'exécution : ${DURATION}s"

echo "Résultats finaux enregistrés dans : $FINAL_OUTPUT_FILE"

