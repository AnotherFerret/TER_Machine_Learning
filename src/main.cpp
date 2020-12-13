
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int victoire_a = 0;
int victoire_b = 0;
const char* data = "graphs/result.data";
const char* victory = "graphs/victory.data";

//classe joueur possedant une reserve pour la mise, une main, et un cerveau ( lien vers fichier )
class player
{
    public:
        int banque;
        int hand;
        const char* brain[3];

        player(const char* filename[3], int money)
        {
            this->banque = money;
            this->hand = 0;
            for (int i = 0; i < 3; i++) {
                this->brain[i] = filename[i];
            }
            //this->brain = fopen(filename, "r+");
        }

        void close_file()
        {
            ;
        }
};

//recupere depuis la ligne correspondant a la carte du bot, le % de chaque action
void get_percent(std::string line, int* couche_p, int* mise_p, int* double_p, int* triple_p)
{
	string tmp_value = "";
	for(int i = 0, j = 0; line[i] != '\0'; i++)
	{
            if(line[i] == ',' && j == 0)
            {
                i++;
                while(line[i] <= 57 && line[i] >= 48)
                {
					tmp_value += line[i];
					i++;
				}
				*couche_p = stoi(tmp_value);
				tmp_value = "";
				j++;
            }
            if(line[i] == ',' && j == 1)
            {
                i++;
                while(line[i] <= 57 && line[i] >= 48)
                {
					tmp_value += line[i];
					i++;
				}
				*mise_p = stoi(tmp_value);
				tmp_value = "";
				j++;
            }
            if(line[i] == ',' && j == 2)
            {
                i++;
                while(line[i] <= 57 && line[i] >= 48)
                {
					tmp_value += line[i];
					i++;
				}
				*double_p = stoi(tmp_value);
				tmp_value = "";
				j++;
            }
            if(line[i] == ',' && j == 3)
            {
                i++;
                while(line[i] <= 57 && line[i] >= 48)
                {
					tmp_value += line[i];
					i++;
				}
				*triple_p = stoi(tmp_value);
				tmp_value = "";
				j++;
			}
		}
}

