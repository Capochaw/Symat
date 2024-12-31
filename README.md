# ![](https://github.com/Capochaw/Symat/blob/main/2024-12-31%3D163410.png)
(INCOMPLETE) A symbolic math WYSIWYG infinity board.

Thank you so much to GUST for [porting the TeX font for unicode](https://www.gust.org.pl/projects/e-foundry/lm-math).
# Compile
You must have ```SDL2``` installed on your PC.
1) Clone this repo
2) ```cd Symat```
3) ```make```
To execute, run ```bin/mathparser confs/calc.sm``` for a basic config for calculus use.
# TODO
[ ] Add fractions
# Create config
The sintaxis is the following:
```)FLAGS("a" "b"```
Where ```a``` is the written text that will convert to ```b``` in UTF8 characters and ```FLAGS``` can be:
```FLAGS```:
* ```B```: Makes a big op definition
* ```P```: Defines a start parenthesis.
* ```PP```: Defines a end parenthesis.
* ```F```: Defines a fraction token.
* ```U```: Defines the subscript operator.
* ```u```: Defines the downscript operator.
