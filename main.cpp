#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>

static std::vector<std::pair<char, int>> alphabet;
static std::ifstream file;

static std::string const textPath = "C:/Dev/Mini projet 1/files/extraitalice.txt";


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

void fillListAlphabet()
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

int main()
{

    fillListAlphabet();

    for (auto& it : alphabet)
    {
        std::cout << it.first << " : " << it.second << std::endl;
    }

    return 0;
}
