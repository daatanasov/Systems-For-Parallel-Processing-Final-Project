#include<iostream>
#include"Bitmap.h"
#include<string>
#include<complex>
#include<cmath>
#include<thread>
#include<vector>
#include<chrono>
#include<sstream>
using namespace std;

const unsigned int black = 0x00000000;
const unsigned int white = 0x00ffffff;
typedef complex<double> c_num;
unsigned int* pixel_arr = nullptr;

void initialize_variables(int num_args, char* argv[], unsigned int& width, unsigned int& height,
	double& x_min, double& x_max, double& y_min, double& y_max, unsigned int& num_threads, string& image_name, bool& quiet, unsigned int& max_iter, unsigned int& bound, unsigned int& decomp_type) {

	for (int i = 1; i < num_args;) {
		string str(argv[i]);

		if (str == "-s" || str == "-size") {

			if (i + 1 == num_args) {
				cout << "Incorrect input\n";
				return;
			}

			istringstream ss(argv[i + 1]);
			char symbol;
			ss >> width;
			ss >> symbol;
			ss >> height;

			if (symbol != 'x') {
				cout << "Incorrect input\n";
				exit(1);
			}

			i += 2;
		}
		else if (str == "-r" || str == "-rect") {

			if (i + 1 == num_args) {
				cout << "Incorrect input\n";
				exit(1);
			}

			istringstream ss(argv[i + 1]);
			char symbol;
			ss >> x_min;
			ss >> symbol;

			if (symbol != ':') {
				cout << "Incorrect input\n";
				exit(1);
			}

			ss >> x_max;
			ss >> symbol;

			if (symbol != ':') {
				cout << "Incorrect input\n";
				exit(1);
			}

			ss >> y_min;
			ss >> symbol;

			if (symbol != ':') {
				cout << "Incorrect input\n";
				exit(1);
			}

			ss >> y_max;
			i += 2;
		}
		else if (str == "-t" || str == "-tasks") {

			if (i + 1 == num_args) {
				cout << "Incorrect input\n";
				exit(1);
			}

			istringstream ss(argv[i + 1]);
			ss >> num_threads;
			i += 2;
		}
		else if (str == "-o" || str == "-output") {

			if (i + 1 == num_args) {
				cout << "Incorrect input\n";
				exit(1);
			}

			istringstream ss(argv[i + 1]);
			ss >> image_name;
			i += 2;
		}
		else if (str == "-q" || str == "-quiet") {
			quiet = true;
			i += 1;
		}
		else if (str == "-max-iter") { // maximum iterations

			if (i + 1 == num_args) {
				cout << "Incorrect input\n";
				exit(1);
			}

			istringstream ss(argv[i + 1]);
			ss >> max_iter;
			i += 2;

		} 
		else if (str == "-bound") { // this bound determines which points are in the Mandelbrot set

			if (i + 1 == num_args) {
				cout << "Incorrect input\n";
				exit(1);
			}

			istringstream ss(argv[i + 1]);
			ss >> bound;
			i += 2;
		}
		else if (str == "-dtype") { // this parameter determines the decomposition type (3 decomposition types are possible)
			
			if (i + 1 == num_args) {
				cout << "Incorrect input\n";
				exit(1);
			}

			istringstream ss(argv[i + 1]);
			ss >> decomp_type;
			i += 2;
		}
		else i += 1;
	}
}

struct thread_struct
{
	thread tr;
	std::chrono::high_resolution_clock::time_point thread_start_time;
	std::chrono::high_resolution_clock::time_point thread_end_time;
	int initial;
};

// diagonal problem decomposition
void thread_runnable1(thread_struct& tr_str, unsigned int num_threads, unsigned int width, unsigned int height, unsigned int max_iter, unsigned int bound, double x_min, double x_max, double y_min, double y_max) // added a bound here
{
	tr_str.thread_start_time = std::chrono::high_resolution_clock::now();

	unsigned int maximum = width * height;
	for (int k = tr_str.initial; k < maximum; k += num_threads) {

		int i = k / width;
		int j = k % width;

		c_num c(x_min + j * (x_max - x_min) / width, y_min + i * (y_max - y_min) / height);
		c_num z(0, 0);

		unsigned int iter_cnt = 0;
		while (abs(z) <= bound && iter_cnt < max_iter) {
			z = exp(z * z + c);
			++iter_cnt;
		}

		if (abs(z) <= bound) {
			pixel_arr[k] = black; // k here
		}
		else {
			pixel_arr[k] = white; // k here
		}
	}

	tr_str.thread_end_time = std::chrono::high_resolution_clock::now();
}

// fine-grained
void thread_runnable2(thread_struct& tr_str, unsigned int num_threads, unsigned int width, unsigned int height, unsigned int max_iter, unsigned int bound, double x_min, double x_max, double y_min, double y_max)
{
	tr_str.thread_start_time = std::chrono::high_resolution_clock::now();

	for (int k = tr_str.initial; k < height; k += num_threads) {

		int i = k; // the number of the row
		for (int j = 0; j < width; ++j) {
			c_num c(x_min + j * (x_max - x_min) / width, y_min + i * (y_max - y_min) / height);
			c_num z(0, 0);

			unsigned int iter_cnt = 0;
			while (abs(z) <= bound && iter_cnt < max_iter) {
				z = exp(z * z + c);
				++iter_cnt;
			}

			if (abs(z) <= bound) {
				pixel_arr[i * width + j] = black;
			}
			else {
				pixel_arr[i * width + j] = white;
			}
		}
	}

	tr_str.thread_end_time = std::chrono::high_resolution_clock::now();
}

