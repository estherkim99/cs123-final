Brush Project

- Design:
I defined openglshape as a virtual class, which is inherited by subclasses
coneshape, cubeshape, cylindershape, openglshape, sphereshape.
The unique pointers to these objects are created at the initialization of the
shapescene. In shapescene initialization, we call renderShapes(), which
initializes the shapes as defined in respective shape classes using parameters
p1 and p2. renderShape is called every time the parameter settings are changed.


- Any known bugs:
None that I know of.
