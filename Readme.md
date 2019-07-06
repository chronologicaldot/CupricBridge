# Cupric Bridge

This project contains wrapper code to allow [Copper](https://github.com/chronologicaldot/CopperLang) code to create and control GUI elements of the [Irrlicht Engine](https://irrlicht.sourceforge.net).

The code is wrapped in the namespace "cubr".

The primary class is CuBridge, and it is responsible for providing the Copper interface for creating the GUI elements and setting/getting their attributes. It can be initiated on the stack (as opposed to the heap) provided that its lifetime or utilization meets that of the Copper engine it uses. This primary class accepts a pointer to an IGUIEnvironment and optionally a GUI element that will act as the root/parent element of all elements that created without a parent. By default, this root is the same as the default root from IGUIEnvironment.

Within the Copper code, the Irrlicht GUI elements are hidden within a generic wrapper object. This class can be passed by assignment since, despite the general rule of Copper objects, a duplicate of the GUI element is NOT created; merely the pointer to the GUI element is shared.

Within the Copper code, the Irrlicht image textures are wrapped within their own special object. This object is intended for setting GUI element attributes where textures are required.

There is an application-wide GUI event handler class that can be utilized to allow Copper functions to handle generic GUI events. Alternatively, there is a GUI watcher class that can be created like a GUI element and can be given a Copper function to call when a specified GUI event occurs.

## Setup

Cupric Bridge requires the [Copper Language Virtual Machine](https://github.com/chronologicaldot/CopperLang) to be placed in a directory adjacent to the Cupric Bridge folder.

## Usage within Copper

To create an Irrlicht GUI element (wrapper class included) in Copper, you must call the gui_create() function and pass to it the serializable name of the GUI element as well as the list of attributes associated with it. See also the Irrlicht method IGUIElement::deserializeAttributes().
```
element = gui_create("button" [
	Rect = [x=0 y=0 x2=200 y2=100]
])
```

### Attribute Storage

Copper objects are automatically converted to and from Irrlicht data structures and primitives. However, they have a unique storage.

Irrlicht rect<s32> class is stored as a Copper object with the members "x", "y", "x2", and "y2". Each of these contain a decimal number.

Irrlicht vector2d<s32> and vector2d<f32> are stored as Copper objects with the members "x" and "y", each of which contain a decimal number.

### API

These functions may be called within Copper code:

- gui_root() - Returns the root GUI element.
- gui_create(name, attributes) - Creates a GUI element from the given name and attribute-container object.
- gui_new_empty() - Creates a new basic GUI element.
- gui_watcher(child) / gui_watcher(child, callback_function) / gui_watcher(child, callback_event, callback_function) - Creates a wrapper class that watches the events of the child GUI element and passes them to the Copper callback_function if they match callback_event.
- gui_parent(child, parent) / gui_parent(child) - Sets/gets the child GUI element's parent to the given one.
- gui_child_with_id(parent, id) - Returns the GUI element child of the given parent with the given ID if found.
- gui_add_child(parent, new_child) - Makes new_child the child of the given parent GUI element.
- gui_remove_child(parent, child) - Removes the given child GUI element from the list of children of the parent.
- gui_attrs(element, attributes) / gui_attrs(element) - Sets/gets all of the attributes of the GUI element through the deserialization method of the element.
- gui_value(element, attributes) / gui_value(element) - Sets/gets only the most commonly changed attributes of the GUI element. NOTE: This acts as an alias for gui_attrs() unless you implement irr::io::EARWF_OFTEN_CHECKED_ATTRS code (see top of cubridge.cpp).
- gui_to_front(element) - Brings the given GUI element to the front of its parent's list of child elements.
- gui_enabled(element, setting) / gui_enabled(element) - Sets/gets the GUI element IsEnabled setting. Attribute is a boolean.
- gui_visible(element, setting) / gui_visible(element) - Sets/gets the GUI element IsVisible setting. Attribute is a boolean.
- gui_position(element, rectangle) / gui_position(element) - Sets/gets the GUI element position as a rectangle. See storage note above.
- gui_id(element, value) / gui_id(element) - Sets/gets the given GUI element's ID value.
- gui_text(element, text) / gui_text(element) - Sets/gets the given GUI element's text value.
- get_texture(path) - Loads the texture from the given path.

## Additional Support

The [Curri](https://github.com/chronologicaldot/Curri) project provides boiler plate code for creating applications with Copper and Cupric Bridge.

## License

All code and files in this project are copyright 2019 Nicolaus Anderson even where not specified.
Feel free to use and modify at will.
You may not claim you wrote this code. That would be lying.
This code is provided as-is without any expressed guarantee of merchantability or fitness. In no way will the authors of this code be responsible for any loss or damages occuring as a result of the usage of this code.
