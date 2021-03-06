require("geometry2.cu")

Button.sizes = [
	tiny = makeRect(10,10)
	small = makeRect(20,20)
	medium = makeRect(40,40)
	large = makeRect(60,60)
	huge = makeRect(100,100)
]

Button.defaults = [
	Rect = Button.sizes.medium
	PushButton = false
	Border = false
	UseAlphaChannel = true
	ScaleImage = false
]

Button.create = [ options ] {
	out.button = gui_create("button", union(Button.defaults options))
	if ( is_member(options, "callback") ) {
		if ( equal_type_value( ret_type(options.callback) type_of([]) ) ) {
			out.watcher = gui_watcher(button, "button click", options.callback)
		}
	}
	# NOTICE: If a watcher is created, you will need to keep it as the parent. Do NOT reparent the button! #
	ret(out)
}
