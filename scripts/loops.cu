for_members = [parent action] {
	ml = member_list(parent)
	mllen = length(ml:)
	i = 0
	loop {
		if ( equal(i: mllen:) ) { stop }
		action(member(parent item_at(ml: i:)))
		++(i:)
	}
}

for_list = [items action] {
	len = length(items:)
	i = 0
	loop {
		if ( equal(i: len:) ) { stop }
		action(item_at(items: i:))
		++(i:)
	}
}

for = [count action] {
	i = 0
	loop {
		if ( gte(i: count:) ) { stop }
		action(copy_of(i)) # copy_of prevents modding i #
		++(i:)
	}
}

while = [condition action] {
	loop {
		if ( not(condition:) ) { stop }
		action:
	}
}
# Example:
i = 0
while([i~i]{ret(lt(this.i: 10))} [i~i]{ ++(this.i:) print(concat("\n" this.i:)) })
#
