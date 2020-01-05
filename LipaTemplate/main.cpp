#include "functions.h"

//LipaLib - Learning Image Processing Autonomic Library

using namespace std;

int main()
{
	int ImgWidth = 1928;
	int ImgHeight = 1448;

	int choose = 0;
	int radius = 1; //promieñ filtracji medianowej

	char podglad = 0;
	bool log = false;
	int segmentCount = 0; //Iloœæ segmentów

	Image3CH ColourImage(ImgWidth, ImgHeight); // Create new image Image3CH - three channels image (width,height)
	Image1CH GrayImage(ImgWidth, ImgHeight);
	Image1CH ThresImage(ImgWidth, ImgHeight);
	Image1CH MedianImage(ImgWidth, ImgHeight);
	Image1CH ClosedImage(ImgWidth, ImgHeight);
	Image3CH SegmentedImage(ImgWidth, ImgHeight);
	Image3CH SegmentedImage2(ImgWidth, ImgHeight);
	Image3CH RecognizedImage(ImgWidth, ImgHeight);

	//Menu
	cout << "Wybierz obraz:\n1 - Ideal\n2 - Noised\n3 - Blurred\n4 - Gradient\n";
	
	for (;;)
	{
		cin >> choose;
		if (choose < 1 || choose > 4)
			cout << "Zly wybor\n";
		else
			break;
	}

	cout << "Wlaczyc podglad? (T/N)\n";
	for (;;)
	{
		cin >> podglad;

		if (podglad == 'T' || podglad == 't')
		{
			log = true;
			break;
		}
		else
			if (podglad == 'N' || podglad == 'n')
			{
				log = false;
				break;
			}
			else
				cout << "Zly wybor\n";
	}

	//Za³adowanie badanego obrazu
	switch (choose)
	{
	case 1:
		ColourImage.LoadImage("img\\ideal.png", LPL_LOAD_FITTED);
		break;
	case 2:
		ColourImage.LoadImage("img\\noised.png", LPL_LOAD_FITTED);
		break;
	case 3:
		ColourImage.LoadImage("img\\blurred.png", LPL_LOAD_FITTED);
		break;
	case 4:
		ColourImage.LoadImage("img\\gradient.png", LPL_LOAD_FITTED);
		break;
	}
	
	cout << "\n\nKonwersja na skale szarosci";
	rgbTogray(ColourImage, GrayImage);
	cout << "\tZrobione.";
	cout << "\n\nFiltracja medianowa.";
	mediana(GrayImage, MedianImage, radius);
	cout << "\tZrobione.";
	cout << "\n\nProgowanie.";
	threshold(MedianImage, ThresImage);
	cout << "\tZrobione.";
	cout << "\n\nOperacja zamkniecia.";
	close(ThresImage, ClosedImage, 3);
	cout << "\tZrobione.";
	cout << "\n\nSegmentacja.\t";
	segmentCount = segment(ClosedImage, SegmentedImage);
	cout << "Ilosc segmentow: " << segmentCount << "\tZrobione.";

	//Kopia obrazu
	for (int i = 0; i < SegmentedImage.width(); i++) //X
		for (int j = 0; j < SegmentedImage.height(); j++) //Y
		{
			SegmentedImage2(i, j).R() = SegmentedImage(i, j).R();
			SegmentedImage2(i, j).G() = SegmentedImage(i, j).G();
			SegmentedImage2(i, j).B() = SegmentedImage(i, j).B();
		}

	cout << "\n\nRozpoznawanie elementow.";
	recognize(SegmentedImage, ColourImage, RecognizedImage, segmentCount, log);
	cout << "\nZrobione.\n\nWpisz dowolny znak, aby zakonczyc.\n";

	//Wyswietlanie poszczegolnych etapow dzia³ania
	if (log)
	{
		ColourImage.ShowImage("Color");
		GrayImage.ShowImage("Gray");
		MedianImage.ShowImage("Median");
		ThresImage.ShowImage("Threshold");
		ClosedImage.ShowImage("Closed");
		SegmentedImage2.ShowImage("Segmented");
		SegmentedImage.ShowImage("Segmented After Recognize");
		RecognizedImage.ShowImage("Recognized");
	}

	cin >> choose;

	return 0;
}


