Brush Project

- ui interaction & settings:
When Canvas2D class object is made as the user starts the program,
the program processes the settings that can be modified by the user
using tools such as slider on the interface. I decided to have a
member variable Brush* m_brush, where the instantiated Brush will be
stored. Canvas2D::newBrush() is called in Canvas2D::settingsChanged(),
assigning a brush matching the user input for brush type, color, and radius
to member variable m_brush.

m_brush is used in mouse interactions (in mouseDown, mouseDragged, and mouseUp).
Since each type of brushes were instantiated and assigned as Brush* class
variable, I could use polymorphism to avoid calling same method (brushDown,
brushDragged, and brushUp) multiple times on separate variables.



- brushDragged defined in superclass Brush:
Using min and max methods to define rowstart, rowend, colstart, colend variable
ensured that the pixels outside the canvas do not get modified.
With maximum and minimum values for row and columns of pixels defined, pixels
were assigned with new RGBA color values in double for loop, where outer loop
iterates over the rows brush radius covers, and the inner loop iterates over
the pixel columns brush radius covers when mouse interaction is made.

To ensure that only the pixels inside the brush radius get affected, I calculated
the distance of each pixel from the origin of mouse interaction mouseX and mouse Y,
and put the proceeding color reassigning process in the if statement that checks
that the distance is less or equal to the radius.



- color blending function
Defining makeMask() in each brush classes, I assigned values to m_mask,
which is a vector of float, of size equal to the radius of the brush.
This allowed the color intensity to change in function of distance from
the center of mouse interaction.
Color blending function, where the final color = mask value * alpha * brush Color
+ (1 - mask value * alpha) * canvas color, was implemented using the provided
static method lerp. By rearranging the color blending function, I identified
that the input variables to lerp function should be: a = canvas color,
b = brush color, and percent = mask value * alpha value.
