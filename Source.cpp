#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <array>
using namespace cv;
using namespace std;

const int maskSize = 5; //основной размер маски
const int L5[] = { 1, 4, 6, 4, 1 }; 
const int E5[] = { -1, -2, 0, 2, 1 }; 
const int S5[] = { -1, 0, 2, 0, -1 };
const int R5[] = { 1, -4, 6, -4, 1 };

vector<int> createMask(const int* a, const int* b) { 
	vector<int> mask;
	for (int i = 0; i < maskSize; i++) {
		for (int j = 0; j < maskSize; j++)
			mask.push_back(a[i] * b[j]);
	}
	return mask;
}


float CalculateMean_dev(Mat &image, Mat &image2)
{
	Scalar Mean = 0, Dev = 0;
	double deviation = 0;
	uchar CurrentInt = 0;
	double MaxInt = 0;
	double MaxDev = 0;
	double totalDev = 0;
	float ArrOrderMeanFirst[2025];
	float ArrOrdenDevFirst[2025];
	float ArrOrderMeanSecond[2025];
	float ArrOrdenDevSecond[2025];
	float Eps = 0;
	int z = 0;
		for (int y = 0; y < image.rows; y += image.rows / 45)
		{
			for (int x = 0; x < image.cols; x += image.cols / 45) 
			{	
				Rect square(x, y, image.rows / 45, image.cols / 45);
				meanStdDev(image(square), Mean, Dev);
				
				for (int k = y; k < y + 10; k++) {
					for (int n = x; n < x + 10; n++) {

						CurrentInt = image.at<uchar>(k, n);
					    if ((int)CurrentInt > MaxInt) MaxInt = (int)CurrentInt;
						
						deviation = abs((int)image.at<uchar>(k, n) - Mean[0]);
						if (deviation > MaxDev) MaxDev = deviation;			
					}
				}
				
				if (MaxInt != 0) ArrOrderMeanFirst[z] = Mean[0] / MaxInt;
				else ArrOrderMeanFirst[z] = 0;
				if (MaxDev != 0) ArrOrdenDevFirst[z] = Dev[0] / MaxDev;
				else ArrOrdenDevFirst[z] = 0;
				z++;
				MaxInt = 0;	
				MaxDev = 0;
			}
		}
		z = 0;
		


		/*2222222222222222222222222222*/
		for (int y = 0; y < image2.rows; y += image2.rows / 45)
		{
			for (int x = 0; x < image2.cols; x += image2.cols / 45)
			{

				Rect square(x, y, image2.rows / 45, image2.cols / 45);
				meanStdDev(image2(square), Mean, Dev);

				for (int k = y; k < y + 10; k++) {
					for (int n = x; n < x + 10; n++) {

						CurrentInt = image2.at<uchar>(k, n);
						if ((int)CurrentInt > MaxInt) MaxInt = (int)CurrentInt;

						deviation = abs((int)image.at<uchar>(k, n) - Mean[0]);
						if (deviation > MaxDev) MaxDev = deviation;
					}
				}

				if (MaxInt != 0) ArrOrderMeanSecond[z] = Mean[0] / MaxInt;
				else ArrOrderMeanSecond[z] = 0;
				if (MaxDev != 0) ArrOrdenDevSecond[z] = Dev[0] / MaxDev;
				else ArrOrdenDevSecond[z] = 0;
				z++;
				MaxInt = 0;
				MaxDev = 0;
			}
		}

		for (int i = 0; i < 2025; i++) {
			Eps += sqrt(pow(ArrOrderMeanFirst[i] - ArrOrderMeanSecond[i] , 2) + pow(ArrOrdenDevFirst[i] - ArrOrdenDevSecond[i], 2));
		}
		Eps /= 45;
		return Eps;
}



