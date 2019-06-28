makeVect = [x y] {
	ret([x=x y=y])
}

makeRect = [x y x2 y2] {
	ret([x=x y=y x2=x2 y2=y2])
}

movedRect = [rect move] {
	ret([
		x=+(rect.x: move.x:)
		y=+(rect.y: move.y:)
		x2=+(rect.x2: move.x:)
		y2=+(rect.y2: move.y:)
	])
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

# Minimum needed are Rect, Visible, Enabled, and Border #
buttonInfo = [
	# Rect = [ x=50, y=50, x2=200, y2=200 ] #
	#Caption = "Push"#
	Visible = true
	Enabled = true
	Border = true
	UseAlphaChannel = true
	ScaleImage = false
]

makeButton = [ x y h w ] {
	info = buttonInfo
	info.Rect = makeRect(x: y: +(x: w:) +(y: h:))
	ret(gui_create("button", info))
}

makeSmallButtonRect = {
	ret(makeRect(0 0 150 150))
}

makeSmallButtonAt = [x y details] {
	info = union(buttonInfo details)
	info.Rect = movedRect(makeSmallButtonRect: makeVect(x y))
	ret(gui_create("button", info))
}

colors.white = [alpha=255 red=255 green=255 blue=255]

button2spriteInfo = [
	Image = get_texture("./Open 2.png")
	ImageRect = makeRect(0 0 50 50)
	PressedImage = get_texture("./icon_New.png")
	PressedImageRect = makeRect(0 0 50 50)
#
	buttonUpIndex = 0
	buttonUpColor = colors.white
	buttonUpLoop = false
	buttonUpScale = false

	buttonDownIndex = 1
#
]

main = {
	b = makeSmallButtonAt(200 200)
	gui_id(b: 1)
	gui_text(b: "Quit")
	b = makeSmallButtonAt(400 200 button2spriteInfo)
	gui_id(b: 2)
	gui_text(b: "2")
	gui_on_button_clicked([elem_id]{
		print("Button clicked\n")
		if ( equal(elem_id: 1) ) {
			print("- Quit\n")
			close_application:
		}
		if ( equal(elem_id: 2) ) {
			print("- 2\n")
		}
	})
}

main:
