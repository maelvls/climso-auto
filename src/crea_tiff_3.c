/* 	createur image TIFF

	Serre Denis - Mars 2004
	
	cree une image TIFF a partir de:
	-largeur image: nbpts_x
	-longueur image: nbpts_y
	-resolution d'impression (ex: 300ppp => rentrer 300)
	-normalisation (par quoi diviser toutes les valeurs pour avoir sur l'image l'occpation 
		de la dynamique souahitee (ex: max du tableau = 0.01 => rentrer 0.01 pour avoir toutes
		valeurs comprises entre 0 et 255); les valeurs < 0 sont seuillees a 0
	-le tableau de valeurs
	-le nom du fichier a ecrire
	
	nov 04: il y a un pb... de fprintf: lorsque j'envoie une image de 3500x3500, les str sont
		bien (0 0) 13 172, mais l'ecriture du fichier.tiff n'est pas 0DAC mais 0AAC...
	il s'avere que le 10 est ecrit 0D, et le 13 est ecrit 0A (desactiver PART1, activer PART2,
		lancer son prgm quelconque normalement, et ouvrir le fichier ecrit dans un editeur 
		hexadecimal pour s'en convaincre). donc remaniement "jambe de bois" pour que ca marche...
	
	modifs cosmetiques en fevrier 2005, et rajout d'un "else if" pour le pb 10-13, le 24/02/05
	passage au 25/02/05 en crea_tiff_old.cp: car ds le nouveau crea_tiff, on va respecter les
		lignes et colonnes, et ne plus avoir besoin de faire un miroir depuis la fct principale
	
	25/02/05: respect des lignes/colonnes, chgts d'appelations, appel autonome de fct miroir
		: dans le mov de utilities_DS: standardisation et bonne ecriture sont nos amis:
		i: no ligne, j: no colonne, i et j partent de bas gauche et vont vers haut droit
		
	crea_tiff_2.cp: introduit la fct read_tiff, et appel a la fct crea_tiff_2: change:
		desormais il faut dire entre quelles et quelles valeurs (0 -> 255) la dynamique de
		l'image doit se faire , et quelles sony les valeurs de saturation du tablo d'entree
	
	20/04/05: les valeurs "3" des parametres 254,256,257,273 sont passes en "4": codage sur 32
	bits, sinon a priori on ne code que sur 2 octets, soit 16 bits; les valeurs du parametre 
	sont donc innaccessibles lors de la lecture

	24/06/05: ajout de fct crea_tiff_ppc
	
	17/11/05: ajout de fct crea_tiff_ppc_complex
	
	13/12/05: - desactivation de l'echange 0A et 0D: pas besoin avec la compil sous Sioux
			  - modif de crea_tiff_3: on laisse le tableau d'entree intact !!!!, a part
			  		des echanges remis, mais surtout plus d'ecretage... les donnees du tableau
			  		ressortent telles qu'elles sont entrees
	
	26/01/06: ajout de fct crea_tiff_2_ppc_complex
	
	v3: suite au changement de la methode de lecture des read_tiff, bien meilleure, on passe
		en v3; il y a aussi epuration, et ajout return_dim_tiff
		on passe en read_tiff_3 et en crea_tiff_4(_ppc_complex)
		
	19/04/06: on passe a ecrire des fichiers en binaire (fopen "b"), de facon a ne plus avoir
		les pbs de 0A et 0D
		
	29/08/06: reprise de read_tiff_3: au niveau de la determination du nb de parametres et de
		la lecture des valeurs, plus possibilite de lire si y'a des bandes EN SUPPOSANT qd
		meme que toutes les bandes sont a la suite les unes des autres

	13/11/06: is.close(), delete(buffer), delete(buffer_int): ce qui permet que crea_tiff_3 est 
		a present compatible xcode !!
		
	28/11/06: manquait une prise en compte du 16 bits dans le crea_tiff_4_ppc_complex
		; ajout de l'option 5 pour la creation du .tiff complex
 
  10/04/2014 suppression des complex
		
	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
using namespace std;

#include "fcts_LK3.h"
#include "crea_tiff_3.h"

//------------------------------------------------------------------------------------
//		fait le miroir d'un tableau selon l'axe horizontal
//		parametres: data: tableau d'entree
//					nbpts_x: nb de points selon l'horizontale
//					nbpts_y: nb de points selon la verticale
//		doit etre valable pour des tableaux de hauteur paire ou impaire
//------------------------------------------------------------------------------------

void miroir_horizon(double **data,int nbpts_x,int nbpts_y)
{	double *intermediaire = (double*) malloc (nbpts_x*sizeof(double));
	int i,j;
	
	for(i=0;i<arrondi_int( ((double)(nbpts_y))/2-0.25 );i++)
	//	le arrondi_int et le -0.25: pour pas avoir d'emm**** avec les aproxxs num
	{	for(j=0;j<nbpts_x;j++)
		{	intermediaire[j] = data[i][j];
			data[i][j] = data[nbpts_y-1-i][j];
			data[nbpts_y-1-i][j] = intermediaire[j];
		}
	}
	free(intermediaire);
}

	
//------------------------------------------------------------------------------------
//		retourne les valeurs hexadecimales
//------------------------------------------------------------------------------------

void hexadec (int j, int *hexa)
{	hexa[0] = j / (256*256*256);
	j=j-hexa[0]*256*256*256;
	hexa[1] = j / (256*256);
	j=j-hexa[1]*256*256;	
	hexa[2] = j / 256;	// division entire : donne les trois octets de poids fort
	hexa[3] = j % 256; // reste : donne l'octet de poids faible
}

//----------------------------------------------------------------------------------------
//		traitement des valeurs sorties apres les operations:
//
//		char *buffer;
//		int *buffer_int;
//		ifstream is;
//		is.open (str,ios::binary);
//		is.seekg(0,ios::end);
//		length = is.tellg();
//		is.seekg(0,ios::beg);
//		buffer = new char [length];
//		is.read (buffer,length);
//		is.close;
//		for(i=0;i<length;i++)
//		{	buffer_int[i] = (int)(buffer[i]);
//		}	
//
//		En sortie de ca, les valeurs de buffer_int vont de -128 a 127; on les remets de 0 a
//			255, dans le bon ordre
//		parametres: le tablo
//					sa longueur
//		le tablo est retourne modifie
//----------------------------------------------------------------------------------------

void traitement_stream(int *buffer_int,int length)
{	int i;
	
	for(i=0;i<length;i++)
	{	if( (buffer_int[i]>=0) && (buffer_int[i] <=127) )
		{	// nada
		}
		else if( (buffer_int[i]<0) && (buffer_int[i] >= -128) )
		{	buffer_int[i] = buffer_int[i] + 256;
		}
		else
		{	printf("pb!!!!: valeurs de buffer_int pas comprises entre -128 et + 127\n");
		}
	}
}
	

//--------------------------------------------------------------------------------------------
//		retourne les parametres nbpts_x et nbpts_y d'header de .tiff
//		parametres: *nbpts: vecteur de int a 3 elements; seront retournes nbpts_x et _y resp,
//						et le nombre de bits/grain de l'image
//						suppose alloue correctement
//					str[256]: nom du .tiff a lire
//					reconnait les images codees sur 8 ou 16 bits
//		NOTA: pourquoi ne pas l'avoir integre directement a read_tiff ?? parce que comme ca
//			on peut allouer sans souci le tableau de lecture de l'image dans le prgm principal
//--------------------------------------------------------------------------------------------

void return_dim_tiff(int *nbpts,char str[256])
{	int length,i,value,value_2,d,e,f;
	char *buffer;
	int *buffer_int;
	int adresse_repertoire,statut;
	int nb_params_1,nb_params_2,nb_params;
	int *adresse_256 = (int*) malloc(4*sizeof(int));
	int nbpts_x;
	int *adresse_257 = (int*) malloc(4*sizeof(int));
	int nbpts_y;
	int *adresse_258 = (int*) malloc(2*sizeof(int));
	int nb_bits;
	
	// ---- securite: on verifie la presence du fichier
	FILE *txt_test = NULL;
	txt_test = fopen(str,"rb");
	if(txt_test == NULL)
	{	printf("pb dans return_dim_tiff!!!!: fichier %s inexistant\n",str);
		exit(1);
	}
	fclose(txt_test);
	
	ifstream is;
	
	is.open (str,ios::binary);		// open file
	
	// get length of the file
	is.seekg(0,ios::end);
	length = is.tellg();
	is.seekg(0,ios::beg);
	
	// allocate memory
	buffer = new char [length];
	buffer_int = new int [length];
	
	// read data as block
	is.read (buffer,length);
	is.close();
	
	for(i=0;i<length;i++)
	{	buffer_int[i] = (int)(buffer[i]);
	}
	
	traitement_stream(buffer_int,length);
	// en sortie on a toutes les valeurs du fichier en memoire, en entier, entre 0 et 255
		
	if(buffer_int[0] == 73)
    {	adresse_repertoire = buffer_int[4] + 16*16*buffer_int[5] + 16*16*16*16*buffer_int[6]
    		+ 16*16*16*16*16*16*16*16*buffer_int[7];
    	//printf("conv intel\n");
    }//	intel
    else if(buffer_int[0] == 77)
    {	adresse_repertoire = buffer_int[7] + 16*16*buffer_int[6] + 16*16*16*16*buffer_int[5]
    		+ 16*16*16*16*16*16*16*16*buffer_int[4];
    		//printf("conv motorola\n");
    }// motorola
    else
    {	printf("pb!!!!: pas un fichier tiff valide\n");
    }// securite
    // adresse du premier repertoire trouvee
    //printf("adresse_repertoire=%d\n",adresse_repertoire);
    
    nb_params_1 = buffer_int[adresse_repertoire];
    nb_params_2 = buffer_int[adresse_repertoire+1];
    if(buffer_int[0] == 73)
    {	nb_params = nb_params_1 + 16*16*nb_params_2;
    	//printf("nb_params = %d\n",nb_params);
    }
    else if(buffer_int[0] == 77)
    {	nb_params = nb_params_2 + 16*16*nb_params_1;
    	//printf("nb_params = %d\n",nb_params);
    }//	on a le nombre de parametres du repertoire
    
    if(buffer_int[0] == 73) // intel
    {	statut = 0;
    	for(i=0;i<nb_params;i++)
    	{	value = buffer_int[adresse_repertoire + i*12 + 2];
    		value_2 = buffer_int[adresse_repertoire + i*12 + 1 + 2];
    		if( (value == 0) && (value_2 == 1) )// parametre 256
    		{	d = buffer_int[adresse_repertoire + i*12 + 3 + 2];
    			adresse_256[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			adresse_256[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			adresse_256[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			adresse_256[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			statut++;
    		}
    		else if( (value == 1) && (value_2 == 1) )// parametre 257
    		{	e = buffer_int[adresse_repertoire + i*12 + 2 + 2];
    			adresse_257[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			adresse_257[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			adresse_257[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			adresse_257[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			statut++;
    		}
    		else if( (value == 2) && (value_2 == 1) )// parametre 258
    		{	f = buffer_int[adresse_repertoire + i*12 + 2 + 2];
    			nb_bits = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			statut++;
    		}
 			//printf("value=%d\tvalue_2=%d\n",value,value_2);
    	}
    	if(statut != 3)
    	{	printf("statut != 3 dans return_dim_tiff\n");
    	}
    	// d,e et f non utiles dans ce cas intel
    	nbpts_x = adresse_256[0] + 256*adresse_256[1] + 256*256*adresse_256[2]
    		+ 256*256*256*256*adresse_256[3];
    	nbpts_y = adresse_257[0] + 256*adresse_257[1]
    		+ 256*adresse_257[2] + 256*256*256*256*adresse_257[3];
    }
    
    else if(buffer_int[0] == 77)
    {	statut = 0;
    	for(i=0;i<nb_params;i++)
    	{	value = buffer_int[adresse_repertoire + i*12 + 2];
    		value_2 = buffer_int[adresse_repertoire + i*12 + 1 + 2];
    		if( (value == 1) && (value_2 == 0) )// parametre 256: nbpts_x
    		{	d = buffer_int[adresse_repertoire + i*12 + 3 + 2];
    			adresse_256[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			adresse_256[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			adresse_256[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			adresse_256[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			statut++;
    		}
    		else if( (value == 1) && (value_2 == 1) )// parametre 257: nbpts_y
    		{	e = buffer_int[adresse_repertoire + i*12 + 3 + 2];
    			adresse_257[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			adresse_257[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			adresse_257[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			adresse_257[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			statut++;
    		}
    		else if( (value == 1) && (value_2 == 2) )// parametre 258
    		{	f = buffer_int[adresse_repertoire + i*12 + 3 + 2];
    			adresse_258[0] = buffer_int[adresse_repertoire + i*12 + 8 + 1 + 2];
    			adresse_258[1] = buffer_int[adresse_repertoire + i*12 + 8 + 3 + 2];
    			// en fct de f on va lire l'un ou l'autre
    			statut++;
    		}
    	}
    	if(statut != 3)
    	{	printf("statut != 3 dans return_dim_tiff\n");
    	}
    	if( d == 4 )
    	{	nbpts_x = adresse_256[3] + 256*adresse_256[2] + 256*256*adresse_256[1]
    		+ 256*256*256*256*adresse_256[0];
    	}
    	else if( d == 3)
    	{	nbpts_x = adresse_256[1] + 256*adresse_256[0];
    	}
    	if( e == 4)
    	{	nbpts_y = adresse_257[3] + 256*adresse_257[2]
    		+ 256*256*adresse_257[1] + 256*256*256*256*adresse_257[0];
    	}
    	else if( e == 3)
    	{	nbpts_y = adresse_257[1] + 256*adresse_257[0];
    	}
    	if( f == 3)
    	{	nb_bits = adresse_258[0];
    	}
    	else if(f == 4)
    	{	nb_bits = adresse_258[1];
    	}
    }
    nbpts[0] = nbpts_x;
    nbpts[1] = nbpts_y;
    nbpts[2] = nb_bits;
    
    free(adresse_256);
    free(adresse_257);
    free(adresse_258);
	delete(buffer);
	delete(buffer_int);
}



//-------------------------------------------------------------------------------------------
//		fct read_tiff_3
//		tire en partie de http://cplusplus.com/ref/iostream/istream/get.html et de
//			"le format tiff et ses modes de compression" de christian monteix
//		lit:- les .tiff non compresses, en niveaux de gris, codes sur 8 ou 16 bits
//			- ok pour formats pc ou mac, et pour palette graph (0/1) -> (blanc/noir) ds
//				tous les sens
//			- le repertoire peut a priori etre situe apres les donnees
//			- param 255: 1 seule representation bitmap (pas de couche, de transparence)
//			- param 262: lit qu'une image en niveaux de gris, donc valeur 0 ou 1
//			- suppose que les params 258, 262 et 273 sont ds le premier repertoire
//			- param 274: suppose que valeur = 1, soit image decrite: 1ere ligne en haut;
//				et premiere colonne a gauche; eventuellement besoin de faire un coup de
//				miroir_horizon donc apres la lecture
//			- suppose que ts les pixels sont decrits a la suite les uns des autres, et
//				commencent a l'adresse renvoyee par la valeur du param 273; il y a quand
//				meme un test qui fait que si on trouve plus d'une bande, la fct va rechercher
//				l'adresse de la premiere bande MAIS supposer que toutes les autres sont a la
//				suite
//			- s'interesse pas a une courbe de reponse de gris et autres parametres fun
//		renvoie: un tableau de donnees comprenant les valeurs des pixels, valeurs hautes
//			correspondant au blanc
//		A NOTER PAR RAPPORT A ImageJ: ImageJ ouvre les images correctement, que l'interpret
//			photo soit 0 ou 1 (param 262); par contre il a en memoire les valeurs elles-memes
//			=> si on soustrait les 2 memes images, l'une avec le param 262 a 0 et l'autre 
//			a 1, on n'obtient pas une image remplie de 0.
//		Validee avec essai_lena_rect.tiff, essai_lena_rect_90.tiff, essai_lena_rect_mac.tiff,
//			pour les 8 bits; Dblet180-seriestrous-10ms_1_*.tif, les * etant reen, conv_intel,
//			et conv_mac; mais donc ai pas pu tester la lecture en bandes en conv intel
//		Pour etre sur que la lecture a ete correcte: le plus simple: recreer le .tiff par
//			crea_tiff_4 et controler la bonne reformation de l'image
//		Utilisation de http://www.cplusplus.com/ref/iostream/istream/seekg.html
//-------------------------------------------------------------------------------------------

void read_tiff_3(double **tableau_result,int nbpts_x,int nbpts_y,char str[256])
{	int length,i,j,value,value_2,d,e,f;
	char *buffer;
	int *buffer_int;
	int adresse_repertoire,statut;
	int nb_params_1,nb_params_2,nb_params;
	int *adresse_273 = (int*) malloc(4*sizeof(int));
	int adresse_valeurs;
	int *adresse_262 = (int*) malloc(4*sizeof(int));
	int valeur_interpret_photo = -1;
	int *value_16 = (int*) malloc(2*sizeof(int));
	
	int *adresse_258 = (int*) malloc(2*sizeof(int));
	int nb_bits = -1;
	int nb_bandes,temp_int;
	
	// ---- securite: on verifie la presence du fichier
	FILE *txt_test = NULL;
	txt_test = fopen(str,"rb");
	if(txt_test == NULL)
	{	printf("pb dans read_tiff!!!!: fichier %s inexistant\n",str);
		exit(1);
	}
	fclose(txt_test);
	
	ifstream is;
	
	is.open (str,ios::binary);		// open file
	
	// get length of the file
	is.seekg(0,ios::end);
	length = is.tellg();
	is.seekg(0,ios::beg);
	
	// allocate memory
	buffer = new char [length];
	buffer_int = new int [length];
	
	// read data as block
	is.read (buffer,length);
	is.close();
	
	for(i=0;i<length;i++)
	{	buffer_int[i] = (int)(buffer[i]);
	}
	
	traitement_stream(buffer_int,length);
	// en sortie on a toutes les valeurs du fichier en memoire, en entier, entre 0 et 255
	
	//printf("buffer_int[0] = %d\n",buffer_int[0]);
	//printf("buffer_int[5] = %d\n",buffer_int[5]);
	//printf("buffer_int[2910193] = %d\n",buffer_int[2910193]);
	
	if(buffer_int[0] == 73)
    {	adresse_repertoire = buffer_int[4] + 16*16*buffer_int[5] + 16*16*16*16*buffer_int[6]
    		+ 16*16*16*16*16*16*16*16*buffer_int[7];
    	// printf("conv intel\n");
    }//	intel
    else if(buffer_int[0] == 77)
    {	adresse_repertoire = buffer_int[7] + 16*16*buffer_int[6] + 16*16*16*16*buffer_int[5]
    		+ 16*16*16*16*16*16*16*16*buffer_int[4];
    	//	printf("conv motorola\n");
    }// motorola
    else
    {	printf("pb!!!!: pas un fichier tiff valide\n");
    }// securite
    // adresse du premier repertoire trouvee
    // printf("adresse_repertoire=%d\n",adresse_repertoire);
    
    nb_params_1 = buffer_int[adresse_repertoire];
    nb_params_2 = buffer_int[adresse_repertoire+1];
    if(buffer_int[0] == 73)
    {	nb_params = nb_params_1 + 16*16*nb_params_2;
    	// printf("nb_params = %d\n",nb_params);
    }
    else if(buffer_int[0] == 77)
    {	nb_params = nb_params_2 + 16*16*nb_params_1;
    	// printf("nb_params = %d\n",nb_params);
    }//	on a le nombre de parametres du repertoire
    
    if(buffer_int[0] == 73) // intel
    {	statut = 0;
    	for(i=0;i<nb_params;i++)
    	{	value = buffer_int[adresse_repertoire + i*12 + 2];
    		value_2 = buffer_int[adresse_repertoire + i*12 + 1 + 2];
    		if( (value == 17) && (value_2 == 1) )// parametre 273
    		{	d = buffer_int[adresse_repertoire + i*12 + 2 + 2];
    			if(d == 3)
    			{	nb_bandes = buffer_int[adresse_repertoire + i*12 + 4 + 2];
    				nb_bandes = nb_bandes + 256*buffer_int[adresse_repertoire + i*12 + 5 + 2];
    			}
    			else if(d == 4)
    			{	nb_bandes = buffer_int[adresse_repertoire + i*12 + 4 + 2];
    				nb_bandes = nb_bandes + 256*buffer_int[adresse_repertoire + i*12 + 5 + 2];
    				nb_bandes = nb_bandes + 256*256*buffer_int[adresse_repertoire + i*12 + 6 + 2];
    				nb_bandes = nb_bandes + 256*256*256*256*buffer_int[adresse_repertoire + i*12 + 7 + 2];
    			}
    			else
    			{	printf("pb!!!!: d != 3 ou 4\n");
    				exit(1);
    			}
    			adresse_273[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			// le +2: car on a les 2*(2octets) qui disent combien y'a de params
    			adresse_273[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			adresse_273[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			adresse_273[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			//printf("adresse_273[0]=%d\n",adresse_273[0]);
	   			//printf("adresse_273[1]=%d\n",adresse_273[1]);
	   			statut++;
    		}
    		else if( (value == 6) && (value_2 == 1) )// parametre 262
    		{	e = buffer_int[adresse_repertoire + i*12 + 2 + 2];
    			adresse_262[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			adresse_262[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			adresse_262[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			adresse_262[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			//printf("adresse_273[0]=%d\n",adresse_273[0]);
	   			//printf("adresse_273[1]=%d\n",adresse_273[1]);
	   			statut++;
    		}
    		else if( (value == 2) && (value_2 == 1) )// parametre 258
    		{	f = buffer_int[adresse_repertoire + i*12 + 2 + 2];
    			nb_bits = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			statut++;
    		}
 			//printf("value=%d\tvalue_2=%d\n",value,value_2);
    	}
    	if(statut != 3)
    	{	printf("y'a un des parametres 258, 262 ou 273 qu'on a pas trouve !\n");
    	}
    	// d, e et f non utiles dans ce cas intel
    	adresse_valeurs = adresse_273[0] + 16*16*adresse_273[1] + 16*16*16*16*adresse_273[2]
    		+ 16*16*16*16*16*16*16*16*adresse_273[3];
    	if(nb_bandes == 1)
    	{	//nada, l'adresse donnee est bien celle des donnees
    	}
    	else// on cherche l'adresse de la premiere bande a l'adresse precedemment calculee
    	//on suppose que toutes les bandes sont a la suite
    	{	temp_int = buffer_int[adresse_valeurs] + 256*buffer_int[adresse_valeurs+1] +
    		256*256*buffer_int[adresse_valeurs+2] + 256*256*256*256*256*buffer_int[adresse_valeurs+3];
    		adresse_valeurs = temp_int;
    		// printf("nb_bandes = %d\n",nb_bandes);
    	}
    	valeur_interpret_photo = adresse_262[0] + 16*16*adresse_262[1]
    		+ 16*16*16*16*adresse_262[2] + 16*16*16*16*16*16*16*16*adresse_262[3];
    	// printf("adresse des valeurs = %d\n",adresse_valeurs);
    	// printf("valeur_interpret_photo = %d\n",valeur_interpret_photo);
    }
    
    else if(buffer_int[0] == 77)
    {	statut = 0;
    	for(i=0;i<nb_params;i++)
    	{	value = buffer_int[adresse_repertoire + i*12 + 2];
    		value_2 = buffer_int[adresse_repertoire + i*12 + 1 + 2];
    		if( (value == 1) && (value_2 == 17) )// parametre 273
    		{	d = buffer_int[adresse_repertoire + i*12 + 3 + 2];
    			if(d == 3)
    			{	nb_bandes = buffer_int[adresse_repertoire + i*12 + 7 + 2];
    				nb_bandes = nb_bandes + 256*buffer_int[adresse_repertoire + i*12 + 6 + 2];
    			}
    			else if(d == 4)
    			{	nb_bandes = buffer_int[adresse_repertoire + i*12 + 7 + 2];
    				nb_bandes = nb_bandes + 256*buffer_int[adresse_repertoire + i*12 + 6 + 2];
    				nb_bandes = nb_bandes + 256*256*buffer_int[adresse_repertoire + i*12 + 5 + 2];
    				nb_bandes = nb_bandes + 256*256*256*256*buffer_int[adresse_repertoire + i*12 + 4 + 2];
    			}
    			else
    			{	printf("pb!!!!: d != 3 ou 4\n");
    				exit(1);
    			}
    			adresse_273[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			//printf("adresse_273[0] = %d\n",adresse_273[0]);
    			adresse_273[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			//printf("adresse_273[1] = %d\n",adresse_273[1]);
    			adresse_273[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			//printf("adresse_273[2] = %d\n",adresse_273[2]);
    			adresse_273[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			//printf("adresse_273[3] = %d\n",adresse_273[3]);
    			statut++;
    		}
    		else if( (value == 1) && (value_2 == 6) )// parametre 262
    		{	e = buffer_int[adresse_repertoire + i*12 + 3 + 2];
    			adresse_262[0] = buffer_int[adresse_repertoire + i*12 + 8 + 2];
    			adresse_262[1] = buffer_int[adresse_repertoire + i*12 + 9 + 2];
    			adresse_262[2] = buffer_int[adresse_repertoire + i*12 + 10 + 2];
    			adresse_262[3] = buffer_int[adresse_repertoire + i*12 + 11 + 2];
    			//printf("adresse_273[0]=%d\n",adresse_273[0]);
	   			//printf("adresse_273[1]=%d\n",adresse_273[1]);
	   			statut++;
    		}
    		else if( (value == 1) && (value_2 == 2) )// parametre 258
    		{	f = buffer_int[adresse_repertoire + i*12 + 3 + 2];
    			adresse_258[0] = buffer_int[adresse_repertoire + i*12 + 8 + 1 + 2];
    			adresse_258[1] = buffer_int[adresse_repertoire + i*12 + 8 + 3 + 2];
    			// en fct de f on va lire l'un ou l'autre
    			statut++;
    		}
    	}
    	if(statut != 3)
    	{	printf("y'a un des parametres 258, 262 ou 273 qu'on a pas trouve !\n");
    	}
    	
    	if( d == 4 )
    	{	adresse_valeurs = adresse_273[3] + 16*16*adresse_273[2] + 16*16*16*16*adresse_273[1]
    		+ 16*16*16*16*16*16*16*16*adresse_273[0];
    	}
    	else if( d == 3)
    	{	adresse_valeurs = adresse_273[1] + 16*16*adresse_273[0];
    	}
    	if(nb_bandes == 1)
    	{	//nada, l'adresse donnee est bien celle des donnees
    	}
    	else// on cherche l'adresse de la premiere bande a l'adresse precedemment calculee
    	//on suppose que toutes les bandes sont a la suite
    	{	temp_int = buffer_int[adresse_valeurs+3] + 256*buffer_int[adresse_valeurs+2] +
    		256*256*buffer_int[adresse_valeurs+1] + 256*256*256*256*256*buffer_int[adresse_valeurs];
    		adresse_valeurs = temp_int;
    		// printf("nb_bandes = %d\n",nb_bandes);
    	}
    	
    	if( e == 4)
    	{	valeur_interpret_photo = adresse_262[3] + 16*16*adresse_262[2]
    		+ 16*16*16*16*adresse_262[1] + 16*16*16*16*16*16*16*16*adresse_262[0];
    	}
    	else if( e == 3)
    	{	valeur_interpret_photo = adresse_262[1] + 16*16*adresse_262[0];
    	}
    	if( f == 3)
    	{	nb_bits = adresse_258[0];
    	}
    	else if(f == 4)
    	{	nb_bits = adresse_258[1];
    	}
    	// printf("adresse des valeurs = %d\n",adresse_valeurs);
    	// printf("valeur_interpret_photo = %d\n",valeur_interpret_photo);
    }
    
    // on a les valeurs de l'intepret photo, l'adresse ou commencent les valeurs, et on sait
    //		sur combien de bits l'image est codee
    
    if(valeur_interpret_photo != 0)
    {	if(valeur_interpret_photo != 1)
    	{	printf("pb!!!!!!: valeur_interpret_photo != 0 ou 1\n");
    		exit(1);
    	}
    }       
   	
   	//exit(1);
   	
   	
   	if(nb_bits == 8)
   	{  	for(i=0;i<nbpts_y;i++)
    	{	for(j=0;j<nbpts_x;j++)
    		{	value = buffer_int[adresse_valeurs + j + (nbpts_y-1-i)*nbpts_x];
    			if(valeur_interpret_photo == 1)
    			{	tableau_result[i][j] = (double)(value);
    			}
    			else
    			{	tableau_result[i][j] = 255 - (double)(value);
    			}
			}
		}
	}
	
	else if(nb_bits == 16)
	{	for(i=0;i<nbpts_y;i++)
    	{	for(j=0;j<nbpts_x;j++)
    		{	if(buffer_int[0] == 73)
    			{	value_16[0] = buffer_int[adresse_valeurs + 2*j + 2*(nbpts_y-1-i)*nbpts_x + 1];
    				value_16[1] = buffer_int[adresse_valeurs + 2*j + 2*(nbpts_y-1-i)*nbpts_x];
    			}
    			else
    			{	value_16[0] = buffer_int[adresse_valeurs + 2*j + 2*(nbpts_y-1-i)*nbpts_x];
    				value_16[1] = buffer_int[adresse_valeurs + 2*j + 2*(nbpts_y-1-i)*nbpts_x + 1];
    			}
    			if(valeur_interpret_photo == 1)
    			{	tableau_result[i][j] = (double)(value_16[0])*256 + (double)(value_16[1]);
    			}
    			else
    			{	tableau_result[i][j] = 65535 - ((double)(value_16[0])*256 + (double)(value_16[1]));
    			}
			}
		}
	}
	
	else
	{	printf("pb!!!!!!: nb_bits != 8 ou 16\n");
	}
	
   	free(value_16);
	delete(buffer);
	delete(buffer_int);
	//free(buffer);
    //free(buffer_int);
    free(adresse_273);
	free(adresse_262);
}

//------------------------------------------------------------------------------------------
//		fct crea_tiff_4; tiree de crea_tiff_2, support de l'ecriture 16 bits
//		parametres: nbpts_x,y: nb de points selon x,y
//		resolution: resolution de la .tiff cree, en dpi (typ 300)
//		val_sat_haute, val_sat_basse: valeurs du tableau d'entree au-dela desquelles les
//			valeurs seront saturees
//		valeur_imag_max,valeur_imag_min: valeur max et min de l'image de sortie (full
//			dynamic: 255 / 0 ou 65535 / 0); si on veut imprimer les valeurs qu'on a obtenu
//			directement, sans toucher la dynamique (ex: tableau avec valeurs entre 1 et 45),
//			alors en valeur_imag_max,valeur_imag_min: arrondi_int(max) et arrondi_int(min)
//			; on aura ainsi comme l'appel a crea_tiff
//		donnees (en double), et nom du fichier
//		nb_bits: 8 ou 16: dit si on va coder sur 8 ou 16 bits
//------------------------------------------------------------------------------------------

void crea_tiff_4(int nbpts_x,int nbpts_y,int resolution,double val_sat_haute,double val_sat_basse,int valeur_imag_max,int valeur_imag_min,double **valeurs_points,char *nom_fichier_tiff,int nb_bits)
{	int i,j;
	FILE *txt;
	double intermed;
	int intermed_int;
	double dynamic = (double)(valeur_imag_max-valeur_imag_min);
		
	int *str = (int *) malloc (138* sizeof (int));
	int *hexa = (int *) malloc (4* sizeof (int));
	
	int temp1,temp2;
	
	if( valeur_imag_min < 0 )
	{	printf("pb!!!!!!: valeur_imag_min < 0\n");
	}
	if( nb_bits == 8)
	{	if(valeur_imag_max > 255)
		{	printf("pb !!!!!!: valeur_imag_max > 255\n");
		}
	}
	else if( nb_bits == 16)
	{	if(valeur_imag_max > 65535)
		{	printf("pb !!!!!!: valeur_imag_max > 65535\n");
		}
	}
	else
	{	printf("pb!!!!: nb_bits != 8 ou 16\n");
	}
	
	/* header */	
	str[0]=0x49; /* convention intel : ordre de lecture des octets */
	str[1]=0x49;
	str[2]=0x2A; /* version 42 */
	str[3]=0x00;
	str[4]=0x08; /* premier directory au byte n¡8 */
	str[5]=0x00;
	str[6]=0x00;
	str[7]=0x00;
	
	str[8]=0x09; /* nombre de parametres */
	str[9]=0x00;
	
	str[10]=0xFE; /* param 254: nouveau type de sous fichier */
	str[11]=0x00;
	str[12]=0x04; /* type entier 32 */
	str[13]=0x00;
	str[14]=0x01; /* nombre de longueurs de 2 octets pour la valeur */ 
	str[15]=0x00;
	str[16]=0x00;
	str[17]=0x00;
	str[18]=0x00; /* valeur = 0 */ 
	str[19]=0x00;
	str[20]=0x00;
	str[21]=0x00;
		
	hexadec(nbpts_x,hexa);
			
	str[22]=0x00; /* param 256: largeur image */
	str[23]=0x01;
	str[24]=0x04; /* type entier 32 */
	str[25]=0x00;
	str[26]=0x01; /* longueur de 2 octets */
	str[27]=0x00;
	str[28]=0x00; 
	str[29]=0x00;
	str[30]=hexa[3]; /* largeur de l'image en pixels */
	str[31]=hexa[2];
	str[32]=hexa[1];
	str[33]=hexa[0];
		
	hexadec(nbpts_y,hexa);
		
	str[34]=0x01; /* param 257: longueur (hauteur) image */
	str[35]=0x01;
	str[36]=0x04; /* entier 32 */
	str[37]=0x00;
	str[38]=0x01; /* longueur de 2 octets */
	str[39]=0x00;
	str[40]=0x00;
	str[41]=0x00;
	str[42]=hexa[3]; /* longueur de l'image */
	str[43]=hexa[2]; 
	str[44]=hexa[1];
	str[45]=hexa[0];
	
	str[46]=0x02; /* param 258 : nombre de bits par grain de l'image */
	str[47]=0x01;
	str[48]=0x03; /* entier */
	str[49]=0x00;
	str[50]=0x01; /* longueur de 2 octets */
	str[51]=0x00;
	str[52]=0x00;
	str[53]=0x00;
	if(nb_bits == 8)
	{	str[54]=0x08;
	}/* 2^8 niveaux de gris */
	else if(nb_bits == 16)
	{	str[54]=0x10;
	}/* 2^16 niveaux de gris */
	str[55]=0x00;
	str[56]=0x00;
	str[57]=0x00;	
	
	str[58]=0x06; /* param 262: mode d'interpretation photometrique */
	str[59]=0x01;
	str[60]=0x03; /* entier */
	str[61]=0x00;
	str[62]=0x01; /* longueur de 2 octets */
	str[63]=0x00;
	str[64]=0x00;
	str[65]=0x00;
	str[66]=0x01; /* 0-> noir et 1-> blanc */
	str[67]=0x00;
	str[68]=0x00;
	str[69]=0x00;
	
	str[70]=0x11; /* param 273 : pointeur vers les donnees de l'image */
	str[71]=0x01;
	str[72]=0x04; /* entier 32 */
	str[73]=0x00;
	str[74]=0x01; /* longueur de 2 octets */
	str[75]=0x00;
	str[76]=0x00;
	str[77]=0x00;
	str[78]=0x8A; /* adresse 138 */
	str[79]=0x00;
	str[80]=0x00;
	str[81]=0x00;
	
	str[82]=0x1A; /* param 282 : rezo en X */
	str[83]=0x01;
	str[84]=0x05; /* rationnel */
	str[85]=0x00;
	str[86]=0x01; /* longueur de 8 octets */
	str[87]=0x00;
	str[88]=0x00;
	str[89]=0x00;
	str[90]=0x7A; /* adresse 122 */
	str[91]=0x00;
	str[92]=0x00;
	str[93]=0x00;
	
	str[94]=0x1B; /* param 283 : rezo en Y */
	str[95]=0x01;
	str[96]=0x05; /* rationnel */
	str[97]=0x00;
	str[98]=0x01; /* longueur de 8 octets */
	str[99]=0x00;
	str[100]=0x00;
	str[101]=0x00;
	str[102]=0x82; /* adresse 130 */
	str[103]=0x00;
	str[104]=0x00;
	str[105]=0x00;	
	
	str[106]=0x1C; /* param 284 : type de config des plans */
	str[107]=0x01;
	str[108]=0x03; /* entier */
	str[109]=0x00;
	str[110]=0x01; /* longueur de 2 octets */
	str[111]=0x00;
	str[112]=0x00;
	str[113]=0x00;
	str[114]=0x01; /* ordre des pixels */
	str[115]=0x00;
	str[116]=0x00;
	str[117]=0x00;
	
	str[118]=0x00; /* pas de repertoire suivant */
	str[119]=0x00;	
	str[120]=0x00; 
	str[121]=0x00;
	
	hexadec(resolution,hexa);
	
	str[122]=hexa[3]; /* rezo x */
	str[123]=hexa[2];
	str[124]=hexa[1]; 
	str[125]=hexa[0];
	str[126]=0x01; 
	str[127]=0x00;
	str[128]=0x00; 
	str[129]=0x00;
	
	str[130]=hexa[3]; /* rezo y */
	str[131]=hexa[2];
	str[132]=hexa[1]; 
	str[133]=hexa[0];
	str[134]=0x01; 
	str[135]=0x00;
	str[136]=0x00; 
	str[137]=0x00;
	
	txt = fopen(nom_fichier_tiff,"wb+");
	// du fait d'ecrire en binaire on n'a plus besoin d'inverser les valeurs 10 et 13, et ca
	//	devrait aussi fctionner sur pc

