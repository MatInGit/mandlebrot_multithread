// Calculate a mandlebrot set and plot it as a bitmap (BMP) file
# include <fstream>
# include <cmath>
# include <complex>
# include <atomic> // for atomic double

// for bitmap file
# include <cstdint>
# include <vector>
# include <iostream>
# include <thread>

// for png file
#include "lodepng.h"

using namespace std;
using namespace std::complex_literals;

//for arguments


int evaluate(const double &x, const double &y, int max_iter)
{
    int iterations = 0;
    std::complex<double> c(x, y);
    std::complex<double> z(0.0, 0.0);

    while (abs(z) < 2 && iterations < max_iter)
    {
        //z = z * z + c;
        z = pow(z, 2) + c;
        iterations++;
    }
    //cout << "completed job on thread " << std::this_thread::get_id() << endl;

    if (iterations == max_iter)
    {
        return 0;
    }
    else
    {
        return iterations;
    }

    // return iterations;
}


int evaluate_chunk(const vector<double> &real_vector, const vector<double> &imaginary_vector, vector<vector<int>> &results, int start, int end, int max_iter)
{
    //cout << "started job on thread " << std::this_thread::get_id() << endl;
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < real_vector.size(); j++)
        {
            results[i][j] = evaluate(real_vector[i], imaginary_vector[j], max_iter);
            // cout << "i: " << i << " j: " << j << endl;
        }
    }
    //cout << "completed job on thread " << std::this_thread::get_id() << endl;
    return 0;
}






int main(int argc, char *argv[])
{   
    // parse the command line arguments 
    // arguments are centre_real, centre_imaginary, range, size, number of threads.
    // if no arguments are given, use defaults

    // set the default values
    double centre_real = -1.4002;
    double centre_imaginary = 0;
    double range = 0.1;
    int size = 5000;
    int number_of_threads = 8;

    // get args
    if (argc > 1)   // 
    {
        centre_real = atof(argv[1]); // 
    }
    if (argc > 2)
    {
        centre_imaginary = atof(argv[2]);
    }
    if (argc > 3)
    {
        range = atof(argv[3]);
    }
    if (argc > 4)
    {
        size = atoi(argv[4]);
    }
    if (argc > 5)
    {
        number_of_threads = atoi(argv[5]);
    }

    // print
    cout << "Mandlebrot set calculator" << endl;

    // set the centre and range
    // double centre = 0;
    // double centre_imaginary = 0;
    // double range = 5;
    // get user input for centre and range to override defaults
    // cout << "Enter centre: ";
    // cin >> centre;
    // cout << "Enter centre imaginary: ";
    // cin >> centre_imaginary;
    // cout << "Enter range: ";
    // cin >> range;

    // set the size
    // int size = 1000;
    // cout << "Enter size: ";
    // cin >> size; 
    cout << "Centre: " << centre_real << " + " << centre_imaginary << "i" << endl;
    cout << "Range: " << range << endl;
    cout << "Size: " << size << endl;

    // use std::thread::hardware_concurrency() to get the number of threads
    // cout << "Number of available threads: " << std::thread::hardware_concurrency() << endl;

    // total computatuions is size * size
    // each thread will do size * size / number of threads
    

    // create a vector of vectors to store the results
    vector<vector<int>> results(size, vector<int>(size, 0));

    // create a vector of threads
    vector<thread> threads;

    // create a vector of doubles to store the real values
    vector<double> real_double_vector(size, 0);
    // create a vector of doubles to store the imaginary values
    vector<double> imaginary_double_vector(size, 0);

    //populate the real and imaginary vectors

    // calculate starting real and imaginary values
    double start_real = centre_real - range/2.0;
    double start_imaginary = centre_imaginary - range/2.0;

    for (int i = 0; i < size; i++)
    {
        real_double_vector[i] = start_real + i * range / size;
        imaginary_double_vector[i] = start_imaginary + i * range / size;

        // cout << real_double_vector[i] << " " << imaginary_double_vector[i] << endl;
    }

    int completed = 0;
    
    // chunk the work into chunks with size / number of threads

    // get the number of available cores
    // int cores = std::thread::hardware_concurrency();

    // find the factors of the number of cores closest to each other
    int f1 = 1;
    int f2 = number_of_threads;
    
    // keep dividing the number of cores by 2 intill f2 is no longer a factor of 2
    while (f2 % 2 == 0)
    {
        f1 *= 2;
        f2 /= 2;
    }
    


    //cout << "f1: " << f1 << " f2: " << f2  << " " <<  f1 * f2 << endl;

    //int chunk_size = size/8;
    int chunk_size_x = size/f1;
    int chunk_size_y = size/f2;

    // evaluate chunks
    for (int i = 0; i < size; i += chunk_size_x)
    {
        for (int j = 0; j < size; j += chunk_size_y)
        {
            // create a thread to evaluate the chunk
            // threads.push_back(thread(evaluate_chunk, std::ref(real_double_vector), std::ref(imaginary_double_vector), std::ref(results), i, i + chunk_size, 255));
            threads.push_back(thread(evaluate_chunk, std::ref(real_double_vector), std::ref(imaginary_double_vector), std::ref(results), i, i + chunk_size_x, 255));
        }   
    }
    


    //cout << threads.size() << endl;
    
    cout << "Waiting for threads to complete" << endl;
    for (auto &thread : threads)
    {
        // update the progress bar
        completed++;
        cout << "\r";
        cout << "Progress: " << completed << "/" << threads.size() << flush;
        thread.join();
    }
    cout << endl;
    cout << "Threads completed" << endl;

    // plot the results
    cout << "Plotting results" << endl;
    // ofstream file;
    // file.open("mandlebrot.txt");
    // for (int i = 0; i < size; i++)
    // {
    //     for (int j = 0; j < real_double_vector.size(); j++)
    //     {
    //         file << results[i][j] << " ";
    //     }
    //     file << endl;
    // }

    // create a vector of unsigned chars to store the bitmap
    vector<unsigned char> bitmap(size * size * 4, 0);

    int max = 0;
    int min = 1000;
    // find the max and min values
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < real_double_vector.size(); j++)
        {
            if (results[i][j] > max)
            {
                max = results[i][j];
            }
            if (results[i][j] < min)
            {
                min = results[i][j];
            }
        }
    }
    if (max > 255)
    {
        max = 255;
    }
    double scaling_factor = 255.0 / (max - min);

    // populate the bitmap
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < real_double_vector.size(); j++)
        {
            // set the colour
            // int colour = results[i][j] % 256;
            int colour = (results[i][j] - min) * scaling_factor;

            // set the bitmap
            bitmap[(i * size + j) * 4 + 0] = colour;
            bitmap[(i * size + j) * 4 + 1] = colour;
            bitmap[(i * size + j) * 4 + 2] = colour;
            bitmap[(i * size + j) * 4 + 3] = 255;
        }
    }

    // save the bitmap
    lodepng::encode("mandlebrot.png", bitmap, size, size);

    


    // cout << imaginary_double_vector << endl;
    return 0;
}

