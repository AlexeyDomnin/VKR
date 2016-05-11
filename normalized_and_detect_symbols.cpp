string letter_cascade_name = "letter_cascade.xml";
CascadeClassifier letter_cascade;
string digit_cascade_name = "digit_cascade.xml";
CascadeClassifier digit_cascade;


//фунция для поворота изображения в горизонтальную ориентацию
Mat normalized_image(Mat image) {

	// создаем матрицы для хранения изображений
	Mat bin_plate;
	Mat gray_plate = image.clone();

	//бинаризация
	threshold(gray_plate, bin_plate, 150, 255);

	//детектор Канни
	Canny(bin_plate, bin_plate, 100, 150);

	//Обрезка изображения
	Rect rect = cutedImage(bin_plate, 10, 9);
	bin_plate = bin_plate(rect);
	gray_plate = gray_plate(rect);
	image = image(rect);

	//Наращивание
	dilate(bin_plate, bin_plate, Mat());

	//Поиск прямых линий на изображении
	vector<Vec2f> lines;
	vector<Vec2f> chosen_lines;
	HoughLines(bin_plate, lines, 1, CV_PI / 180, 100, 0, 0);

	//Фильтрация линий
	if (lines.size() >= 2) {
		float rho = lines[0][0], theta = lines[0][1];
		chosen_lines.push_back(lines[0]);
		for (int i = 1; i < lines.size(); i++)
		{
			if ((abs(lines[i][0] - rho) < 40) || lines[i][1] > 1.58 || lines[i][1] < 1.56) {
				continue;
			}
			else {
				rho = lines[i][0];
				theta = lines[i][1];
				chosen_lines.push_back(lines[i]);
				Point pt1, pt2;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;
				pt1.x = cvRound(x0 + 1000 * (-b));
				pt1.y = cvRound(y0 + 1000 * (a));
				pt2.x = cvRound(x0 - 1000 * (-b));
				pt2.y = cvRound(y0 - 1000 * (a));
				line(color_plate, pt1, pt2, Scalar(0, 255, 0), 1, CV_AA);
			}

		}
	}

	//Получение углов прямых
	int top_line = -99999;
	double top_angle = 1.5708;
	for (int i = 0; i < chosen_lines.size(); i++) {
		if ((chosen_lines[i][0]<gray_plate.rows / 2) && (chosen_lines[i][0]>top_line)) {
			top_line = chosen_lines[i][0];
			top_angle = chosen_lines[i][1];
		}
	}
	int bottom_line = 99999;
	double bottom_angle = 1.5708;
	for (int i = 0; i < chosen_lines.size(); i++) {
		if (chosen_lines[i][0]>gray_plate.rows / 2 && chosen_lines[i][0] < bottom_line) {
			bottom_line = chosen_lines[i][0];
			bottom_angle = chosen_lines[i][1];
		}
	}

	//составление матрицы поворота
	Point2f pt(gray_plate.cols / 2., gray_plate.rows / 2.);
	Mat r;
	if (top_angle != 1.5708)
		r = getRotationMatrix2D(pt, -(90 - (top_angle * 180 / CV_PI)), 1.0);
	else
		r = getRotationMatrix2D(pt, -(90 - (bottom_angle * 180 / CV_PI)), 1.0);

	//поворот изображения
	warpAffine(gray_plate, gray_plate, r, Size(gray_plate.cols, gray_plate.rows));
	return gray_plate;
}

