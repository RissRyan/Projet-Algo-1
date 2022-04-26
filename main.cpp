#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <bitset>
#include <filesystem >

#include "Tree.cpp"

static std::vector<std::pair<char, int>> alphabet; // caractère + fréquence
static std::vector<std::pair<char, std::string>> codes; // codage de Huffman pour chaque caractère
static std::vector<Tree> trees; // liste des noeuds
static std::vector<Tree*> huffman; // liste permettant de faire l'algorithme

// objets permettant de lire/écrire avec les fichiers
static std::ifstream file; 
static std::ofstream fileW;

// Chemin vers les fichiers
static std::string const textPath1 = "Q:/GitHub/Projet-Algo-1/files/textesimple.txt";
static std::string const textPath2 = "Q:/GitHub/Projet-Algo-1/files/extraitalice.txt";
static std::string const textPath3 = "Q:/GitHub/Projet-Algo-1/files/alice.txt";

static std::string const codePath1 = "Q:/GitHub/Projet-Algo-1/files/textesimple_comp.bin";
static std::string const codePath2 = "Q:/GitHub/Projet-Algo-1/files/extraitalice_comp.bin";
static std::string const codePath3 = "Q:/GitHub/Projet-Algo-1/files/alice_comp.bin";

static std::string const freqPath1 = "Q:/GitHub/Projet-Algo-1/files/textesimple_freq.txt";
static std::string const freqPath2 = "Q:/GitHub/Projet-Algo-1/files/extraitalice_freq.txt";
static std::string const freqPath3 = "Q:/GitHub/Projet-Algo-1/files/alice_freq.txt";


// Fonction de comparaison

bool cmp(std::pair<char, int>& a, std::pair<char, int>& b)
{
    if (a.second == b.second)
    {
        return a.first < b.first;
    }
    else
    {
        return a.second < b.second;
    }
}

// Fonction permettant d'associer chaque caractère à sa fréquence

void fillListAlphabet(std:: string textPath)
{
    std::string lettre;

    file.open(textPath);

    if (file.is_open())
    {
        while (file)
        {
            lettre = file.get();
            bool isFind = false;

            // Si la lettre est déjà present dans notre tableau on incrémente la fréquence

            for (auto& it : alphabet) {

                if (it.first == lettre[0])
                {
                    isFind = true;
                    it.second += 1;
                }
            }

            // Sinon on l'ajoute
            
            if(!isFind)
            {
                alphabet.push_back(std::make_pair(lettre[0], 1));
            }
        }

        // On trie le tableau selon les fréquences

        std::sort(alphabet.begin(), alphabet.end(), cmp);

        std::cout << "Operation successfully performed\n";
        file.close();
    }
    else
    {
        std::cout << "Error opening file";
    }
}

// Fonction générant l'arbre binaire

Tree* generateTree()
{
    if (huffman.size() == 1)
    {
        // Retourne la racine
        return huffman[0];
    }
    else
    {
        // Booléen

        bool inserted = false;

        // Prend les deux arbres de fréqunces minimales
        Tree* t1 = huffman[0];
        Tree* t2 = huffman[1];


        // Crée l'arbre parent et on l'insère

        Tree* node = new Tree("IN", t1->getFrequency() + t2->getFrequency());

        node->setChild(t1, 'l');
        node->setChild(t2, 'r');

        t1->setAncestor(node);
        t2->setAncestor(node);

        trees.push_back(*node);


        for (auto it = huffman.begin(); it != huffman.end(); ++it)
        {
            if (node->getFrequency() <= (*it)->getFrequency())
            {
                huffman.insert(it, node);
                inserted = true;
                break;
            }
        }

        if (!inserted)
        {
            huffman.push_back(node);
        }

        // Supprime les elements minimaux

        huffman.erase(huffman.begin());
        huffman.erase(huffman.begin());

        // Cette fonction est récursive

        generateTree();
    }
}

// Fonction permettant de donner le code pour un caractère donné

