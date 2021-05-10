# Pointer with super variable. usage: a = Pwsv(b b.c), a: #
Pwsv = [parent child]{
	ret([
		parent ~ parent
		child ~ child
	]{
		ret( xwsv(this.parent this.child) )
	})
}
