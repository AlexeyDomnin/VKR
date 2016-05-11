string plate_cascade_name = "plate_cascade.xml";
CascadeClassifier plate_cascade;


// Функия для локализации номера методом Виолы-Джонса
vector<Mat> detect_plate(Mat image) {
	
	//загрузка каскада
	if (!plate_cascade.load(plate_cascade_name)) {
		printf("--(!)Error loading\n");
		system("pause");
		return -1;
	}
	//инициализируем вектор для хранения локализованных номеров
	vector<Mat> plates;

	//создаем дополнительные изображения
	Mat clone_img = image.clone();
	Mat gray_image(image.rows, image.cols, CV_8UC1);

	//перевод изображения в ттенки серого, применение фильтра размытия, детектора Канни и обрезка изображения
	cvtColor(image, gray_image, COLOR_BGR2GRAY);
	GaussianBlur(gray_image, gray_image, Size(9, 9), 0);
	Canny(gray_image, gray_image, 100, 200);
	Rect cutRectangle = cutedImage(gray_image, 3, 2);
	Mat gray_image_roi = clone_img(cutRectangle);

	// применение метода В-Д
	vector<Rect> potential_plate;
	plate_cascade.detectMultiScale(gray_image_roi, potential_plate, 1.1, 4, 0, Size(60, 20), Size(600, 200));
	for (size_t i = 0; i < potential_plate.size(); i++) {
		potential_plate[i].x += cutRectangle.x;
		potential_plate[i].y += cutRectangle.y;
		plates.push_back(clone_img(potential_plate[i]));
	}

	// возвращаем наполненный вектор локализованных номеров
	return plates;
}

// Метод для обрезки изображения (r - регулирует максимальную обрезку по бокам, t - регулирует максимальную обрезку сверху)
Rect cutedImage(Mat image, int r, int t) {
	
	// обрезка изображения слева
	vector<int> vector_left;
	for (int i = 0; i < image.rows; i++) {
		int counter = 0;
		for (int j = 0; j < image.cols / r; j++) {
			if (Scalar(image.at<uchar>(i, j))[0] == 255) {
				vector_left.push_back(j);
				break;
			}
		}
	}
	double delta_left = 0;
	for (int i = 0; i < vector_left.size(); i++) {
		delta_left += vector_left[i];
	}
	if (vector_left.size() != 0)
		delta_left /= vector_left.size();

	// обрезка изображения сверху
	vector<int> vector_up;
	for (int i = delta_left; i < image.cols; i++) {
		int counter = 0;
		for (int j = 0; j < image.rows / t; j++) {
			if (Scalar(image.at<uchar>(j, i))[0] == 255) {
				vector_up.push_back(j);
				break;
			}
		}
	}
	double delta_up = 0;
	for (int i = 0; i < vector_up.size(); i++) {
		delta_up += vector_up[i];
	}
	if (vector_up.size() != 0)
		delta_up /= vector_up.size();

	// обрезка изображения справа
	vector<int> vector_right;
	for (int i = delta_up; i < image.rows; i++) {
		int counter = 0;
		for (int j = image.cols - 1; j >= (image.cols*r) / r; j--) {
			if (Scalar(image.at<uchar>(i, j))[0] == 255) {
				vector_right.push_back(image.cols - j);
				break;
			}
		}
	}
	double delta_right = 0;
	for (int i = 0; i < vector_right.size(); i++) {
		delta_right += vector_right[i];
	}
	if (vector_right.size() != 0)
		delta_right /= vector_right.size();

	// обрезка изображения снизу
	vector<int> vector_down;
	vector_down.push_back(99999);
	for (int i = delta_left; i<image.cols - delta_right; i++) {
		int counter = 0;
		for (int j = image.rows - 1; j >= 0; j--) {
			if (Scalar(image.at<uchar>(j, i))[0] == 255) {
				if (vector_down[vector_down.size() - 1]>image.rows - j) {
					vector_down.push_back(image.rows - j);
				}
				break;
			}
		}
	}
	double delta_down = 0;
	if (vector_down.size() != 1) {
		if (vector_down[vector_down.size() - 1] >= 5) {
			delta_down = vector_down[vector_down.size() - 1] - 5;
		}
		else {
			delta_down = vector_down[vector_down.size() - 1];
		}
	}

	// получение координат полного прямоугольника, по которому обрезать изображение
	int x, y, w, h;
	x = delta_left;
	y = delta_up;
	w = image.cols - delta_left - delta_right;
	h = image.rows - delta_up - delta_down;

	// возвращаем прямоугольник
	return Rect(x, y, w, h);
}