// coarse-grained
void thread_runnable3(thread_struct& tr_str, unsigned int num_threads, unsigned int width, unsigned int height, unsigned int max_iter, unsigned int bound, double x_min, double x_max, double y_min, double y_max) 
{
	tr_str.thread_start_time = std::chrono::high_resolution_clock::now();

	int num_rows = height / num_threads; // every thread executes this many rows
	int array_part = num_rows * width;
	int from = tr_str.initial * array_part;
	int to = from + array_part;

	for (int k = from; k < to; ++k) {
		int i = k / width;
		int j = k % width;

		c_num c(x_min + j * (x_max - x_min) / width, y_min + i * (y_max - y_min) / height);
		c_num z(0, 0);

		unsigned int iter_cnt = 0;
		while (abs(z) <= bound && iter_cnt < max_iter) {
			z = exp(z * z + c);
			++iter_cnt;
		}

		if (abs(z) <= bound) {
			pixel_arr[i * width + j] = black;
		}
		else {
			pixel_arr[i * width + j] = white;
		}
	}

	if (tr_str.initial == num_threads - 1) { // the last thread does the remaining work
		
		from = to;
		to = width * height;

		for (int k = from; k < to; ++k) {
			int i = k / width;
			int j = k % width;

			c_num c(x_min + j * (x_max - x_min) / width, y_min + i * (y_max - y_min) / height);
			c_num z(0, 0);

			unsigned int iter_cnt = 0;
			while (abs(z) <= bound && iter_cnt < max_iter) {
				z = exp(z * z + c); 
				++iter_cnt;
			}

			if (abs(z) <= bound) {
				pixel_arr[i * width + j] = black;
			}
			else {
				pixel_arr[i * width + j] = white;
			}
		}
	}

	tr_str.thread_end_time = std::chrono::high_resolution_clock::now();
}

void executor(const char* file_name, unsigned int num_threads, unsigned int width, unsigned int height, unsigned int max_iter, unsigned int bound, vector<thread_struct>& vector_of_structs, bool quiet_mode, double x_min, double x_max, double y_min, double y_max, unsigned int decomp_type) {

	auto start_all_threads = std::chrono::high_resolution_clock::now();

	void (*f_ptr)(thread_struct&, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, double, double, double, double) = nullptr;

	if (decomp_type == 1) f_ptr = thread_runnable1;
	else if (decomp_type == 2) f_ptr = thread_runnable2;
	else if (decomp_type == 3) f_ptr = thread_runnable3;
	else {
		cout << "Invalid decomposition argument\n";
		return;
	}

	for (int i = 0; i < num_threads; ++i) {
		vector_of_structs[i].initial = i;
		vector_of_structs[i].tr = thread(f_ptr, ref(vector_of_structs[i]), num_threads, width, height, max_iter, bound, x_min, x_max, y_min, y_max); // check the reference
		
		if (!quiet_mode) {
			cout << "Thread number " << i << " started\n";
		}
	}

	for (int i = 0; i < num_threads; ++i) {
		vector_of_structs[i].tr.join();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (vector_of_structs[i].thread_end_time - vector_of_structs[i].thread_start_time);
		
		if (!quiet_mode) {
			cout << "Thread number " << i << " finished\n";
			cout << "Thread " << i << " execution time was: " << elapsed.count() << " milliseconds\n";
		}
	}

	auto finish_all_threads = std::chrono::high_resolution_clock::now();
	auto elapsed_total = std::chrono::duration_cast<std::chrono::milliseconds>(finish_all_threads - start_all_threads);
	std::cout << "Total time taken (the clock was started right before the master started the first worker and stopped right after the last worker finished): " << elapsed_total.count() << "milliseconds\n";
	cout << "Threads used in current run: " << num_threads << '\n';

	int ret_val = SaveBitmap(file_name, pixel_arr, width, height);

	if (ret_val != 0)
	{
		switch (ret_val)
		{
		case 1: cout << "Error, cannot open file, image is not generated\n"; break;
		case 2: cout << "Error, wrong file type, image is not generated\n"; break;
		case 3: cout << "Error, buffer too small, image is not generated\n"; break;
		case 4: cout << "Error, file read error, image is not generated\n"; break;
		case 5: cout << "Error, cannot alocate memory, image is not generated\n"; break;
		case 6: cout << "Error, zero size, image is not generated\n"; break;
		}
	}
}

int main(int argc, char* argv[])
{
	unsigned int width = 1024, height = 1024, max_iter = 256, number_of_threads = 4, bound = 8, decomp_type = 1;
	double xmin = -2.0;
	double xmax = 2.0;
	double ymin = -2.0;
	double ymax = 2.0;
	bool quiet_mode = false;
	string image_name = "zad19.bmp";

	// initialize_variables(argc, argv, width, height, xmin, xmax, ymin, ymax, number_of_threads, image_name, quiet_mode, max_iter, bound, decomp_type);

	pixel_arr = new(nothrow) unsigned int[width * height];

	if (pixel_arr == nullptr) {
		cout << "Cannot allocate memory\n";
		return 1;
	}

	vector<thread_struct>vector_of_structs(number_of_threads);

	executor(image_name.c_str(), number_of_threads, width, height, max_iter, bound, vector_of_structs, quiet_mode, xmin, xmax, ymin, ymax, decomp_type);

	delete[] pixel_arr; // free used memory

	return 0;
}