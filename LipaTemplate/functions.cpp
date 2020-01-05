#include "functions.h"

using namespace std;

// Konwersja obrazu kolorowego, na czarnobia³y
void rgbTogray(Image3CH& rgbImg, Image1CH& grayImg)
{
	//Check if image sizes are equal
	if (rgbImg.width() == grayImg.width() && rgbImg.height() == grayImg.height())
	{
		for (int i = 0; i < rgbImg.width(); i++) //iterate by rows
			for (int j = 0; j < rgbImg.height(); j++) //iterate by cols
			{
				grayImg(i, j).Intensity() = (rgbImg(i, j).Red() + rgbImg(i, j).Green() + rgbImg(i, j).Blue()) / 3; // I = (1/3)*(R+G+B) (i,j) - (number of row, number of col)
			}
	}
	else
	{
		cerr << "Image sizes mismatch" << endl; //print error
		return;
	}
}

//Znajduje maksymaln¹ intensywnoœæ w obrazie
double findMax(Image1CH& InputGreyImg)
{
	double max = 0;
	//Wybielanie obrazu
	for (int i = 0; i < InputGreyImg.width(); i++) //iterate by rows
		for (int j = 0; j < InputGreyImg.height(); j++) //iterate by cols
		{
			if (InputGreyImg(i, j).Intensity() > max)
				max = InputGreyImg(i, j).Intensity();
		}
	return max;
}

//Zwraca pozycjê max obszaru w tablicy
int findMaxArea(int *area, int segmentCount)
{
	int max = 0;
	int position = 0;
	for (int i = 0; i < segmentCount; i++)
	{
		if (area[i] > max)
		{
			max = area[i];
			position = i;
		}
	}
	return position;
}

//Znajduje minimaln¹ intensywnoœæ w obrazie
double findMin(Image1CH& InputGreyImg)
{
	double min = 1;
	//Wybielanie obrazu
	for (int i = 0; i < InputGreyImg.width(); i++) //iterate by rows
		for (int j = 0; j < InputGreyImg.height(); j++) //iterate by cols
		{
			if (InputGreyImg(i, j).Intensity() < min)
				min = InputGreyImg(i, j).Intensity();
		}
	return min;
}

//Obliczanie wartoœci progowania
double thresholdValue(Image1CH& InputImage)
{
	int *histogram = new int[256];
	int pochodna[256] = { 0 };
	int maxima[256] = { 0 };
	int tmp = 0, blackPeakLocation = 0, whitePeakLocation = 0;

	for (int i = 0; i < 255; i++)
		histogram[i] = 0;

	for (int i = 0; i < InputImage.width(); i++) //X
		for (int j = 0; j < InputImage.height(); j++) //Y
		{
			histogram[int(InputImage(i, j).Intensity() * 255)] ++;
		}

	//liczenie pochodnej
	for (int i = 0; i < 255; i++)
		pochodna[i] = histogram[i + 1] - histogram[i];


	//szukanie maximow lokalnych
	for (int i = 1; i < 256; i++)
	{
		if (pochodna[i - 1] > 0 && pochodna[i] < 0)
		{
			maxima[tmp] = histogram[i];
			tmp++;
		}
	}

	int *peakLocation = new int[tmp];

	int counter = 0;

	for (int j = 0; j < tmp; j++)
	{
		for (int i = 0; i < 255; i++)
		{
			if (histogram[i] == maxima[j])
			{
				peakLocation[counter] = i;
				counter++;
				break;
			}
		}
	}

	int blackPeakTmp = 0, whitePeakTmp = 0;
	for (int i = 0; i < tmp; i++)
	{
		//Black peak
		if (peakLocation[i] < 128)
		{
			if (histogram[peakLocation[i]] > blackPeakTmp)
			{
				blackPeakTmp = histogram[peakLocation[i]];
				blackPeakLocation = peakLocation[i];
			}
		}
		//White peak
		else
		{
			if (histogram[peakLocation[i]] > whitePeakTmp)
			{
				whitePeakTmp = histogram[peakLocation[i]];
				whitePeakLocation = peakLocation[i];
			}
		}
	}

	delete[]histogram;

	return (blackPeakLocation + whitePeakLocation) / 2.0 / 255.0;
}

