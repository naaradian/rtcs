var MENU_POS=[
// Level 0 block configuration
{
	// Item's width in pixels
	'width'      : 120,
	// Item's height in pixels
	'height'     : 20,
	// if Block Orientation is vertical
	'vertical'   : true,
	// block behaviour if single frame:	
	// 1 - shift to the edge, 2 - flip relatively to left upper corner
	'wise_pos'   : 1,
	// Block outing table parameters: 
	// [cellpadding, cellspacing, border]
	'table'      : [0, 3, 0],
	// Time Delay in milliseconds before menu collapses after mouse 
	// pointer lefts all items
	'hide_delay' : 300,
	// Time Delay in milliseconds before subling block expands  
	// after mouse pointer overs an item
	'expd_delay' : 300,
	// Style class names for the level
	'css' : {
		// Block outing table class
		'table' : 'mtable',
		// Item inner tag style class for all item states or
		// classes for [<default state>, <hovered state>, <clicked state>]
		'inner' : 'minner',
		// Item outer tag style class for all item states or
		// classes for [<default state>, <hovered state>, <clicked state>]
		'outer' : ['moout', 'moover', 'modown']
	}
},
// Level 1 block configuration
{
	// Vertical Offset between adjacent levels in pixels
	'block_top'  : 4,
	// Horizontal Offset between adjacent levels in pixels
	'block_left' : 86,
	'width'      : 235
}
// Rest level configurations are inherited from level 1
]


