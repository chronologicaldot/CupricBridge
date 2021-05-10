makeVec2 = [x y] {ret([x=x y=y])}
makeRect = [x y x2 y2]{ ret([x=x y=y x2=x2 y2=y2]) }
makeCRect = [x y x2 y2]{ ret([corner1=makeVec2(x y) corner2=makeVec2(x2 y2)]) }
makeRectFromVec2 = [v] { ret([x=0 y=0 x2=v.x y2=v.y]) }

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

Vec2Ops = [
	addVecs = [v1 v2]{
		ret(makeVec2(+(v1.x: v2.x:), +(v1.y: v2.y:)))
	}
	subtractVecs = [v1 v2]{
		ret(makeVec2(-(v1.x: v2.x:), -(v1.y: v2.y:)))
	}
	addScalar = [v s] {
		ret(makeVec2(+(v.x: s:) +(v.y: s:)))
	}
	subtractScalar = [v s] {
		ret(makeVec2(-(v.x: s:) -(v.y: s:)))
	}
	addShiftXY = [v x y]{
		ret(makeVec2(+(v.x: x:) +(v.y: y:)))
	}
	multiplyScalar = [v s]{
		ret(makeVec2(*(v.x: s:), *(v.y: s:)))
	}
	divideScalar = [v s]{
		ret(makeVec2(/(v.x: s:), /(v.y: s:)))
	}
	lengthSquared = [v] {
		ret(+(*(v.x: v.x:) *(v.y: v.y:)))
	}
	dot = [v1 v2]{
		ret(+(*(v1.x: v2.x:) *(v1.y: v2.y:)))
	}
	cross = [v1 v2]{
		ret(-(*(v1.x: v2.y:) *(v1.y: v2.x:)))
	}
]

RectOps = [
	width = [r]{
		ret(abs(-(r.x: r.x2:)))
	}
	height = [r]{
		ret(abs(-(r.y: r.y2:)))
	}
	area = [r]{
		ret(abs(*(-(r.x: r.x2:) -(r.y: r.y2:))))
	}
	addVec = [r v]{
		ret(makeRect(+(r.x: v.x:) +(r.y: v.y:) +(r.x2: v.x:) +(r.y2: v.y:)))
	}
	shiftByVec2 = [r v]{
		++( r.x: v.x: )
		++( r.y: v.y: )
		++( r.x2: v.x: )
		++( r.y2: v.y: )
	}
	makeSameSizeRect = [r]{
		ret(makeRect(0 0 RectOps.width(r) RectOps.height(r)))
	}
]

makeVec2FromRect = [r]{
	ret( makeVec2(RectOps.width(r) RectOps.height(r)) )
}
