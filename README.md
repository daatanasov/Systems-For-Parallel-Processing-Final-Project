## Systems For Parallel Processing course at Sofia University (Faculty of Mathematics and Informatics) - summer semester 2019/2020

### Final project
#### This is my final project for the course

* **Problem description**: you can find the description of the problem in Bulgarian [here](./zad19-fractal.pdf)

* **Project documentation**: you can find the documentation of the project in Bulgarian [here](./documentation.pdf)

**How to use**
* you can provide **command line arguments** to the program. The format and the meaning of the arguments are described in the [problem description](./zad19-fractal.pdf), [project documentation](./documentation.pdf) and the [source code](./project.cpp) of the project 

* if you want to provide **command line arguments** then you should uncomment the call to the function **initialize_variables** before executing the program. After that provide some **command line arguments**
    * you are not obliged to provide all of the described **command line arguments** as long as you keep some hardcoded value for each unset argument
    * you should provide all **command line arguments** in case you don't have any harcoded values
* if you don't want to provide any **command line arguments** then don't uncomment the function **initialize_variables**
    * you can use the hardcoded values or change them as you like

* **Examples of correctly passed command line arguments**:
    * **all command line arguments set**: ***-s 640x480 -r -2.0:2.0:-1.0:1.0 -t 4 -o problem19.png -quiet -max-iter 128 -bound 6 -dtype 3***
    * **some of the command line arguments set**: ***-s 640x480 -o problem19.png -bound 6*** (valid if the unset arguments have hardcoded values)

**Results**
1. ![Fractal_image_1024x1024](./zad19_1024x1024.png)
* image 1. was generated when I executed the program on my machine with the hardcoded values

**Remark1**: observe the parallel execution and the *time needed to compute the fractal when you use* **1 thread, 2 threads, 4 threads, 8 threads, 16 threads and 32 threads**. ***Mind your hardware parallelism when testing the program***

**Remark2**: parallel execution speeds up fractal image generation immensely, but if the image to be generated is large then the program may still run for several minutes