# Usage: assert_type(obj "mytype") #
assert_type = [arg type] {
	assert(matching(type_of(arg:) type:))
}