//decision = 0 si le joueur s'est couché, 1 si le joueur a gagné en misant, 2 si le joueur a gagn� en triplant
void learn(const char* brain, int decision, int mise, int hand, int power)
{
    std::string line;

    int couche_p = 0;				//probabilités de chaque action
    int mise_p = 0;
    int double_p = 0;
    int triple_p = 0;
    int recompense = mise*3;		//la valeur qu'on va tenter de donner comme récompense
    int recompense_final = 0;		//la valeur finale dépend de ce uq'on peut soustraire aux valeurs du cerveau, parfois on ne retirera qu'à deux ou à un cerveau
									//éventuellement on pourrait diviser récompense par 2 ou par 1 ( en incrémentant ) si on veut réellement retirer toujours la même 
									//valeur car cette solution fera plus lentement converger
	int diviseur = 3;
    int min_value = 5000;			//valeur minimale des probas de chaque action

    // Au lieu de modifier le fichier brain on va le reecrire avec la ligne modifier dans le fichier temp puis renommer le fichier temp
    std::fstream my_file;
    my_file.open(brain, std::fstream::in | std::fstream::out);


    std::fstream file_temp;
    file_temp.open("temp.txt", std::fstream::out); //cree le fichier si il n'existe pas, le referme puis réouvre pour initialiser, la manipulation est necessaire même si ca parait etrange ! Ne pas enlever !
    file_temp.close();
    file_temp.open("temp.txt", std::fstream::in | std::fstream::out); //ouvre le fichier

    // On parcours jusqu'a la ligne que l'on cherche (celle de la main), tout en copiant dans file_temp
    std::getline(my_file, line);
    file_temp << line << endl;

    while(std::getline(my_file, line) && ((line[0])-48) != hand)
    {
        file_temp << line << endl;
    }
    string line_res = line.c_str();
    get_percent(line_res, &couche_p, &mise_p, &double_p, &triple_p);

    //Si le joueur s'est couché (on arrive ici que si le joueur a eu raison de se coucher, c a d sa carte etait inferieure a celle de l'adversaire )
    if (decision == 0 && couche_p < power-recompense && couche_p + recompense > 0  )
    {
        if(mise_p < recompense+min_value)
		{
			diviseur--;
		}
		if(double_p < recompense+min_value)
		{
			diviseur--;
		}
		if(triple_p < recompense+min_value)
		{
			diviseur--;
		}
		
        if(mise_p > recompense+min_value && mise_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			mise_p -= recompense/diviseur;
		}
		if(double_p > recompense+min_value && double_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			double_p -= recompense/diviseur;
		}
		if(triple_p > recompense+min_value && triple_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			triple_p -= recompense/diviseur;
		}
        couche_p += recompense_final;
    }
    //Si le joueur a misé et l'a emporté
    if (decision == 1 && mise_p < power-recompense && mise_p + recompense > 0)
    {
		if(couche_p < recompense+min_value )
		{
			diviseur--;
		}
		if(double_p < recompense+min_value)
		{
			diviseur--;
		}
		if(triple_p < recompense+min_value)
		{
			diviseur--;
		}
		
        if(couche_p > recompense+min_value && couche_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			couche_p -= recompense/diviseur;
		}
		if(double_p > recompense+min_value && double_p < power-min_value )
        {
			recompense_final += recompense/diviseur;
			double_p -= recompense/diviseur;
		}
		if(triple_p > recompense+min_value && triple_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			triple_p -= recompense/diviseur;
		}
        mise_p += recompense_final;
    }
    //Si le joueur a doublé et l'a emporté
    if (decision == 2 && double_p < power-recompense && double_p + recompense > 0 )
    {
		if(couche_p < recompense+min_value)
		{
			diviseur--;
		}
		if(mise_p < recompense+min_value)
		{
			diviseur--;
		}
		if(triple_p < recompense+min_value)
		{
			diviseur--;
		}
		
        if(couche_p > recompense+min_value && couche_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			couche_p -= recompense/diviseur;
		}
		if(mise_p > recompense+min_value && mise_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			mise_p -= recompense/diviseur;
		}
		if(triple_p > recompense+min_value && triple_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			triple_p -= recompense/diviseur;
		}
        double_p += recompense_final;
    }
    //Si le joueur a triplé et l'a emporté
    if (decision == 3 && triple_p < power-recompense && triple_p + recompense > 0)
    {
		
		if(couche_p < recompense+min_value)
		{
			diviseur--;
		}
		if(mise_p < recompense+min_value)
		{
			diviseur--;
		}
		if(double_p < recompense+min_value)
		{
			diviseur--;
		}
        if(couche_p > recompense+min_value && couche_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			couche_p -= recompense/diviseur;
		}
		if(mise_p > recompense+min_value && mise_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			mise_p -= recompense/diviseur;
		}
		if(double_p > recompense+min_value && double_p < power-min_value)
        {
			recompense_final += recompense/diviseur;
			double_p -= recompense/diviseur;
		}
        triple_p += recompense_final;
    }

	file_temp << hand << "," << couche_p << "," << mise_p << "," << double_p  << "," << triple_p << endl;


    // on copie la fin du fichier dans le fichier temporaire
    while (std::getline(my_file, line)){
        file_temp << line << endl;
    }

    my_file.close();
    file_temp.close();

    // supprimer le fichier existant et le remplace par le temporaire
    if (std::remove(brain) != 0)
        {
            perror( " Error removing file" );
        }
    else
    {
        if (rename("temp.txt", brain) != 0) { perror( " Error renaming file" );}
    }
}

