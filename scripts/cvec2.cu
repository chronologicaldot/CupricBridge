# Easier to work with than [x=0 y=0]; Slower due to member copy #
CVec2 = [
	x=0 y=0
	px, py
	addVec = [p] {
		ret(CVec2(+(super.x: p.x:) +(super.y: p.y:)))
	}
	subtractVec = [p]{
		ret(CVec2(-(super.x: p.x:) -(super.y: p.y:)))
	}
	multiplyScalar = [s]{
		ret(CVec2(*(super.x: s:) *(super.y: s:)))
	}
	divideScalar = [s]{
		ret(CVec2(/(super.x: s:) /(super.y: s:)))
	}
	lengthSquared = {
		ret(+(*(super.x: super.x:) *(super.y: super.y:)))
	}
	dot = [p] {
		ret(+(*(super.x: p.x:) *(super.y: p.y:)))
	}
	cross = [p] {
		ret(-(*(super.x: p.y:) *(super.y: p.x:)))
	}
] {
	out = this
	out.x = px:
	out.y = py:
	ret(copy_of(out))
}