//буквы с номерной пластины
vector<Mat> letters_for_neural_network(Mat image) {

	//загрузка каскада
	if (!letter_cascade.load(letter_cascade_name)) {
		printf("--(!)Error loading\n");
		system("pause");
		return -1;
	}
	//поиск букв методом В-Д
	vector<Rect> letters;
	letter_cascade.detectMultiScale(image, letters, 1.1, 3, 0, Size(20, 30), Size(45,60))
	letters = sort(letters);
	vector<Rect> letters_fnn;
	resize(image, image, Size(250, 90));
	int count_of_letters = 0;
	//Фильтрация найденных букв
	for (int i = 0; i < letters.size(); i++) {

		if (letters[i].x < 35 || letters[i].x > 110) {

			if (letters[i].x < 170) {

				if (letters[i].y < 25) {

					if (letters_fnn.size() == 0) {

						if (letters[i].height* letters[i].width < 2500)
							letters_fnn.push_back(letters[i]);
						else
							continue;
					}
					else {
						if (abs(letters[i].x - letters_fnn[letters_fnn.size() - 1].x) < 10 || letters[i].height* letters[i].width > 2500)
							continue;
						else {
							letters_fnn.push_back(letters[i]);
							count_of_letters++;
							if (count_of_letters == 3)
								break;
						}
					}
				}
			}
		}
	}
	
	for (int i = 0; i < letters_fnn.size(); i++) {
		letters_fnn[i].height = 45;
		letters_fnn[i].width = 30;
	}

	//обрезка найденных букв
	vector<Mat> output;
	for (int i = 0; i &lt; letters_fnn.size(); i++) {
		output.push_back(image(letter_fnn[i]));
		resize(output[i], output[i], Size(20, 25));
	}
	//возвращение букв
	return output;
}

//цифры с номерной пластины
vector<Mat> digits_for_neural_network(Mat image) {
	//загрузка каскада
	if (!digit_cascade.load(digit_cascade_name)) {
		printf("--(!)Error loading\n");
		system("pause");
		return -1;
	}
	//поиск цифр методов Виолы-Джонса
	vector<Rect> digits;
	digit_cascade.detectMultiScale(image, digits, 1.03, 2, 0, Size(20,30), Size(90, 120));

	//фильтрация найденных цифр
	vector<Rect> sort_digits = sort(digits);
	vector<Rect> digits_fnm;
	vector<Rect> region_fnm;
	int count_of_digits = 0;
	for (int i = 0; i < sort_digits.size(); i++) {
		if (sort_digits[i].x >= 35) {
			if (sort_digits[i].x < 110) {
				if (sort_digits[i].y < 25) {
					if (digits_fnm.size() == 0) {
						if (sort_digits[i].height*sort_digits[i].width < 2500)
							digits_fnm.push_back(sort_digits[i]);
						else
							continue;
					}

					else {
						if (abs(sort_digits[i].x - digits_fnm[digits_fnm.size() - 1].x) < 10 || sort_digits[i].height*sort_digits[i].width>2500)
							continue;
						else {
							digits_fnm.push_back(sort_digits[i]);
							count_of_digits++;
							if (count_of_digits == 3)
								break;
						}
					}
				}
			}		
		}
	}
	
	for (int i = 0; i < digits_fnm.size(); i++) {
		digits_fnm[i].height = 50;
		digits_fnm[i].width = 30;
	}
	//фильтрация региона
	int count_of_region = 0;
	for (int i = sort_digits.size() - 1; i >= 0; i--) {
		if (sort_digits[i].x > 170) {
			if (sort_digits[i].y < 30) {
				if (sort_digits[i].y >= 5) {
					if (region_fnm.size() == 0) {
						region_fnm.push_back(sort_digits[i]);
					}
					else {
						if (abs(sort_digits[i].x - region_fnm[region_fnm.size() - 1].x) < 10 || sort_digits[i].height*sort_digits[i].width>2000)
							continue;
						else {
							sort_digits[i].width = 25;
							sort_digits[i].height = 40;
							region_fnm.push_back(sort_digits[i]);
							count_of_region++;
							if (count_of_region == 3)
								break;
						}
					}
				}
			}
		}
	}
	
	region_fnm = sort(region_fnm);
	for (int i = 0; i < region_fnm.size(); i++) {
		region_fnm[i].height = 35;
		region_fnm[i].width = 20;
	}

	//обрезка найденных цифр
	vector<Mat> output;
	for (size_t i = 0; i < digits_fnm.size(); i++)	{
		output.push_back(image(digits_fnm[i]));
		resize(output[i], output[i], Size(20, 25));
	}

	for (size_t i = 0; i < region_fnm.size(); i++){
		output.push_back(image(region_fnm[i]));
		resize(output[i + digits_fnm.size()], output[i + digits_fnm.size()],Size(20, 25));
	}

	//возвращение цифр
	return output;
}
