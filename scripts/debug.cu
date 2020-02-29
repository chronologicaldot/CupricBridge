# Usage: assert_type(obj "mytype") #
assert_type = [arg type] {
	assert(matching(type_of(arg:) type:))
}

# Usage: assert_type(obj "mytypename") #
assert_typename = [arg typename] {
	assert(matching(typename_of(arg:) typename:))
}

# Usage: assert_string("mystring") #
assert_string = [arg] {
	#assert(are_type(type_of("") arg))#
	assert(are_string(arg))
}

# Usage: assert_numeric(98765) #
assert_numeric = [arg] {
	#assert(are_type(type_of(0) arg))#
	assert(are_number(arg))
}