// Progowanie obrazu
void threshold(Image1CH& InputGreyImg, Image1CH& OutputThresImage)
{
	//double threshold = thresholdValue(InputGreyImg);
	double threshold = (findMax(InputGreyImg) + findMin(InputGreyImg)) / 2;
	for (int i = 0; i < InputGreyImg.width(); i++) //iterate by rows
		for (int j = 0; j < InputGreyImg.height(); j++) //iterate by cols
		{
			if (InputGreyImg(i, j).Intensity() >= threshold)
				OutputThresImage(i, j).Intensity() = 1;
			if (InputGreyImg(i, j).Intensity() < threshold)
				OutputThresImage(i, j).Intensity() = 0;
		}
}

//Partycjonowanie do algorytmu Hoare'a
int partition(double * matrix, int a, int b)
{
	double e, tmp;
	e = matrix[a];        //elemennt dzielacy

	while (a<b)
	{
		while ((a<b) && (matrix[b] >= e))
			b--;
		while ((a<b) && (matrix[a]<e))
			a++;

		if (a<b)
		{
			tmp = matrix[a];
			matrix[a] = matrix[b];
			matrix[b] = tmp;
		}
	}
	return a;
}

//algorytm Hoare'a
double hoare(double * matrix, int size)
{
	int i = 0, k = 0;
	int j = size - 1;
	int w = (size + 1) / 2;

	while (i != j)
	{
		k = partition(matrix, i, j);
		k = k - i + 1;
		if (k >= w)
			j = i + k - 1;
		else
		{
			w -= k;
			i += k;
		}
	}
	return matrix[i];
}

// Mediana obrazu
void mediana(Image1CH& InputGreyImg, Image1CH& MedianImage, int radius)
{
	int matrix_counter = 0;
	int size = (radius * 2 + 1)*(radius * 2 + 1);
	double *matrix = new double[size];

	for (int i = radius; i < InputGreyImg.width() - radius; i++) //iterate by rows
	{
		for (int j = radius; j < InputGreyImg.height() - radius; j++) //iterate by cols
		{
			matrix_counter = 0;
			//Chodzenie po macierzy w obrazie
			for (int k = i - radius; k <= i + radius; k++)
			{
				for (int l = j - radius; l <= j + radius; l++)
				{
					matrix[matrix_counter] = InputGreyImg(k, l).Intensity();
					matrix_counter++;
				}
			}
			MedianImage(i, j).Intensity() = hoare(matrix, size);
		}
	}
	delete[] matrix;
}

//Erozja
void erosion(Image1CH& InputBinImg, Image1CH& OutputBinImg, int radius)
{
	for (int i = radius; i < InputBinImg.width() - radius; i++) //iterate by rows
	{
		for (int j = radius; j < InputBinImg.height() - radius; j++) //iterate by cols
		{
			if (InputBinImg(i, j).Intensity() == 1.0)
			{
				//Chodzenie po macierzy w obrazie
				for (int k = i - radius; k <= i + radius; k++)
				{
					for (int l = j - radius; l <= j + radius; l++)
					{
						OutputBinImg(k, l).Intensity() = 1;
					}
				}
			}

		}
	}
}

