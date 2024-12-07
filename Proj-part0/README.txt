To build the code from command line (on MacOS or Linux), do:

g++ ECGraphicViewImp.cpp SimpleObserver.cpp test-gv.cpp -lallegro -lallegro_font -lallegro_primitives -lallegro_image -lallegro_main

Then run:

./a.out

You press the space bar and then a rectangle should be displayed.

Note: on some system (like my new MacBook), you will have to change the building instructions. For example, on my Mac, I need to do:

g++ ECGraphicViewImp.cpp SimpleObserver.cpp test-gv.cpp $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5  --libs --cflags) 

Contact your TA for help on building the starter code.

