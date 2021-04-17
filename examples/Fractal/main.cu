# (C) 2019 Nicolaus Anderson.
Feel free to modify and share. #

# Utilities #

makeVect = [x y] {
	ret([x=x y=y])
}

movedVectX = [vect x]{
	vect.x = +(vect.x: x:)
	ret(vect)
}

movedVectY = [vect y]{
	vect.y = +(vect.y: y:)
	ret(vect)
}

makeRect = [x y x2 y2] {
	ret([x=x y=y x2=x2 y2=y2])
}

movedRect = [rect shift] {
	ret([
		x=+(rect.x: shift.x:)
		y=+(rect.y: shift.y:)
		x2=+(rect.x2: shift.x:)
		y2=+(rect.y2: shift.y:)
	])
}

foreach = [items do] {
	ml = member_list(items)
	mllen = length(ml:)
	i = 0
	loop {
		if ( equal(i: mllen:) ) { stop }
		do(member(items: item_at(ml: i:)))
		++(i:)
	}
}

forlist = [items do] {
	len = length(items:)
	i = 0
	loop {
		if ( equal(i: len:) ) { stop }
		do(item_at(items: i:))
		++(i:)
	}
}

for = [count do] {
	i = 0
	loop {
		if ( gte(i: count:) ) { stop }
		do(copy_of(i)) # copy_of prevents modding i #
		++(i:)
	}
}

leaper = [leap]{
	ret([i=0 leap=leap]{
		i2 = this.i
		this.i = +(this.i: this.leap:)
		ret(i2:)
	})
}

complex = [
	data = [
		real = 0.0
		imag = 0.0
	]

	add = [ a b ] {
		++(a.real: b.real:)
		++(a.imag: b.imag:)
	}

	sbtr = [ a b ] {
		--(a.real: b.real:)
		--(a.imag: b.imag:)
	}

	mult = [ a b ] {
		real = a.real:
		a.real = -(*(real: b.real:) *(a.imag: b.imag:))
		a.imag = +(*(real: b.imag:) *(a.imag: b.real:))
	}

	divd = [ a b ] {
		d = +(*(a.real: b.real:) *(b.imag: b.imag:))
		a.real = /(*(a.real: b.real:) d:)
		a.imag = /(*(a.imag: -(0 b.imag:)) d:)
	}

	sqr = [ a ] {
		#ret( -( *(a.real: a.real:) *(a.imag: a.imag:) ) )#
		ret( *(a.real: a.real:) )
	}
]

#-----------------#

image = image_create(300, 300, "A8R8G8B8")
print(typename_of(image:), "\n")
imageSize = image_size(image:)
c = complex.data

print("nts: " num_to_str(90.123456789, 7) "\n")

for(imageSize.width [w i~image]{
	w = +(0.5 w:)
	for(imageSize.height [h w~w]{
		h = +(0.5 h:)
		z = complex.data
		z.real = *(-(0.5 /(this.w: imageSize.width:)) 2)
		z.imag = *(-(0.5 /(h: imageSize.height:)) 2)
		c = z
		i = 0
		loop {
			if ( any( gte(complex.sqr(z) 4), gte(i: 20) ) ) { stop }
			z2 = z
			complex.mult(z z2)
			complex.add(z c)
			++(i:)
		}
		if ( equal(i: 20) ) {
			set_pixel( image: this.w: h: [red=0 green=100 blue=0 alpha=255] )
		} else {
			i = *(i: 10)
			set_pixel( image: this.w: h: [red=i green=i blue=i alpha=255] )
			#print("Pixel: ", num_to_str(this.w:) " " num_to_str(h:) "\t")#
		}
	})
})

tex = image_to_texture(image:, "button_tex")

gui_create("button" [
	Rect = [x=0 y=0 x2=300 y2=300]
	Image = tex
])
