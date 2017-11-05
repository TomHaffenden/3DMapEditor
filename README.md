# 3DMapEditor
My first year university project. Create 3D worlds with custom models and textures. Includes features such as parenting, saving/loading and dynamic lighting.

# Controls
W,A,S,D - First person camera movement

C - Toggle camerea controls

F5 - Quick save and test map

Mouse 1 - Selected object

Delete - Delete currently selected object

# UI
The "Window" menu allows you to show/hide and lock GUI windows

The "File" menu is ued to create new maps, save your currently opened map, or open previously saved maps.
Example maps include - "Example3". "Lights", "Velocity" and "Bowling"

To create a new object select the "create new object" gui window and extend one of the two tabs.
Enter the name of a custom .obj file that has been added to the models folder. If a cube is created instead of the intended model then your object could not be loaded. Either your file is in the wrong directory, or in the wrong format (e.g. square faces, without normal included).

To edit object properties make sure an object is currently selected and select the "Object window" gui window and you will be able to see all object properies that can be editied. You can type values into the boxes by double clicking or simple drag the slider up or down to edit it's value.

To Texture an object scroll to the bottom of the "Object Window" gui window and select the "Texture Path:" text field. Make sure the texture file is within the "materials/textures/" project folder and enter the textures file path excluding "materials/textures/", but including the file extention. Note only .tga and .bmp files are supported.
If a pick and black texture is applied to the model instead of the intended texture then your texture has failed to load. Please ensure it is in the right folder or named correctly.
If you would like to also load a specular map then place the file in the same directory and add "_specular" to the end of the files name.

Finally to parent objects to one another simple select the soon to be child object and press the "Parent Object" button, then select the object you would like to parent it to.
