makeRect = [x y x2 y2] {
	ret([x=x y=y x2=x2 y2=y2])
}

# Minimum needed are Rect, Visible, Enabled, and Border #
buttonInfo = [
	# Rect = [ x=50, y=50, x2=200, y2=200 ] #
	Caption = "Push Me"
	Visible = true
	Enabled = true
	TabStop = false
	TabGroup = false
	TabOrder = 0
	LeftAlign = "upperLeft" # or lowerRight, center, scale #
	RightAlign = "upperLeft"
	TopAlign = "upperLeft"
	BottomAlign = "upperLeft"
	NoClip = false
	UseAlphaChannel = false
	Border = true
]
# a.elem = gui_create("button" a.info) #
#gui_parent(a.elem: gui_root:)#

b_text = list("Quit","1","2","3","4","5","Pink","7","8")
ids = 0

makeButton = [ x y h w ] {
	#info = union(buttonInfo [ Rect=makeRect(x: y: +(x: w:) +(y: h:)) ])#
	info = buttonInfo
	info.Rect = makeRect(x: y: +(x: w:) +(y: h:))
	info.Id = ids:
	ids = +(ids: 1)
	ret(gui_create("button", info))
}

makeButtonAt = [ slotX slotY ] {
	w = 150
	h = 150
	x = *(-(slotX: 1) w:)
	y = *(-(slotY: 1) h:)
	ret(makeButton(x y w h))
}

foreach = [items do] {
	ml = member_list(items)
	mllen = length(ml:)
	i = 0
	loop {
		if ( equal(i: mllen:) ) { stop }
		do(item_at(items: i:))
		++(i:)
	}
}

main = {
	x = 1, y = 1
	loop {
		if ( gt(x: 3) ) { y = +(y: 1) , x = 1 }
		if ( gt(y: 3) ) { stop }
		b = makeButtonAt(x y)
		gui_text(b: item_at(b_text: +(*(-(y: 1) 3) -(x: 1))))
		# gui_text(b: num_to_str(gui_id(b:))) #
		x = +(x: 1)
	}
	sb = gui_child_with_id(gui_root: 1)
	sbinfo.PushButton = true
	sbinfo.Pressed = true
	gui_value(sb: sbinfo)
	#sbinfo = gui_value(sb:)
	foreach(sbinfo [item] {
		print(type_of(item:) "\n")
	})#
}

main:

