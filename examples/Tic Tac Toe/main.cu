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

buttonInfo = [
	Visible = true
	Enabled = true
	Border = true
	UseAlphaChannel = true
	#ScaleImage = false#
]

scalingButtonInfo = [
	LeftAlign = "scale"
	RightAlign = "scale"
	TopAlign = "scale"
	BottomAlign = "scale"
]

isEven = [v]{
	ret(equal(%(v: 2) 0))
}

# Specific details #

#screenSize = [width=600 height=700]#
screenSize = gui_position(gui_root:)
screenSize.width = screenSize.x2
screenSize.height = screenSize.y2

gameDefaults = [
	slotSize = 200
]

gameBoard # GUI element (wrapping element) #
gameTable # list of 9 slots #
gameOver = false
mainMenuBar # GUI element #

gameSettings = [
	AIenabled = false
	AIlevel = 1
]

app = [
	closeID = 100
	saveGameID = 101
	loadGameID = 102
	newGameID = 103
	AIvsHumanID = 104
	AILevelID = 105
	bannerID = 50
]

getGameImage = [imageName] {
	ret(get_texture(concat("./img/" imageName: ".png")))
}
getCloseImage = { ret(getGameImage("exit")) }
getSaveGameImage = { ret(getGameImage("save")) }
getLoadGameImage = { ret(getGameImage("load")) }
getNewGameImage = { ret(getGameImage("new")) }
getHumanvsHumanImage = { ret(getGameImage("human_vs_human")) }
getAIvsHumanImage = { ret(getGameImage("ai_vs_human")) }
getEmptySlotImage = { ret(getGameImage("q")) }
getOSlotImage = { ret(getGameImage("circle")) }
getXSlotImage = { ret(getGameImage("x")) }
getPlayer1WinsBanner = { ret(getGameImage("player1wins")) }
getPlayer2WinsBanner = { ret(getGameImage("player2wins")) }
getDrawBanner = { ret(getGameImage("draw")) }

menuBarHeight = {
	ret(-(screenSize.height: *(gameDefaults.slotSize: 3)))
}

makeMenuBar = {
	#ret(gui_new_empty([#
	ret(gui_create("toolBar" [
		Rect = makeRect(0 0 screenSize.width menuBarHeight:)
		Visible = true
		Enabled = true
		RightAlign = "scale"
		BottomAlign = "scale"
	]))
}

makeMenuButton = [pos size id image parent tooltip]{
	b = gui_create("button" union(buttonInfo [
		Rect = makeRect(pos.x: pos.y: +(pos.x: size:) +(pos.y: size:))
		LeftAlign = "scale"
		RightAlign = "scale"
		BottomAlign = "lowerRight"
		Id = id
		Image = image
		#ScaleImage = true#
		ToolTip = tooltip
	]))
	gui_parent(b: parent:)
	ret(b:)
}

makeMenuAIDropDown = [pos size id parent] {
	info = [
		Visible = true
		Enabled = true
		Rect = makeRect(pos.x: pos.y: +(pos.x: *(size: 4)) +(pos.y: size:))
		LeftAlign = "scale"
		RightAlign = "scale"
		BottomAlign = "lowerRight"
		Id = id
		NoClip = true
		ItemCount = 4
		Item0Text = "Basic AI"
		Item0Enabled = true
		Item1Text = "Decent AI"
		Item1Enabled = true
		Item2Text = "Hard AI"
		Item2Enabled = true
		Item3Text = "Impossible AI"
		Item3Enabled = true
	]
	e = gui_create("dropdown" info)
	gui_parent(e: parent:)
}

gameButtonInfo = [
	basics = [
			Visible = true
			Enabled = true
			UseAlphaChannel = true
			#ScaleImage = true#
			Border = true
		]

	rect = [slotPos]{
		x = *(slotPos.x: gameDefaults.slotSize:)
		y = *(slotPos.y: gameDefaults.slotSize:)
		x2 = +(x: gameDefaults.slotSize:)
		y2 = +(y: gameDefaults.slotSize:)
		ret(makeRect(x y x2 y2))
	}
]

makeGameBoard = {
	iToXY = [i] {
		ret([ x = %(i: 3), y = /(i: 3) ])
	}
	gameBoard = gui_new_empty([
		Rect = makeRect(0 menuBarHeight: screenSize.width: screenSize.height:)
		Visible = true
		Enabled = true
		RightAlign = "lowerRight"
		TopAlign = "scale"
		BottomAlign = "lowerRight"
	])
	for(9 [i iToXY~iToXY] {
		b = gui_create("button")
		gui_parent(b: gameBoard:)
		gui_attrs(b: union(gameButtonInfo.basics scalingButtonInfo [
			Rect=gameButtonInfo.rect(this.iToXY(i))
			Id = +(i: 1)
			Image = getEmptySlotImage:
		] ))
	})
}

buildGUI = {
	mainMenuBar = makeMenuBar:

	size = -(menuBarHeight: 20)
	shift = +(size: 10)
	l = leaper(size)
	pos = makeVect(10 10)
	makeMenuButton(pos size app.newGameID getNewGameImage: mainMenuBar "New Game")
#
	pos = movedVectX(pos shift)
	makeMenuButton(pos size app.loadGameID getLoadGameImage: mainMenuBar "Load Game")
	pos = movedVectX(pos shift)
	makeMenuButton(pos size app.saveGameID getSaveGameImage: mainMenuBar "Save Game")
#
	pos = movedVectX(pos shift)
	b = makeMenuButton(pos size app.AIvsHumanID getHumanvsHumanImage: mainMenuBar "Switch between AI vs Human and Human vs Human")
	gui_value(b: [PushButton=true Pressed=false])
	pos = movedVectX(pos shift)
	makeMenuButton(pos size app.closeID getCloseImage: mainMenuBar "Exit Game")

	pos = movedVectX(pos shift)
	makeMenuAIDropDown(pos size app.AILevelID mainMenuBar)

	makeGameBoard:

	# Victory banner #
	ssw2 = /(screenSize.width: 2)
	ssh2 = /(screenSize.height: 2)
	banner = gui_create("image" [
		Rect = makeRect(-(ssw2: 128) -(ssh2: 128) +(ssw2: 128) +(ssh2: 128))
		Id = app.bannerID
		Visible = false
		Enabled = false
		LeftAlign = "center"
		RightAlign = "center"
		TopAlign = "center"
		BottomAlign = "center"
		UseAlphaChannel = true
		#ScaleImage = true#
		Color = [red=255 green=255 blue=255 alpha=255]
		Image = getPlayer1WinsBanner:
	])
	gui_parent(banner: gameBoard:)
}

buildGUI:

playerTurnSequence = [playerCount]{
	ret([
		playerCount = playerCount:
		current = 1
		reset = { super.current = 1 }
	]{
		++(this.current:)
		if ( gt(this.current: this.playerCount:) ) {
			this.current = 1
		}
	})
}
playerTurn = playerTurnSequence(2)

displayWinnerBanner = [winnerID] {
	banner = gui_child_with_id(gameBoard: app.bannerID:)
	if ( equal(winnerID: 1) ) {
		gui_value(banner: [Texture=getPlayer1WinsBanner:])
	}
	elif ( equal(winnerID: 2) ) {
		gui_value(banner: [Texture=getPlayer2WinsBanner:])
	}
	else {
		gui_value(banner: [Texture=getDrawBanner:])
	}
	gui_visible(banner: true)
}

clearWinnerBanner = {
	banner = gui_child_with_id(gameBoard: app.bannerID:)
	gui_visible(banner: false)
}

resetGameTable = {
	gameTable = list(0 0 0 0 0 0 0 0 0)
}
resetGameTable:

resetGame = {
	clearWinnerBanner:
	for(9 [i]{
		b = gui_child_with_id(gameBoard: +(i: 1))
		info = gameButtonInfo.basics
		info.Image = getEmptySlotImage:
		gui_attrs(b: info)
	})
	resetGameTable:
	playerTurn.reset:
	gameOver = false
}

isGameBoardFull = {
	b.count = 0
	for(length(gameTable:) [i b~b]{
		if ( gt(item_at(gameTable: i:) 0) ) {
			++(this.b.count:)
		}
	})
	ret(equal(b.count: length(gameTable:)))
}

getPlayerImageFromID = [player] {
	if ( equal(player: 1) ) {
		ret(getOSlotImage:)
	}
	else {
		ret(getXSlotImage:)
	}
}

# The button IDs are 1-9 #
markButton = [elem_id player]{
	tableSlot = -(elem_id: 1)
	if ( gt(item_at(gameTable: tableSlot:) 0) ) {
		ret(false)
	}
	replace(gameTable: tableSlot: player:)
	b = gui_child_with_id(gameBoard: elem_id:)
	info = gameButtonInfo.basics
	info.Image = getPlayerImageFromID(player)
	gui_attrs(b: info)
	ret(true)
}

tableSequences = list(
	list(0 1 2) list(3 4 5) list(6 7 8)
	list(0 3 6) list(1 4 7) list(2 5 8)
	list(0 4 8) list(2 4 6)
)