/* PART2 : si on veut tester l'ecriture en hexadec de l'executable */	
	/*for(i=0;i<=255;i++)
	{	j = i;
		fprintf(txt,"%c",j);
	}//*/

/* PART3 : autres tests d'ecriture */
	/*fprintf(txt,"%c",255);
	fprintf(txt,"%c",10);
	fprintf(txt,"%c",(char)(255));
	fprintf(txt,"%c",(char)(10));
	fprintf(txt,"%c",65535);
	fprintf(txt,"%c",(char)(65535));
	fprintf(txt,"%c",0);
	fprintf(txt,"%c",(char)(0));
	fprintf(txt,"%c%c",65535);
	fprintf(txt,"%c%c",(char)(65535));
	fprintf(txt,"%c",0);
	fprintf(txt,"%c",(char)(0));
	fprintf(txt,"%c",65534/256);
	fprintf(txt,"%c",65534%256);
	*/
	
	for(i=0;i<=137;i++)
	{	fprintf(txt,"%c",str[i]);
	}
	
	for(i=nbpts_y-1;i>=0;i--)
	{	for(j=0;j<nbpts_x;j++)
		{	intermed = valeurs_points[i][j];
			if(intermed < val_sat_basse)
			{	intermed = val_sat_basse;
			}
			if(intermed > val_sat_haute)
			{	intermed = val_sat_haute;
			}
			intermed = valeur_imag_min + (intermed-val_sat_basse)/(val_sat_haute-val_sat_basse)*dynamic;
			intermed_int = arrondi_int(intermed);
			if(nb_bits == 8)
			{	fprintf(txt,"%c",intermed_int);
			}
			else if(nb_bits == 16)
			{	temp1 = intermed_int/256;
				temp2 = intermed_int%256;
				// on a les valeurs a ecrire
				fprintf(txt,"%c",temp2);
				fprintf(txt,"%c",temp1);
				// dans cet ordre car on est en conv intel
			}
		}
	}
	fclose(txt);
	
	free(str);
	free(hexa);
	
}		


