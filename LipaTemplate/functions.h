#include "lipa.h"
#include <iostream>
#include <math.h>

#define M_PI 3.14159265358979323846

static int  maxRecursionDepth = 100000;

//Struktura opisujaca typ segmentu
struct segmentTypes
{
	double R = 0;
	double G = 0;
	double B = 0;
	double malinowskaMin = 0;
	double malinowskaMax = 0;
	double circularityMin = 0;
	double circularityMax = 0;
};

//Struktura opisujaca deskryptory danego segmentu
struct descriptorsStruct
{
	int area = 0; // Pole segmentu
	int perimeter = 0; // Obwód segmentu
	double circularity = 0;
	double malinowska = 0;
	int segmentType = 0; // Typ segmentu
	int membership = 0; // Numer segmentu do którego nale¿y (Tylko w przypadku np. dzwonek, pik itp.)
};

//Struktura opisujaca kartê
struct cardsStruct
{
	int membership = 0; //Przynale¿noœæ do danego segmentu
	int spade = 0; // Pik
	int	diamond = 0; // Dzwonek
	int club = 0; // Trefl
	int value = 0;
};

// Konwersja obrazu kolorowego, na czarnobia³y
void rgbTogray(Image3CH& rgbImg, Image1CH& grayImg);

//Znajduje maksymaln¹ intensywnoœæ w obrazie
double findMax(Image1CH& InputGreyImg);

//Zwraca pozycjê max obszaru w tablicy
int findMaxArea(int *area, int segmentCount);

//Znajduje minimaln¹ intensywnoœæ w obrazie
double findMin(Image1CH& InputGreyImg);

//Obliczanie wartoœci progowania
double thresholdValue(Image1CH& InputImage);

// Progowanie obrazu
void threshold(Image1CH& InputGreyImg, Image1CH& OutputThresImage);

//Partycjonowanie do algorytmu Hoare'a
int partition(double * matrix, int a, int b);

//algorytm Hoare'a
double hoare(double * matrix, int size);

// Mediana obrazu
void mediana(Image1CH& InputGreyImg, Image1CH& MedianImage, int radius);

//Erozja
void erosion(Image1CH& InputBinImg, Image1CH& OutputBinImg, int radius);

//Dylatacja
void dilatation(Image1CH& InputBinImg, Image1CH& OutputBinImg, int radius);

//Operacja zamkniêcia
void close(Image1CH& InputBinImg, Image1CH& OututBinImg, int radius);

//Operacja otwarcia
void open(Image1CH& InputBinImg, Image1CH& OututBinImg, int radius);

//Funkcja rekurencyjna flaguj¹ca wszystkie pixele na danej wyspie
void segmentationMain(Image1CH& InputImage, Image3CH& OutputImage, int x, int y, bool previousColor, double flag, int recursionDepth);

//Wype³nia tablice polami i obwodami segmentów o odpowiadj¹ch im kolorze
void calcPerimeterAndArea(Image3CH& InputImage, int segmentCount, descriptorsStruct *descriptors);

//Znajduje piksel nieposegmentowanego fragmentu obrazu. Zwraca true, jeœli nie znajdzie ¿adnego.
bool findUnChecked(int& x, int&y, Image3CH& InputImage);

//Segmentuje obraz i zwraca iloœæ segmentów
int segment(Image1CH& InputBinImg, Image3CH& OutputImage);

//liczy wyznacznik okr¹g³oœci i malinowskiej
void calcDescriptors(int segmentCount, descriptorsStruct *descriptors);

//Rozpoznaje elementy na posegmentowanym obrazie
void recognize(Image3CH& InputSegmentedImg, Image3CH& ColourImage, Image3CH& RecognizedImage, int segmentCount, bool log);