//un tour de jeu (voir todo list)
//pA = joueur A, pB = joueur B, mise = l'�tat de la mise actuelle ( en cas d'�galit� entre les joueurs la mise reste )
int play(player* pA, player* pB, int* mise)
{

    std::string line;
    ifstream brainA(pA->brain[0]);
    ifstream brainB;
    
    int proba_power = 100000;

    int couche_p = 0;           //la proba de se coucher du joueur actuel
    int mise_p = 0;             //proba de miser
    int double_p = 0;           //proba de doubler
    int triple_p = 0;           //proba de tripler
    int choice_p = 0;           //le choix au hasard
    
    int recompense_A = 0;			//La récompense en cas de victoire pour joueur A
    int recompense_B = 0;			//La récompense en cas de victoire pour joueur B
    int alpha = 5000;				//le diviseur de la récompense pour une convergence plus ou moins rapide

    int decision_A = -1;         //la decision qu'� prit A 
    int decision_B = -1;         //la decision qu'� prit B

    int mise_init = 5;          //la mise initiale obligatoire

    pA->hand = rand()%10;       //pioche de A
    pB->hand = rand()%10;       //pioche de B

    pA->banque -= mise_init;
    pB->banque -= mise_init;
    *mise += mise_init*2;

    //std::cout << "---------Player A starts---------" << line[0] << endl;

    //v�rifie toutes les lignes pour trouver celle qui correspond � la carte pioch�e
    while(std::getline(brainA, line) && (line[0])-48 != pA->hand)
    {
        ;
    }
    get_percent(line, &couche_p, &mise_p, &double_p, &triple_p);
    brainA.close();
    
    //somme des % pour avoir des intervalles, ex : 25,25,25,25 -> [0,25] [25,50] [50,75] [75,100]
    mise_p += couche_p;
    double_p += mise_p;
    triple_p += double_p;

    choice_p = rand()%proba_power;
    if(choice_p < couche_p)
    {
		//je me couche
        decision_A = 0;
        recompense_A = (proba_power-couche_p)/alpha;
    }
    else if(choice_p > couche_p && choice_p < mise_p)
    {
		//je mise
        pA->banque -= mise_init;
        *mise += mise_init;
        decision_A = 1;
        recompense_A = (proba_power-(mise_p - couche_p))/alpha;
        brainB.open(pB->brain[0]);
    }
    else if(choice_p > mise_p && choice_p < double_p)
    {
		//je double
        pA->banque -= mise_init*2;
        *mise += mise_init*2;
        decision_A = 2;
        recompense_A = (proba_power-(double_p - mise_p))/alpha;
        brainB.open(pB->brain[1]);
    }
    else if(choice_p > double_p)
    {
		//je triple
		pA->banque -= mise_init*3;
        *mise += mise_init*3;
        decision_A = 3;
        recompense_A = (proba_power-(triple_p - double_p))/alpha;
        brainB.open(pB->brain[2]);
    }

    //std::cout << "---------Player B starts---------" << line[0] << endl;
    while(std::getline(brainB, line) && (line[0])-48 != pB->hand)
    {
        ;
    }
    get_percent(line, &couche_p, &mise_p, &double_p, &triple_p);
    brainB.close();

    if(decision_A > 0)
    {
		//somme des % pour avoir des intervalles, ex : 25,25,25,25 -> [0,25] [25,50] [50,75] [75,100]
        mise_p += couche_p;
        double_p += mise_p;
        triple_p += double_p;

        choice_p = rand()%proba_power;
        if(choice_p < couche_p || ((choice_p > couche_p && choice_p < mise_p) && (decision_A >= 2)) || ((choice_p > mise_p && choice_p < double_p) && (decision_A == 3))) 
        {
			//Si j'ai choisi de me coucher OU choisi de miser et A a doublé/triplé OU choisi de doubler alors que A a triplé
            decision_B = 0;
            recompense_B = (proba_power - couche_p) / alpha;
        }
        else if((choice_p > couche_p && choice_p < mise_p) || (choice_p > mise_p && decision_A == 1)) 
        {
			//si j'ai choisi de miser  OU j'ai choisi de doubler/tripler et A a choisi de miser
            pB->banque -= mise_init;
            *mise += mise_init;
            recompense_B = (proba_power - (mise_p - couche_p)) / alpha;
            decision_B = 1;
        }
        else if((choice_p > mise_p && choice_p < double_p) || (choice_p > double_p && decision_A == 2)) //Si j'ai choisi de doubler OU j'ai choisi de tripler et A a doublé
        {
            pB->banque -= mise_init*2;
            *mise += mise_init*2;
            recompense_B = (proba_power-(double_p - mise_p)) / alpha;
            decision_B = 2;
        }
        else if(choice_p > double_p && decision_A == 3)
        {
            pB->banque -= mise_init*3;
            *mise += mise_init*3;
            recompense_B = (proba_power - (triple_p - double_p)) / alpha;
            decision_B = 3;
        }
    }

    //apprentissage si B se couche, uniquement si c'est une bonne décision
    if((decision_B == 0 && pB->hand < pA->hand) || (decision_B == 1 && decision_A > 1 && pB->hand < pA->hand) || (decision_B == 2 && decision_A > 2 && pB->hand < pA->hand))
    {
        learn(pB->brain[decision_A-1], decision_B, recompense_B, pB->hand, proba_power);
    }
    
    //apprentissage si A se couche, uniquement si c'est une bonne décision
    if(decision_A == 0 && pA->hand < pB->hand)
    {
        learn(pA->brain[0], decision_A, recompense_A, pA->hand, proba_power);
    }

    //Apprentissage victoire appel du cerveau 0 de A (car b a choisi se coucher = 0, miser = 1, doubler = 2)
    if((pA->hand > pB->hand || decision_B == 0) && decision_A != 0)
    {
        //récompense A
        //LRI
        learn(pA->brain[0], decision_A, recompense_A, pA->hand, proba_power);
        
        //LRP pour tests, retiré dans le modèle initial
        /*
        if(decision_B != 0)
        {
			learn(pB->brain[decision_A-1], decision_B, -recompense_B, pB->hand, proba_power);
		}*/
        
        pA->banque += *mise;
        *mise = 0;
        
        victoire_a++;
    }
    else if((pA->hand < pB->hand || decision_A == 0) && decision_B != 0)
    {
        //recompense B
        //La decision X de A Active le cerveau X-1 de B, d'ou le "decision_A-1"
        if(decision_A > 0)
        {
			//LRI
			learn(pB->brain[decision_A-1], decision_B, recompense_B, pB->hand, proba_power);
			
			//LRP pour tests, retiré dans le modèle initial
			/*
			if(decision_A != 0)
			{
				learn(pA->brain[0], decision_A, -recompense_A, pA->hand, proba_power);
			}*/
		}
        
        pB->banque += *mise;
        *mise = 0;
        
        victoire_b++;
    }
    else if(pA->hand == pB->hand)
    {
        //std::cout << "nobody wins, la mise reste" << "\n";
        //learn(pA->brain, decision_A, 0, pA->hand, pB->hand);
        //learn(pB->brain, decision_B, 0, pB->hand, pA->hand);
    }
    else
    {
        perror("erreur : situation imprevue, pas de gagnant et pas d'egalité");
    }
    return(0);
}