int main(int argc, char* argv[])
{
	float AvgEps = 0;
	setlocale(LC_ALL, "Russian");
	Mat image, imageBlur, blura, image2, blura2;
	
	Mat filteredImage[30]; // массив для изображений, подвергнутых очередной маске
	string FirstPictureName;
	string SecondPictureName;
	cout << "Введите название первого изображения:" << "                " << "Доступные изображения: " << "bird.jpg, car.jpg, f1.jpg, face.jpg, " <<
		endl << "                                                     forest1.jpg, forest2.jpg, islang.jpg, moto.jpeg, " << endl << 
		"                                                     rock.jpg, src.jpg, sun1.jpg, zebra.jpg." << endl;
	
	cin >> FirstPictureName;
	image = imread(FirstPictureName, IMREAD_GRAYSCALE); 
	
	cout << "Введите название второго изображения:" << endl;
	cin >> SecondPictureName;
	image2 = imread(SecondPictureName, IMREAD_GRAYSCALE);
	imshow("Source1", image);
	imshow("Source2", image2);
	
	blur(image, blura, Size(5, 5)); // получение сглаженного изображения
	blur(image2, blura2, Size(5, 5)); // получение сглаженного изображения
	
	

	vector<int> mask = createMask(L5, E5); // получение маски, используя определенные вектора Лавса
	filter2D(blura, filteredImage[0], -1, mask); // применение маски 
	filter2D(blura2, filteredImage[1], -1, mask); // 2
	mask = createMask(E5, L5);
	filter2D(blura, filteredImage[2], -1, mask);
	filter2D(blura2, filteredImage[3], -1, mask);
	add(filteredImage[0], filteredImage[2], filteredImage[2]);
	imshow("1 изображение (L5E5/E5L5)", filteredImage[2]);
	add(filteredImage[1], filteredImage[3], filteredImage[3]);
	imshow("2 изображение (L5E5/E5L5)", filteredImage[3]);
	AvgEps += CalculateMean_dev(filteredImage[2], filteredImage[3]);
	

	mask = createMask(L5, R5);
	filter2D(blura, filteredImage[4], -1, mask);
	filter2D(blura2, filteredImage[5], -1, mask);
	mask = createMask(R5, L5);
	filter2D(blura, filteredImage[6], -1, mask);
	filter2D(blura2, filteredImage[7], -1, mask);
	add(filteredImage[4], filteredImage[6], filteredImage[6]);
	imshow("1 изображение (L5R5/R5L5)", filteredImage[6]);
	add(filteredImage[5], filteredImage[7], filteredImage[7]);
	imshow("2 изображение (L5R5/R5L5)", filteredImage[7]);
	AvgEps += CalculateMean_dev(filteredImage[6], filteredImage[7]);

	mask = createMask(E5, S5);
	filter2D(blura, filteredImage[8], -1, mask);
	filter2D(blura2, filteredImage[9], -1, mask);
	mask = createMask(S5, E5);
	filter2D(blura, filteredImage[10], -1, mask);
	filter2D(blura2, filteredImage[11], -1, mask);
	add(filteredImage[8], filteredImage[10], filteredImage[10]);
	imshow("1 изображение (E5S5/S5E5)", filteredImage[10]);
	add(filteredImage[9], filteredImage[11], filteredImage[11]);
	imshow("2 изображение (E5S5/S5E5)", filteredImage[11]);
	AvgEps += CalculateMean_dev(filteredImage[10], filteredImage[11]);
	
	

	mask = createMask(S5, S5);
	filter2D(blura, filteredImage[12], -1, mask);
	filter2D(blura2, filteredImage[13], -1, mask);
	AvgEps += CalculateMean_dev(filteredImage[12], filteredImage[13]);
	imshow("1 изображение (S5S5/S5S5)", filteredImage[12]);
	imshow("2 изображение (S5S5/S5S5)", filteredImage[13]);


	mask = createMask(R5, R5);
	filter2D(blura, filteredImage[14], -1, mask);
	filter2D(blura2, filteredImage[15], -1, mask);
	AvgEps += CalculateMean_dev(filteredImage[14], filteredImage[15]);
	imshow("1 изображение (R5R5/R5R5)", filteredImage[14]);
	imshow("2 изображение (R5R5/R5R5)", filteredImage[15]);

	mask = createMask(L5, S5);
	filter2D(blura, filteredImage[16], -1, mask);
	filter2D(blura2, filteredImage[17], -1, mask);
	mask = createMask(S5, L5);
	filter2D(blura, filteredImage[18], -1, mask);
	filter2D(blura2, filteredImage[19], -1, mask);
	add(filteredImage[16], filteredImage[18], filteredImage[18]);
	imshow("1 изображение (L5S5/S5L5)", filteredImage[18]);
	add(filteredImage[17], filteredImage[19], filteredImage[19]);
	imshow("2 изображение (L5S5/S5L5)", filteredImage[19]);
	AvgEps += CalculateMean_dev(filteredImage[18], filteredImage[19]);
	

	mask = createMask(E5, E5);
	filter2D(blura, filteredImage[20], -1, mask);
	filter2D(blura2, filteredImage[21], -1, mask);
	AvgEps += CalculateMean_dev(filteredImage[20], filteredImage[21]);
	imshow("1 изображение (E5E5/E5E5)", filteredImage[20]);
	imshow("2 изображение (E5E5/E5E5)", filteredImage[21]);
	
	mask = createMask(E5, R5);
	filter2D(blura, filteredImage[22], -1, mask);
	filter2D(blura2, filteredImage[23], -1, mask);
	mask = createMask(R5, E5);
	filter2D(blura, filteredImage[24], -1, mask);
	filter2D(blura2, filteredImage[25], -1, mask);
	add(filteredImage[22], filteredImage[24], filteredImage[24]);
	imshow("1 изображение (R5E5/E5R5)", filteredImage[24]);
	add(filteredImage[23], filteredImage[25], filteredImage[25]);
	imshow("2 изображение (R5E5/E5R5)", filteredImage[25]);
	AvgEps += CalculateMean_dev(filteredImage[24], filteredImage[25]);
	
	mask = createMask(S5, R5);
	filter2D(blura, filteredImage[26], -1, mask);
	filter2D(blura2, filteredImage[27], -1, mask);
	mask = createMask(R5, S5);
	filter2D(blura, filteredImage[28], -1, mask);
	filter2D(blura2, filteredImage[29], -1, mask);
	add(filteredImage[26], filteredImage[28], filteredImage[28]);
	imshow("1 изображение (R5S5/S5R5)", filteredImage[28]);
	add(filteredImage[27], filteredImage[29], filteredImage[29]);
	imshow("2 изображение (R5S5/S5R5)", filteredImage[29]);
	AvgEps += CalculateMean_dev(filteredImage[28], filteredImage[29]);
	
	cout << endl << "Средняя ошибка:" << AvgEps / 9;
	waitKey(0);
	destroyAllWindows();
	return 0;
}