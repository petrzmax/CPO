# CPO
Project for Digital image processing on Mchtr PW

The goal of this project was to write a program which recognizes cards on the picture.
It should recognize cards, count them, recognize type of the cards and their numbers.
After this step program calculates the arithmetic average of the recognized even cards. 

The program should be resistant to image noise, uneven lighting and a blurred image.

Project is set to run properly only in Release mode.

## Recognition steps:

### 1. Load color image

![1](https://raw.githubusercontent.com/petrzmax/CPO/master/results/1_Color.png)

### 2. Convert color image to black and white

![2](https://raw.githubusercontent.com/petrzmax/CPO/master/results/2_Black_and_white.png)

### 3. Perform median filtration on the image

![3](https://raw.githubusercontent.com/petrzmax/CPO/master/results/3_after_median_filtration.png)

### 4. Perform thresholding

![4](https://raw.githubusercontent.com/petrzmax/CPO/master/results/4_after_threshold.png)

### 5. Perform closing operation

![5](https://raw.githubusercontent.com/petrzmax/CPO/master/results/5_after_closing_operation.png)

### 6. Perform segmentation 

![6](https://raw.githubusercontent.com/petrzmax/CPO/master/results/6_after_segmentation.png)

### 7. Recognize segments

![7](https://raw.githubusercontent.com/petrzmax/CPO/master/results/7_after_segmentation_with_recognized_segments.png)

### 8. Show recognized segments with colors characteristic for them

![8](https://raw.githubusercontent.com/petrzmax/CPO/master/results/8_recognized_segments_with_characteristic_colors_only.png)

### 9. Show the results

![9](https://raw.githubusercontent.com/petrzmax/CPO/master/results/results.png)

---
Coded by Artur Petrzak in 2017
