vecXSwap = [v1 v2] {
	x = v2.x
	v2.x = v1.x
	v1.x = x
}

vecYSwap = [v1 v2] {
	y = v2.y
	v2.y = v1.y
	v1.y = y
}

CRect2 = [
	corner1 = [x=0 y=0]
	corner2 = [x=0 y=0]
	addVec = [v]{
		++( super.corner1.x: v.x: )
		++( super.corner1.y: v.y: )
		++( super.corner2.x: v.x: )
		++( super.corner2.y: v.y: )
	}
	shift = [x y]{
		++( super.corner1.x: x: )
		++( super.corner1.y: y: )
		++( super.corner2.x: x: )
		++( super.corner2.y: y: )
	}
	makeCorner1UpperLeft = {
		if ( gt(super.corner1.x: super.corner2.x:) ) {
			vecXSwap(super.corner1 super.corner2)
		}
		if ( gt(super.corner1.y: super.corner2.y:) ) {
			vecYSwap(super.corner1 super.corner2)
		}
	}
	width = {
		ret(abs(-(corner1.x: corner2.x:)))
	}
	height = {
		ret(abs(-(corner1.y: corner2.y:)))
	}
	area = {
		ret(*(super.width: super.height:))
	}
]
