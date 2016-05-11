// Функия для локализации нмоера методом контурного анализа
vector<Mat> detect_plate_c(Mat image) {

	//инициализируем вектор для хранения локализованных номеров
	vector<Mat> plates;

	// инициализируем матрицу для повышения четкости изображения
	float kernel[9];
	kernel[0] = -0.1;
	kernel[1] = -0.1;
	kernel[2] = -0.1;
	kernel[3] = -0.1;
	kernel[4] = 2;
	kernel[5] = -0.1;
	kernel[6] = -0.1;
	kernel[7] = -0.1;
	kernel[8] = -0.1;

	// создаем матрицу
	CvMat kernel_matrix = cvMat(3, 3, CV_32FC1, kernel);

	// накладываем фильтр для увеличения четкости
	cvFilter2D(image, image, &kernel_matrix, cvPoint(-1, -1));

	// создаём одноканальные картинки
	gray = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	bin = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	contour_work = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

	// переводим изображения в оттенки серого
	cvCvtColor(image, gray, CV_RGB2GRAY);

	// применяем детектор границ Канни
	Canny(gray, contour_work, 50, 150, 3);

	// Операции наращивания и эрозии
	dilate(contour_work, contour_work, NULL, 1);
	erode(contour_work, contour_work, NULL, 1);

	// инициализируем память и пространство для хранения контуров
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contours = 0;

	// получаем численные представления конутров (прямоугольники с контурами)
	cvFindContours(contour_work, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	// процелура отсеивания ненужных контуров
	for (CvSeq* seq0 = contours; seq0 != 0; seq0 = seq0->h_next) {
		double area = fabs(cvContourArea(seq0));
		double perimetr = cvContourPerimeter(seq0);

		if (double(area / (perimetr*perimetr) > 27 && double(area / (perimetr*perimetr) < 28){
			CvRect gosnomer;
			CvPoint pt1, pt2;
			gosnomer = cvBoundingRect(seq0, NULL);
			pt1.x = gosnomer.x;
			pt2.x = (gosnomer.x + gosnomer.width);
			pt1.y = gosnomer.y;
			pt2.y = (gosnomer.y + gosnomer.height);
			double ratio = double(double(gosnomer.width) / double(gosnomer.height));


			if ((2.0 < fabs(ratio) && fabs(ratio) <= 6.0)) {

				plates.push_back(image(gosnomer));
			}

		}
	}

	//Возвращаем заполненый вектор локализованных номеров
	return plates;
}