//-------------------------------------------------------------------------------------
//		creation d'une image raw 32 bits
//		parametres: nbpts_x,y: nb de points selon x,y
//					val_sat_haute, val_sat_basse: valeurs du tableau d'entree au-dela desquelles les
//							valeurs seront saturees
//					valeur_imag_max,valeur_imag_min: valeur max et min de l'image de sortie (full
//							dynamic: 255 / 0 ou 65535 / 0 ou 4294967296 / 0); si on veut imprimer les valeurs qu'on a obtenu
//							directement, sans toucher la dynamique (ex: tableau avec valeurs entre 1 et 45),
//							alors en valeur_imag_max,valeur_imag_min: arrondi_int(max) et arrondi_int(min)
//					donnees (en double), et nom du fichier
//					nb_bits: 8, 16 ou 32: dit si on va coder sur 8, 16 ou 32 bits
//-------------------------------------------------------------------------------------

void crea_raw(int nbpts_x,int nbpts_y,double val_sat_haute,double val_sat_basse,long int valeur_imag_max,long int valeur_imag_min,double **valeurs_points,char *nom_fichier_raw,int nb_bits)
{	int i,j;
	FILE *txt;
	double intermed;
	int intermed_int;
	double dynamic = (double)(valeur_imag_max-valeur_imag_min);
	int *hexa = (int *) malloc (4* sizeof (int));
	int temp1,temp2;
	
	const int valeur_max_codage_32 = 4294967295;
	
	if( valeur_imag_min < 0 )
	{	printf("pb!!!!!!: valeur_imag_min < 0\n");
	}
	if( nb_bits == 8)
	{	if(valeur_imag_max > 255)
		{	printf("pb !!!!!!: valeur_imag_max > 255\n");
		}
	}
	else if( nb_bits == 16)
	{	if(valeur_imag_max > 65535)
		{	printf("pb !!!!!!: valeur_imag_max > 65535\n");
		}
	}
	else if( nb_bits == 32)
	{	if(valeur_imag_max > valeur_max_codage_32)
		{	printf("pb !!!!!!: valeur_imag_max > 4294967295\n");
		}
	}
	else
	{	printf("pb!!!!: nb_bits != 8 ou 16 ou 32\n");
	}
	
	txt = fopen(nom_fichier_raw,"wb+");
	
	for(i=nbpts_y-1;i>=0;i--)
	{	for(j=0;j<nbpts_x;j++)
		{	intermed = valeurs_points[i][j];
			if(intermed < val_sat_basse)
			{	intermed = val_sat_basse;
			}
			if(intermed > val_sat_haute)
			{	intermed = val_sat_haute;
			}
			intermed = valeur_imag_min + (intermed-val_sat_basse)/(val_sat_haute-val_sat_basse)*dynamic;
			intermed_int = arrondi_int(intermed);
			if(nb_bits == 8)
			{	fprintf(txt,"%c",intermed_int);
			}
			else if(nb_bits == 16)
			{	temp1 = intermed_int/256;
				temp2 = intermed_int%256;
				// on a les valeurs a ecrire
				fprintf(txt,"%c",temp2);
				fprintf(txt,"%c",temp1);
				// dans cet ordre car on est en conv intel
			}
			else if(nb_bits == 32)
			{	hexadec(intermed_int,hexa);
				fprintf(txt,"%c",hexa[0]);
				fprintf(txt,"%c",hexa[1]);
				fprintf(txt,"%c",hexa[2]);
				fprintf(txt,"%c",hexa[3]);
			}
		}
	}
	fclose(txt);
	
	free(hexa);
}