findWinner = {
	winner.id = 0
	tableSel = [idx] {
		ret(item_at(gameTable: idx:))
	}
	checkSequence = [seq t~tableSel] {
		ts = [idx t~this.t s~seq]{
			ret(this.t(item_at(this.s: idx:)))
		}
		who = ts(0)
		who.same = true
		seqlen = length(seq:)
		for(seqlen: [i w~who ts~ts]{
			if ( not(equal(this.ts(i) this.w:)) ) {
				this.w.same = false
			}
		})
		if ( who.same: ) { ret(who:) }
		else { ret(0) }
	}
	forlist(tableSequences: [item cs~checkSequence w~winner] {
		id = this.cs(item)
		if ( gt(id: 0) ) {
			this.w.id = id:
		}
	})
	ret(winner.id:)
}

runGameClick = [
	buttonID
	mark = [buttonID] {
		marked = markButton(buttonID playerTurn.current:)
		if ( marked: ) {
			playerTurn:
		}
	}
	post = {
		winner = findWinner:
		if ( any(not(equal(winner: 0)) isGameBoardFull:) ) {
			gameOver = true
			displayWinnerBanner(winner)
		}
	}
] {
	gameOver = isGameBoardFull:
	if ( gameOver: ) {
		ret:
	}
	r = this.mark(buttonID)
	this.post:
}