//Dylatacja
void dilatation(Image1CH& InputBinImg, Image1CH& OutputBinImg, int radius)
{
	//Wybielanie obrazu
	for (int i = 0; i < OutputBinImg.width(); i++) //iterate by rows
		for (int j = 0; j < OutputBinImg.height(); j++) //iterate by cols
		{
			OutputBinImg(i, j).Intensity() = 1;
		}

	for (int i = radius; i < InputBinImg.width() - radius; i++) //iterate by rows
	{
		for (int j = radius; j < InputBinImg.height() - radius; j++) //iterate by cols
		{
			if (InputBinImg(i, j).Intensity() == 0.0)
			{
				//Chodzenie po macierzy w obrazie
				for (int k = i - radius; k <= i + radius; k++)
				{
					for (int l = j - radius; l <= j + radius; l++)
					{
						OutputBinImg(k, l).Intensity() = 0.0;
					}
				}
			}
		}
	}
}

//Operacja zamkniêcia
void close(Image1CH& InputBinImg, Image1CH& OututBinImg, int radius)
{
	Image1CH TempBinImg(InputBinImg.width(), InputBinImg.height());
	dilatation(InputBinImg, TempBinImg, radius);
	erosion(TempBinImg, OututBinImg, radius);
}

//Operacja otwarcia
void open(Image1CH& InputBinImg, Image1CH& OututBinImg, int radius)
{
	Image1CH TempBinImg(InputBinImg.width(), InputBinImg.height());
	erosion(InputBinImg, TempBinImg, radius);
	dilatation(TempBinImg, OututBinImg, radius);
}

//Funkcja rekurencyjna flaguj¹ca wszystkie pixele na danej wyspie
void segmentationMain(Image1CH& InputImage, Image3CH& OutputImage, int x, int y, bool previousColor, double flag, int recursionDepth)
{
	//Ograniczenie G³êbokoœci rekurencji
	if (recursionDepth > maxRecursionDepth)
		return;
	if (x < 0 || x >= InputImage.width())
		return;
	if (y < 0 || y >= InputImage.height())
		return;
	if (OutputImage(x, y).B() == 1)
		return;

	bool color = bool(InputImage(x, y).Intensity());

	if (color == previousColor)
	{
		OutputImage(x, y).G() = flag;
		OutputImage(x, y).B() = 1;
	}
	else
		return;

	segmentationMain(InputImage, OutputImage, x, y - 1, previousColor, flag, recursionDepth++); // góra	
	segmentationMain(InputImage, OutputImage, x + 1, y, previousColor, flag, recursionDepth++); // prawo
	segmentationMain(InputImage, OutputImage, x, y + 1, previousColor, flag, recursionDepth++); // dó³
	segmentationMain(InputImage, OutputImage, x - 1, y, previousColor, flag, recursionDepth++); // lewo

																								//segmentationMain(InputImage, OutputImage, x + 1, y - 1, previousColor, flag, recursionDepth++); // prawo-góra	
																								//segmentationMain(InputImage, OutputImage, x + 1, y + 1, previousColor, flag, recursionDepth++); // prawo-dó³	
																								//segmentationMain(InputImage, OutputImage, x - 1, y + 1, previousColor, flag, recursionDepth++); // lewo-dó³	
																								//segmentationMain(InputImage, OutputImage, x - 1, y - 1, previousColor, flag, recursionDepth++); // lewo-góra
}

//Wype³nia tablice polami i obwodami segmentów o odpowiadj¹ch im kolorze
void calcPerimeterAndArea(Image3CH& InputImage, int segmentCount, descriptorsStruct *descriptors)
{
	// A pixel is part of the perimeter if it is nonzero and it is connected to at least one zero-valued pixel.
	double colorTmp = 0.0;
	for (int i = 0; i < InputImage.width(); i++) //X
		for (int j = 0; j < InputImage.height(); j++) //Y
		{
			for (int k = 0; k < segmentCount; k++)
			{
				colorTmp = k / 255.0;
				if (InputImage(i, j).G() == colorTmp)
				{
					descriptors[k].area++;
					if (i > 0 && i < InputImage.width() - 1 && j > 0 && j < InputImage.height() - 1)
						if (InputImage(i, j - 1).G() != colorTmp || InputImage(i + 1, j).G() != colorTmp || InputImage(i, j + 1).G() != colorTmp || InputImage(i - 1, j).G() != colorTmp ||
							InputImage(i + 1, j - 1).G() != colorTmp || InputImage(i + 1, j + 1).G() != colorTmp || InputImage(i - 1, j + 1).G() != colorTmp || InputImage(i - 1, j - 1).G() != colorTmp)
							descriptors[k].perimeter++;
				}
			}
		}
	return;
}