//le main, initialise les variables, les joueurs...
int main (int argc, char** argv) {

    srand(time(NULL));  					//Initialisation suivant timestamp
    int round = 250000;            		   	//nombre de round initial si pas de valeur donnée
    if(argv[1] != "")
    {             
		round = stoi(argv[1]);             //nombre de tours de jeu
	}
    int round_max = round;					//nombre de tours définit, utilisé pour courbes
    int init_money = stoi(argv[2]);			//banque des IAs
    int mise = 0;

    const char* brainA[3] = {"brainA1.txt","brainA2.txt","brainA3.txt"};
    const char* brainB[3] = {"brainB1.txt","brainB2.txt","brainB3.txt"};

	//init joueurs
    player pA(brainA, init_money);
    player pB(brainB, init_money);
    
    fstream f;
    fstream f2;
    f.open(data, std::fstream::out);
    f2.open(victory, std::fstream::out);

	//on lance les parties en boucle, la boucle sert également à récupérer les données pour les courbes
    while(round > 0 && pA.banque > 0 && pB.banque > 0)
    {
        play(&pA, &pB, &mise);
        round--;
        f << pA.banque - init_money << " " << pB.banque - init_money << " " << round_max-round << "\n";
        f2 << (double)victoire_a/(round_max-round) << " " << (double)victoire_b/(round_max-round) << " " << round_max-round << "\n";
        //std::cout << "\n===round=== " << 5000-round << "\n";
    }
    std::cout << "Player A banque : " << pA.banque << "\n";
    std::cout << "Player B banque : " << pB.banque << "\n";
    pA.close_file();
    pB.close_file();
    f.close();
    f2.close();

    return 0;
}
