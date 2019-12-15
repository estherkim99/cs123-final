README

How to use:

Our project is runnable in the Shapes/Sceneview tab. Follow these steps to run our pool table project!

1) Open up data/pool.xml and change the filepaths to point to the texture images located in data/textures. These filepaths are absolute, so you will have to change them to reflect the correct file path on your machine.
2) Go to file->open, and open up data/pool.xml.
3) You can use the pool table with or without orbit camera, but orbit camera may be easier to see all the features.
4) Click shoot to send the cue ball on its way! It's direction can be manipulated by the knob above.


Design decisions:

Our main decision for this project was to create a PoolScene file that derives from SceneviewScene, where we deal with collision detection. Bump mapping, texture mapping, and shadow mapping are handled in our shaders. Check out our github link for more info: https://github.com/estherkim99/cs123-final


Known bugs:

We have some small bugs with shadow mapping (i.e., not all balls create shadows).
