This project parses and analyzes a program. Finally, it interprets the program to check if it's valid.
Returns errors which are in the program at their appropriate line number. An example, like the following, should be put into `test.txt`:
```
PROGRAM Cylinder
    /*This is a comment*/
    /*Computing the volume of a sphere*/
    INT r, a, h;
    r = 8;
    h = 10;
    a = 0;
    FLOAT volume;
    volume = 3.14 * r * r * h;
    IF ( r > 0 AND h > 0 ) THEN
        PRINT("The volume of the sphere is: ", volume);
    ELSE
        PRINT("No computations should be done!");
    END IF;

END PROGRAM
```

Upon running `main.cpp`:
```
The volume of the sphere is: 2009.6
Successful Execution
```
