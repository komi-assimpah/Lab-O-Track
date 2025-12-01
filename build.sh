#!/bin/bash
# Script de compilation pour Lab-O-Track

echo "🔧 Compilation du projet Lab-O-Track..."
echo ""

# Nettoyage
echo "1️⃣ Nettoyage des anciens fichiers..."
make clean

echo ""
echo "2️⃣ Compilation..."
make

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Compilation réussie !"
    echo ""
    echo "Pour uploader sur l'Arduino :"
    echo "  make upload"
else
    echo ""
    echo "❌ Erreur de compilation"
    echo "Vérifie les messages d'erreur ci-dessus"
fi