runAI = [
	lookup = list:

	pickFirstOpenSlot = {
		i = 0
		tableLen = length(gameTable:)
		loop {
			if ( gte(i: tableLen:) ) { stop }
			if ( equal(item_at(gameTable: i:) 0) ) {
				runGameClick.mark(+(i: 1))
				stop
			}
			++(i:)
		}
	}

	getTableStep = {
		step = 0
		i = 0
		l = length(gameTable:)
		loop {
			if ( gte(i: l:) ) { stop }
			if ( gt(item_at(gameTable: i:) 0) ) {
				++(step:)
			}
			++(i:)
		}
		ret(step:)
	}

	takeCenter = {
		if ( equal(item_at(gameTable: 4) 0) ) {
			runGameClick.mark(5)
			ret(true)
		}
		ret(false)
	}

	pickWinMove = {
		# Find an empty slot on the end of or between two marked. A winning sequence has a value of 4. #
		winseq.idx = 0 # <- where to make the click to perform the winning move #
		winseq.found = false
		forlist(tableSequences: [seq winseq~winseq]{
			total.value = 0
			this.winseq.valid = true
			forlist(seq: [i total~total winseq~this.winseq]{
				gt_slot = item_at(gameTable: i:)
				this.total.value = +(this.total.value: gt_slot:)
				if ( equal(gt_slot: 1) ) {
					this.winseq.valid = false
				} 
				elif ( equal(gt_slot: 0) ) {
					this.winseq.idx = i
				}
			})
			if ( all(equal(total.value: 4) this.winseq.valid:) ) {
				this.winseq.found = true
				this.winseq.savedIdx = this.winseq.idx
			}
		})
		if ( winseq.found: ) {
			runGameClick.mark(+(winseq.savedIdx: 1))
			ret(true)
		}
		ret(false)
	}

	pickBlockingMove = {
		pick.idx = 0
		pick.savedIdx = 0
		pick.found = false
		forlist(tableSequences: [seq pick~pick]{
			occupancy.count = 0
			occupancy.total = 0
			forlist(seq: [i oc~occupancy pick~this.pick]{
				gt_slot = item_at(gameTable: i:)
				this.oc.total = +(this.oc.total: gt_slot:)
				if ( gt(gt_slot: 0) ) {
					++(this.oc.count:)
				} else {
					this.pick.idx = i
				}
			})
			if ( equal(occupancy.total: occupancy.count: 2) ) {
				this.pick.found = true
				this.pick.savedIdx = this.pick.idx
			}
		})
		if ( pick.found: ) {
			runGameClick.mark(+(pick.savedIdx: 1))
			ret(true)
		}
		ret(false)
	}

# Pillar Move:
   X
     O
       O
#
	counterPillarMove = {
		gtie = [idx value] { ret(equal(item_at(gameTable: idx:) value:)) }
		step = super.getTableStep:
		if ( equal(step: 3) ) {
			if ( all(gtie(0 2) gtie(4 1) gtie(8 1)) ) {
				runGameClick.mark(3) # upper right corner #
				ret(true)
			}
		}
		ret(false)
	}

# LR Move:
   _ _ _
   _ X O
   _ O _
#
	counterLRMove = {
		gtie = [idx value] { ret(equal(item_at(gameTable: idx:) value:)) }
		step = super.getTableStep:
		if ( equal(step: 3) ) {
			if ( all(gtie(4 2) gtie(5 1) gtie(7 1)) ) {
				runGameClick.mark(9) # lower right corner #
				ret(true)
			}
		}
		ret(false)
	}

# Belt Move:
   _ _ O
   _ X _
   O _ _
#
	counterBeltMove = {
		gtie = [idx value] { ret(equal(item_at(gameTable: idx:) value:)) }
		step = super.getTableStep:
		if ( equal(step: 3) ) {
			if ( all(gtie(4 2) gtie(2 1) gtie(6 1)) ) {
				runGameClick.mark(2) # top middle #
				ret(true)
			}
		}
		ret(false)
	}

# Hockey Stick:
   O _ _
   _ X O
   _ _ _
#
	counterHockeyStickMove = {
		gtie = [idx value] { ret(equal(item_at(gameTable: idx:) value:)) }
		step = super.getTableStep:
		if ( equal(step: 3) ) {
			if ( gtie(4 2) ) {
				if ( gtie(0 1) ) {
					if ( gtie(5 1) ) {
						runGameClick.mark(8) # bottom middle #
						ret(true)
					}
					elif( gtie(7 1) ) {
						runGameClick.mark(6) # right middle #
						ret(true)
					}
				}
				elif ( gtie(2 1) ) {
					if ( gtie(7 1) ) {
						runGameClick.mark(6) # right middle #
						ret(true)
					}
				}
				elif ( gtie(5 1) ) {
					if ( gtie(6 1) ) {
						runGameClick.mark(8) # bottom middle #
						ret(true)
					}
				}
			}
		}
		ret(false)
	}

]{
	if ( equal(gameSettings.AIlevel: 1) ) {
		this.pickFirstOpenSlot:
	}
	elif( equal(gameSettings.AIlevel: 2) ) {
		if ( not(this.takeCenter:) ) {
			if ( not(this.pickWinMove:) ) {
				this.pickFirstOpenSlot:
			}
		}
	}
	elif( equal(gameSettings.AIlevel: 3) ) {
		if (not(this.pickWinMove:)) {
			if (not(this.pickBlockingMove:)) {
				if ( not(this.takeCenter:) ) {
					if ( not(this.counterPillarMove:) ) {
						this.pickFirstOpenSlot:
					}
				}
			}
		}
	}
	elif ( equal(gameSettings.AIlevel: 4) ) {
		if (not(this.pickWinMove:)) {
			if (not(this.pickBlockingMove:)) {
				if ( not(this.takeCenter:) ) {
					if ( not(this.counterPillarMove:) ) {
						if ( not(this.counterLRMove:) ) {
							if ( not(this.counterBeltMove:) ) {
								if ( not(this.counterHockeyStickMove:) ) {
									this.pickFirstOpenSlot:
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		this.pickFirstOpenSlot:
	}
}

gui_on_button_clicked([elem_id]{
	assert(are_int(elem_id:))

	if ( equal(elem_id: app.closeID:) ) {
		close_application:
		ret(true)
	}
	elif ( equal(elem_id: app.saveGameID:) ) {
		save_cu(gameTable)
		ret(true)
	}
	elif ( equal(elem_id: app.loadGameID:) ) {
		load_cu(gameTable)
		ret(true)
	}
	elif ( equal(elem_id: app.newGameID:) ) {
		resetGame:
		ret(true)
	}
	elif ( equal(elem_id: app.AIvsHumanID:) ) {
		e = gui_child_with_id(mainMenuBar: app.AIvsHumanID:)
		v = gui_attrs(e:)
		gameSettings.AIenabled = not(gameSettings.AIenabled:)
		if ( gameSettings.AIenabled: ) {
			v.Image = getAIvsHumanImage:
		} else {
			v.Image = getHumanvsHumanImage:
		}
		gui_attrs(e: v)
		resetGame:
		ret(true)
	}
	elif ( all(gt(elem_id: 0) gte(9 elem_id:)) ) {
		if ( not(gameOver:) ) {
			runGameClick(elem_id)
			if ( all(not(gameOver:) gameSettings.AIenabled: equal(playerTurn.current: 2)) ) {
				runAI:
				runGameClick.post:
			}
		}
		ret(true)
	}
	ret(false)
})

gui_on_combobox_changed([elem_id]{
	if ( equal(elem_id: app.AILevelID:) ) {
		e = gui_child_with_id(mainMenuBar: app.AILevelID:)
		v = gui_value(e:)
		gameSettings.AIlevel = +(1 v.Selected:)
		resetGame:
		ret(true)
	}
	ret(false)
})

#gui_on_listbox_changed([elem_id]{
	if ( equal(elem_id: app.AILevelID:) ) {
		e = gui_child_with_id(mainMenuBar: app.AILevelID:)
		v = gui_value(e:)
		gameSettings.AIlevel = +(1 v.Selected:)
		resetGame:
		ret(true)
	}
	ret(false)
})#