//Znajduje piksel nieposegmentowanego fragmentu obrazu. Zwraca true, jeœli nie znajdzie ¿adnego.
bool findUnChecked(int& x, int&y, Image3CH& InputImage)
{
	for (int i = 0; i < InputImage.width(); i++) //X
		for (int j = 0; j < InputImage.height(); j++) //Y
		{
			if (InputImage(i, j).B() != 1)
			{
				x = i;
				y = j;
				return false;
			}
		}
	return true;
}

//Segmentuje obraz i zwraca iloœæ segmentów
int segment(Image1CH& InputBinImg, Image3CH& OutputImage)
{
	/*
	Legenda:

	Kana³ Blue:
	Blue == 1 - zbadany obszar
	Blue == 0 - niezbadany obszar
	Kana³ Green:
	numer segmentu (max 255 segmentów);
	*/

	int x = 0, y = 0, i = 0;
	//Image1CH TempBinImg(InputBinImg.width(), InputBinImg.height());

	do
	{
		segmentationMain(InputBinImg, OutputImage, x, y, bool(InputBinImg(x, y).Intensity()), i / 255.0, 0);
		i++;
	} while (findUnChecked(x, y, OutputImage) == false);

	return i;
}

//liczy wyznacznik okr¹g³oœci i malinowskiej
void calcDescriptors(int segmentCount, descriptorsStruct *descriptors)
{
	for (int i = 0; i < segmentCount; i++)
	{
		descriptors[i].circularity = descriptors[i].perimeter / M_PI;
		descriptors[i].malinowska = (descriptors[i].perimeter / (2.0 * sqrt(M_PI*descriptors[i].area))) - 1;
	}
	return;
}