//--------------------------------------------------------------------------------------------------
//		creation d'une image tiff couleur
//		parametres: nbpts_x,y: nb de points selon x,y; les memes pour r,g et b
//				resolution: resolution de la .tiff cree, en dpi (typ 300)
//				*val_sat_haute, *val_sat_basse: valeurs du tableau d'entree au-dela desquelles les
//					valeurs seront saturees; vecteurs a 3 compos, dans l'ordre: r,g,b
//				*valeur_imag_max,*valeur_imag_min: valeur max et min de l'image de sortie (full
//					dynamic: 255 / 0 ou 65535 / 0); si on veut imprimer les valeurs qu'on a obtenu
//					directement, sans toucher la dynamique (ex: tableau avec valeurs entre 1 et 45),
//					alors en valeur_imag_max[k],valeur_imag_min[k]: arrondi_int(max) et arrondi_int(min)
//					pour chacune des 3 compos r,g,b (k = 0,1,2)
//				vecteurs a 3 compos, dans l'ordre: r,g,b
//				donnees (en double), selon un tableau[r,g puis b][i][j] et nom du fichier a creer
//				nb_bits: 8 ou 16: dit si on va coder sur 8 ou 16 bits/couche
//--------------------------------------------------------------------------------------------------

void crea_tiff_4_color(int nbpts_x,int nbpts_y,int resolution,double *val_sat_haute,double *val_sat_basse,int *valeur_imag_max,int *valeur_imag_min,double ***valeurs_points,char *nom_fichier_tiff,int nb_bits)
{	int i,j,k;
	FILE *txt;
	double intermed;
	int intermed_int;
	
	double *dynamic = (double*) malloc (3*sizeof(double));
	for(k=0;k<3;k++)
	{	dynamic[k] = (double)(valeur_imag_max[k]-valeur_imag_min[k]);
	}
	
	if( (nb_bits != 8) && (nb_bits != 16))
	{	printf("pb!!!!: nb_bits = %d\n",nb_bits);
		exit(1);
	}
	
	int *str = (int *) malloc (180* sizeof (int));
	int *hexa = (int *) malloc (4* sizeof (int));
	
	int temp1,temp2;
	
	for(k=0;k<3;k++)
	{	if( valeur_imag_min[k] < 0 )
		{	printf("pb!!!!!!: valeur_imag_min[%d] < 0\n",k);
		}
		if( nb_bits == 8)
		{	if(valeur_imag_max[k] > 255)
			{	printf("pb !!!!!!: valeur_imag_max[%d] > 255\n",k);
			}
		}
		else if( nb_bits == 16)
		{	if(valeur_imag_max[k] > 65535)
			{	printf("pb !!!!!!: valeur_imag_max[%d] > 65535\n",k);
			}
		}
		else
		{	printf("pb!!!!: nb_bits != 8 ou 16\n");
		}
	}
	
	/* header */	
	str[0]=0x49; /* convention intel : ordre de lecture des octets */
	str[1]=0x49;
	str[2]=0x2A; /* version 42 */
	str[3]=0x00;
	str[4]=0x08; /* premier directory au byte n¡8 */
	str[5]=0x00;
	str[6]=0x00;
	str[7]=0x00;
	
	str[8]=0x0C; /* nombre de parametres */
	str[9]=0x00;
	
	str[10]=0xFE; /* param 254: nouveau type de sous fichier */
	str[11]=0x00;
	str[12]=0x04; /* type entier 32 */
	str[13]=0x00;
	str[14]=0x01; /* nombre de longueurs de 2 octets pour la valeur */ 
	str[15]=0x00;
	str[16]=0x00;
	str[17]=0x00;
	str[18]=0x00; /* valeur = 0 */ 
	str[19]=0x00;
	str[20]=0x00;
	str[21]=0x00;
		
	hexadec(nbpts_x,hexa);
			
	str[22]=0x00; /* param 256: largeur image */
	str[23]=0x01;
	str[24]=0x04; /* type entier 32 */
	str[25]=0x00;
	str[26]=0x01; /* longueur de 2 octets */
	str[27]=0x00;
	str[28]=0x00; 
	str[29]=0x00;
	str[30]=hexa[3]; /* largeur de l'image en pixels */
	str[31]=hexa[2];
	str[32]=hexa[1];
	str[33]=hexa[0];
		
	hexadec(nbpts_y,hexa);
		
	str[34]=0x01; /* param 257: longueur (hauteur) image */
	str[35]=0x01;
	str[36]=0x04; /* entier 32 */
	str[37]=0x00;
	str[38]=0x01; /* longueur de 2 octets */
	str[39]=0x00;
	str[40]=0x00;
	str[41]=0x00;
	str[42]=hexa[3]; /* longueur de l'image */
	str[43]=hexa[2]; 
	str[44]=hexa[1];
	str[45]=hexa[0];
	
	str[46]=0x02; /* param 258 : nombre de bits par grain de l'image */
	str[47]=0x01;
	str[48]=0x03; /* entier */
	str[49]=0x00;
	str[50]=0x03; 
	str[51]=0x00;
	str[52]=0x00;
	str[53]=0x00;
	str[54]=0xAE;	// adresse 174
	str[55]=0x00;
	str[56]=0x00;
	str[57]=0x00;	
	
	str[58]=0x06; /* param 262: mode d'interpretation photometrique */
	str[59]=0x01;
	str[60]=0x03; /* entier */
	str[61]=0x00;
	str[62]=0x01; /* longueur de 2 octets */
	str[63]=0x00;
	str[64]=0x00;
	str[65]=0x00;
	str[66]=0x02; /* 0,0,0-> noir et maxi,maxi,maxi-> blanc */
	str[67]=0x00;
	str[68]=0x00;
	str[69]=0x00;
	
	str[70]=0x11; /* param 273 : pointeur vers les donnees de l'image */
	str[71]=0x01;
	str[72]=0x04; /* entier 32 */
	str[73]=0x00;
	str[74]=0x01; /* longueur de 2 octets */
	str[75]=0x00;
	str[76]=0x00;
	str[77]=0x00;
	str[78]=0xB4; /* adresse 180 */
	str[79]=0x00;
	str[80]=0x00;
	str[81]=0x00;
	
	str[82]=0x15; /* param 277: nb de grains de l'image /pixel */
	str[83]=0x01;
	str[84]=0x03; /* entier */
	str[85]=0x00;
	str[86]=0x01; /* longueur de 2 octets */
	str[87]=0x00;
	str[88]=0x00;
	str[89]=0x00;
	str[90]=0x03; /* 3 grains / pixel */
	str[91]=0x00;
	str[92]=0x00;
	str[93]=0x00;
	
	hexadec(nbpts_y,hexa);
	
	str[94]=0x16; /* param 278: nb de lignes/bande */
	str[95]=0x01;
	str[96]=0x04; /* entier */
	str[97]=0x00;
	str[98]=0x01; /* longueur de 2 octets */
	str[99]=0x00;
	str[100]=0x00;
	str[101]=0x00;
	str[102]=hexa[3]; /* longueur de l'image */
	str[103]=hexa[2];
	str[104]=hexa[1];
	str[105]=hexa[0];
	
	if(nb_bits == 8)
	{	hexadec(nbpts_y*nbpts_x*3,hexa);
	}
	else if(nb_bits == 16)
	{	hexadec(nbpts_y*nbpts_x*6,hexa);
	}
	
	str[106]=0x17; /* param 279: nb d'octets par bande */
	str[107]=0x01;
	str[108]=0x04; /* entier */
	str[109]=0x00;
	str[110]=0x01; /* longueur de 2 octets */
	str[111]=0x00;
	str[112]=0x00;
	str[113]=0x00;
	str[114]=hexa[3]; /* longueur de l'image */
	str[115]=hexa[2];
	str[116]=hexa[1];
	str[117]=hexa[0];
	
	str[118]=0x1A; /* param 282 : rezo en X */
	str[119]=0x01;
	str[120]=0x05; /* rationnel */
	str[121]=0x00;
	str[122]=0x01; /* longueur de 8 octets */
	str[123]=0x00;
	str[124]=0x00;
	str[125]=0x00;
	str[126]=0x9E; /* adresse 158 */
	str[127]=0x00;
	str[128]=0x00;
	str[129]=0x00;
	
	str[130]=0x1B; /* param 283 : rezo en Y */
	str[131]=0x01;
	str[132]=0x05; /* rationnel */
	str[133]=0x00;
	str[134]=0x01; /* longueur de 8 octets */
	str[135]=0x00;
	str[136]=0x00;
	str[137]=0x00;
	str[138]=0xA6; /* adresse 166 */
	str[139]=0x00;
	str[140]=0x00;
	str[141]=0x00;	
	
	str[142]=0x1C; /* param 284 : type de config des plans */
	str[143]=0x01;
	str[144]=0x03; /* entier */
	str[145]=0x00;
	str[146]=0x01; /* longueur de 2 octets */
	str[147]=0x00;
	str[148]=0x00;
	str[149]=0x00;
	str[150]=0x01; /* ordre des pixels: pour chaque pixel, on ecrit la valeur rouge puis la verte puis la bleue */
					// rq: j'ai essaye avec couche rouge puis verte puis bleu, rien a faire, ca voulait pas...
	str[151]=0x00;
	str[152]=0x00;
	str[153]=0x00;
	
	str[154]=0x00; /* pas de repertoire suivant */
	str[155]=0x00;	
	str[156]=0x00; 
	str[157]=0x00;
	
	hexadec(resolution,hexa);
	
	str[158]=hexa[3]; /* rezo x */
	str[159]=hexa[2];
	str[160]=hexa[1]; 
	str[161]=hexa[0];
	str[162]=0x01; 
	str[163]=0x00;
	str[164]=0x00; 
	str[165]=0x00;
	
	str[166]=hexa[3]; /* rezo y */
	str[167]=hexa[2];
	str[168]=hexa[1]; 
	str[169]=hexa[0];
	str[170]=0x01; 
	str[171]=0x00;
	str[172]=0x00; 
	str[173]=0x00;
	
	if(nb_bits == 8)
	{	str[174]=0x08;
		str[176]=0x08;
		str[178]=0x08;
	}// si chaque couche sur 8 bits
	else if(nb_bits == 16)
	{	str[174]=0x10;
		str[176]=0x10;
		str[178]=0x10;
	}// si chaque couche sur 16 bits
	str[175]=0x00;
	str[177]=0x00;
	str[179]=0x00;
	
	txt = fopen(nom_fichier_tiff,"wb+");
	
	for(i=0;i<=179;i++)
	{	fprintf(txt,"%c",str[i]);
	}
	
	for(i=nbpts_y-1;i>=0;i--)
	{	for(j=0;j<nbpts_x;j++)
		{	for(k=0;k<3;k++)
			{	intermed = valeurs_points[k][i][j];
				if(intermed < val_sat_basse[k])
				{	intermed = val_sat_basse[k];
				}
				if(intermed > val_sat_haute[k])
				{	intermed = val_sat_haute[k];
				}
				intermed = valeur_imag_min[k] + (intermed-val_sat_basse[k])/(val_sat_haute[k]-val_sat_basse[k])*dynamic[k];
				intermed_int = arrondi_int(intermed);
				if(nb_bits == 8)
				{	fprintf(txt,"%c",intermed_int);
				}
				else if(nb_bits == 16)
				{	temp1 = intermed_int/256;
					temp2 = intermed_int%256;
					// on a les valeurs a ecrire
					fprintf(txt,"%c",temp2);
					fprintf(txt,"%c",temp1);
					// dans cet ordre car on est en conv intel
				}
				else
				{	printf("pb!!!!!: nb_bits = %d\n",nb_bits);
					exit(1);
				}
			}
		}
	}
	fclose(txt);
	
	free(dynamic);
	free(str);
	free(hexa);
}		