std::string codeLetter(char letter)
{
    Tree* tree = nullptr;
    std::string code;

    // On part de la racine correspondant à notre caractère

    for (auto it = trees.begin(); it != trees.end(); ++it)
    {
        if ((*it).getLetter() != "IN" && ((*it).getLetter())[0] == letter)
        {
            tree = &(*it);
            break;
        }
    }

    // On remonte jusqu'à la racine pour générer le code

    while (tree->getAncestor() != nullptr)
    {
        Tree* theAncestor = tree->getAncestor();

        if (theAncestor->getChildren().first == tree)
        {
            code += "0";
        }

        if (theAncestor->getChildren().second == tree)
        {
            code += "1";
        }

        tree = theAncestor;
    }

    // On le met dans le bon ordre

    std::reverse(code.begin(), code.end());

    return code;
}

// Fonction permettant de remplir la liste des caractères et de leurs codes (optimisation)

void codeAllLetters()
{
    for (auto& it : alphabet)
    {
        codes.push_back(std::make_pair(it.first, codeLetter(it.first)));
    }
}

// On génère le texte compressé

void generateCode(Tree* src, std::string textPath, std::string codePath)
{
    fileW.open(codePath, std::ofstream::out | std::ofstream::binary);
    file.open(textPath);

    std::string lettre;
    std::string code;
    std::string waitingBits = ""; // bits en attente, on peut écrire que par paquets de 8 (char)

    if (file.is_open())
    {
        while (file)
        {
            lettre = file.get();

            // On trouve le code correspondant au caracètre dans la liste "codes"

            for (auto it = codes.begin(); it != codes.end(); ++it)
            {
                if ((*it).first == lettre[0])
                {
                    code = (*it).second;
                    break;
                }
            }

            // Selon la taille du code du caractère en traitement et les bits en attente on se place dans différents cas

            if (code.size() + waitingBits.size() > 8)
            {
                waitingBits += code;
                std::bitset<8> myBits(waitingBits.substr(0, 8));
                waitingBits.erase(0, 8);
                unsigned long i = myBits.to_ulong();
                unsigned char c = static_cast<unsigned char>(i);
                fileW << c;

            }
            else if (code.size() + waitingBits.size() < 8)
            {
                waitingBits += code;
            }
            else
            {
                std::bitset<8> myBits(waitingBits + code);
                waitingBits = "";
                unsigned long i = myBits.to_ulong();
                unsigned char c = static_cast<unsigned char>(i);
                fileW << c;
            }

        }

        file.close();
    }
    else
    {
        std::cout << "Error opening file";
    }

    fileW.close();
}

// Fonction pour l'étape 4

void compressionRatio(std::string original, std::string huffmaned)
{
    float res = 1.0f -  (float)std::filesystem::file_size(huffmaned) / (float)std::filesystem::file_size(original);
    std::cout << "Le taux de compression pour " << original << " est de " << res << " %.\n";
}

// Fonction pour l'étape 5

void avgSize()
{
    float res = 0.0f;

    for (auto it = codes.begin(); it != codes.end(); ++it)
    {
        res += (float)(*it).second.size();
    }

    std::cout << "Le nombre de bits moyen pour coder une lettre est : " << res / (float)codes.size() << std::endl;
}

// "Routine" pour les 5 étapes

void codageHuffman(std::string textPath, std::string codePath, std::string freqPath)
{

    fillListAlphabet(textPath); // On associe chaque caractère à sa fréquence

    // On crée les noeuds pour chaque caractère

    for (auto& it : alphabet)
    {
        std::string lettre(1, it.first);
        trees.emplace_back(lettre, it.second);
    }

    for (auto& it : trees)
    {
        //it.afficher();
        huffman.push_back(&it);
    }

    // generateTree() nous retourne la racine de l'arbre

    Tree* root = generateTree();

    codeAllLetters(); // on code chaque caractère

    generateCode(root, textPath, codePath); // on génère le texte compressé

    compressionRatio(textPath, codePath); // Etape 4

    avgSize(); // Etape 5

    // On crée nomtxt_freq.txt et on y met les informations qu'il faut

    fileW.open(freqPath);

    fileW << alphabet.size() << std::endl;

    for (auto& it : alphabet)
    {
        fileW << it.first << " " << it.second << std::endl;
    }

    // On remet tout à zéro

    fileW.close();

    alphabet.clear();

    for (auto& it : trees)
    {
        it.~Tree();
    }

    trees.clear();

    huffman.clear();

    codes.clear();
}

int main()
{
    trees.reserve(1000);

    codageHuffman(textPath1, codePath1, freqPath1);
    codageHuffman(textPath2, codePath2, freqPath2);
    codageHuffman(textPath3, codePath3, freqPath3);

    return 0;
}