//Rozpoznaje elementy na posegmentowanym obrazie
void recognize(Image3CH& InputSegmentedImg, Image3CH& ColourImage, Image3CH& RecognizedImage, int segmentCount, bool log)
{
	/*
	Legenda:

	Kana³ Red:
	Numer wykrytego elementu

	Kana³ Blue:
	Po segmentacji:
	Blue == 1 - zbadany obszar
	Blue == 0 - niezbadany obszar
	Po rozpoznaniu:
	Blue == numer segmentu (karty) do którego nale¿y dana figura

	Kana³ Green:
	numer segmentu (max 255 segmentów);

	T³o - Pierwszy napotkany obszar - zawsze Green = 0;
	*/

	int segmentTypesCount = 5; //Iloœæ zdefiniowanych obiektów do wykrycia
	int segmentNumberTemp = 0;
	int cardNumberTemp = 0;
	int cards = 0; //Iloœæ wykrytych kart
	int card = 0; //Zmienna pomocnicza do

	descriptorsStruct *descriptors = new descriptorsStruct[segmentCount]; //Tablica struktur deskryptorów
	segmentTypes *segmentDeterminant = new segmentTypes[segmentTypesCount]; //Tablica struktur zawieraj¹ca kolory odpowiadaj¹ce segmentom oraz przedzia³y dla wyznaczników

																			//Liczy pole, obwód oraz wyznaczniki ka¿dego segmentu
	calcPerimeterAndArea(InputSegmentedImg, segmentCount, descriptors);
	calcDescriptors(segmentCount, descriptors);

#pragma region Definicje rozpoznawania
	//T³o - 0
	//Kolor
	segmentDeterminant[0].R = 0;
	segmentDeterminant[0].G = 0;
	segmentDeterminant[0].B = 0;

	//Determinant
	segmentDeterminant[0].malinowskaMin = 0;
	segmentDeterminant[0].malinowskaMax = 0;
	segmentDeterminant[0].circularityMin = 0;
	segmentDeterminant[0].circularityMax = 0;

	//Karta - 1
	//Kolor
	segmentDeterminant[1].R = 127.0 / 255.0;
	segmentDeterminant[1].G = 255.0 / 255.0;
	segmentDeterminant[1].B = 212.0 / 255.0;

	//Determinant
	segmentDeterminant[1].malinowskaMin = 0.40;
	segmentDeterminant[1].malinowskaMax = 2.40;
	segmentDeterminant[1].circularityMin = 700;
	segmentDeterminant[1].circularityMax = 1600;

	//Dzwonki - 2
	//Kolor
	segmentDeterminant[2].R = 255.0 / 255.0;
	segmentDeterminant[2].G = 0.0 / 255.0;
	segmentDeterminant[2].B = 0.0 / 255.0;

	//Determinant
	segmentDeterminant[2].malinowskaMin = 0.40;
	segmentDeterminant[2].malinowskaMax = 0.60;
	segmentDeterminant[2].circularityMin = 40;
	segmentDeterminant[2].circularityMax = 61;

	//Piki - 3
	//Kolor
	segmentDeterminant[3].R = 3.0 / 255.0;
	segmentDeterminant[3].G = 0.0 / 255.0;
	segmentDeterminant[3].B = 253.0 / 255.0;

	//Determinant
	segmentDeterminant[3].malinowskaMin = 0.46;
	segmentDeterminant[3].malinowskaMax = 0.76;
	segmentDeterminant[3].circularityMin = 61;
	segmentDeterminant[3].circularityMax = 86;

	//Trefle - 4
	//Kolor
	segmentDeterminant[4].R = 25.0 / 255.0;
	segmentDeterminant[4].G = 36.0 / 255.0;
	segmentDeterminant[4].B = 124.0 / 255.0;

	////Determinant
	segmentDeterminant[4].malinowskaMin = 0.50;
	segmentDeterminant[4].malinowskaMax = 0.74;
	segmentDeterminant[4].circularityMin = 86;
	segmentDeterminant[4].circularityMax = 100;
#pragma endregion 

	//Rozpoznawanie segmentów
	for (int i = 0; i < segmentCount; i++)
	{
		for (int k = 0; k < segmentTypesCount; k++)
		{
			if (descriptors[i].malinowska > segmentDeterminant[k].malinowskaMin & descriptors[i].malinowska < segmentDeterminant[k].malinowskaMax &
				descriptors[i].circularity > segmentDeterminant[k].circularityMin & descriptors[i].circularity < segmentDeterminant[k].circularityMax)
			{
				descriptors[i].segmentType = k;
			}

		}
	}

	//Malowanie rozpoznanych segmentów na okreœlony kolor, okreœlanie przynale¿noœci segmentu (na segmencie o jakim numerze, znajduje siê rozpoznany znak), odrzucanie b³êdnie rozpoznanych segmentów, które
	//maj¹ nieodpowiedni kolor.
	for (int i = 0; i < InputSegmentedImg.width(); i++) //X
		for (int j = 0; j < InputSegmentedImg.height(); j++) //Y
		{
			segmentNumberTemp = int(InputSegmentedImg(i, j).G() * 255);
			for (int k = 0; k < segmentTypesCount; k++)
			{
				if (descriptors[segmentNumberTemp].segmentType == k)
				{
					InputSegmentedImg(i, j).R() = k / 255.0;
					RecognizedImage(i, j).R() = segmentDeterminant[k].R;
					RecognizedImage(i, j).G() = segmentDeterminant[k].G;
					RecognizedImage(i, j).B() = segmentDeterminant[k].B;

					if (k > 1 && k < segmentTypesCount)
					{
						InputSegmentedImg(i, j).B() = cardNumberTemp / 255.0;
						descriptors[segmentNumberTemp].membership = cardNumberTemp;
					}
					//Jeœli dzwonek
					if (k == 2 && ColourImage(i, j).R() < (ColourImage(i, j).G() + ColourImage(i, j).B()) / 2)
					{
						descriptors[segmentNumberTemp].segmentType = 0;
					}

				}
			}
			//Jeœli aktualny piksel to karta
			if (descriptors[segmentNumberTemp].segmentType == 1)
			{
				cardNumberTemp = InputSegmentedImg(i, j).G() * 255;
			}
		}

	//Jeœli wybrany zosta³ podgl¹d, wypisuje dane segmentów w tabeli
	if (log)
	{
		//Wypisuje segmenty ich pola, obwody i wyznaczniki
		cout << endl << "Nr.\tPole:\t\tObwod:\t\tOkraglosc\tMalinowska:\tTyp Segmentu:\n";
		for (int i = 0; i < segmentCount; i++)
		{
			cout << i << "\t" << descriptors[i].area << "\t\t" << descriptors[i].perimeter << "\t\t" << descriptors[i].circularity << "\t\t"
				<< descriptors[i].malinowska << "\t\t" << descriptors[i].segmentType << endl;
		}
	}

	//Liczenie kart
	for (int i = 0; i < segmentCount; i++)
	{
		//Karta
		if (descriptors[i].segmentType == 1)
			cards++;
	}

	//Tworzenie tablicy struktór cardsStruct dla ka¿dej rozpoznanej karty
	cardsStruct *result = new cardsStruct[cards];

	//Przepisywanie ka¿dej wykrytej karcie numeru jej segmentu, do nowej tablicy struktur
	for (int i = 0; i < segmentCount; i++)
	{
		//Jeœli dany segment jest kart¹
		if (descriptors[i].segmentType == 1)
		{
			result[card].membership = i;
			card++;
		}
	}

	//Zlicza iloœæ znalezionych na karcie figur
	for (int k = 0; k < cards; k++)
	{
		for (int i = 0; i < segmentCount; i++)
		{
			//Jeœli aktualnie sprawdzany segment nale¿y do aktualnie sprawdzanej karty, to zlicza iloœæ znalezionych na karcie figur
			if (descriptors[i].membership == result[k].membership)
			{
				// Dzwonki
				if (descriptors[i].segmentType == 2)
					result[k].diamond++;

				// Pik
				if (descriptors[i].segmentType == 3)
					result[k].spade++;

				// Trefl
				if (descriptors[i].segmentType == 4)
					result[k].club++;
			}
		}
	}

	cout << "\n\nWyniki:\n\nIlosc kart: " << cards << "\n\nKarty to:\n\n";

	//Wypisuje znalezione karty
	for (int i = 0; i < cards; i++)
	{
		result[i].value = result[i].diamond + result[i].spade + result[i].club;
		//Dzwonek
		if (result[i].diamond > result[i].spade + result[i].club)
			cout << result[i].value << " Dzwonek\n";

		//Pik
		if (result[i].spade > result[i].diamond + result[i].club)
			cout << result[i].value << " Pik\n";

		//Trefl
		if (result[i].club > result[i].diamond + result[i].spade)
			cout << result[i].value << " Trefl\n";
	}

	double srednia = 0.0;
	card = 0;
	for (int i = 0; i < cards; i++)
	{
		if (result[i].value % 2 == 0)
		{
			srednia += result[i].value;
			card++;
		}
	}

	srednia = srednia / card;

	//Srednia arytmetyczna kart parzystych
	cout << "\nSrednia arytmetyczna kart parzystych: " << srednia;

	delete[]result;
	delete[]segmentDeterminant;
	delete[]descriptors;
}