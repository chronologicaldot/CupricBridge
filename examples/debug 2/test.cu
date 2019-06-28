-1 = -(0 1)

makeVect = [x y] {
	ret([x=x y=y])
}

makeRect = [x y x2 y2] {
	ret([x=x y=y x2=x2 y2=y2])
}

makeSolidColor = [red green blue] {
	ret([
		alpha = 255
		red = red
		green = green
		blue = blue
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

create_tab = [tabcontrol tabnumber tabrect name backcolor] {
	tab = gui_create("tab")
	tabattrs = [
		Visible = true
		Enabled = true
		Rect = tabrect
		Name = name
		Caption = name
		Id = tabnumber
		TabNumber = tabnumber
		DrawBackground = true
		BackColor = backcolor
		OverrideTextColorEnabled = true
		TextColor = makeSolidColor(0 55 0)
	]
	#gui_attrs(tab: tabattrs)#
	gui_parent(tab: tabcontrol:)
	gui_attrs(tab: tabattrs)
	ret(tab:)
}

main = {
	tabrect = makeRect(0 0 300 500)
	tabcontrol = gui_create("tabControl" [
		Visible = true
		Enabled = true
		Rect = tabrect
		ActiveTab = -1
		Border = true
		FillBackground = true
		TabHeight = 20
		#TabMaxWidth = -(tabrect.x2: 20)#
		TabVerticalAlignment = "upperLeft"
	])
	tabrect = makeRect(0 100 300 400)
	tab = create_tab(tabcontrol, 0, tabrect, "Tabby", makeSolidColor(255 0 0))
	tab = create_tab(tabcontrol, -1, tabrect, "Tiana", makeSolidColor(0 0 255))
}

main:
